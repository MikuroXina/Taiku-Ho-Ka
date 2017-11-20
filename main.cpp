#include "MainController.hpp"

int main() {
	MainController mc{};
	mc.init();
	while (mc.run() == 0) {
		mc.init();
	}
	return 0;
}
