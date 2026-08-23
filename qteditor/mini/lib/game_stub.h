#ifndef GAME_STUB_H
#define GAME_STUB_H

// Stub declarations for game runtime functions not needed in editor

#ifdef __cplusplus
extern "C" {
#endif

// Game state stubs
extern int Game_mode;
extern int Frame_count;
extern float Frametime;

// Object system stubs  
extern int Object_next_signature;

// Level/room stubs
extern int Num_rooms;
extern int Current_level;

// Rendering stubs
extern int Rendering_enabled;

// Input stubs
extern int Key_down[256];

// Simple stub macros
#define MAX_PLAYERS 1
#define MAX_OBJECTS 1000
#define MAX_ROOMS 500

#ifdef __cplusplus
}
#endif

#endif // GAME_STUB_H
