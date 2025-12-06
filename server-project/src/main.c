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

#include "protocol.h"

#define NO_ERROR 0

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

	// TODO: Implement server logic

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

	// TODO: Create UDP socket
	 my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	    if (my_socket < 0) {
	        perror("socket creation failed.\n");
	        clearwinsock();
	        return -1;
	    }

	// TODO: Configure server address

	    struct sockaddr_in sad;
	     	memset(&sad, 0, sizeof(sad));
	     	sad.sin_family = AF_INET;
	     	sad.sin_addr.s_addr = inet_addr(server_ip);
	     	sad.sin_port = htons(port);

	// TODO: Bind socket

	        if (bind(my_socket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
	            perror("bind() failed.\n");
	            closesocket(my_socket);
	            clearwinsock();
	            return -1;
	        }

	        if (listen(my_socket, QLEN) < 0) {
	               perror("listen() failed.\n");
	               closesocket(my_socket);
	               clearwinsock();
	               return -1;
	           }
	        printf("Weather server running on port %d...\n", SERVER_PORT);


	    	// TODO: Implement UDP datagram reception loop
	        struct sockaddr_in cad;
	        int client_socket;
	        socklen_t_w client_len;

	        while (1) {
	            printf("Waiting for a client...\n");

	            client_len = sizeof(cad);
	            client_socket = accept(my_socket, (struct sockaddr*)&cad, &client_len);

	            if (client_socket < 0) {
	                perror("accept() failed\n");
	                continue;
	            }

	            printf("Client connected: %s\n", inet_ntoa(cad.sin_addr));

	            // 6) Gestione della richiesta

	            weather_request_t req;
	            int bytes = recv(client_socket, (char *)&req, sizeof(req), 0);

	            if (bytes <= 0) {
	                printf("Invalid request or connection closed.\n");
	                closesocket(client_socket);
	                continue;
	            }

	            weather_response_t res;
	            res.status = 0; // default = success
	            res.type = req.type;
	            res.value = 0.0f;

	            // Validazione tipo richiesto
	            if (req.type != 't' && req.type != 'h' &&
	                        req.type != 'w' && req.type != 'p') {

	                        res.status = 2; // richiesta non valida (tipo errato)
	                        res.type = '\0'; // Come da specifica errori
	                    }
	                    // 2. Validazione Città (Vuota o Non in lista)
	                    else if (strlen(req.city) == 0 || !is_city_valid(req.city)) {
	                        res.status = 1; // città non disponibile
	                        res.type = '\0'; // Come da specifica errori
	                    }
	                    else {
	                    	format_city_name(req.city);
	                        // Tipo valido E Città valida -> genera valore
	                        switch (req.type) {
	                            case 't': res.value = get_temperature(); break;
	                            case 'h': res.value = get_humidity();    break;
	                            case 'w': res.value = get_wind();        break;
	                            case 'p': res.value = get_pressure();    break;
	                        }
	                    }

	            //  Invio risposta
	            send(client_socket, (char *)&res, sizeof(res), 0);

	            printf("Request served. Closing connection.\n");
	            closesocket(client_socket);
	        }

	        closesocket(my_socket);
	        clearwinsock();
	        return 0;
} // main end
