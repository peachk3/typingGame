// startScreen.cpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;
using namespace sf;

bool showStartScreen(RenderWindow& window) {
    // 창 생성
    Vector2f windowSize(1280, 720);
    window.create(VideoMode({ static_cast<unsigned int>(windowSize.x), static_cast<unsigned int>(windowSize.y) }), "Typing Game Start Screen");

    // 배경 이미지
    Texture bgTexture;
    if (!bgTexture.loadFromFile("assets/image/start.png")) {
        cerr << "시작 배경 이미지 로딩 실패!" << endl;
        return false;
    }
    Sprite background(bgTexture);
    background.setScale(Vector2f(
        windowSize.x / static_cast<float>(bgTexture.getSize().x),
        windowSize.y / static_cast<float>(bgTexture.getSize().y)
    ));

    // 폰트 로드
    Font font;
    if (!font.openFromFile("assets/font/D2Coding.ttf")) {
        cerr << "폰트 로딩 실패!" << endl;
        return false;
    }

    // START 버튼
    RectangleShape startButton(Vector2f(430, 140));
    startButton.setPosition({ windowSize.x / 2.f - 230, windowSize.y * 0.65f });
    startButton.setFillColor(Color(225, 225, 255, 0));  // 투명 색


    // 메인 루프
    while (window.isOpen()) {
        while (const std::optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (event->is<Event::MouseButtonPressed>()) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    return true; // 게임 시작으로 전환
                }
            }
        }

        window.clear();
        window.draw(background);
        window.draw(startButton);
        window.display();
    }

    return false;
}
