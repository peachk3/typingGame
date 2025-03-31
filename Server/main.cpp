#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "networkManager.h"
#include "gameManager.h"
#include <iostream>
#include <locale>
#include <thread>
#include <codecvt>
#include <chrono>
using namespace sf;

int main() {
    int MAIN_WINDOW_WIDTH = 1280;
    int MAIN_WINDOW_HEIGHT = 720;   
    
    // windowâ 3.0 ����
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"���� ���� ȭ��");
    sf::Font font;
    
    NetworkManager client;
    GameManager game;

    // ��Ʈ �ε� �����Ͻ� ���Ӽ� Ȯ���غ���
    if (!font.openFromFile("D2Coding.ttf")) {
        std::cerr << "��Ʈ �ε� ����!" << std::endl;
        return -1;
    }

    Text buttonText(font, L"���� ����", 40);
    buttonText.setPosition(Vector2f(300.f, 250.f));
    buttonText.setFillColor(Color::White);

    RectangleShape button(Vector2f(200, 80));
    button.setPosition(Vector2f(300.f, 250.f));
    button.setFillColor(Color::Blue);

    while (window.isOpen()) {
        // ���ο� ����� �̺�Ʈ ó��
        std::optional<Event> event;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // ���콺 Ŭ�� �� ��ư Ŭ�� Ȯ��
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (button.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {

                    if (!client.connectToServer()) {
                        return -1;
                    }

                    // �ε�â ���� ���� ������
                    bool isMatching = false;
                    std::thread loadingThread([&]() {
                        game.showLoadingScreen(window, font, isMatching);
                        std::cout << "�ε�â ����!!" << std::endl;

                    });


                    if (client.waitForMatch(window, font)) {
                        isMatching = true;
                        loadingThread.join();
                        std::cout << "��ġ �Ϸ�Ǿ���!!" << std::endl;
                        game.showLoadingScreen(window, font, isMatching);  // ��Ī ���� ī��Ʈ�ٿ� ����
                        //if (game.showCountdown(window, font)) {
                        std::cout << "���� ����!!" << std::endl;
                        game.runGame(window, font);
                        //}
                    }

                }
            }
        }

        // ȭ�� �׸���
        window.clear(Color::White);
        window.draw(button);
        window.draw(buttonText);
        window.display();
    }
    return 0;
}