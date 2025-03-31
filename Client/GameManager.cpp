#include "gameManager.h"
#include <locale>
#include <codecvt>
#include <thread>
#include <chrono>
#include <iostream>
#include <SFML/Network.hpp>

using namespace std;

// ID 입력창
void showIDInputScreen(sf::RenderWindow& window, sf::Font& font, std::string& clientID) {
    sf::Text idText(font, L"아이디를 입력하세요: ", 30);
    idText.setFillColor(sf::Color::White);
    idText.setPosition(sf::Vector2f(200.f, 300.f));

    sf::Text inputText(font, "", 30);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(sf::Vector2f(200.f, 350.f));

    window.clear();
    window.draw(idText);
    window.draw(inputText);
    window.display();

    bool inputComplete = false;
    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                if (textEntered->unicode == 8 && clientID.length() > 0) {  // 백스페이스
                    clientID.pop_back();
                }
                else if (textEntered->unicode < 128) {  // 문자 입력
                    clientID += static_cast<char>(textEntered->unicode);
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Enter) {
                    inputComplete = true;  // Enter 키로 입력 완료
                }
            }
        }
        inputText.setString(clientID);  // 입력된 아이디 화면에 표시
        window.clear();
        window.draw(idText);
        window.draw(inputText);
        window.display();
        if (inputComplete) {
            break;
        }
    }
}

// 로딩 화면
void showLoadingScreen(sf::RenderWindow& window, sf::Font& font) {
    sf::Text loadingText(font, L"매칭 대기 중...", 50);
    loadingText.setFillColor(sf::Color::White);
    loadingText.setPosition({ 200.f, 300.f });

    window.clear();
    window.draw(loadingText);
    window.display();
}

// 카운트다운
void showCountdown(sf::RenderWindow& window, sf::Font& font) {
    sf::Text countdownText(font, "3", 80);
    countdownText.setFillColor(sf::Color::White);
    countdownText.setPosition(sf::Vector2f(600.f, 300.f));

    for (int i = 3; i >= 1; --i) {
        countdownText.setString(std::to_string(i));
        window.clear();
        window.draw(countdownText);
        window.display();
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 1초 대기
    }
}

// 게임 시작 화면
void runGame(sf::RenderWindow& window, sf::Font& font) {
    sf::Text gameText(font, L"게임 시작!", 60);
    gameText.setFillColor(sf::Color::White);
    gameText.setPosition(sf::Vector2f(500.f, 300.f));

    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }

        window.clear();
        window.draw(gameText);
        window.display();
    }
}

// 서버에서 매칭 완료 응답 받을 때까지 대기
void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font) {
    std::cout << "waitForGameStart: 진입 " << std::endl;
    // 소켓을 non-blocking 모드로 전환
    socket.setBlocking(false);
    showLoadingScreen(window, font);

    char buffer[256] = { 0 };
    std::size_t received = 0;
    bool matched = false;

    while (window.isOpen() && !matched) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }
        }
        sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
        if (status == sf::Socket::Status::Done) {
            std::string message(buffer, received);
            std::cout << "Received message: " << message << std::endl;
            if (message.find("매칭 완료! 게임 시작!") != std::string::npos) {
                matched = true;
                sf::Text loadingText(font, L"매칭완료", 50);
                break;
            }
        }
        sf::sleep(sf::milliseconds(100));
    }

    if (matched) {
        showCountdown(window, font);
        sf::Text loadingText(font, L"showCountdown", 50);
        runGame(window, font);
        sf::Text loadingText(font, L"runGame", 50);
    }
    socket.setBlocking(true);
}
