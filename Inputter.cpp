#include <iostream>

#include "Inputter.hpp"
#include "Data.hpp"
#include "MainController.hpp"

void Inputter::update() {
	while (SDL_PollEvent(&event) != 0) {
		if (controller->getData()->quit) return;
		if (event.type == SDL_QUIT) {
			controller->quit();
		}
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				controller->quit();
				break;
			case SDLK_LEFT:
				controller->rotateLeft();
				break;
			case SDLK_RIGHT:
				controller->rotateRight();
				break;
			case SDLK_SPACE:
				if (!(controller->getData()->isPressedSpace)) {
					controller->shoot();
				}

				break;
			}
		}
		if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				controller->releasedSpace();
				break;
			}
		}
		SDL_Delay(1);
	}
}
