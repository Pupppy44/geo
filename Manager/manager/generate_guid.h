#pragma once
#include <string>
#pragma comment(lib, "rpcrt4.lib") // For GUID generation

namespace manager {
	// Generates a random GUID
	inline std::string generate_guid() {
		GUID guid;
		CoCreateGuid(&guid);

		wchar_t* guid_str;
		UuidToStringW(&guid, (RPC_WSTR*)&guid_str);

		std::wstring guid_wstr(guid_str);
		std::string guid_str_mb = std::string(guid_wstr.begin(), guid_wstr.end());

		RpcStringFreeW((RPC_WSTR*)&guid_str);

		return guid_str_mb;
	}
}