#ifndef RANKING_UI_H
#define RANKING_UI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>

class RankingUI {
public:
    RankingUI();
    ~RankingUI();

    void show();            // 랭킹 창을 띄우는 함수
    void openDataFromDB();  // DB에서 랭킹 데이터 가져오기

private:
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
};

#endif  // RANKING_UI_H
