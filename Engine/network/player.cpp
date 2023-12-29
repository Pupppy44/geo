#include "player.h"
#include "../server/server.h"

namespace geo {
	namespace network {
		player::player(pascal::peer p, server::server& s) : peer(p), server(&s) {
			type("player");

			define("kick", [&](sol::variadic_args args) -> sol::object {
				auto player = server->players.get_player(peer.guid);
				std::string reason = args[0].as<std::string>();

				// Remove the player from the game
				server->players.remove_player(player, reason);

				return sol::nil;
			});
		};

		void player::init() {};

		void player::render() {};
	}
}