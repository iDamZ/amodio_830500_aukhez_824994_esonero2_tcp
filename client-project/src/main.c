/*
 * main.c
 *
 * UDP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP client
 * portable across Windows, Linux, and macOS.
 */
#define _POSIX_C_SOURCE 200112L
#if defined WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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
#include <errno.h>
#include <stdlib.h>
#include <string.h> /* for memset() */
#include <ctype.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement client logic

	char *server_ip = SERVER_IP;
	   int port = SERVER_PORT;
	char* input_string = NULL; //Stringa da inviare

	for (int i = 1; i < argc; i++) {
	        if (strcmp(argv[i], "-s") == 0 && (i + 1) < argc) {
	            server_ip = argv[++i];
	        } else if (strcmp(argv[i], "-p") == 0 && (i + 1) < argc) {
	            port = atoi(argv[++i]);
	        } else if (strcmp(argv[i], "-r") == 0 && (i + 1) < argc) {
	            input_string = argv[++i];
	        }
	    }

	    // Controllo obbligatorio: se manca -r, terminiamo
	    if (input_string == NULL) {
	        fprintf(stderr, "Errore: parametro -r obbligatorio\n");
	        return -1;
	    }

	    // Esempio: "t bari" -> type='t', city="bari"

	    //Controlli vari
	    if (strchr(input_string, '\t') != NULL) {
	    		fprintf(stderr, "Errore: La richiesta contiene caratteri di tabulazione non ammessi.\n");
	    		return -1;
	    	}


	    if (input_string[1] != ' ' && input_string[1] != '\0') {
	    		fprintf(stderr, "Errore: Il tipo di richiesta deve essere un singolo carattere.\n");
	    		return -1;
	    	}

	    //Identificazione inizio nome città
	    char *city_start = input_string + 1;
	    if (*city_start == ' ') {
	    		city_start++; //Salta lo spazio separatore se presente
	    	}

	    // La città deve entrare in req.city
	    if (strlen(city_start) > 63) {
	    		fprintf(stderr, "Errore: Il nome della citta' supera la lunghezza massima consentita (63 caratteri).\n");
	    		return -1;
	    	}

	    // Preparazione della struttura richiesta
	    weather_request_t req;
	    memset(&req, 0, sizeof(req));
	    req.type = input_string[0];
	    strcpy(req.city, city_start);

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	int fromSize;
	weather_response_t resp;
	int respStringLen;

	// TODO: Create UDP socket
	if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
	        perror("socket() failed");
	        clearwinsock();
	        return -1;
	    }

	// TODO: Configure server address

	struct hostent *he;
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = PF_INET;
	echoServAddr.sin_port = htons(port);

	if ((echoServAddr.sin_addr.s_addr = inet_addr(server_ip)) == INADDR_NONE) {
			if ((he = gethostbyname(server_ip)) == NULL) {
				fprintf(stderr, "Errore: convertire l'host %s\n", server_ip);
				closesocket(my_socket);
				clearwinsock();
				return -1;
			}
			memcpy(&echoServAddr.sin_addr, he->h_addr_list[0], he->h_length);
		}

	// TODO: Implement UDP communication logic

	    char buffer[BUFFER_SIZE];
	    int msg_len = 0;
	    buffer[0] = req.type;
	    msg_len++;
	    strcpy(buffer + 1, req.city);
	    msg_len += strlen(req.city) + 1;


	    if (sendto(my_socket, buffer, msg_len, 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != msg_len) {
	            perror("sendto() sent different number of bytes than expected");
	            closesocket(my_socket);
	            clearwinsock();
	            return -1;
	        }

	fromSize = sizeof(fromAddr);
	respStringLen = recvfrom(my_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&fromAddr, &fromSize);

	if (respStringLen < 0) {
	        perror("recvfrom() failed");
	        closesocket(my_socket);
	        clearwinsock();
	        return -1;
	    }


	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
	        fprintf(stderr, "Error: received a packet from unknown source.\n");
	        closesocket(my_socket);
	        clearwinsock();
	        return -1;
	    }

	//estrazione risposta in byte e conversione
	uint32_t net_status;
	uint32_t net_val_int;
	uint32_t host_val_int;

	memcpy(&net_status, buffer, sizeof(uint32_t));
	resp.status = ntohl(net_status);
	resp.type = buffer[sizeof(uint32_t)];
	memcpy(&net_val_int, buffer + sizeof(uint32_t) + sizeof(char), sizeof(uint32_t));
	host_val_int = ntohl(net_val_int);
	memcpy(&resp.value, &host_val_int, sizeof(float));

	//nome server
	char host_name[NI_MAXHOST];
		if (getnameinfo((struct sockaddr*)&fromAddr, sizeof(fromAddr),host_name, sizeof(host_name), NULL, 0, 0) != 0) {
			strcpy(host_name, inet_ntoa(fromAddr.sin_addr));
		}
		char *ip_str = inet_ntoa(fromAddr.sin_addr);

	//Normalizzazione Nome citta
	if (strlen(req.city) > 0) {
	            req.city[0] = toupper(req.city[0]);
	            for(int k=1; req.city[k]; k++) req.city[k] = tolower(req.city[k]);
	        }

	//Stampa Formattata
	if (resp.status == 0) {
		printf("Ricevuto risultato dal server %s (ip %s). ", host_name, ip_str);
		switch (req.type) {
	            case 't':
	                printf("%s: Temperatura = %.1f°C\n", req.city, resp.value);
	                break;
	            case 'h':
	                printf("%s: Umidità = %.1f%%\n", req.city, resp.value);
	                break;
	            case 'w':
	                printf("%s: Vento = %.1f km/h\n", req.city, resp.value);
	                break;
	            case 'p':
	                printf("%s: Pressione = %.1f hPa\n", req.city, resp.value);
	                break;
	            default:
	                // Caso in cui il server risponde successo ma il tipo non è riconosciuto dal client
	                printf("Tipo sconosciuto\n");
	                break;
	        }
	} else if (resp.status == 1) {
			// ERRORE: Città non disponibile
			printf("Ricevuto risultato dal server %s (ip %s). Città non disponibile\n", host_name, ip_str);
	} else {
			// ERRORE: Richiesta non valida (status 2 o altro)
			printf("Ricevuto risultato dal server %s (ip %s). Richiesta non valida\n", host_name, ip_str);
	}

	// TODO: Close socket
	// closesocket(my_socket);

	printf("Client terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
