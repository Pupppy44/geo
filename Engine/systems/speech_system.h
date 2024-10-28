#pragma once
#include "../tree/object.h"
#include <sapi.h>

namespace geo {
	namespace core {
		class game;
	}

	namespace systems {
		// System for text-to-speech
		class speech_system : public tree::object {
		public:
			speech_system(core::game&);

			void init() {};
			void render() {};
		private:
			core::game* game;
			ISpVoice* voice;
		};
	}
}
