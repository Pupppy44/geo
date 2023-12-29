#include "../engine/server/server.h"

int main(int argc, char* argv[]) {
	if (argc < 4) {
		argv[1] = (char*)"g:\\games\\2.xml";
		argv[2] = (char*)"1337";
		argv[3] = (char*)"00000000-0000-0000-0000-000000000000";
	}
    
	geo::server::server server;
	server.id = argv[3];
	server.load(argv[1]);
	server.start(std::stoi(argv[2]));
}