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
        std::cerr << "폰트 로드 실패!" << std::endl;
        return; // 폰트 로드 실패 시 더 이상 진행하지 않음
    }

    float windowWidth = 900.f;
    float windowHeight = 600.f;

    // titleText 및 backButtonText를 안전하게 초기화
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

    // 버튼의 가로 크기 및 간격 설정
    float buttonWidth = 150.f; // 버튼 가로 크기
    float buttonHeight = 40.f; // 버튼 세로 크기
    float spacing = 60.f; // 버튼 간격

    // 버튼들이 배치될 X 시작 위치 계산 (중앙 정렬)
    float totalButtonsWidth = 3 * buttonWidth + 2 * spacing; // 세 버튼의 총 너비 + 간격
    float buttonStartX = (windowWidth - totalButtonsWidth) / 2.f; // 중앙 정렬을 위한 X 시작 위치

    // 총 점수 버튼 설정
    totalScoreButton.setSize({ buttonWidth, buttonHeight });
    totalScoreButton.setFillColor(sf::Color::White);
    totalScoreButton.setPosition({ buttonStartX, 70.f });

    // 총 점수 버튼 텍스트 설정
    totalScoreButtonText.emplace(font, L"총 점수", 20);
    totalScoreButtonText->setFillColor(sf::Color::Black);
    totalScoreButtonText->setPosition({
        totalScoreButton.getPosition().x + (totalScoreButton.getSize().x / 2.f - totalScoreButtonText->getLocalBounds().size.x / 2.f),
        totalScoreButton.getPosition().y + (totalScoreButton.getSize().y / 2.f - totalScoreButtonText->getLocalBounds().size.y / 2.f)
        });

    // 총 게임 시간 버튼 설정
    totalTimeButton.setSize({ buttonWidth, buttonHeight });
    totalTimeButton.setFillColor(sf::Color::White);
    totalTimeButton.setPosition({ buttonStartX + totalScoreButton.getSize().x + spacing, 70.f });

    // 총 게임 시간 버튼 텍스트 설정
    totalTimeButtonText.emplace(font, L"총 게임 시간", 20);
    totalTimeButtonText->setFillColor(sf::Color::Black);
    totalTimeButtonText->setPosition({
        totalTimeButton.getPosition().x + (totalTimeButton.getSize().x / 2.f - totalTimeButtonText->getLocalBounds().size.x / 2.f),
        totalTimeButton.getPosition().y + (totalTimeButton.getSize().y / 2.f - totalTimeButtonText->getLocalBounds().size.y / 2.f)
        });

    // 평균 타수 버튼 설정
    avgSpeedButton.setSize({ buttonWidth, buttonHeight });
    avgSpeedButton.setFillColor(sf::Color::White);
    avgSpeedButton.setPosition({ buttonStartX + totalScoreButton.getSize().x + totalTimeButton.getSize().x + 2 * spacing, 70.f });

    // 평균 타수 버튼 텍스트 설정
    avgSpeedButtonText.emplace(font, L"평균 타수", 20);
    avgSpeedButtonText->setFillColor(sf::Color::Black);
    avgSpeedButtonText->setPosition({
        avgSpeedButton.getPosition().x + (avgSpeedButton.getSize().x / 2.f - avgSpeedButtonText->getLocalBounds().size.x / 2.f),
        avgSpeedButton.getPosition().y + (avgSpeedButton.getSize().y / 2.f - avgSpeedButtonText->getLocalBounds().size.y / 2.f)
        });


    std::cout << "RankingUI 생성자 완료" << std::endl;
    openDataFromDB();
}



RankingUI::~RankingUI() {
    std::cout << "RankingUI 소멸자 호출됨." << std::endl;
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
        const int fixedUserIDWidth = 15;  // user_id 고정 너비
        const int fixedPointWidth = 6;   // point 고정 너비 (최대 999999)

        while (res->next()) {
            string userID = res->getString("user_id");
            int point = res->getInt("point");

            // user_id 길이 맞추기 (짧으면 공백 추가, 길면 자름)
            if (userID.length() < fixedUserIDWidth) {
                userID.append(fixedUserIDWidth - userID.length(), ' ');
            }
            else {
                userID = userID.substr(0, fixedUserIDWidth);
            }

            // point를 6자리로 맞추기 (앞에 공백 추가)
            string pointStr = to_string(point);
            if (pointStr.length() < fixedPointWidth) {
                pointStr.insert(0, fixedPointWidth - pointStr.length(), ' ');
            }

            // 최종적으로 정렬된 텍스트 구성
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
                // 버튼 클릭 처리 (중복된 코드 제거)
                if (backButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    rankingWindow.close();
                }
                else if (totalScoreButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    std::cout << "Total Score 버튼 클릭됨." << std::endl;
                }
                else if (totalTimeButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    std::cout << "Total Time 버튼 클릭됨." << std::endl;
                }
                else if (avgSpeedButton.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    std::cout << "Avg Speed 버튼 클릭됨." << std::endl;
                }
                else if (scrollBar.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(rankingWindow)))) {
                    isDragging = true;
                    dragStartY = Mouse::getPosition(rankingWindow).y - scrollBar.getPosition().y;
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

        // 스크롤 처리
        sf::View scrollView = view;
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