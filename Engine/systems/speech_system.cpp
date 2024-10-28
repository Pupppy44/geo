#include "speech_system.h"
#include "../core/game.h"

namespace geo {
    namespace systems {
        speech_system::speech_system(core::game& g) : game(&g) {
            type("speech_system");

            // Initialization
            CoInitialize(NULL);
            CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&voice);

            define("speak", [&](sol::variadic_args args) -> sol::object {
                std::string words = args[0].as<std::string>();
                
				// Speak the words
				voice->Speak(util::string_to_wchar(words), SPF_ASYNC, NULL);
                
                return sol::nil;
            });
        }
    }
}