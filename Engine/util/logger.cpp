#include "logger.h"
#include <colorconsole.hpp>

#ifdef MANAGER
namespace manager {
#else
namespace geo {
#endif
	void LOG(std::string msg) {
		std::cout << PREFIX << dye::aqua("log: ") << msg << std::endl;
	}

	void WARN(std::string msg) {
		std::cout << PREFIX << dye::yellow("warn: ") << msg << std::endl;
	}

	void ERR(std::string msg) {
		std::cout << PREFIX << dye::red("error: ") << msg << std::endl;
	}

	void DEBUG(std::string msg) {
		std::cout << PREFIX << dye::purple("debug: ") << msg << std::endl;
	}
}