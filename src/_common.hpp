// https://github.com/leetCipher/Malware.development/blob/main/nop-pe-code/nop-pe-code.cpp

#pragma once

#include <Windows.h>
#include <string>

namespace common {
	inline std::string string(const std::wstring& wstr) {
		return std::string(wstr.begin(), wstr.end());
	}

	inline void self_destruct() {
		HMODULE module = GetModuleHandle(NULL);
		HANDLE process = GetCurrentProcess();

		IMAGE_DOS_HEADER dos_header = {};

		ReadProcessMemory(
			process,
			(LPCVOID)module,
			&dos_header,
			sizeof(dos_header),
			NULL
		);

		LPCVOID address = (LPBYTE)module + dos_header.e_lfanew;

		IMAGE_SECTION_HEADER sect_header = {};

		LPCVOID text_sect_header = (LPBYTE)address + sizeof(IMAGE_NT_HEADERS64);

		ReadProcessMemory(
			GetCurrentProcess(),
			(LPCVOID)text_sect_header,
			&sect_header,
			sizeof(sect_header),
			NULL
		);

		BYTE* text_sect = (BYTE*)module + sect_header.VirtualAddress;

		DWORD old = 0;

		VirtualProtect(
			text_sect,
			sect_header.Misc.VirtualSize,
			PAGE_EXECUTE_READWRITE,
			&old
		);
		
		for (int i = 0; i < sect_header.Misc.VirtualSize; i++) {
			text_sect[i] = 0x90;
		}

		MessageBoxA(
			NULL,
			"Failed to clean up",
			"Error",
			MB_OK | MB_ICONERROR
		);
	}
}