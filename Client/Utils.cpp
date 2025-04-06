#include "Utils.h"
#include <filesystem>
#include <vector>
#include <random>
#include <iostream>

namespace fs = std::filesystem;

std::wstring getRandomFile(const std::wstring& folderPath, const std::wstring& extension) {
    static int callCount = 0; // 호출 횟수 확인
    callCount++;

    std::vector<std::wstring> files;
    std::wcout << L"[디버깅] getRandomFile 실행됨" << std::endl;
    std::wcout << L"getRandomFile 실행 (" << callCount << L"번째 호출)" << std::endl;


    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == extension) {
            files.push_back(entry.path().wstring());
        }
    }

    if (files.empty()) {
        std::cerr << "파일이 없습니다." << std::endl;
        return L"";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, files.size() - 1);
    
    return files[dis(gen)];
}
