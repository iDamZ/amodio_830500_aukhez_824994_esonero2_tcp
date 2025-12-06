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
#define SERVER_IP "127.0.0.1"

#define SERVER_PORT 56700
#define QLEN 6
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

// Lista delle città supportate (totale 10)
const char *VALID_CITIES[] = {
    "bari", "roma", "milano", "napoli", "torino",
    "palermo", "genova", "bologna", "firenze", "venezia"
};
const int NUM_CITIES = 10;

/*
 * ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================
 */

// Add here the signatures of the functions you implement
int are_strings_equal_case_insensitive(const char *s1, const char *s2);
int is_city_valid(const char *city);
void format_city_name(char *city);
float get_temperature(void);
float get_humidity(void);
float get_wind(void);
float get_pressure(void);

#endif /* PROTOCOL_H_ */
