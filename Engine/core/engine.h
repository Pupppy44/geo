#pragma once
#include "../tree/tree.h"
#include <d2d1_3.h>
#include <d3d11_1.h>

namespace geo {
	namespace core {
		class game;
		
		class engine {
		public:
			engine(core::game& game) :
				game(&game),
				queue_stop(false),
				rendering(false),
				d2d_factory(nullptr),
				d2d_device(nullptr),
				d2d_context(nullptr),
				dxgi_swap_chain(nullptr),
				d2d_back_buffer(nullptr),
				d3d_device(nullptr),
				d3d_context(nullptr)
			{};

			// Initiate the engine
			void init();

			// Destroy the engine!
			void destroy();

			// Render the next frame onto the screen
			void render();

			// Get the rendering screen as a bitmap
			ID2D1Bitmap1* get_screen();

			// Get the Direct2D context
			ID2D1DeviceContext5* get_context();
		public:
			// Game objects tree
			tree::tree tree;
		private:
			// Initiate all independent resources
			void init_independent_resources();

			// Initiate all Direct2D and Direct3D resources
			void init_resources();

			// Check if the engine should be rendering or stop it
			bool stop();
		private:
			core::game* game;
			// Determines if the engine should stop or not
			bool queue_stop;
			// Determines if the engine is rendering or not
			bool rendering;
			
			ID3D11Device1* d3d_device;
			ID3D11DeviceContext1* d3d_context;

			ID2D1Device6* d2d_device;
			ID2D1Factory7* d2d_factory;
			ID2D1DeviceContext5* d2d_context;

			IDXGISwapChain1* dxgi_swap_chain;
			ID2D1Bitmap1* d2d_back_buffer;
		};
	}
}
