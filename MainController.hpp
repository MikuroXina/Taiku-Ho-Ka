#include <string>

#include "SoundManager.hpp"

struct Data;

class MainController {
private:
	Data *data;
	SoundManager sound{};

	void checkSDLError();
	int getRandomEnemyType();
public:
	MainController();
	~MainController();

	void init();
	void update();
	int run();

	// Move events
	void rotateLeft();
	void rotateRight();

	// Shoot events
	void shoot();

	// Other events
	void quit();
	void fail();
	void explosion(int, int);
	void updateDisplay();

	Data const* getData() const { return data; }
};
