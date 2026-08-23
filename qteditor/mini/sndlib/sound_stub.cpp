// Stub implementations for sound library functions

#include "hlsoundlib.h"
#include "mixer.h"

// Initialize sound system (stub)
int Sound_Init(void) {
    return 1; // Success but no actual sound
}

// Close sound system (stub)
void Sound_Close(void) {
    // Nothing to do in stub
}

// Play a sound (stub)
int Sound_Play(int handle, int loop) {
    return -1; // No sound played
}

// Stop a sound (stub)
void Sound_Stop(int handle) {
    // Nothing to stop
}

// Set volume (stub)
void Sound_SetVolume(int handle, float vol) {
    // Volume setting ignored in stub
}

// Load a sound file (stub)
int Sound_Load(const char* filename) {
    return -1; // No sound loaded
}

// Free a sound (stub)
void Sound_Free(int handle) {
    // Nothing to free
}

// Mixer stubs
int Mixer_Init(void) {
    return 1;
}

void Mixer_Close(void) {
}

void Mixer_Update(void) {
}
