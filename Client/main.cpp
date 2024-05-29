#include <core/game.h>
#include <util/api.h>
using namespace geo;

int main()
{	
	core::game game;
	game.init();

	util::api::upload_avatar(game.player.username, game.player.avatar);

	//game.start_local_game("X:\\Geo\\Assets\\ui\\geo_menu.xml");
	game.start_local_game("X:\\GeoGames\\Word Bomb\\online\\word_bomb.xml");
	//game.start_online_game("127.0.0.1", 1337);
}