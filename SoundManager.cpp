#include <iostream>

#include "SoundManager.hpp"

SoundManager::SoundManager() {
	Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
}

SoundManager::~SoundManager() {
	for (int i=0; i<4; i+=1) {
		Mix_FreeChunk(chunks[i]);
	}

	Mix_CloseAudio();
	Mix_Quit();
}

int SoundManager::registerSound(char const* file) {
	if (loadedCount >= 4) { return -1; }
	chunks[loadedCount] = Mix_LoadWAV(file);
	if (chunks[loadedCount] == nullptr) {
		std::cout << Mix_GetError() << std::endl;
	}
	int id = loadedCount;
	loadedCount += 1;
	return id;
}

void SoundManager::playSound(int id) {
	if (Mix_PlayChannel(-1, chunks[id], 0) == -1) {
		std::cout << "Error to play sound, ID: " << id << std::endl;
	}
}
