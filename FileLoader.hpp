#pragma once
#include <string>
#include <optional>

class FileLoader {
public:
    std::optional<std::wstring> openFileDialog();
    std::wstring loadText(const std::wstring& filepath);

private:
    std::wstring ansiToWstring(const std::string& str);
    std::wstring utf8ToWstring(const std::string& str);
    bool isUtf8(const std::string& content);
};