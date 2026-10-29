#ifndef OSIRIS_STUB_H
#define OSIRIS_STUB_H

// Stub declarations for Osiris scripting system

#ifdef __cplusplus
extern "C" {
#endif

// Basic types
typedef int tOSIRISMODULEID;
typedef void* tOSIRISCONTEXT;

// Module IDs
#define OSIRIS_MODULE_MAIN 0
#define OSIRIS_MODULE_TRIGGER 1

// Script handle
typedef struct {
    int script_id;
    int active;
} tScriptHandle;

// Stub function declarations
int OSIRIS_Init(void);
void OSIRIS_Close(void);
int OSIRIS_ExecuteScript(tScriptHandle* handle, int event_type);

#ifdef __cplusplus
}
#endif

#endif // OSIRIS_STUB_H
