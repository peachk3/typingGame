#include "gameManager.h"
#include <locale>
#include <codecvt>
#include <thread>
#include <chrono>
#include <iostream>
#include <SFML/Network.hpp>

using namespace std;

// ID �Է�â
void showIDInputScreen(sf::RenderWindow& window, sf::Font& font, std::string& clientID) {
    sf::Text idText(font, L"���̵� �Է��ϼ���: ", 30);
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
                if (textEntered->unicode == 8 && clientID.length() > 0) {  // �齺���̽�
                    clientID.pop_back();
                }
                else if (textEntered->unicode < 128) {  // ���� �Է�
                    clientID += static_cast<char>(textEntered->unicode);
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Enter) {
                    inputComplete = true;  // Enter Ű�� �Է� �Ϸ�
                }
            }
        }
        inputText.setString(clientID);  // �Էµ� ���̵� ȭ�鿡 ǥ��
        window.clear();
        window.draw(idText);
        window.draw(inputText);
        window.display();
        if (inputComplete) {
            break;
        }
    }
}

// �ε� ȭ��
void showLoadingScreen(sf::RenderWindow& window, sf::Font& font) {
    sf::Text loadingText(font, L"��Ī ��� ��...", 50);
    loadingText.setFillColor(sf::Color::White);
    loadingText.setPosition({ 200.f, 300.f });

    window.clear();
    window.draw(loadingText);
    window.display();
}

// ī��Ʈ�ٿ�
void showCountdown(sf::RenderWindow& window, sf::Font& font) {
    sf::Text countdownText(font, "3", 80);
    countdownText.setFillColor(sf::Color::White);
    countdownText.setPosition(sf::Vector2f(600.f, 300.f));

    for (int i = 3; i >= 1; --i) {
        countdownText.setString(std::to_string(i));
        window.clear();
        window.draw(countdownText);
        window.display();
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 1�� ���
    }
}

// ���� ���� ȭ��
void runGame(sf::RenderWindow& window, sf::Font& font) {
    sf::Text gameText(font, L"���� ����!", 60);
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

// �������� ��Ī �Ϸ� ���� ���� ������ ���
void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font) {
    std::cout << "waitForGameStart: ���� " << std::endl;
    // ������ non-blocking ���� ��ȯ
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
            if (message.find("��Ī �Ϸ�! ���� ����!") != std::string::npos) {
                matched = true;
                sf::Text loadingText(font, L"��Ī�Ϸ�", 50);
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
