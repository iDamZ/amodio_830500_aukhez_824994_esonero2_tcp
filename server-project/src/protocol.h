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

// Indirizzo IP di default del server
#define SERVER_IP "127.0.0.1"

// Porta UDP di default del server
#define SERVER_PORT 56700

// Dimensione massima del messaggio
#define BUFFER_SIZE 512
/*
 * ============================================================================
 * PROTOCOL DATA STRUCTURES
 * ============================================================================
 */

// Struttura per la Richiesta (Client -> Server)
typedef struct {
    // Tipo di dato meteo richiesto:
    // 't' = Temperature (Temperatura)
    // 'h' = Humidity (Umidità)
    // 'w' = Wind (Vento)
    // 'p' = Pressure (Pressione)
    char type;

    // Nome della città richiesta (stringa terminata da null)
    char city[64];
} weather_request_t;

// Struttura per la Risposta (Server -> Client)
typedef struct {
    // Codice di stato della risposta:
    // 0 = Successo (Ok)
    // 1 = Città non disponibile (No City)
    // 2 = Richiesta non valida (tipo errato) (Invalid)
    unsigned int status;

    // Eco del tipo di dato richiesto (o '\0' in caso di errore)
    char type;

    // Valore numerico della misurazione (float)
    float value;
} weather_response_t;

// Lista delle città supportate dal server (totale 10)
const char *VALID_CITIES[] = {
    "bari", "roma", "milano", "napoli", "torino",
    "palermo", "genova", "bologna", "firenze", "venezia"
};
const int NUM_CITIES = 10;

/*
 * ============================================================================
 * FUNCTION PROTOTYPES (Dichiarazioni di funzione utilizzate nel main.c)
 * ============================================================================
 */

int are_strings_equal_case_insensitive(const char *s1, const char *s2);
int is_city_valid(const char *city);
void format_city_name(char *city);
float get_temperature(void);
float get_humidity(void);
float get_wind(void);
float get_pressure(void);

#endif /* PROTOCOL_H_ */
