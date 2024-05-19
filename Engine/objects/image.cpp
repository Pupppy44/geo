#include "image.h"

namespace geo {
	namespace objects {
		image::image() {
			type("image");
		};

		void image::init() {
			// Create D2D factory
			context->GetFactory(&factory);

			// Create WIC factory
			if (FAILED(CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&wic_factory)
			))) {
				return;
			}

			update();
		};

		void image::render() {
			// Check if image updated
			if (get_property<std::string>("path") != current_path) {
				update();
				DEBUG("image updated (id=" + id() + ")");
			}
			
			std::string path = get_property<std::string>("path");
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");
			float opacity = get_property<float>("opacity");
			float radius = get_property<float>("radius");

			if (path == "" || bitmap == NULL) return;

			// Set the border of the image
			factory->CreateRoundedRectangleGeometry(
				D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radius, radius),
				&border
			);

			// Create a layer for the border
			ID2D1Layer* layer = NULL;
			context->CreateLayer(&layer);

			// Push the border layer to the context
			context->PushLayer(
				D2D1::LayerParameters(
					D2D1::InfiniteRect(),
					border
				),
				layer
			);

			// Draw the image
			context->DrawBitmap(
				bitmap,
				D2D1::RectF(x, y, x + width, y + height),
				opacity,
				D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC
			);

			// Remove border layer from the context
			context->PopLayer();

			if (layer) layer->Release();
			if (border) border->Release();
		};


		void image::update() {
			std::string path = get_property<std::string>("path");

			if (path == "") return;
			
			// If the image is a remote resource, download it and store it in a temp directory
			if (path.starts_with("http")) {
				DEBUG("downloading image from " + get_property<std::string>("path") + " (image id=" + id() + ")");

				std::string file = util::download_file(get_property<std::string>("path"));

				tree::property path_prop(tree::property_type::STRING, "path", file);
				set_property(path_prop);
				
				path = file;
			}

			// Create decoder
			HRESULT hr = wic_factory->CreateDecoderFromFilename(
				util::string_to_wchar(path.data()),
				NULL,
				GENERIC_READ,
				WICDecodeMetadataCacheOnLoad,
				&decoder
			);
			
			// Load the image
			if (SUCCEEDED(hr)) {
				decoder->GetFrame(0, &source);
				wic_factory->CreateFormatConverter(&converter);

				converter->Initialize(
					source,
					GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone,
					NULL,
					0.f,
					WICBitmapPaletteTypeMedianCut
				);

				context->CreateBitmapFromWicBitmap(
					converter,
					NULL,
					&bitmap
				);
			}
			else {
				ERR("couldn't load image from its path (id = " + id() + ")");
			}

			// Set the width and height of the image if not set
			if (!get_property<float>("width") || !get_property<float>("height")) {
				float width = bitmap->GetSize().width;
				float height = bitmap->GetSize().height;
				
				set_property(tree::property(tree::property_type::NUMBER, "width", std::to_string(width)));
				set_property(tree::property(tree::property_type::NUMBER, "height", std::to_string(height)));
			}

			// Set current path for future checks
			current_path = path;
		}
	}
}