#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
// ��� ������ ��� ����ü
struct TypingStats {
    double wpm = 0.0;
    double accuracy = 0.0;
    double elapsedTime = 0.0;  // �� ����
    double tmp = 0;    //Typing Per Minute �ѱ��� Ÿ ��
    int totalKeyPress = 0;
};

class TypingGame
{
public:
    // ���� ����
    void startNewGame(sf::RenderWindow& window, sf::Font& font, int fontSize,
        std::vector<std::vector<std::wstring>>& sentences);

    // ��� â
    void showResultWindow(sf::RenderWindow& window, sf::Font& font, int fontSize,
        double wpm, double accuracy, double time, double tpm,
        std::vector<std::vector<std::wstring>>& sentences);

    // ���� ���� ���
    void drawOriginalText(sf::RenderWindow& window, sf::Font& font, int fontSize,
        std::vector<std::vector<std::wstring>>& fileAllLines);

    // ���� �Է� ���
    void drawUserInputText(
        sf::RenderWindow& window,
        sf::Font& font,
        int fontSize,
        std::vector<std::vector<std::wstring>>& userInputs,
        std::vector<std::vector<std::wstring>>& sentences);

    // ��Ȯ�� ���
    static double calculateAccuracyWithBackspace(int totalTyped, int backspaceCount);

    // ���� ���
    TypingStats updateStats(
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime,
        std::wstring& currentSentence,
        std::wstring& userInput,
        int backspacePressCount,
        int totalKeyPress);
};

