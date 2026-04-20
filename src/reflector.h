#ifndef REFLECTOR_H
#define REFLECTOR_H

#define ALPHABET_SIZE 26

typedef struct {
    char wiring[ALPHABET_SIZE];
    char name[8]; // e.g., "UKW-B"
} Reflector;

// Create reflector from wiring and name
Reflector createReflector(const char* wiring, const char* name);

// Reflect signal (bidirectional, fixed wiring)
int reflectSignal(const Reflector* reflector, int signal);

#endif // REFLECTOR_H

