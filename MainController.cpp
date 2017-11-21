#include <iostream>
#include <random>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "MainController.hpp"
#include "Data.hpp"
#include "Inputter.hpp"

void MainController::checkSDLError() {
	const char *error = SDL_GetError();
	if (*error != '\0') {
		std::cout<<"SDL Error:"<<error<<std::endl;
		SDL_ClearError();
	}
}

int MainController::getRandomEnemyType() {
	static std::uniform_int_distribution<int> dice(0, 2);
	return dice(data->mt);
}

MainController::MainController() {
	data = new Data;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout<<std::string(SDL_GetError())<<std::endl;
	}

	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	constexpr Uint32 width = 800;
	constexpr Uint32 height = 500;
	data->window = SDL_CreateWindow("たいくーほーか", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!(data->window)) {
		std::cout<<"Unable to create a window!"<<std::endl;
	}
	checkSDLError();

	data->context = SDL_GL_CreateContext(data->window);
	checkSDLError();

	SDL_GL_SetSwapInterval(1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glViewport(0, 0, width, height);

	// Setup font
	data->font = TTF_OpenFont("PixelMplus12-Regular.ttf", 24);
	if (data->font == nullptr) {
		std::cout << "Unable to open the font: " << TTF_GetError() << std::endl;
		throw -1;
	}

	data->renderer = SDL_CreateRenderer(data->window, -1, SDL_RENDERER_PRESENTVSYNC);

	// Setup texture
	data->playerTex = IMG_Load("Player.png");
	data->enemy1Tex = IMG_Load("Enemy1.png");
	data->enemy2Tex = IMG_Load("Enemy2.png");
	data->enemy3Tex = IMG_Load("Enemy3.png");
	for (int i = 0; i < 11; i+=1) {
		data->explodeTex[i] = IMG_Load(("explode" + std::to_string(i) + ".png").c_str());
	}

	// Setup sounds
	sound.registerSound("Shoot.wav"); //   Id:0
	sound.registerSound("Explode.wav"); // Id:1
	sound.registerSound("Scored.wav"); //  Id:2
	sound.registerSound("Fail.wav"); //    Id:3
}

MainController::~MainController() {
	SDL_FreeSurface(data->playerTex);
	SDL_FreeSurface(data->enemy1Tex);
	SDL_FreeSurface(data->enemy2Tex);
	SDL_FreeSurface(data->enemy3Tex);
	for (int i = 0; i < 11; i+=1) {
		SDL_FreeSurface(data->explodeTex[i]);
	}
	SDL_DestroyRenderer(data->renderer);

	SDL_GL_DeleteContext(data->context);
	SDL_DestroyWindow(data->window);

	TTF_CloseFont(data->font);
	data->font = nullptr;

	TTF_Quit();
	SDL_Quit();

	for (auto const& obj : data->enemyPool) {
		delete obj;
	}
	for (auto const& obj : data->bulletPool) {
		delete obj;
	}
	for (auto const& obj : data->explodePool) {
		delete obj;
	}

	delete data;
}

void MainController::init() {
	for (auto const& obj : data->enemyPool) {
		delete obj;
	}
	for (auto const& obj : data->bulletPool) {
		delete obj;
	}
	for (auto const& obj : data->explodePool) {
		delete obj;
	}

	data->quit = false;
	data->fail = false;
	data->rotation = 0.0;
	data->enemyPool = std::vector<struct Enemy*>();
	data->bulletPool = std::vector<struct Bullet*>();
	data->explodePool = std::vector<struct Explode*>();
	data->score = 0;

	// Delete events
	SDL_PumpEvents();
	SDL_FlushEvent(SDL_KEYDOWN);
	SDL_FlushEvent(SDL_KEYUP);
}

void MainController::update() {
	for (auto const& bullet : data->bulletPool) {
		bullet->update();
		if ((1 > bullet->body.posX || bullet->body.posX > 801) ||
	(1 > bullet->body.posY)) {
			explosion(bullet->body);
			delete bullet;
			data->bulletPool.erase(std::remove(data->bulletPool.begin(), data->bulletPool.end(), bullet), data->bulletPool.end());
		}
	}

	for (auto& enemy: data->enemyPool) {
		enemy->update();
		for (auto const& bullet : data->bulletPool) {
			if (enemy->body.isCollision(bullet->body)) {
				explosion(enemy->body);
				delete enemy;
				data->enemyPool.erase(std::remove(data->enemyPool.begin(), data->enemyPool.end(), enemy), data->enemyPool.end());
			}
		}
	}

	for (auto const& explode : data->explodePool) {
		explode->subTexStep += 1;
		if (explode->subTexStep >= 11) {
			delete explode;
			data->explodePool.erase(std::remove(data->explodePool.begin(), data->explodePool.end(), explode), data->explodePool.end());
		}
	}
}

int MainController::run() {
	Inputter inputter{};
	inputter.controller = this;
	updateDisplay();
	do {
		inputter.update();
		update();
		updateDisplay();
	} while (!(data->quit));

	return data->fail ? 0 : 1;
}

void MainController::quit() {
	std::cout<<"Quitting..."<<std::endl;
	data->quit = true;
}

void MainController::fail() {
	// Render sprite
	SDL_Color red = {0xff, 0x00, 0x00};
	SDL_Surface *spriteImage1 = TTF_RenderText_Solid(data->font, "FAIL!", red);
	SDL_Texture *spriteTex1 = SDL_CreateTextureFromSurface(data->renderer, spriteImage1);

	SDL_Rect spriteViewRect1;
	spriteViewRect1.x = 400;
	spriteViewRect1.y = 200;
	spriteViewRect1.w = 5000;
	spriteViewRect1.h = 5000;

	TTF_SizeText(data->font, "FAIL!", &(spriteViewRect1.w), &(spriteViewRect1.h));

	SDL_RenderCopy(data->renderer, spriteTex1, NULL, &spriteViewRect1);

	SDL_DestroyTexture(spriteTex1);
	SDL_FreeSurface(spriteImage1);

	SDL_GL_SwapWindow(data->window);

	sound.playSound(3);

	SDL_Delay(1500);
	data->fail = true;
	quit();
}

void MainController::updateDisplay() {
	SDL_SetRenderDrawColor(data->renderer, 0, 0, 0, 255);
	SDL_RenderClear(data->renderer);

	// Draw player
	{
		SDL_Rect playerRect;
		playerRect.x = 300;
		playerRect.y = 400;
		playerRect.w = 200;
		playerRect.h = 200;
		SDL_Point playerCenter;
		playerCenter.x = 100;
		playerCenter.y = 120;
		SDL_Texture *playerTexBuf = SDL_CreateTextureFromSurface(data->renderer, data->playerTex);

		SDL_RenderCopyEx(data->renderer, playerTexBuf, NULL, &playerRect, -data->rotation, &playerCenter, SDL_FLIP_NONE);

		SDL_DestroyTexture(playerTexBuf);
	}

	// Draw enemies
	{
		//SDL_Texture *enemy1TexBuf = SDL_CreateTextureFromSurface(data->renderer, data->enemy1Tex);
		//SDL_Texture *enemy2TexBuf = SDL_CreateTextureFromSurface(data->renderer, data->enemy2Tex);
		//SDL_Texture *enemy3TexBuf = SDL_CreateTextureFromSurface(data->renderer, data->enemy3Tex);
		for (auto const& enemy : data->enemyPool) {
			switch (enemy->type) {
			case Enemy::Type::helicopter:

				break;
			case Enemy::Type::bomber:

				break;
			case Enemy::Type::ufo:
				break;
			}
		}
	}

	// Draw bullets
	{
		SDL_SetRenderDrawColor(data->renderer, 238, 244, 66, 255);
		for (auto const& bullet : data->bulletPool) {
			SDL_Rect bulletRect;
			bulletRect.x = bullet->body.posX;
			bulletRect.y = bullet->body.posY;
			bulletRect.w = 8;
			bulletRect.h = 8;
			SDL_RenderDrawRect(data->renderer, &bulletRect);
		}
	}

	// Draw explosion
	{
		for (auto const& explode : data->explodePool) {
			SDL_Rect explodeRect;
			explodeRect.x = explode->posX - 37;
			explodeRect.y = explode->posY - 37;
			explodeRect.w = 75;
			explodeRect.h = 75;
			SDL_Texture *explodeTexBuf = SDL_CreateTextureFromSurface(data->renderer, data->explodeTex[explode->subTexStep]);

			SDL_RenderCopy(data->renderer, explodeTexBuf, NULL, &explodeRect);

			SDL_DestroyTexture(explodeTexBuf);
		}
	}

	// Draw socre
	SDL_Color green = {0x00, 0xff, 0x00};
	char const *text = ("Score: " + std::to_string(data->score)).c_str();
	SDL_Surface *scoreImage = TTF_RenderText_Solid(data->font, text, green);
	SDL_Texture *scoreTex = SDL_CreateTextureFromSurface(data->renderer, scoreImage);

	SDL_Rect scoreViewRect;
	scoreViewRect.x = 0;
	scoreViewRect.y = 0;
	scoreViewRect.w = 100;
	scoreViewRect.h = 100;

	TTF_SizeText(data->font, text, &(scoreViewRect.w), &(scoreViewRect.h));

	SDL_RenderCopy(data->renderer, scoreTex, NULL, &scoreViewRect);

	SDL_DestroyTexture(scoreTex);
	SDL_FreeSurface(scoreImage);

	SDL_GL_SwapWindow(data->window);

	SDL_Delay(1);
}

void MainController::rotateLeft() {
	if (data->rotation <= 50) {
		data->rotation += 4;
	}
}

void MainController::rotateRight() {
	if (data->rotation >= -50) {
		data->rotation -= 4;
	}
}

void MainController::shoot() {
	auto shot = new MyBullet;
	shot->body.posX = 400;
	shot->body.posY = 520;
	shot->rotate = -(90 + data->rotation) * M_PI / 180.0;
	shot->speed = 0.1;
	data->bulletPool.push_back(shot);
}

void MainController::explosion(struct Ridgebody const& body) {
	auto explode = new Explode;
	explode->posX = static_cast<int>(body.posX);
	explode->posY = static_cast<int>(body.posY);
	data->explodePool.push_back(explode);
}
