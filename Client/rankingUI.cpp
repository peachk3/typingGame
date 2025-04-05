#include "RankingUI.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <SFML/Graphics/Color.hpp>

using namespace std;
using namespace sf;
using namespace sql;

#define SERVER_IP   "tcp://210.119.12.60:3306"
#define USERNAME    "root"
#define PASSWORD    "1234"
#define DATABASE    "teamDB"

RankingUI::RankingUI() {
    if (!font.openFromFile("D2Coding.ttf")) {
        std::cerr << "��Ʈ �ε� ����!" << std::endl;
        return; // ��Ʈ �ε� ���� �� �� �̻� �������� ����
    }

    float windowWidth = 900.f;
    float windowHeight = 600.f;

    // titleText �� backButtonText�� �����ϰ� �ʱ�ȭ
    titleText.emplace(font, L"��ŷ ���", 40);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition({ windowWidth / 2.f - titleText->getLocalBounds().size.x / 2.f, 10.f });

    backButton.setFillColor(sf::Color::White);
    backButton.setSize({ 200.f, 50.f });
    backButton.setPosition({ windowWidth / 2.f - backButton.getSize().x / 2.f, windowHeight - 70.f });

    backButtonText.emplace(font, "Back", 30);
    backButtonText->setFillColor(sf::Color::Black);
    backButtonText->setPosition({
        backButton.getPosition().x + (backButton.getSize().x / 2.f - backButtonText->getLocalBounds().size.x / 2.f),
        backButton.getPosition().y + 10.f
        });

    // ��ư�� ���� ũ�� �� ���� ����
    float buttonWidth = 150.f; // ��ư ���� ũ��
    float buttonHeight = 40.f; // ��ư ���� ũ��
    float spacing = 60.f; // ��ư ����

    // ��ư���� ��ġ�� X ���� ��ġ ��� (�߾� ����)
    float totalButtonsWidth = 3 * buttonWidth + 2 * spacing; // �� ��ư�� �� �ʺ� + ����
    float buttonStartX = (windowWidth - totalButtonsWidth) / 2.f; // �߾� ������ ���� X ���� ��ġ

    // �� ���� ��ư ����
    totalScoreButton.setSize({ buttonWidth, buttonHeight });
    totalScoreButton.setFillColor(sf::Color::White);
    totalScoreButton.setPosition({ buttonStartX, 70.f });

    // �� ���� ��ư �ؽ�Ʈ ����
    totalScoreButtonText.emplace(font, L"�� ����", 20);
    totalScoreButtonText->setFillColor(sf::Color::Black);
    totalScoreButtonText->setPosition({
        totalScoreButton.getPosition().x + (totalScoreButton.getSize().x / 2.f - totalScoreButtonText->getLocalBounds().size.x / 2.f),
        totalScoreButton.getPosition().y + (totalScoreButton.getSize().y / 2.f - totalScoreButtonText->getLocalBounds().size.y / 2.f)
        });

    // �� ���� �ð� ��ư ����
    totalTimeButton.setSize({ buttonWidth, buttonHeight });
    totalTimeButton.setFillColor(sf::Color::White);
    totalTimeButton.setPosition({ buttonStartX + totalScoreButton.getSize().x + spacing, 70.f });

    // �� ���� �ð� ��ư �ؽ�Ʈ ����
    totalTimeButtonText.emplace(font, L"�� ���� �ð�", 20);
    totalTimeButtonText->setFillColor(sf::Color::Black);
    totalTimeButtonText->setPosition({
        totalTimeButton.getPosition().x + (totalTimeButton.getSize().x / 2.f - totalTimeButtonText->getLocalBounds().size.x / 2.f),
        totalTimeButton.getPosition().y + (totalTimeButton.getSize().y / 2.f - totalTimeButtonText->getLocalBounds().size.y / 2.f)
        });

    // ��� Ÿ�� ��ư ����
    avgSpeedButton.setSize({ buttonWidth, buttonHeight });
    avgSpeedButton.setFillColor(sf::Color::White);
    avgSpeedButton.setPosition({ buttonStartX + totalScoreButton.getSize().x + totalTimeButton.getSize().x + 2 * spacing, 70.f });

    // ��� Ÿ�� ��ư �ؽ�Ʈ ����
    avgSpeedButtonText.emplace(font, L"��� Ÿ��", 20);
    avgSpeedButtonText->setFillColor(sf::Color::Black);
    avgSpeedButtonText->setPosition({
        avgSpeedButton.getPosition().x + (avgSpeedButton.getSize().x / 2.f - avgSpeedButtonText->getLocalBounds().size.x / 2.f),
        avgSpeedButton.getPosition().y + (avgSpeedButton.getSize().y / 2.f - avgSpeedButtonText->getLocalBounds().size.y / 2.f)
        });


    std::cout << "RankingUI ������ �Ϸ�" << std::endl;
    openDataFromDB();
}



RankingUI::~RankingUI() {
    std::cout << "RankingUI �Ҹ��� ȣ���." << std::endl;
}

void RankingUI::openDataFromDB() {
    rankingTexts.clear();

    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* conn;
        sql::Statement* stmt;
        sql::ResultSet* res;

        driver = sql::mysql::get_mysql_driver_instance();
        conn = driver->connect(SERVER_IP, USERNAME, PASSWORD);
        conn->setSchema(DATABASE);

        stmt = conn->createStatement();
        res = stmt->executeQuery("SELECT user_id, point FROM rankings ORDER BY point DESC LIMIT 100;");

        float startY = 150.f;
        float spacing = 50.f;
        int rank = 1;
        const int fixedUserIDWidth = 15;  // user_id ���� �ʺ�
        const int fixedPointWidth = 6;   // point ���� �ʺ� (�ִ� 999999)

        while (res->next()) {
            string userID = res->getString("user_id");
            int point = res->getInt("point");

            // user_id ���� ���߱� (ª���� ���� �߰�, ��� �ڸ�)
            if (userID.length() < fixedUserIDWidth) {
                userID.append(fixedUserIDWidth - userID.length(), ' ');
            }
            else {
                userID = userID.substr(0, fixedUserIDWidth);
            }

            // point�� 6�ڸ��� ���߱� (�տ� ���� �߰�)
            string pointStr = to_string(point);
            if (pointStr.length() < fixedPointWidth) {
                pointStr.insert(0, fixedPointWidth - pointStr.length(), ' ');
            }

            // ���������� ���ĵ� �ؽ�Ʈ ����
            string rankText = to_string(rank) + ". " + userID + " | Point: " + pointStr;

            sf::Text text(font, rankText, 30);
            text.setFillColor(sf::Color::White);

            float textWidth = text.getLocalBounds().size.x;
            text.setPosition({ 450.f - textWidth / 2.f, startY });

            rankingTexts.push_back(text);
            startY += spacing;
            rank++;
        }

        delete res;
        delete stmt;
        delete conn;
    }
    catch (sql::SQLException& e) {
        std::cerr << "MySQL ����: " << e.what() << std::endl;
    }
}

void RankingUI::show() {
    sf::RenderWindow rankingWindow(sf::VideoMode({ 900, 600 }), L"��ŷ ����");

    sf::View view = rankingWindow.getDefaultView();
    view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });

    float scrollOffset = 0.f;
    const float maxScrollOffset = std::max(0.f, static_cast<float>(rankingTexts.size() * 50 - 300));

    // ��ũ�� �� �߰�
    sf::RectangleShape scrollBarBackground(sf::Vector2f(10.f, 400.f));
    scrollBarBackground.setFillColor(sf::Color(50, 50, 50));
    scrollBarBackground.setPosition({ 870.f, 100.f });

    sf::RectangleShape scrollBar(sf::Vector2f(10.f, 100.f));
    scrollBar.setFillColor(sf::Color(200, 200, 200));
    scrollBar.setPosition({ 870.f, 100.f });

    bool isDragging = false;
    float dragStartY = 0.f;

    while (rankingWindow.isOpen()) {
        while (const optional<Event> event = rankingWindow.pollEvent()) {
            if (event->is<Event::Closed>()) {
                rankingWindow.close();
                return;
            }
            else if (event->is<Event::MouseButtonPressed>()) {
                // ��ư Ŭ�� ó�� (�ߺ��� �ڵ� ����)
                if (backButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    rankingWindow.close();
                }
                else if (totalScoreButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    std::cout << "Total Score ��ư Ŭ����." << std::endl;
                }
                else if (totalTimeButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    std::cout << "Total Time ��ư Ŭ����." << std::endl;
                }
                else if (avgSpeedButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    std::cout << "Avg Speed ��ư Ŭ����." << std::endl;
                }
                else if (scrollBar.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    isDragging = true;
                    dragStartY = Mouse::getPosition(rankingWindow).y - scrollBar.getPosition().y;
                }
            }
            else if (event->is<Event::MouseWheelScrolled>()) {
                if (const auto* scrollEvent = event->getIf<Event::MouseWheelScrolled>()) {
                    // ���콺 �� ��ũ�� ��, scrollOffset ���� ����
                    scrollOffset -= scrollEvent->delta * 30.f;
                    scrollOffset = std::clamp(scrollOffset, 0.f, maxScrollOffset);

                    // ��ũ�ѹ� ��ġ ����
                    float scrollPercentage = scrollOffset / maxScrollOffset;
                    float scrollBarHeight = 100.f;
                    float scrollBarPosition = 100.f + (400.f - scrollBarHeight) * scrollPercentage;
                    scrollBar.setPosition({ 870.f, scrollBarPosition });
                }
            }
            else if (event->is<Event::MouseButtonReleased>()) {
                isDragging = false;
            }
            else if (event->is<Event::MouseMoved>() && isDragging) {
                float newY = Mouse::getPosition(rankingWindow).y - dragStartY;
                newY = std::clamp(newY, 100.f, 400.f);
                scrollBar.setPosition({ 870.f, newY });

                float scrollPercentage = (newY - 100.f) / (400.f - 100.f);
                scrollOffset = scrollPercentage * maxScrollOffset;
            }
        }

        rankingWindow.clear(sf::Color::Black);

        rankingWindow.draw(*titleText);
        rankingWindow.draw(backButton);
        rankingWindow.draw(*backButtonText);

        rankingWindow.draw(totalScoreButton);
        rankingWindow.draw(*totalScoreButtonText);

        rankingWindow.draw(totalTimeButton);
        rankingWindow.draw(*totalTimeButtonText);

        rankingWindow.draw(avgSpeedButton);
        rankingWindow.draw(*avgSpeedButtonText);

        // ��ũ�� ó��
        sf::View scrollView = view;
        scrollView.setCenter({ view.getCenter().x, 300.f + scrollOffset });
        rankingWindow.setView(scrollView);

        // �ؽ�Ʈ�� ���̵� �ƿ� ȿ�� ����
        for (auto& text : rankingTexts) {
            float textY = text.getPosition().y - scrollOffset;
            std::uint8_t alpha = 255;

            // ���� ���̵� �ƿ�
            if (textY < 100.f) {
                alpha = static_cast<std::uint8_t>(std::max(0.f, (textY - 50.f) / 50.f * 255.f));
            }
            // �Ʒ��� ���̵� �ƿ�
            else if (textY > 450.f) {
                alpha = static_cast<std::uint8_t>(std::max(0.f, (500.f - textY) / 50.f * 255.f));
            }

            // ���� �� ���� (���� �� ����)
            text.setFillColor(sf::Color(255, 255, 255, alpha));
            rankingWindow.draw(text);
        }

        rankingWindow.setView(view);

        rankingWindow.draw(scrollBarBackground);
        rankingWindow.draw(scrollBar);

        rankingWindow.display();
    }
}