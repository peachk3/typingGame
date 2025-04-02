#include "FileLoader.hpp"
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <filesystem>
#pragma comment(lib, "Comdlg32.lib")
#include <iostream>

/*#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <windows.h>     // Windows API - 파일 선택기 열기 위해 필요
#include <commdlg.h>     // GetOpenFileName 함수
#include <optional>      // 선택적 반환
#include <fstream>       // 파일 읽어오기 위해
#include <sstream>
#include <string>
#pragma comment(lib, "Comdlg32.lib")*/

std::optional<std::wstring> FileLoader::openFileDialog() {
    wchar_t filename[MAX_PATH] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"텍스트 파일\0*.txt\0모든 파일\0*.*\0파이썬 파일\0*.py\0자바 파일\0*.java\0C++ 파일\0*.cpp\0\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = L"파일 열기";

    if (GetOpenFileNameW(&ofn)) {
        std::wcout << L"[INFO] 파일 선택됨: " << filename << std::endl;
        return std::wstring(filename);
    }
    else {
        DWORD err = CommDlgExtendedError();
        std::wcerr << L"[ERROR] 파일 선택 실패! 오류 코드: " << err << std::endl;
    }
    return std::nullopt;
}

std::wstring FileLoader::loadText(const std::wstring& filepath) {
    std::ifstream file(std::filesystem::path(filepath), std::ios::binary);
    if (!file) return L"파일이 열리지 않습니다.";

    unsigned char bom[3] = { 0 };
    file.read(reinterpret_cast<char*>(bom), 3);
    file.seekg(0);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::wstring result;

    if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
        result = utf8ToWstring(content);
    }
    else if (isUtf8(content)) {
        result = utf8ToWstring(content);
    }
    else {
        result = ansiToWstring(content);
    }

    result.erase(std::remove(result.begin(), result.end(), L'\0'), result.end());

    return result;
}

std::wstring FileLoader::ansiToWstring(const std::string& str) {
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(len, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &result[0], len);
    return result;
}

std::wstring FileLoader::utf8ToWstring(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], len);
    return result;
}

bool FileLoader::isUtf8(const std::string& content) {
    int expected = 0;
    for (unsigned char c : content) {
        if (expected == 0) {
            if ((c >> 5) == 0x6) expected = 1;
            else if ((c >> 4) == 0xE) expected = 2;
            else if ((c >> 3) == 0x1E) expected = 3;
            else if ((c >> 7)) return false;
        }
        else {
            if ((c >> 6) != 0x2) return false;
            expected--;
        }
    }
    return true;
}
