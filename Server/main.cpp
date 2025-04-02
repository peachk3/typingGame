#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "networkManager.h"
#include "gameManager.h"
#include <iostream>
#include <locale>
#include <thread>
#include <codecvt>
#include <chrono>
using namespace std;
using namespace sf;

int main() {
    int MAIN_WINDOW_WIDTH = 1280;
    int MAIN_WINDOW_HEIGHT = 720;   
    
    // window창 3.0 버전
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"게임 메인 화면");
    sf::Font font;
   
    NetworkManager client;
    GameManager game;
    
    Texture profileTexture, backGroundTexture, typingButtonTexture, matchButtonTexture, korButtonTexture;

    // 폰트 로드 실패일시 종속성 확인해보기
    if (!font.openFromFile("D2Coding.ttf")) {
        std::cerr << "폰트 로드 실패!" << std::endl;
        return -1;
    }

    // 배경 이미지 로드
    if (!backGroundTexture.loadFromFile("./assets/image/bgimg.png")) {
        cerr << "배경 이미지 로드 실패!" << endl;
        return -1;
    }

    // 이미지 로드
    if (!typingButtonTexture.loadFromFile("./assets/image/button.png")) {
        cerr << "Typing 버튼 이미지 로드 실패!" << endl;
        return -1;
    }
    if (!matchButtonTexture.loadFromFile("./assets/image/button.png")) {
        cerr << "Match 버튼 이미지 로드 실패!" << endl;
        return -1;
    }
    if (!korButtonTexture.loadFromFile("./assets/image/button.png")) {
        cerr << "koreng 버튼 이미지 로드 실패!" << endl;
        return -1;
    }
    if (!profileTexture.loadFromFile("./assets/image/user.png")) {
        cerr << "프로필 이미지 로드 실패!" << endl;
        return -1;
    }

    // 프로필 Sprite
    Sprite profileSprite(profileTexture);
    Vector2u profileTextureSize = profileTexture.getSize();
    float scalexX = 50.f / profileTextureSize.x;  // 가로 크기를 50px로 조정
    float scaleyY = 50.f / profileTextureSize.y;   // 세로 크기를 50px로 조정
    profileSprite.setScale(Vector2(scalexX, scaleyY));
    //profileSprite.setTexture(profileTexture);
    profileSprite.setPosition(Vector2f(20.f, 20.f));  // 화면 왼쪽 상단에 위치


    // 프로필 Text 설정 (왼쪽 상단)
    Text profileText(font, "My Profile", 20);
    profileText.setPosition(Vector2f(100.f, 20.f));  // 이미지 오른쪽에 텍스트 배치
    //profileText.setCharacterSize(30);
    profileText.setFillColor(Color::Black);

    // 배경 sprite 설정
    Sprite backGroundSprite(backGroundTexture);
    Vector2u backTextureSize = backGroundTexture.getSize();
    //backGroundSprite.setScale(Vector2(scaleX, scaleY));
    backGroundSprite.setTexture(backGroundTexture);
    backGroundSprite.setPosition(Vector2f(0.f, 0.f));

    // 불투명 배경 설정
    sf::RectangleShape whiteBackground(sf::Vector2f(800.f, 400.f));
    whiteBackground.setPosition({ 200.f, 150.f });
    whiteBackground.setFillColor(sf::Color(255, 255, 255, 200)); // 마지막 - 투명도
    whiteBackground.setOutlineThickness(1.f);
    whiteBackground.setOutlineColor(sf::Color(100, 100, 100)); // 테두리 색상

    // 한글 Sprite 설정
    Sprite korengButtonSprite(typingButtonTexture);
    Vector2u textureSize = typingButtonTexture.getSize();
    float scaleX = 50.f / textureSize.x;  // 가로 크기를 50px로 조정
    float scaleY = 50.f / textureSize.y;   // 세로 크기를 50px로 조정
    korengButtonSprite.setScale(Vector2(scaleX, scaleY));
    korengButtonSprite.setTexture(typingButtonTexture);
    korengButtonSprite.setPosition(Vector2f(320.f, 190.f));


    // 한글 Text 설정
    Text korengButtonText(font, L"한글 타자 연습");
    korengButtonText.setPosition(Vector2f(400.f, 200.f));  // 이미지 오른쪽에 텍스트 배치
    korengButtonText.setCharacterSize(30);
    korengButtonText.setFillColor(Color::Black);

    // 영문 Sprite 설정
    Sprite typingButtonSprite(typingButtonTexture);
    //Vector2u textureSize = typingButtonTexture.getSize();
    //float scaleX = 100.f / textureSize.x;  // 가로 크기를 100px로 조정
    //float scaleY = 50.f / textureSize.y;   // 세로 크기를 50px로 조정
    typingButtonSprite.setScale(Vector2(scaleX, scaleY));
    //typingButtonSprite.setScale(Vector2f(0.5f, 0.5f));
    typingButtonSprite.setTexture(typingButtonTexture);
    typingButtonSprite.setPosition(Vector2f(320.f, 290.f)); // x,y

     
    // 영문 Text 설정
    Text typingButtonText(font, L"영문 타자 연습");
    typingButtonText.setPosition(Vector2f(400.f, 300.f));  // 이미지 오른쪽에 텍스트 배치
    typingButtonText.setCharacterSize(30);
    typingButtonText.setFillColor(Color::Black);

    // 코딩 대결 Sprite 설정
    Sprite matchButtonSprite(typingButtonTexture);
    //Vector2u textureSize = typingButtonTexture.getSize();
    //float scaleX = 100.f / textureSize.x;  // 가로 크기를 100px로 조정 
    //float scaleY = 50.f / textureSize.y;   // 세로 크기를 50px로 조정
    matchButtonSprite.setScale(Vector2(scaleX, scaleY));
    //matchButtonSprite.setScale(Vector2f(0.5f, 0.5f));
    matchButtonSprite.setTexture(typingButtonTexture);
    matchButtonSprite.setPosition(Vector2f(320.f, 390.f));


    // 코딩 대결 Text 설정
    Text matchButtonText(font, L"코딩 대결");
    matchButtonText.setPosition(Vector2f(400.f, 400.f));  // 이미지 오른쪽에 텍스트 배치
    matchButtonText.setCharacterSize(30);
    matchButtonText.setFillColor(Color::Black);


    // 스프라이트 설정
    typingButtonSprite.setTexture(typingButtonTexture);
    matchButtonSprite.setTexture(matchButtonTexture);
    korengButtonSprite.setTexture(korButtonTexture);
    //profileSprite.setTexture(profileTexture);

    Text buttonText(font, L"게임 시작", 40);
    buttonText.setPosition(Vector2f(300.f, 250.f));
    buttonText.setFillColor(Color::White);

    // 본래 파란색 버튼
    //RectangleShape button(Vector2f(200, 80));
    //button.setPosition(Vector2f(300.f, 250.f));
    //button.setFillColor(Color::Blue);

    while (window.isOpen()) {
        // 새로운 방식의 이벤트 처리
        std::optional<Event> event;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // 마우스 클릭 시 버튼 클릭 확인
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (matchButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                    cout << "1:1 Match Button Clicked!" << endl;

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
                        if (loadingThread.joinable()) loadingThread.join();
                        //loadingThread.join();
                        std::cout << "매치 완료되었슴!!" << std::endl;
                        //game.showLoadingScreen(window, font, isMatching);  // 매칭 이후 카운트다운 실행
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
        window.draw(backGroundSprite); // 배경 사진 
        window.draw(whiteBackground);  // 하얀색 배경 
        window.draw(profileSprite);
        window.draw(profileText);    // 왼쪽 상단에 profileText 그리기
        window.draw(typingButtonSprite);  // Typing 버튼 이미지 그리기
        window.draw(typingButtonText);  // Typing Text 이미지 그리기
        window.draw(matchButtonSprite);  // 1:1 Match 버튼 이미지 그리기
        window.draw(matchButtonText);  // match Text 그리기
        window.draw(korengButtonSprite);  // 한영 버튼 이미지 그리기
        window.draw(korengButtonText);  // 한영 Text 이미지 그리기
        //window.draw(button);
        //window.draw(buttonText);
        window.display();
    }
    return 0;
}