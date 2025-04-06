#ifndef RANKING_UI_H
#define RANKING_UI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>

// ���� ���� ������
enum class SortMode {
    TotalScore,
    TotalTime,
    AvgSpeed
};

class RankingUI {
public:
    RankingUI();
    ~RankingUI();

    void show();            // ��ŷ â�� ���� �Լ�

private:
    void openDataFromDB();  // ���� ���ؿ� ���� DB �ҷ�����
    void refreshRanking();  // ���� �� ���ΰ�ħ

    sf::Font font;

    sf::RectangleShape backButton;
    sf::RectangleShape totalScoreButton;   // �� ���� ��ư
    sf::RectangleShape totalTimeButton;    // �� ���� �ð� ��ư
    sf::RectangleShape avgSpeedButton;  // ��� ��Ȯ�� ��ư

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> backButtonText;
    std::optional<sf::Text> totalScoreButtonText;  // �� ���� ��ư �ؽ�Ʈ
    std::optional<sf::Text> totalTimeButtonText;   // �� ���� �ð� ��ư �ؽ�Ʈ
    std::optional<sf::Text> avgSpeedButtonText; // ��� ��Ȯ�� ��ư �ؽ�Ʈ

    std::vector<sf::Text> rankingTexts;  // ��ŷ ������ �ؽ�Ʈ��

    float scrollOffset = 0.f;
    float maxScrollOffset = 0.f;

    SortMode currentSort = SortMode::TotalScore; // ���� ���� ����
};

#endif  // RANKING_UI_H
