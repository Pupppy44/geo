#pragma once
#include "../tree/object.h"
#include <iostream>
#include <thread>
#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "winmm")
using namespace geo::tree;

namespace geo {
	namespace objects {
		class audio : public object {
		public:
			audio();
			
			void init();
			void render();
			
			// Update audio path if it changes
			void update();

			// Play audio
			void play();
		private:
			std::string current_path;
			
			IGraphBuilder* graph = NULL;
			IMediaControl* control = NULL;
			IMediaSeeking* seek = NULL;
			IBasicAudio* aud = NULL;
			IMediaEventEx* evt = NULL;
		};
	}
}

