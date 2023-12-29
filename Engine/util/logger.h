#pragma once
#include <string>

// Use the manager's namespace if we're compiling the manager
#ifdef MANAGER
namespace manager {
#else
namespace geo {
#endif

#ifdef SERVER
#define PREFIX "[geo-server] "
#elif MANAGER
#define PREFIX "[geo-manager] "
#else
#define PREFIX "[geo] "
#endif
	
	void LOG(std::string);

	void WARN(std::string);

	void ERR(std::string);

	void DEBUG(std::string);
}

