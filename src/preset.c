#include "preset.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define PRESETS_DIR "presets/"

static void ensure_presets_dir() {
    struct stat st = {0};
    if (stat(PRESETS_DIR, &st) == -1) {
        mkdir(PRESETS_DIR, 0700);
    }
}

bool preset_save(const EnigmaMachine* machine, const char* presetName) {
    ensure_presets_dir();

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", presetName);

    // Rotors
    cJSON *rotors = cJSON_CreateArray();
    for (int i = 0; i < NUM_ROTORS; i++) {
        cJSON_AddItemToArray(rotors, cJSON_CreateString(machine->rotorNames[i]));
    }
    cJSON_AddItemToObject(root, "rotors", rotors);

    // Ring Settings
    cJSON *rings = cJSON_CreateIntArray(machine->ringSettings, NUM_ROTORS);
    cJSON_AddItemToObject(root, "ring_settings", rings);

    // Start Positions
    cJSON *starts = cJSON_CreateIntArray(machine->startPositions, NUM_ROTORS);
    cJSON_AddItemToObject(root, "start_positions", starts);

    // Reflector
    cJSON_AddStringToObject(root, "reflector", machine->reflector.name);

    // Plugboard
    cJSON *plugs = cJSON_CreateArray();
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (machine->plugboard.map[i] > i) { // only add each pair once
            cJSON *pair = cJSON_CreateArray();
            char from[2] = {'A' + i, '\0'};
            char to[2] = {'A' + machine->plugboard.map[i], '\0'};
            cJSON_AddItemToArray(pair, cJSON_CreateString(from));
            cJSON_AddItemToArray(pair, cJSON_CreateString(to));
            cJSON_AddItemToArray(plugs, pair);
        }
    }
    cJSON_AddItemToObject(root, "plugboard", plugs);

    char *jsonStr = cJSON_Print(root);
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "%s%s.enigma", PRESETS_DIR, presetName);

    FILE *f = fopen(filePath, "w");
    if (f) {
        fputs(jsonStr, f);
        fclose(f);
    }

    cJSON_free(jsonStr);
    cJSON_Delete(root);

    return f != NULL;
}

bool preset_load(EnigmaMachine* machine, const char* presetName) {
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "%s%s.enigma", PRESETS_DIR, presetName);

    FILE *f = fopen(filePath, "r");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *data = malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(data);
    free(data);

    if (!root) return false;

    // Reset machine to default first
    *machine = createEnigmaMachine();

    // Rotors
    cJSON *rotors = cJSON_GetObjectItem(root, "rotors");
    if (cJSON_IsArray(rotors)) {
        const char *names[NUM_ROTORS];
        char nameBufs[NUM_ROTORS][8];
        for (int i = 0; i < NUM_ROTORS && i < cJSON_GetArraySize(rotors); i++) {
            strncpy(nameBufs[i], cJSON_GetArrayItem(rotors, i)->valuestring, 7);
            nameBufs[i][7] = '\0';
            names[i] = nameBufs[i];
        }
        setRotorOrder(machine, names);
    }

    // Rings
    cJSON *rings = cJSON_GetObjectItem(root, "ring_settings");
    if (cJSON_IsArray(rings)) {
        int ringSettings[NUM_ROTORS];
        for (int i = 0; i < NUM_ROTORS && i < cJSON_GetArraySize(rings); i++) {
            ringSettings[i] = cJSON_GetArrayItem(rings, i)->valueint;
        }
        setRingSettings(machine, ringSettings);
    }

    // Starts
    cJSON *starts = cJSON_GetObjectItem(root, "start_positions");
    if (cJSON_IsArray(starts)) {
        int startPositions[NUM_ROTORS];
        for (int i = 0; i < NUM_ROTORS && i < cJSON_GetArraySize(starts); i++) {
            startPositions[i] = cJSON_GetArrayItem(starts, i)->valueint;
        }
        setStartPositions(machine, startPositions);
    }

    // Reflector
    cJSON *refl = cJSON_GetObjectItem(root, "reflector");
    if (cJSON_IsString(refl)) {
        setReflector(machine, refl->valuestring);
    }

    // Plugboard
    cJSON *plugs = cJSON_GetObjectItem(root, "plugboard");
    if (cJSON_IsArray(plugs)) {
        for (int i = 0; i < cJSON_GetArraySize(plugs); i++) {
            cJSON *pair = cJSON_GetArrayItem(plugs, i);
            if (cJSON_IsArray(pair) && cJSON_GetArraySize(pair) == 2) {
                char from = cJSON_GetArrayItem(pair, 0)->valuestring[0];
                char to = cJSON_GetArrayItem(pair, 1)->valuestring[0];
                addPlugPairToMachine(machine, from, to);
            }
        }
    }

    cJSON_Delete(root);
    return true;
}

int preset_list(char names[][64], int maxPresets) {
    ensure_presets_dir();
    DIR *d = opendir(PRESETS_DIR);
    if (!d) return 0;

    int count = 0;
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL && count < maxPresets) {
        char *ext = strrchr(dir->d_name, '.');
        if (ext && strcmp(ext, ".enigma") == 0) {
            size_t len = ext - dir->d_name;
            if (len >= 64) len = 63;
            strncpy(names[count], dir->d_name, len);
            names[count][len] = '\0';
            count++;
        }
    }
    closedir(d);
    return count;
}

bool preset_delete(const char *presetName) {
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "%s%s.enigma", PRESETS_DIR, presetName);
    return remove(filePath) == 0;
}
