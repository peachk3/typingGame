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
    
    // window창 3.0 버전
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"게임 메인 화면");
    sf::Font font;
    
    NetworkManager client;
    GameManager game;

    // 폰트 로드 실패일시 종속성 확인해보기
    if (!font.openFromFile("D2Coding.ttf")) {
        std::cerr << "폰트 로드 실패!" << std::endl;
        return -1;
    }

    Text buttonText(font, L"게임 시작", 40);
    buttonText.setPosition(Vector2f(300.f, 250.f));
    buttonText.setFillColor(Color::White);

    RectangleShape button(Vector2f(200, 80));
    button.setPosition(Vector2f(300.f, 250.f));
    button.setFillColor(Color::Blue);

    while (window.isOpen()) {
        // 새로운 방식의 이벤트 처리
        std::optional<Event> event;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // 마우스 클릭 시 버튼 클릭 확인
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (button.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {

                    if (!client.connectToServer()) {
                        return -1;
                    }

                    // 로딩창 실행 별도 스레드
                    bool isMatching = false;
                    std::thread loadingThread([&]() {
                        game.showLoadingScreen(window, font, isMatching);
                        std::cout << "로딩창 띄우기!!" << std::endl;

                    });


                    if (client.waitForMatch(window, font)) {
                        isMatching = true;
                        loadingThread.join();
                        std::cout << "매치 완료되었슴!!" << std::endl;
                        game.showLoadingScreen(window, font, isMatching);  // 매칭 이후 카운트다운 실행
                        //if (game.showCountdown(window, font)) {
                        std::cout << "게임 시작!!" << std::endl;
                        game.runGame(window, font);
                        //}
                    }

                }
            }
        }

        // 화면 그리기
        window.clear(Color::White);
        window.draw(button);
        window.draw(buttonText);
        window.display();
    }
    return 0;
}