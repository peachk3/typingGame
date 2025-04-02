#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
// 통계 정보를 담는 구조체
struct TypingStats {
    double wpm = 0.0;
    double accuracy = 0.0;
    double elapsedTime = 0.0;  // 초 단위
    double tmp = 0;    //Typing Per Minute 한국식 타 수
    int totalKeyPress = 0;
};

class TypingGame
{
public:
    // 게임 시작
    void startNewGame(sf::RenderWindow& window, sf::Font& font, int fontSize,
        std::vector<std::vector<std::wstring>>& sentences);

    // 결과 창
    void showResultWindow(sf::RenderWindow& window, sf::Font& font, int fontSize,
        double wpm, double accuracy, double time, double tpm,
        std::vector<std::vector<std::wstring>>& sentences);

    // 기준 문장 출력
    void drawOriginalText(sf::RenderWindow& window, sf::Font& font, int fontSize,
        std::vector<std::vector<std::wstring>>& fileAllLines);

    // 유저 입력 출력
    void drawUserInputText(
        sf::RenderWindow& window,
        sf::Font& font,
        int fontSize,
        std::vector<std::vector<std::wstring>>& userInputs,
        std::vector<std::vector<std::wstring>>& sentences);

    // 정확도 계산
    static double calculateAccuracyWithBackspace(int totalTyped, int backspaceCount);

    // 스텟 기록
    TypingStats updateStats(
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime,
        std::wstring& currentSentence,
        std::wstring& userInput,
        int backspacePressCount,
        int totalKeyPress);
};

