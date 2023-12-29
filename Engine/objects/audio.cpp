#include "audio.h"
#include <iostream>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

namespace geo {
	namespace objects {
		audio::audio() {
			type("audio");
			
			define("play", [&](sol::variadic_args args) -> sol::object {
				PlaySoundA(get_property<std::string>("path").data(), NULL, SND_FILENAME | SND_ASYNC);

				return sol::nil;
			});
		};
		
		void audio::init() {};

		void audio::render() {};
	}
}