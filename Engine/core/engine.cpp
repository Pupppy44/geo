#include "game.h"
#include "engine.h"
#include "window.h"
#include "../server/server.h"
#include "../util/tween.h"

namespace geo {
	namespace core {
		void engine::init() {
			// Initiate all resources
			init_independent_resources();
			init_resources();

			queue_stop = false;
			rendering = true;
		}

		void engine::clear() {
			// Queue a stop. The engine will stop rendering after the current frame
			queue_stop = true;

			// Clear the game tree
			tree.clear_objects();
		}

		void engine::render() {
			if (!stop()) {
				d2d_context->BeginDraw();
				d2d_context->Clear(game->window.get_background());

				// Render objects in the game tree
				for (auto& obj : tree.get_objects()) {
					obj->context = d2d_context;
					obj->render();
				}

				// Step all tweens
				auto& tweens = game->runner.callbacks.tweens;
				for (auto it = tweens.begin(); it != tweens.end();) {
					auto& tw = *it;
					
					float value = tw.step();
					tw.callback(value, tw.elapsed);

					// Remove tween callback from list if it's done
					if (tw.current_value == tw.target_value) {
						it = tweens.erase(it);
					}
					else {
						++it;
					}
				}

				DXGI_PRESENT_PARAMETERS parameters = { 0 };
				parameters.DirtyRectsCount = 0;
				parameters.pDirtyRects = nullptr;
				parameters.pScrollRect = nullptr;
				parameters.pScrollOffset = nullptr;

				dxgi_swap_chain->Present1(1, 0, &parameters);

				d2d_context->EndDraw();
			}
		}

		ID2D1Bitmap1* engine::get_screen() {
			return d2d_back_buffer;
		}

		ID2D1DeviceContext5* engine::get_context() {
			return d2d_context;
		}

		void engine::init_independent_resources() {
			D2D1_FACTORY_OPTIONS options;
			ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

			D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory7), &options, reinterpret_cast<void**>(&d2d_factory));
		}

		void engine::init_resources() {
			RECT client_rect;
			GetClientRect(game->window.get_hwnd(), &client_rect);

			D2D1_SIZE_U size = D2D1::SizeU(
				client_rect.right - client_rect.left,
				client_rect.bottom - client_rect.top
			);

			D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_12_2,
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};

			UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

			// Create the devices and contexts
			ID3D11Device* device;
			ID3D11DeviceContext* context;
			D3D_FEATURE_LEVEL returnedFeatureLevel;
			D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &returnedFeatureLevel, &context);

			// Query the 11.1 interfaces
			device->QueryInterface(__uuidof(ID3D11Device1), (void**)&d3d_device);
			context->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&d3d_context);

			// Query the DXGI and D2D interfaces
			IDXGIDevice* dxgiDevice;
			d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

			if (dxgiDevice) {
				d2d_factory->CreateDevice(dxgiDevice, &d2d_device);
			}

			d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d_context);

			IDXGIAdapter* dxgiAdapter;
			dxgiDevice->GetAdapter(&dxgiAdapter);

			IDXGIFactory2* dxgiFactory;
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

			// Create the swap chain
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

			swapChainDesc.Width = 0;
			swapChainDesc.Height = 0;
			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;
			swapChainDesc.Scaling = DXGI_SCALING_NONE;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapChainDesc.Flags = 0;

			if (d3d_device) {
				dxgiFactory->CreateSwapChainForHwnd(d3d_device, game->window.get_hwnd(), &swapChainDesc, nullptr, nullptr, &dxgi_swap_chain);
			}

			// Initiate and create the back buffer
			IDXGISurface* dxgiBackBuffer;
			dxgi_swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

			D2D1_BITMAP_PROPERTIES1 bitmapProperties =
				D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
					D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96, 96);

			if (dxgiBackBuffer) {
				d2d_context->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bitmapProperties, &d2d_back_buffer);
			}

			d2d_context->SetTarget(d2d_back_buffer);
		}

		bool engine::stop() {
			if (queue_stop) {
				// Reset queue and stop rendering
				queue_stop = false;
				rendering = false;
				
				return true;
			}
			// If the engine is destroyed, don't render
			else if (!rendering) {
				return true;
			}
			else {
				return false;
			}
		}

		void engine::reset_stop() {
			queue_stop = false;
			rendering = true;
		}
	}
}