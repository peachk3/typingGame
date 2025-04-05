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

    void show();            // ��ŷ â�� ���� �Լ�
    void openDataFromDB();  // DB���� ��ŷ ������ ��������

private:
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
};

#endif  // RANKING_UI_H
