#pragma once

#include <Windows.h>
#include <string>

using std::wstring;

namespace registry {
	const wstring SYSTEM = L"Software\\Policies\\Microsoft\\Windows\\System";
	const wstring POLICIES = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
	const wstring RUN_ONCE = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce";

	inline bool create_key(
		HKEY root,
		const wstring key,
		const wstring name,
		const wstring data
	) {
		HKEY handle = NULL;

		LONG result = RegCreateKeyExW(
			root,
			key.c_str(),
			NULL, NULL, NULL,
			KEY_SET_VALUE,
			NULL,
			&handle,
			NULL
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		result = RegSetValueExW(
			handle,
			name.c_str(),
			NULL, REG_SZ,
			reinterpret_cast<CONST BYTE*>(data.c_str()),
			static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t))
		);

		RegCloseKey(handle);
		return result == ERROR_SUCCESS;
	}

	inline bool create_key(
		HKEY root,
		const wstring key,
		const wstring name,
		DWORD data
	) {
		HKEY handle = NULL;

		LONG result = RegCreateKeyExW(
			root,
			key.c_str(),
			NULL, NULL, NULL,
			KEY_SET_VALUE,
			NULL,
			&handle,
			NULL
		);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		result = RegSetValueExW(
			handle,
			name.c_str(),
			NULL, REG_DWORD,
			reinterpret_cast<CONST BYTE*>(&data),
			sizeof(DWORD)
		);

		RegCloseKey(handle);
		return result == ERROR_SUCCESS;
	}
}