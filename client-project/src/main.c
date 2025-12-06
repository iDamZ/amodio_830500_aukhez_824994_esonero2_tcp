/*
 * main.c
 *
 * UDP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP client
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
#include <errno.h>
#include <stdlib.h>
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
	char* input_string = NULL; // Stringa da inviare

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
	    weather_request_t req;
	        memset(&req, 0, sizeof(req));
	        req.type = input_string[0];
	        if (strlen(input_string) > 1) {
	            char *city_start = input_string + 1;
	            // Se c'è uno spazio subito dopo il tipo, lo saltiamo
	            if (*city_start == ' ') {
	                city_start++;
	            }
	            strncpy(req.city, city_start, 63);
	        } else {
	            req.city[0] = '\0';
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

	int my_socket;

	// TODO: Create UDP socket

	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_socket < 0) {
	perror("socket creation failed.\n");
	closesocket(my_socket);
	clearwinsock();
	return -1;
	}

	// TODO: Configure server address

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(server_ip);
	sad.sin_port = htons(port);

	// TODO: Implement UDP communication logic

	if (connect(my_socket, (struct sockaddr *)&sad, sizeof(sad))< 0)
	{
	perror( "Failed to connect.\n" );
	closesocket(my_socket);
	clearwinsock();
	return -1;
	}

	if (send(my_socket, (char *)&req, sizeof(req), 0) < 0) {
	        perror("Send failed");
	        closesocket(my_socket);
	        return -1;
	    }



	int bytes_rcvd;
	int total_bytes_rcvd = 0;
	weather_response_t resp;
	while (total_bytes_rcvd < (int)sizeof(resp)) {
	        bytes_rcvd = recv(my_socket, (char *)&resp + total_bytes_rcvd, (int)sizeof(resp) - total_bytes_rcvd, 0);

	        if (bytes_rcvd == 0) {
	            // Connessione chiusa dal server
	            if (total_bytes_rcvd < (int)sizeof(resp)) {
	                 perror("Connessione chiusa prematuramente");
	                 closesocket(my_socket);
	                 clearwinsock();
	                 return -1;
	            } else {
	                 break;
	            }
	        }
	        else if (bytes_rcvd < 0) {
	            perror("Receive failed");
	            closesocket(my_socket);
	            clearwinsock();
	            return -1;
	        }

	        total_bytes_rcvd += bytes_rcvd;
	    }
	//Stampa Formattata
	printf("Ricevuto risultato dal server ip %s. ", server_ip);
	if (resp.status == 0) {

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
	        printf("Città non disponibile\n");
	    } else {
	        // ERRORE: Richiesta non valida (status 2 o altro)
	        printf("Richiesta non valida\n");
	    }

	// TODO: Close socket
	// closesocket(my_socket);


	printf("Client terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
