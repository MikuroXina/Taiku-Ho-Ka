#include "MainController.hpp"
#include "Data.hpp"

int main() {
	MainController mc{};
	mc.init(100);
	while (mc.run() == 0) {
		int highscore = mc.getData()->highscore;
		mc.init(highscore);
	}
	return 0;
}
