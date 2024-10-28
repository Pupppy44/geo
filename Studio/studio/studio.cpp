#include "studio.h"

namespace studio {
	void studio::init() {		
		game.init();
		window.init();
	}

	void studio::start() {
		window.start();
	}
}