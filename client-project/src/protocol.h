/*
 * protocol.h
 *
 * Shared header file for UDP client and server
 * Contains protocol definitions, data structures, constants and function prototypes
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

/*
 * ============================================================================
 * PROTOCOL CONSTANTS
 * ============================================================================
 */

// #define ...
#define SERVER_PORT 56700  // Server port (change if needed)
#define SERVER_IP "127.0.0.1"  // Server port (change if needed)
#define BUFFER_SIZE 512    // Buffer size for messages
#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
/*
 * ============================================================================
 * PROTOCOL DATA STRUCTURES
 * ============================================================================
 */

// Weather request and response structures 
// Messaggio di Richiesta (Client -> Server)
typedef struct {
    char type;        // 't', 'h', 'w', 'p'
    char city[64];    // Nome città (stringa null-terminated)
} weather_request_t;

// Messaggio di Risposta (Server -> Client)
typedef struct {
    unsigned int status;  // 0=Ok, 1=No City, 2=Invalid
    char type;            // Eco del tipo
    float value;          // Valore numerico
} weather_response_t;
/*
 * ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================
 */

// Add here the signatures of the functions you implement


#endif /* PROTOCOL_H_ */
