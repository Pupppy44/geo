#pragma once
#include <iostream>
#include <chrono>
#include <functional>
#define _USE_MATH_DEFINES // For PI
#include <math.h>

namespace geo {
	namespace util {
        class tween {
        public:
            enum class easing {
                linear,
                sine_ease_in,
                sine_ease_out,
                sine_ease_in_out
            };

            tween(float initial_value, float target_value, int time_ms, easing easing_mode = easing::sine_ease_in_out)
                : initial_value(initial_value), target_value(target_value), time_ms(time_ms), easing_mode(easing_mode) {
                start_time = std::chrono::high_resolution_clock::now();
                current_value = initial_value;
            }

            ~tween() {};

            float step() {
                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float, std::milli> elapsed = now - start_time;
                float t = elapsed.count() / time_ms;
                this->elapsed = elapsed.count();
                
                if (t >= 1.0f) {
                    current_value = target_value;
                    return current_value;
                }

                switch (easing_mode) {
                case easing::sine_ease_in:
                    current_value = initial_value + (target_value - initial_value) * sine_ease_in(t);
                    break;
                case easing::sine_ease_out:
                    current_value = initial_value + (target_value - initial_value) * sine_ease_out(t);
                    break;
                case easing::sine_ease_in_out:
                    current_value = initial_value + (target_value - initial_value) * sine_ease_in_out(t);
                    break;
                case easing::linear:
                default:
                    current_value = initial_value + (target_value - initial_value) * t;
                    break;
                }

                return current_value;
            }
        public:
            float initial_value;
            float target_value;
            int time_ms;
            float elapsed;
            std::chrono::high_resolution_clock::time_point start_time;
            float current_value = 0.f;
            easing easing_mode;
            std::function<void(float, float)> callback = nullptr;
        private:
            float sine_ease_in(float t) {
                return -cos(t * M_PI_2) + 1.0f;
            }

            float sine_ease_out(float t) {
                return sin(t * M_PI_2);
            }

            float sine_ease_in_out(float t) {
                return -0.5f * (cos(M_PI * t) - 1.0f);
            }
        };
	}
}