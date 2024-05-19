#include "audio.h"

namespace geo {
	namespace objects {
		audio::audio() {
			type("audio");
			
			define("play", [&](sol::variadic_args args) -> sol::object {
				play();
				return sol::nil;
			});

			define("stop", [&](sol::variadic_args args) -> sol::object {
				set_property(property(property_type::BOOLEAN, "playing", "false"));
				control->Stop();
				return sol::nil;
			});

			define("pause", [&](sol::variadic_args args) -> sol::object {
				set_property(property(property_type::BOOLEAN, "playing", "false"));
				control->Pause();
				return sol::nil;
			});

			define("resume", [&](sol::variadic_args args) -> sol::object {
				set_property(property(property_type::BOOLEAN, "playing", "true"));
				control->Run();
				return sol::nil;
			});
		};
		
		void audio::init() {	
			if (FAILED(::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
				IID_IGraphBuilder, (void**)&graph))) {
				return;
			}

			graph->QueryInterface(IID_IMediaControl, (void**)&control);
			graph->QueryInterface(IID_IBasicAudio, (void**)&aud);
			control->QueryInterface(IID_IMediaSeeking, (void**)&seek);
			graph->QueryInterface(IID_IMediaEventEx, reinterpret_cast<void**>(&evt));

			// Load audio file via update
			update();

			// Get and set duration of song
			// TODO: Not working
			LONGLONG duration;
			seek->GetDuration(&duration);
			duration /= 1000000000;
			set_property(property(property_type::NUMBER, "duration", std::to_string(duration)));

			// Autoplay if enabled
			if (get_property<bool>("autoplay")) {
				play();
			}
		};

		void audio::render() {
			// Check if audio path updated
			if (get_property<std::string>("path") != current_path) {
				update();
				DEBUG("audio updated (id=" + id() + ")");
			}
		};

		void audio::update() {
			std::string path = get_property<std::string>("path");

			if (path == "") return;

			// If the audio is a remote resource, download it and store it in a temp directory
			if (path.starts_with("http")) {
				DEBUG("downloading audio from " + get_property<std::string>("path") + " (audio id=" + id() + ")");

				std::string file = util::download_file(get_property<std::string>("path"));

				tree::property path_prop(tree::property_type::STRING, "path", file);
				set_property(path_prop);

				path = file;
			}

			// Set current path
			current_path = path;
		};
		
		void audio::play() {
			std::string path = get_property<std::string>("path");
			bool playing = get_property<bool>("playing");

			// Reset audio if already playing
			if (playing) return;

			set_property(property(property_type::BOOLEAN, "playing", "true"));

			// Run the audio in a separate thread so it doesn't block the main thread
			std::jthread([&, path]() {
				// Render and run audio file
				graph->RenderFile(util::string_to_wchar(path.data()), NULL);
				control->Run();

				// Wait for audio to finish
				long evCode;
				evt->WaitForCompletion(INFINITE, &evCode);

				// Reset audio
				control->Stop();

				set_property(property(property_type::BOOLEAN, "playing", "false"));

				// Loop if loop is true
				if (evCode == EC_COMPLETE && get_property<bool>("loop")) {
					play();
				}
			}).detach();
		}
	}
}