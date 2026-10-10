// Stub implementations for sound library functions

#include <QtGlobal>
#include <QDebug>
#include <cstdint>
#include <string_view>

#include "sndlib/hlsoundlib.h"
#include "mixer.h"

#define PRINT_STUB(x) \
{ \
  static bool printed_##__FUNCTION__ = false; \
  if( !printed_##__FUNCTION__ ) \
  { \
    printed_##__FUNCTION__ = true; \
    qDebug() << "Function \"" << __FUNCTION__ << "\" is a stub!"; \
  } \
}

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
int Sound_Load(std::string_view filename) {
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

// ==================== hlsSystem ====================

bool hlsSystem::IsActive(void) { PRINT_STUB(__FUNCTION__); return m_f_hls_system_init != 0; }

int hlsSystem::InitSoundLib(oeApplication *sos, sound_mixer mixer_type, sound_quality_type quality,
                            bool f_kill_sound_lib) {
  PRINT_STUB(__FUNCTION__);
  return 1;
}

void hlsSystem::SetLLSoundQuantity(int n_sounds) { PRINT_STUB(__FUNCTION__); }

int hlsSystem::GetLLSoundQuantity() { PRINT_STUB(__FUNCTION__); return 0; }

void hlsSystem::PauseSounds(bool f_all_sounds) { PRINT_STUB(__FUNCTION__); }

void hlsSystem::ResumeSounds() { PRINT_STUB(__FUNCTION__); }

bool hlsSystem::Emulate3dSound(int sound_obj_index) { PRINT_STUB(__FUNCTION__); return false; }

bool hlsSystem::ComputePlayInfo(int sound_obj_index, vector3 *virtual_pos, vector3 *virtual_vel,
                                float *adjusted_volume) {
  PRINT_STUB(__FUNCTION__);
  return false;
}

void hlsSystem::StopSound(int sound_obj_index, sound_kill_type f_immediately) { PRINT_STUB(__FUNCTION__); }

int hlsSystem::Play3dSound(int sound_index, pos_state *cur_pos, object *cur_obj, sound_priority priority,
                           float volume, int flags, float offset) {
  PRINT_STUB(__FUNCTION__);
  return 0;
}

int hlsSystem::Play3dSound(int sound_index, pos_state *cur_pos, float volume, int flags, float offset) {
  PRINT_STUB(__FUNCTION__);
  return 0;
}

int hlsSystem::Play3dSound(int sound_index, object *cur_obj, float volume, int flags, float offset) {
  PRINT_STUB(__FUNCTION__);
  return 0;
}

int hlsSystem::Play3dSound(int sound_index, sound_priority priority, pos_state *cur_pos, float volume, int flags,
                           float offset) {
  PRINT_STUB(__FUNCTION__);
  return 0;
}

int hlsSystem::Play3dSound(int sound_index, sound_priority priority, object *cur_obj, float volume, int flags,
                           float offset) {
  PRINT_STUB(__FUNCTION__);
  return 0;
}

int hlsSystem::PlayStream(int unique_handle, void *data, int size, int stream_format, float volume,
                          void *(*stream_callback)(void *user_data, int handle, int *size)) {
  PRINT_STUB(__FUNCTION__);
  return 0;
}

int hlsSystem::Play2dSound(int sound_index, sound_priority priority, float volume, float pan, uint16_t frequency) {
  PRINT_STUB(__FUNCTION__);
  return 0;
}

int hlsSystem::Update2dSound(int hlsound_uid, float volume, float pan) { PRINT_STUB(__FUNCTION__); return 0; }

void hlsSystem::StopSoundLooping(int hlsound_uid) { PRINT_STUB(__FUNCTION__); }

void hlsSystem::StopSoundImmediate(int hlsound_uid) { PRINT_STUB(__FUNCTION__); }

void hlsSystem::StopObjectSound(int objhandle) { PRINT_STUB(__FUNCTION__); }

void hlsSystem::SetVolumeObject(int objhandle, float volume) { PRINT_STUB(__FUNCTION__); }

void hlsSystem::SetMasterVolume(float volume) { PRINT_STUB(__FUNCTION__); }

float hlsSystem::GetMasterVolume() { PRINT_STUB(__FUNCTION__); return 1.0f; }

void hlsSystem::Add2dSoundQueued(int q_num, int sound_index, float volume, float pan, uint16_t frequency) {
  PRINT_STUB(__FUNCTION__);
}

void hlsSystem::KillQueue(int q_num) { PRINT_STUB(__FUNCTION__); }

void hlsSystem::KillAllQueues() { PRINT_STUB(__FUNCTION__); }

bool hlsSystem::SetSoundQuality(sound_quality_type quality) { PRINT_STUB(__FUNCTION__); return false; }

sound_quality_type hlsSystem::GetSoundQuality(void) { PRINT_STUB(__FUNCTION__); return sound_quality_type::normal; }

bool hlsSystem::SetSoundMixer(sound_mixer mixer_type) { PRINT_STUB(__FUNCTION__); return false; }

sound_mixer hlsSystem::GetSoundMixer(void) { PRINT_STUB(__FUNCTION__); return sound_mixer::none; }

bool hlsSystem::IsSoundPlaying(int hlsound_uid) { PRINT_STUB(__FUNCTION__); return false; }

void hlsSystem::SetMidiVolume() { PRINT_STUB(__FUNCTION__); }

void hlsSystem::GetMidiVolume() { PRINT_STUB(__FUNCTION__); }

void hlsSystem::PlayMidi() { PRINT_STUB(__FUNCTION__); }

void hlsSystem::StopMidi() { PRINT_STUB(__FUNCTION__); }

void hlsSystem::PauseMidi() { PRINT_STUB(__FUNCTION__); }

void hlsSystem::ResumeMidi() { PRINT_STUB(__FUNCTION__); }