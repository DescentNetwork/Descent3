#ifndef SOUNDLOAD_H_
#define SOUNDLOAD_H_

int AllocSound();
void FreeSound(int n);
int GetNextSound(int n);
int GetPrevSound(int n);
std::optional<uint32_t> FindSoundName(const std::string &name);
void RemapSounds();
int LoadSoundFile(const char *filename, float import_volume, bool f_get_data = false);

#endif
