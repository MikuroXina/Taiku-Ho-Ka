#include <OpenGL/gl.h>
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_ttf.h>

#include <random>
#include <vector>
#include <cmath>

struct Data {
public:
	bool quit = false;
	bool fail = false;
	double rotation = 0.0;
	std::vector<struct Enemy*> enemyPool{};
	std::vector<struct Bullet*> bulletPool{};
	std::vector<struct Explode*> explodePool{};
	unsigned int score = 0;

	SDL_Window *window{};
	SDL_GLContext context = NULL;
	SDL_Renderer *renderer = nullptr;
	SDL_Surface *playerTex = nullptr;
	SDL_Surface *enemy1Tex = nullptr;
	SDL_Surface *enemy2Tex = nullptr;
	SDL_Surface *enemy3Tex = nullptr;
	SDL_Surface *explodeTex[11] = {};

	TTF_Font *font;

	std::random_device rd;
	std::mt19937 mt;

	Data() : mt(rd()) { ; }
};

struct Ridgebody {
	double posX = 0, posY = 0;
	int collisionRadius = 1;
	bool isCollision(Ridgebody const& a) const {
		return (std::sqrt(posX * posX + posY * posY) - std::sqrt(a.posX * a.posX + a.posY * a.posY)) <= 1;
	}
};

struct Enemy {
public:
	Ridgebody body;
	bool moveToRight = false;
	enum struct Type {
		helicopter,
		bomber,
		ufo,
	};
	Type type = Type::helicopter;
	void update() {
		body.posX = (moveToRight ? 5 : -5);
	}
};

struct Bullet {
	Ridgebody body;
	virtual void update() = 0;
	virtual ~Bullet() = default;
};

struct MyBullet : Bullet {
public:
	double rotate = 0;
	double speed = 0;
	void update() override {
		Bullet::body.posX += (std::cos(rotate) * 180.0 / M_PI) * speed;
		Bullet::body.posY += (std::sin(rotate) * 180.0 / M_PI) * speed;
	}
	virtual ~MyBullet() = default;
};

struct EnemyBullet : Bullet {
	void update() override {
		Bullet::body.posY -= 2;
	}
	virtual ~EnemyBullet() = default;
};

struct Explode {
	int posX = 0, posY = 0;
	int subTexStep = 0;
};
