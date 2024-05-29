#pragma once
#include <iostream>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Headers.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;

namespace geo {
    namespace util {
        class api {
        public:
            // Upload an avatar
			static bool upload_avatar(std::string name, std::string avatar) {
                _upload_avatar(name, avatar).get();
                return true;
			}
        private:
            static IAsyncOperation<IBuffer> _buffer_from_file(std::wstring path)
            {
                try
                {
                    StorageFile file = co_await StorageFile::GetFileFromPathAsync(path);
                    IBuffer buffer = co_await FileIO::ReadBufferAsync(file);
                    co_return buffer;
                }
                catch (hresult_error const& ex)
                {
					co_return nullptr;
                }
            }

            static IAsyncAction _upload_avatar(std::string name, std::string avatar)
            {
                try
                {
                    // Get the buffer from the file
                    IBuffer buffer = co_await _buffer_from_file(winrt::to_hstring(avatar).data());

					if (buffer == nullptr)
					{
						co_return;
					}

                    // Create Uri for the request
                    Uri uri{ winrt::to_hstring("https://geoapi.glitch.me/api/upload_avatar?name=" + name) };
                    HttpClient http_client{};

                    // Create HttpBufferContent using the IBuffer
                    HttpBufferContent content{ buffer };

                    // Create HttpRequestMessage
                    HttpRequestMessage request{ HttpMethod::Post(), uri };

                    // Set the content of the request to the HttpBufferContent
                    request.Content(content);

                    // Add custom User-Agent header
                    request.Headers().UserAgent().ParseAdd(L"GeoClient/1.0");

                    // Set content type to image/bmp
                    content.Headers().ContentType(winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue::HttpMediaTypeHeaderValue(L"image/bmp"));

                    // Send the request
                    HttpResponseMessage response = co_await http_client.SendRequestAsync(request);
                }
                catch (hresult_error const& ex)
                {
					
                }
            }
        };
    }
}