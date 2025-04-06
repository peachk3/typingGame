#pragma once
#include <string>
#include <optional>



std::optional<std::wstring> openFileDialog();
std::wstring loadText(const std::wstring& filepath);


std::wstring ansiToWstring(const std::string& str);
std::wstring utf8ToWstring(const std::string& str);
bool isUtf8(const std::string& content);
