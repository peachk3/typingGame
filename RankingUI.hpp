#ifndef RANKING_UI_H
#define RANKING_UI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>

// 정렬 기준 열거형
enum class SortMode {
    TotalScore,
    TotalTime,
    AvgSpeed
};

class RankingUI {
public:
    RankingUI();
    ~RankingUI();

    void show();            // 랭킹 창을 띄우는 함수

private:
    void openDataFromDB();  // 정렬 기준에 따라 DB 불러오기
    void refreshRanking();  // 정렬 후 새로고침

    sf::Font font;

    sf::RectangleShape backButton;
    sf::RectangleShape totalScoreButton;   // 총 점수 버튼
    sf::RectangleShape totalTimeButton;    // 총 게임 시간 버튼
    sf::RectangleShape avgSpeedButton;  // 평균 정확도 버튼

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> backButtonText;
    std::optional<sf::Text> totalScoreButtonText;  // 총 점수 버튼 텍스트
    std::optional<sf::Text> totalTimeButtonText;   // 총 게임 시간 버튼 텍스트
    std::optional<sf::Text> avgSpeedButtonText; // 평균 정확도 버튼 텍스트

    std::vector<sf::Text> rankingTexts;  // 랭킹 데이터 텍스트들

    float scrollOffset = 0.f;
    float maxScrollOffset = 0.f;

    SortMode currentSort = SortMode::TotalScore; // 현재 정렬 상태
};

#endif  // RANKING_UI_H