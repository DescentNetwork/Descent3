#ifndef NETWORK_STUB_H
#define NETWORK_STUB_H

// Stub declarations for networking (not needed in standalone editor)

#ifdef __cplusplus
extern "C" {
#endif

// Network mode constants
#define NETWORK_MODE_NONE 0
#define NETWORK_MODE_LOCAL 1

// Stub network state
extern int Network_game_active;

// Stub functions
int Network_Init(void);
void Network_Close(void);
int Network_SendMessage(const char* msg, int len);
int Network_ReceiveMessage(char* buf, int max_len);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_STUB_H
