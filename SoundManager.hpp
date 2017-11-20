#include <SDL_mixer.h>

class SoundManager {
private:
	Mix_Chunk *chunks[4]{};
	int loadedCount=0;

public:
	SoundManager();
	~SoundManager();

	int registerSound(char const*);
	void playSound(int);
};
