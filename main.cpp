#include "MainController.hpp"
#include "Data.hpp"

int main() {
	MainController mc{};
	unsigned int highscore = 100;
	mc.init(highscore);
	while (mc.run() == 0) {
		if (highscore < mc.getData()->score) {
			highscore = mc.getData()->score;
		}
		mc.init(highscore);
	}
	return 0;
}
