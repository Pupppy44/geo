#include <core/game.h>
#include <util/api.h>
using namespace geo;

#ifdef _DEBUG
int main(int argc, char* argv[]) {
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#endif

	std::string ap = util::get_user_avatar();
	DEBUG(ap);
	
	core::game game;
	game.init();

	game.start_local_game("X:\\GeoGames\\Sandbox\\textbox_test.xml", false);

	if (argc == 2) {
#ifdef _DEBUG
		std::string path = argv[1];
#else
		std::string path = util::wchar_to_string(argv[1]);
#endif
		game.start_local_game(path);
	}
	else {
#ifdef _DEBUG
		//game.start_local_game("X:\\Geo\\Assets\\ui\\geo_menu.xml", false);
		//game.start_local_game("X:\\GeoGames\\Word Bomb\\build\\word_bomb.xml");
		//game.start_local_game("X:\\GeoGames\\Ghostland\\build\\ghostland.xml");
		game.start_online_game("127.0.0.1", 1337);
#else
		game.start_local_game(util::get_ui(UI_MENU), false);
#endif
	}
}