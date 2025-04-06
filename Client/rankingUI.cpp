#include "RankingUI.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <Windows.h>
#include <sstream>  // std::wstringstream 사용을 위해 필요

using namespace std;
using namespace sf;
using namespace sql;

#define SERVER_IP   "tcp://210.119.12.60:3306"
#define USERNAME    "root"
#define PASSWORD    "1234"
#define DATABASE    "teamDB"

std::wstring StringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstrTo(size_needed - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}

RankingUI::RankingUI() {
    if (!font.openFromFile("assets/font/D2Coding.ttf")) {
        std::cerr << "폰트 로드 실패!" << std::endl;
        return;
    }

    float windowWidth = 900.f;
    float windowHeight = 600.f;

    titleText.emplace(font, L"랭킹 목록", 40);
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

    float buttonWidth = 150.f;
    float buttonHeight = 40.f;
    float spacing = 60.f;

    float totalButtonsWidth = 3 * buttonWidth + 2 * spacing;
    float buttonStartX = (windowWidth - totalButtonsWidth) / 2.f;

    // Total Score
    totalScoreButton.setSize({ buttonWidth, buttonHeight });
    totalScoreButton.setPosition({ buttonStartX, 70.f });

    totalScoreButtonText.emplace(font, L"총 점수", 20);
    totalScoreButtonText->setPosition({
        totalScoreButton.getPosition().x + (buttonWidth / 2.f - totalScoreButtonText->getLocalBounds().size.x / 2.f),
        totalScoreButton.getPosition().y + (buttonHeight / 2.f - totalScoreButtonText->getLocalBounds().size.y / 2.f)
        });

    // Total Time
    totalTimeButton.setSize({ buttonWidth, buttonHeight });
    totalTimeButton.setPosition({ buttonStartX + buttonWidth + spacing, 70.f });

    totalTimeButtonText.emplace(font, L"총 게임 시간", 20);
    totalTimeButtonText->setPosition({
        totalTimeButton.getPosition().x + (buttonWidth / 2.f - totalTimeButtonText->getLocalBounds().size.x / 2.f),
        totalTimeButton.getPosition().y + (buttonHeight / 2.f - totalTimeButtonText->getLocalBounds().size.y / 2.f)
        });

    // Average Speed
    avgSpeedButton.setSize({ buttonWidth, buttonHeight });
    avgSpeedButton.setPosition({ buttonStartX + 2 * (buttonWidth + spacing), 70.f });

    avgSpeedButtonText.emplace(font, L"평균 타수", 20);
    avgSpeedButtonText->setPosition({
        avgSpeedButton.getPosition().x + (buttonWidth / 2.f - avgSpeedButtonText->getLocalBounds().size.x / 2.f),
        avgSpeedButton.getPosition().y + (buttonHeight / 2.f - avgSpeedButtonText->getLocalBounds().size.y / 2.f)
        });

    std::cout << "RankingUI 생성자 완료" << std::endl;
    refreshRanking();
}

RankingUI::~RankingUI() {
    std::cout << "RankingUI 소멸자 호출됨." << std::endl;
}

void RankingUI::refreshRanking() {
    openDataFromDB();

    float visibleHeight = 400.f; // 100~500 영역
    maxScrollOffset = std::max(0.f, static_cast<float>(rankingTexts.size() * 50 - visibleHeight));
    scrollOffset = std::clamp(scrollOffset, 0.f, maxScrollOffset);
}

void RankingUI::openDataFromDB() {
    rankingTexts.clear();

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect(SERVER_IP, USERNAME, PASSWORD));
        conn->setSchema(DATABASE);

        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res;

        float startY = 150.f;
        float spacing = 50.f;
        int rank = 1;

        switch (currentSort) {
        case SortMode::TotalScore: {
            std::string query = "SELECT user_id, point FROM rankings ORDER BY point DESC LIMIT 100;";
            res.reset(stmt->executeQuery(query));

            const int fixedUserIDWidth = 12;
            const int fixedPointWidth = 6;

            while (res->next()) {
                std::string userID = res->getString("user_id");
                int point = res->getInt("point");

                // user_id 길이 고정
                if (userID.length() < fixedUserIDWidth) {
                    userID.append(fixedUserIDWidth - userID.length(), ' ');
                }
                else {
                    userID = userID.substr(0, fixedUserIDWidth);
                }

                // 점수 폭 고정
                std::string pointStr = std::to_string(point);
                if (pointStr.length() < fixedPointWidth) {
                    pointStr.insert(0, fixedPointWidth - pointStr.length(), ' ');
                }

                std::wstring rankText = std::to_wstring(rank) + L". " + StringToWString(userID)
                    + L" | " + StringToWString(pointStr) + L"점";
                sf::Text text(font, rankText, 30);
                text.setFillColor(sf::Color::White);
                text.setPosition({ 450.f - text.getLocalBounds().size.x / 2.f, startY });

                rankingTexts.push_back(text);
                startY += spacing;
                rank++;
            }
            break;
        }

        case SortMode::TotalTime: {
            std::string query = "SELECT user_id, total_play_time FROM user_stats ORDER BY total_play_time DESC LIMIT 100;";
            res.reset(stmt->executeQuery(query));

            const int fixedUserIDWidth = 12;

            while (res->next()) {
                std::string userID = res->getString("user_id");
                int totalTime = res->getInt("total_play_time");

                // user_id 길이 고정
                if (userID.length() < fixedUserIDWidth) {
                    userID.append(fixedUserIDWidth - userID.length(), ' ');
                }
                else {
                    userID = userID.substr(0, fixedUserIDWidth);
                }

                int hours = totalTime / 3600;
                int minutes = (totalTime % 3600) / 60;
                int seconds = totalTime % 60;

                std::wstringstream timeStream;
                timeStream << std::setw(2) << std::setfill(L'0') << hours << L":"
                    << std::setw(2) << std::setfill(L'0') << minutes << L":"
                    << std::setw(2) << std::setfill(L'0') << seconds;

                // ex) 01:05:09 형식

                std::wstring rankText = std::to_wstring(rank) + L". " + StringToWString(userID)
                    + L" | " + timeStream.str();
                sf::Text text(font, rankText, 30);
                text.setFillColor(sf::Color::White);
                text.setPosition({ 450.f - text.getLocalBounds().size.x / 2.f, startY });

                rankingTexts.push_back(text);
                startY += spacing;
                rank++;
            }
            break;
        }

        case SortMode::AvgSpeed: {
            std::string query = "SELECT user_id, avg_speed FROM user_stats ORDER BY avg_speed DESC LIMIT 100;";
            res.reset(stmt->executeQuery(query));

            const int fixedUserIDWidth = 12;
            const int fixedSpeedWidth = 6;

            while (res->next()) {
                std::string userID = res->getString("user_id");
                int avgSpeed = res->getInt("avg_speed");

                // user_id 길이 고정
                if (userID.length() < fixedUserIDWidth) {
                    userID.append(fixedUserIDWidth - userID.length(), ' ');
                }
                else {
                    userID = userID.substr(0, fixedUserIDWidth);
                }

                // 속도 폭 고정
                std::string speedStr = std::to_string(avgSpeed);
                if (speedStr.length() < fixedSpeedWidth) {
                    speedStr.insert(0, fixedSpeedWidth - speedStr.length(), ' ');
                }

                std::wstring rankText = std::to_wstring(rank) + L". " + StringToWString(userID)
                    + L" | " + StringToWString(speedStr) + L"타";
                sf::Text text(font, rankText, 30);


                text.setFillColor(sf::Color::White);
                text.setPosition({ 450.f - text.getLocalBounds().size.x / 2.f, startY });

                rankingTexts.push_back(text);
                startY += spacing;
                rank++;
            }
            break;
        }
        }

    }
    catch (sql::SQLException& e) {
        std::cerr << "MySQL 오류: " << e.what() << std::endl;
    }
}

void RankingUI::show() {
    sf::RenderWindow rankingWindow(sf::VideoMode({ 900, 600 }), L"랭킹 보기");

    sf::View view = rankingWindow.getDefaultView();
    view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });

    float scrollOffset = 0.f;
    const float maxScrollOffset = std::max(0.f, static_cast<float>(rankingTexts.size() * 50 - 300));

    // 스크롤 바 추가
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
                Vector2f mousePos(Mouse::getPosition(rankingWindow));

                if (backButton.getGlobalBounds().contains(mousePos)) {
                    rankingWindow.close();
                }
                else if (totalScoreButton.getGlobalBounds().contains(mousePos)) {
                    currentSort = SortMode::TotalScore;
                    refreshRanking();
                }
                else if (totalTimeButton.getGlobalBounds().contains(mousePos)) {
                    currentSort = SortMode::TotalTime;
                    refreshRanking();
                }
                else if (avgSpeedButton.getGlobalBounds().contains(mousePos)) {
                    currentSort = SortMode::AvgSpeed;
                    refreshRanking();
                }
                else if (scrollBar.getGlobalBounds().contains(mousePos)) {
                    isDragging = true;
                    dragStartY = mousePos.y - scrollBar.getPosition().y;
                }
            }
            else if (event->is<Event::MouseWheelScrolled>()) {
                if (const auto* scrollEvent = event->getIf<Event::MouseWheelScrolled>()) {
                    // 마우스 휠 스크롤 시, scrollOffset 값을 조정
                    scrollOffset -= scrollEvent->delta * 30.f;
                    scrollOffset = std::clamp(scrollOffset, 0.f, maxScrollOffset);

                    // 스크롤바 위치 조정
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

        // 버튼 색 갱신
        totalScoreButton.setFillColor(currentSort == SortMode::TotalScore ? Color(255, 0, 0) : Color(128, 128, 128));
        totalTimeButton.setFillColor(currentSort == SortMode::TotalTime ? Color(0, 255, 0) : Color(128, 128, 128));
        avgSpeedButton.setFillColor(currentSort == SortMode::AvgSpeed ? Color(0, 0, 255) : Color(128, 128, 128));

        rankingWindow.clear(Color::Black);
        rankingWindow.draw(*titleText);
        rankingWindow.draw(backButton);
        rankingWindow.draw(*backButtonText);
        rankingWindow.draw(totalScoreButton);
        rankingWindow.draw(*totalScoreButtonText);
        rankingWindow.draw(totalTimeButton);
        rankingWindow.draw(*totalTimeButtonText);
        rankingWindow.draw(avgSpeedButton);
        rankingWindow.draw(*avgSpeedButtonText);

        View scrollView = view;
        scrollView.setCenter({ view.getCenter().x, 300.f + scrollOffset });
        rankingWindow.setView(scrollView);

        // 텍스트에 페이드 아웃 효과 적용
        for (auto& text : rankingTexts) {
            float textY = text.getPosition().y - scrollOffset;
            std::uint8_t alpha = 255;

            // 위쪽 페이드 아웃
            if (textY < 100.f) {
                alpha = static_cast<std::uint8_t>(std::max(0.f, (textY - 50.f) / 50.f * 255.f));
            }
            // 아래쪽 페이드 아웃
            else if (textY > 450.f) {
                alpha = static_cast<std::uint8_t>(std::max(0.f, (500.f - textY) / 50.f * 255.f));
            }

            // 글자 색 조정 (알파 값 변경)
            text.setFillColor(sf::Color(255, 255, 255, alpha));
            rankingWindow.draw(text);
        }

        rankingWindow.setView(view);
        rankingWindow.draw(scrollBarBackground);
        rankingWindow.draw(scrollBar);
        rankingWindow.display();
    }
}