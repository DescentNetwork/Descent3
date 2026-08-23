// Stub implementations for game runtime functions

#include "game_stub.h"
#include "network_stub.h"
#include "osiris_stub.h"

// Game state variables
int Game_mode = 0;
int Frame_count = 0;
float Frametime = 0.033f; // ~30fps default
int Object_next_signature = 1;
int Num_rooms = 0;
int Current_level = -1;
int Rendering_enabled = 0;
int Key_down[256] = {0};

// Network stubs
int Network_game_active = 0;

int Network_Init(void) { return 0; }
void Network_Close(void) {}
int Network_SendMessage(const char* msg, int len) { return 0; }
int Network_ReceiveMessage(char* buf, int max_len) { return 0; }

// OSIRIS stubs
int OSIRIS_Init(void) { return 1; }
void OSIRIS_Close(void) {}
int OSIRIS_ExecuteScript(tScriptHandle* handle, int event_type) { return 0; }

// Additional game function stubs
void Game_Init(void) {}
void Game_Close(void) {}
void Game_DoFrame(void) {}
int LoadLevel(int level_num) { return 0; }
void FreeLevel(void) {}

// Physics stubs
void Physics_Init(void) {}
void Physics_SimulateObject(int objnum, float time) {}

// AI stubs
void AI_Init(void) {}
void AI_Frame(void) {}

// Multiplayer stubs  
int MultiInit(void) { return 0; }
void MultiClose(void) {}
