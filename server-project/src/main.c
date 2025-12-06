/*
 * main.c
 *
 * UDP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP server
 * portable across Windows, Linux, and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

// Assumi che protocol.h definisca:
// - weather_request_t, weather_response_t
// - SERVER_IP, SERVER_PORT
// - NUM_CITIES, VALID_CITIES[]
#include "protocol.h"

#define NO_ERROR 0
// Dimensione massima del messaggio (dovrebbe essere sufficiente per il nostro protocollo)

#if defined WIN32
    typedef int socklen_t_w;
#else
    typedef socklen_t socklen_t_w;
#endif

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int are_strings_equal_case_insensitive(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return 0;
        }
        s1++;
        s2++;
    }
    return (*s1 == '\0' && *s2 == '\0');
}

int is_city_valid(const char *city) {
    for (int i = 0; i < NUM_CITIES; i++) {
        if (are_strings_equal_case_insensitive(city, VALID_CITIES[i])) {
            return 1;
        }
    }
    return 0;
}
void format_city_name(char *city) {
    // Controllo di sicurezza per stringhe nulle o vuote
    if (city == NULL || city[0] == '\0') {
        return;
    }

    // 1. Trasforma la prima lettera in Maiuscolo
    city[0] = toupper((unsigned char)city[0]);

    // 2. Trasforma tutte le lettere successive in minuscolo
    for (int i = 1; city[i] != '\0'; i++) {
        city[i] = tolower((unsigned char)city[i]);
    }
}

float get_temperature(void) {
    return -10.0f + (float)rand() / RAND_MAX * (40.0f - (-10.0f));
}

float get_humidity(void) {
    return 20.0f + (float)rand() / RAND_MAX * (100.0f - 20.0f);
}

float get_wind(void) {
    return (float)rand() / RAND_MAX * 100.0f;
}

float get_pressure(void) {
    return 950.0f + (float)rand() / RAND_MAX * (1050.0f - 950.0f);
}


int main(int argc, char *argv[]) {
	printf("Starting UDP server...\n");

	char *server_ip = SERVER_IP;
	int port = SERVER_PORT;

	for (int i = 1; i < argc; i++) {
	    if (strcmp(argv[i], "-s") == 0 && (i + 1) < argc) {
	        server_ip = argv[++i];
	    } else if (strcmp(argv[i], "-p") == 0 && (i + 1) < argc) {
	        port = atoi(argv[++i]);
	    }
	}

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

    srand(time(NULL)); // Random generator

	int my_socket;

	// **MODIFICA 1: Creazione Socket UDP**
	my_socket = socket(PF_INET, **SOCK_DGRAM**, **IPPROTO_UDP**);
	if (my_socket < 0) {
	    perror("socket creation failed.\n");
	    clearwinsock();
	    return -1;
	}

	// **MODIFICA 2: Configurazione indirizzo Server**
	struct sockaddr_in sad; // Server Address Descriptor
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(server_ip);
	sad.sin_port = htons(port);

	// **MODIFICA 3: Bind del socket**
	if (bind(my_socket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
	    perror("bind() failed.\n");
	    closesocket(my_socket);
	    clearwinsock();
	    return -1;
	}

	// Rimosse listen() e accept() specifiche di TCP

	printf("Weather UDP server running on IP %s port %d...\n", server_ip, port);


	// **MODIFICA 4: Ciclo di Gestione Richieste (UDP)**
    struct sockaddr_in client_address; // Client Address Descriptor
    socklen_t_w client_len;
    char buffer[MAX_MSG_SIZE];
    int bytes_received;

    while (1) {
        printf("\nWaiting for a datagram...\n");

        client_len = sizeof(client_address);
        // Uso di recvfrom per ricevere datagrammi UDP
        bytes_received = recvfrom(my_socket, buffer, sizeof(weather_request_t), 0,
                                  (struct sockaddr*)&client_address, &client_len);

        if (bytes_received <= 0) {
            // Un errore di ricezione (diverso da chiusura connessione che non si applica in UDP)
            perror("recvfrom() failed or connection closed (shouldn't happen in UDP)");
            continue;
        }

        if (bytes_received != sizeof(weather_request_t)) {
            printf("Received datagram of incorrect size (%d bytes). Ignoring.\n", bytes_received);
            continue;
        }

        // La richiesta è contenuta nel buffer e viene castata alla struttura
        weather_request_t *req = (weather_request_t *)buffer;

        printf("Request received from: %s:%d\n", inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));


        weather_response_t res;
        res.status = 0; // default = success
        res.type = req->type;
        res.value = 0.0f;

        // Validazione tipo richiesto
        if (req->type != 't' && req->type != 'h' &&
                    req->type != 'w' && req->type != 'p') {

                    res.status = 2; // richiesta non valida (tipo errato)
                    res.type = '\0'; // Come da specifica errori
                }
                // Validazione Città (Vuota o Non in lista)
                else if (strlen(req->city) == 0 || !is_city_valid(req->city)) {
                    res.status = 1; // città non disponibile
                    res.type = '\0'; // Come da specifica errori
                }
                else {
                	format_city_name(req->city);
                    // Tipo valido E Città valida -> genera valore
                    switch (req->type) {
                        case 't': res.value = get_temperature(); break;
                        case 'h': res.value = get_humidity();    break;
                        case 'w': res.value = get_wind();        break;
                        case 'p': res.value = get_pressure();    break;
                    }
                }

        // **Invio risposta (UDP)**
        // Uso di sendto per inviare datagrammi UDP all'indirizzo del client salvato in client_address
        if (sendto(my_socket, (char *)&res, sizeof(res), 0,
                   (struct sockaddr *)&client_address, client_len) != sizeof(res)) {
            perror("sendto() sent a different number of bytes than expected");
        }

        printf("Response sent to client.\n");
    }

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
