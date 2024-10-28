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
			LONGLONG duration;
			seek->GetDuration(&duration);
			float durationInSeconds = static_cast<float>(duration) / 10000000.0;
			set_property(property(property_type::NUMBER, "duration", std::to_string(durationInSeconds)));

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

			// If the audio is from Base64, load it
			if (path.starts_with("base64:")) {
				DEBUG("loading audio from base64 (audio id=" + id() + ")");

				std::string file = util::base64_to_file(path);

				tree::property path_prop(tree::property_type::STRING, "path", file);
				set_property(path_prop);

				path = file;
			}

			// Set current path
			current_path = path;

			// Render audio file
			graph->RenderFile(util::string_to_wchar(path.data()), NULL);
		};
		
		void audio::play() {
			std::string path = get_property<std::string>("path");
			bool playing = get_property<bool>("playing");

			// Reset audio if already playing
			if (playing) return;

			// Run the audio in a separate thread so it doesn't block the main thread
			std::jthread([&, path]() {
				// Set the playing flag to true
				set_property(property(property_type::BOOLEAN, "playing", "true"));

				// Stop the graph first to reset its state
				control->Stop();

				// Seek to the beginning of the audio file
				LONGLONG start = 0;
				seek->SetPositions(&start, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);

				// Restart the audio
				control->Run();

				// Wait for the media to complete
				long evCode;
				do {
					evt->WaitForCompletion(INFINITE, &evCode);
					if (evCode == EC_COMPLETE) {
						if (get_property<bool>("loop")) {
							// Stop the graph first to reset its state
							control->Stop();

							// Seek to the beginning of the audio file
							seek->SetPositions(&start, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);

							// Restart the audio
							control->Run();
						}
						else {
							set_property(property(property_type::BOOLEAN, "playing", "false"));
							break;
						}
					}
				} while (true);
			}).detach();
		}
	}
}