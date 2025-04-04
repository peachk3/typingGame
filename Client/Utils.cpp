#include "Utils.h"
#include <filesystem>
#include <vector>
#include <random>
#include <iostream>

namespace fs = std::filesystem;

std::string getRandomFile(const std::string& folderPath, const std::string& extension) {
    static int callCount = 0; // ȣ�� Ƚ�� Ȯ��
    callCount++;

    std::vector<std::string> files;
    std::wcout << L"[�����] getRandomFile �����" << std::endl;
    std::wcout << L"getRandomFile ���� (" << callCount << L"��° ȣ��)" << std::endl;


    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == extension) {
            files.push_back(entry.path().string());
        }
    }

    if (files.empty()) {
        std::cerr << "������ �����ϴ�." << std::endl;
        return "";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, files.size() - 1);
    
    return files[dis(gen)];
}
