#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#pragma once    // <<------- 중복헤더 한번만 참조하게 해주는 전처리기 C++ 에선 이거 쓰십쇼
#include "gameManager.h"
#include "networkManager.h"
#include <locale>
#include <codecvt>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>  // 문자열 스트림을 사용하기 위한 헤더
#include <iomanip>  // 시간을 형식에 맞게 출력하기 위한 헤더
#include <SFML/Network.hpp>
//#include <SFML/Network.hpp>
//#include <iostream>
//#include <NetworkManager.cpp>

GameManager game;

//NetworkManager network;
//bool connectToServer() {
//    if (socket.connect({ 210, 119, 12, 65}, 53000) == sf::Socket::Status::Done) {
//        std::cout << "서버 연결 성공!" << std::endl;
//        return true;
//    }
//    else {
//        std::cerr << "서버 연결 실패!" << std::endl;
//        return false;
//    }
//}



//void ServerManager::handleClient(sf::TcpSocket* client) {
//    while (true) {
//        sf::Packet packet;
//        if (client->receive(packet) == sf::Socket::Status::Done) {
//            std::string tag;
//            float x, y;
//            packet >> tag >> x >> y;
//
//            if (tag == "MOVE") {
//                std::cout << "클라이언트 이동 감지: (" << x << ", " << y << ")" << std::endl;
//
//                // 모든 클라이언트에게 이동 데이터 전송
//                for (auto& c : connectedClients) {
//                    sf::Packet sendPacket;
//                    sendPacket << "MOVE" << x << y;
//                    c->send(sendPacket);
//                }
//            }
//        }
//    }
//}

// 플레이어 위치 갱신 함수

//void GameManager::updatePlayerPosition(int clientID, float x, float y) {
//    std::map<int, sf::Sprite*> players;
//    players[1] = &player1Sprite;
//    players[2] = &player2Sprite;
//    if (players.find(clientID) != players.end()) {  // 플레이어가 존재하면
//        players[clientID].setPosition(x, y);  // 위치 업데이트
//        std::cout << "플레이어 " << clientID << " 위치 갱신: (" << x << ", " << y << ")" << std::endl;
//    }
//    else {
//        std::cerr << "플레이어 " << clientID << "를 찾을 수 없음!" << std::endl;
//    }
//}

// GameManager.cpp에서 sendMove() 호출하는 방법 : NetWorkManager를 통해서 실행
//void GameManager::playerMove(float x, float y) {
//    // 클라이언트가 이동하면 서버에 전송
//    network.sendMove(x, y);
//}


using namespace sf;
using namespace std;
//GameManager game;
//NetworkManager client;
//NetworkManager::getInstance();
NetworkManager& client = NetworkManager::getInstance();
// 화면 크기 정의
const float SCREEN_WIDTH = 1280.f;
const float SCREEN_HEIGHT = 720.f;
const int GWINDOW_WIDTH = 300;
const int GWINDOW_HEIGHT = 250;
const float ROTATION_SPEED = 60.f;  // 회전 속도 (1초당 n도 회전)

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

// 로딩창 설정
void GameManager::showLoadingScreen(sf::RenderWindow& window, sf::Font& font, bool& isMatching) {

    RenderWindow gameWindow(VideoMode({ GWINDOW_WIDTH, GWINDOW_HEIGHT }), L"게임 화면");

    // 텍스처와 스프라이트 로드
    Texture texture;
    if (!texture.loadFromFile("C:/Project/gameProject/typingGame/assets/image/loading.png")) {  // "loading.png"는 준비해야 함
        cerr << "이미지 로드 실패!" << endl;
        return;
    }
     
    Sprite sprite(texture);

    //sprite.setPosition(Vector2f(float(GWINDOW_WIDTH / 2), float(GWINDOW_HEIGHT / 2))); // 초기 위치 설정

    // 스프라이트 원점 설정 (중앙으로 회전하도록)
    sprite.setOrigin(Vector2f(texture.getSize().x / 2.f, texture.getSize().y / 2.f));
    sprite.setPosition(Vector2f(GWINDOW_WIDTH / 2, GWINDOW_HEIGHT / 2));  // 화면 중앙으로 이동

    Text statusText(font, L"상대 검색중...", 30);
    statusText.setFillColor(Color::Black);
    statusText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 75, (GWINDOW_HEIGHT / 2) - 100));

    Text timerText(font, "00:00", 30);  // 시간 표시 텍스트
    timerText.setFillColor(Color::Black);
    timerText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 35, (GWINDOW_HEIGHT / 2) - 70));

    Text matchingText(font, L"상대 연결 완료!", 30);  // Matching! 텍스트
    matchingText.setFillColor(Color::Black);
    matchingText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 100, (GWINDOW_HEIGHT / 2) - 100));

    Text countdownText(font, "", 50);  // 카운트다운 텍스트
    countdownText.setFillColor(Color::Black);
    countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 10, (GWINDOW_HEIGHT / 2 - 20)));  // Matching! 아래에 표시

    Clock clock;  // 게임 실행 시간 측정을 위한 시계
    Clock countdownClock;  // 카운트다운을 위한 시계
    Time elapsedTime;

    //bool isMatching = false;  // 매칭 상태 추적
    int countdown = 3;  // 카운트다운 시작
    bool countdownStarted = false;
    bool gameStartDisplayed = false;


    // 서버에서 받은 카운트다운 시작 시각 (서버 시간)
    // 이 값은 서버와 연결된 네트워크 코드에서 설정해야 함
    Time serverStartTime;
    bool serverTimeReceived = false;
    bool gameStarted = false;

    while (gameWindow.isOpen()) {
        // 이벤트 처리 (SFML 3.0.0 방식)
        std::optional<Event> event;
        while (const std::optional event = gameWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                std::cout << "gameWindow.close()" << std::endl;
                gameWindow.close();
                return;
            }
        }

        // 네트워크에서 매칭 정보 및 카운트다운 시작 시간 수신 (예시)
        // 실제로는 네트워크 코드에서 이 변수들을 설정해야 합니다
        if (isMatching && !serverTimeReceived) {
            // 서버로부터 카운트다운 시작 시간 수신 (예시)
            // serverStartTime = 서버에서 받은 시간
            cout << "카운트다운 시작!" << endl;
            serverTimeReceived = true;
            countdownClock.restart(); // 서버 시간을 받은 시점부터 로컬 시계 시작
        }
        // 매칭이 된 경우 카운트다운 처리
        if (isMatching && serverTimeReceived) {
            //cout << "isMatching: " << isMatching << "serverTimeReceived:" << serverTimeReceived << endl;
            gameWindow.clear(Color::White);
            gameWindow.draw(matchingText);

            // 서버 시간에 맞춰 로컬 카운트다운 계산
            float elapsedCountdownTime = countdownClock.getElapsedTime().asSeconds();

            // 카운트다운 값 계산 (서버 시간 기준)
            int currentCountdown = countdown - static_cast<int>(elapsedCountdownTime);
            //std::cout << "currentCountdown: " << currentCountdown << std::endl;
            if (currentCountdown <= 0 && !gameStarted) {
                // 카운트다운 종료, 게임 시작
                countdownText.setString("Start Game!");
                countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 135, GWINDOW_HEIGHT / 2 - 50));
                countdown = 0;  // 카운트다운 시작
                gameWindow.draw(countdownText);
                //gameWindow.draw(countdownText);
                //gameWindow.display();
                std::cout << "잠시후 게임 시작!!" << std::endl;
                gameStarted = true;  // 게임 시작 플래그 설정
                isMatching = 0;  // 매칭 상태 변경 (중복 실행 방지)
                // 잠시 대기 후 게임 시작
                std::this_thread::sleep_for(std::chrono::seconds(1));
                gameWindow.close();
                return;
            }
            else {
                // 카운트다운 표시
                if (gameStarted != true) {
                    countdownText.setString(to_string(currentCountdown));
                    gameWindow.draw(countdownText);
                }
                else {
                    gameWindow.close();
                    return;
                }
            }

            gameWindow.display();
        }

        // 매칭 실패했을 경우
        else if (gameStarted != true) {
            // deltaTime을 계산하여 회전 속도 적용
            Time deltaTime = clock.restart();  // 이전 프레임에서 경과한 시간
            elapsedTime += deltaTime;  // 전체 경과 시간 누적

            // 이미지 360도 회전
            sprite.rotate(degrees(ROTATION_SPEED) * deltaTime.asSeconds());  // 초당 ROTATION_SPEED도씩 회전

            // 경과된 시간 계산 (초 단위로)
            int seconds = static_cast<int>(elapsedTime.asSeconds());
            int minutes = seconds / 60;
            seconds = seconds % 60;

            // 00:00 형식으로 시간 텍스트 업데이트
            stringstream ss;
            ss << setw(2) << setfill('0') << minutes << ":" << setw(2) << setfill('0') << seconds;
            timerText.setString(ss.str());

            // 화면 그리기
            gameWindow.clear(Color::White);
            gameWindow.draw(sprite);  // 회전하는 이미지 그리기
            gameWindow.draw(statusText);  // 연결 상태 메시지 그리기
            gameWindow.draw(timerText);   // 시간 텍스트 그리기
            gameWindow.display();

        }
    }
}


//void GameManager::updatePlayerPosition(int playerID, float x, float y, float distance) {
//    sf::Packet packet;
//    packet << "MOVE" << playerID << x << y << distance;  // 이동 정보 패킷 생성
//
//    if (serverSocket.send(packet) != sf::Socket::Status::Done) {
//        std::cerr << "서버에 위치 데이터 전송 실패!" << std::endl;
//    }
//}
// 폰트 NanumGothic //

// 게임 시작
void GameManager::runGame(sf::RenderWindow& window, sf::Font& font) {


    std::cout << "게임 시작창 이동 완료!!" << std::endl;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    Texture ladderImg, bgTexture;
   

    if (!bgTexture.loadFromFile("C:/Project/gameProject/typingGame/assets/image/bgimg_game.png")) {
        cerr << "게임 배경 이미지 로드 실패!" << endl;
        //return -1; // 이미지 로드 실패 시 종료
    }
    Sprite bgSprite(bgTexture);
    bgSprite.setTexture(bgTexture);
    bgSprite.setPosition(Vector2f(0.f, 0.f));

    // 이미지 로드
    if (!ladderImg.loadFromFile("C:/Project/gameProject/typingGame/assets/image/bgimg_ladder.png")) {
        cerr << "사다리 이미지 로드 실패!" << endl;
        //return;
    }

    // 플레이어 이미지 로드
    Texture player1Texture, player2Texture;
    if (!player1Texture.loadFromFile("C:/Project/gameProject/typingGame/assets/image/player1_1.png") || !player1Texture.loadFromFile("C:/Project/gameProject/typingGame/assets/image/player1_2.png")) {
        cout << "플레이어 이미지 로드 실패!" << endl;
        //return -1;
    }
    if (!player2Texture.loadFromFile("C:/Project/gameProject/typingGame/assets/image/player2_1.png") || !player2Texture.loadFromFile("C:/Project/gameProject/typingGame/assets/image/player2_2.png")) {
        cout << "플레이어 이미지 로드 실패!" << endl;
        //return -1;
    }

    // 초기 위치 설정
    // 플레이어1 애니메이션 스프라이트 로드
    std::vector<Texture> player1Textures(3);
    player1Textures[0].loadFromFile("C:/Project/gameProject/typingGame/assets/image/player1_1.png");
    player1Textures[1].loadFromFile("C:/Project/gameProject/typingGame/assets/image/player1_2.png");
    // player1Textures[2].loadFromFile("player1_3.png");
    Sprite player1Sprite(player1Textures[0]);
    player1Sprite.setPosition(Vector2f(20.f, 0.f));
    int player1Frame = 0;

    // 플레이어2 애니메이션 스프라이트 로드
    std::vector<Texture> player2Textures(3);
    player2Textures[0].loadFromFile("C:/Project/gameProject/typingGame/assets/image/player2_1.png");
    player2Textures[1].loadFromFile("C:/Project/gameProject/typingGame/assets/image/player2_2.png");
    //player2Textures[2].loadFromFile("player1_3.png");
    Sprite player2Sprite(player2Textures[0]);
    player2Sprite.setPosition(Vector2f(20.f, 45.f));
    int player2Frame = 0;

    //float moveDistance = 20.f; // 한 문장 입력 시 이동 거리


    sf::Sprite spriteLadder(ladderImg);
    // 사다리 배경 설정
    spriteLadder.setTexture(ladderImg);
    spriteLadder.setPosition(Vector2f(0.f, 0.f));
    /*
    // 사다리 1 이미지 설정
    Sprite ladderImgSpriteLeft(ladderImg);
    //Vector2u ladderImgSize = ladderImg.getSize();
    //// 원하는 크기로 조정 (예: 200x200)
    //float scaleX = 30.f / ladderImgSize.x;
    //float scaleY = 50.f / ladderImgSize.y;
    ladderImgSpriteLeft.setTexture(ladderImg);
    ladderImgSpriteLeft.setPosition(Vector2f(100.f, 150.f));     // 왼쪽

    // 사다리 2 이미지 설정
    Sprite ladderImgSpriteRight(ladderImg);
    //Vector2u ladderImgSize = ladderImg.getSize();
    //// 원하는 크기로 조정 (예: 200x200)
    //float scaleX = 30.f / ladderImgSize.x;
    //float scaleY = 50.f / ladderImgSize.y;
    ladderImgSpriteRight.setTexture(ladderImg);
    ladderImgSpriteRight.setPosition(Vector2f(1080.f, 150.f));     // 오른쪽 
    */
    sf::Text gameText(font, L"게임 시작~~!", 60);

    gameText.setFillColor(Color::Red);
    gameText.setPosition(Vector2f(480.f, 50.f)); // (왼쪽 -> 오른쪽, 위 -> 아래)

    //------------------------------------타자 입력창 만들기------------------------------------
    // 타자 연습 문장 리스트
    vector<wstring> sentences = { L"A", L"A", L"A",L"A", L"A", L"A",L"A", L"A", L"A", L"A",L"A", L"A", L"A", L"A",  L"A", L"A",L"A", L"A", L"A", L"A", L"A", L"A", L"A",L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"Z"};
    size_t currentSentenceIndex = 0;
    wstring userInput;

    // 현재 문장 표시
    Text currentSentence(font, sentences[currentSentenceIndex], 50);
    FloatRect textBounds = currentSentence.getGlobalBounds();
    currentSentence.setOrigin(textBounds.getCenter());
    currentSentence.setFillColor(Color::Black);
    currentSentence.setPosition(Vector2f(1280.f / 2, 300.f)); // 가운데 정렬

    // 입력창 배경 생성
    sf::RectangleShape inputBackground(sf::Vector2f(500.f, 40.f)); // 입력창 크기
    FloatRect inputTextBounds = inputBackground.getGlobalBounds();
    inputBackground.setOrigin(inputTextBounds.getCenter());

    inputBackground.setPosition({ 1280.f / 2, 500.f });
    inputBackground.setFillColor(sf::Color(240, 240, 240)); // 밝은 회색 배경
    inputBackground.setOutlineThickness(2.f);
    inputBackground.setOutlineColor(sf::Color(100, 100, 100)); // 테두리 색상

    // 다음 문장 표시
    Text nextSentence(font, L"", 20);
    if (currentSentenceIndex + 1 < sentences.size()) {
        nextSentence.setString(sentences[currentSentenceIndex + 1]);
    }
    FloatRect nextTextBounds = nextSentence.getGlobalBounds();
    nextSentence.setOrigin(nextTextBounds.getCenter());
    nextSentence.setFillColor(Color(32, 32, 32)); // 검은색에 가까운 회색
    nextSentence.setPosition(Vector2f(1280.f / 2, 400.f)); // 가운데

    // 불투명 배경 설정
    sf::RectangleShape Background(sf::Vector2f(400.f, 70.f));
    // 배경 크기 가져오기
    FloatRect bgBounds = Background.getGlobalBounds();
    Background.setOrigin(bgBounds.getCenter()); // 중앙 기준 정렬

    Background.setFillColor(sf::Color(224, 224, 224, 200)); // 마지막 - 투명도
    Background.setPosition(Vector2f(1280.f / 2, 400.f));
    //Background.setOutlineThickness(1.f);
    //Background.setOutlineColor(sf::Color(139, 0, 0)); // 테두리 색상

    // 입력 표시
    Text userInputText(font, L"", 30);
    userInputText.setFillColor(Color::Green);
    userInputText.setPosition(Vector2f(400.f, 485.f)); // 화면 정중앙

    //--------------------------------------------------------------------------------------------

    // userID 받아와서 출력 (
    wstring user1ID = L"사용자 1"; // id 받아와서 넣기
    wstring user2ID = L"사용자 2"; // id 받아와서 넣기

    Text user1Text(font, user1ID, 20);
    Text user2Text(font, user2ID, 20); 
    user1Text.setFillColor(Color::Black);
    user2Text.setFillColor(Color::Black);

    user1Text.setPosition(Vector2f(600.f, 500.f)); // (왼쪽 -> 오른쪽, 위 -> 아래)
    user2Text.setPosition(Vector2f(600.f, 100.f)); // (왼쪽 -> 오른쪽, 위 -> 아래)

    window.clear(Color::White);
    window.draw(bgSprite);
    window.draw(spriteLadder);              // 사다리 bg
    window.draw(player1Sprite);             // player1
    window.draw(player2Sprite);             // player2
    window.draw(Background);
    window.draw(gameText);
    window.draw(userInputText);
    //window.draw(ladderImgSpriteLeft);       // 왼쪽
    //window.draw(ladderImgSpriteRight);      // 오른쪽
    window.draw(currentSentence);
    window.draw(nextSentence);
    //window.draw(user1Text);
    //window.draw(user2Text);
    window.draw(inputBackground);
    window.display();

    // 커서 설정 추가
    sf::Clock cursorClock;
    bool showCursor = true;
    sf::RectangleShape cursor(sf::Vector2f(2.f, 30.f));
    cursor.setFillColor(sf::Color::Black);
    cursor.setPosition({ 400.f, 485.f });


    // 이동 방향 정의 (0: 오른쪽, 1: 아래쪽, 2: 왼쪽, 3: 위쪽)
    int player1Direction = 0;
    int player2Direction = 0;
    float player1MoveDist = 0;
    float player2MoveDist = 0;
    client.addPlayer(1, &player1Sprite);
    client.addPlayer(2, &player2Sprite);
    // 잔상 효과를 위한 반투명 레이어
    sf::RectangleShape fadeEffect(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    fadeEffect.setFillColor(sf::Color(255, 255, 255, 50));  // 반투명한 흰색


    while (window.isOpen()) {
        bool isWindowClosed = false;

        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                isWindowClosed = true;
                break;
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    isWindowClosed = true;
                    break;
                }
            }   
  
            // 텍스트 입력 처리
            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                if (textEntered->unicode == 8) { // 백스페이스 처리
                    if (!userInput.empty()) {
                        userInput.pop_back();
                    }
                }
                else if (textEntered->unicode == 13) { // 엔터 입력 시 이동 및 애니메이션A
                    if (userInput == sentences[currentSentenceIndex]) {
                        currentSentenceIndex++;
                        int check = 0;
                        if (currentSentenceIndex < sentences.size()) {
                            for (int i = 0; i < 6; i++) {
                                player1Sprite.setColor(sf::Color(255, 255, 255, 255));
                                // Player1 가로 이동거리: 1240px, 세로 이동거리: 680px > (1240+680) * 2 = 3840px
                                // Player2 가로 이동거리: 1150px, 세로 이동거리: 590px > (1150+590) * 2 = 3480px
                                // 문장 수에 따라 이동속도 결정하도록 수정 필요, player2 이동속도 = player1 이동속도 / 1.1034
                                //  1. 방향에 따라 먼저 이동
                                switch (player1Direction) {
                                case 0:  // 오른쪽 이동
                                    player1Sprite.move(Vector2f(25.f, 0.f));    // x, y 변수 만들기 // 엔터 쳐지면 서버에 Direction 메세지 보내기
                                    break;                                      // 서버에서 Direction 받아서 처리
                                case 1:  // 아래쪽 이동                         // 클라1 함수, 클라2 함수 작성
                                    player1Sprite.move(Vector2f(0.f, 25.f));
                                    break;
                                case 2:  // 왼쪽 이동
                                    player1Sprite.move(Vector2f(-25.f, 0.f));
                                    break;
                                case 3:  // 위쪽 이동
                                    player1Sprite.move(Vector2f(0.f, -25.f));
                                    break;
                                }
                                

                                //  2. 이동 후 위치 확인하여 회전 적용
                                if (player1Sprite.getPosition().x >= SCREEN_WIDTH - 5 && player1Direction == 0) {
                                    player1Direction = 1;  // 아래로 이동
                                    player1Sprite.rotate(degrees(90.f));  // 90도 회전
                                }
                                else if (player1Sprite.getPosition().y >= SCREEN_HEIGHT - 5 && player1Direction == 1) {
                                    player1Direction = 2;  // 왼쪽 이동
                                    player1Sprite.rotate(degrees(90.f));;  // 180도 회전
                                }
                                else if (player1Sprite.getPosition().x - 5 <= 0 && player1Direction == 2) {
                                    player1Direction = 3;  // 위로 이동
                                    player1Sprite.rotate(degrees(90.f));;  // 270도 회전
                                }
                                else if (player1Sprite.getPosition().y - 5 <= 0 && player1Direction == 3) {
                                    player1Direction = 0;  // 오른쪽 이동
                                    player1Sprite.rotate(degrees(90.f));;  // 0도 회전
                                }
                                // 현재 위치 가져오기
                            
                                
                                //// 서버에 MOVE 메시지 전송
                                //sf::Packet packet;
                                //packet << "MOVE" << x << y;
                                //if (ServerManager::litener.send(packet) != sf::Socket::Status::Done) {
                                //    std::cerr << "서버에 위치 데이터 전송 실패!" << std::endl;
                                //}
                                
                                // player2는 player1보다 이동속도가 'n/1.1034' 느리게 이동해야함(이동거리 보정)
                                //  1. 방향에 따라 먼저 이동
                                switch (player2Direction) {
                                case 0:  // 오른쪽 이동
                                    player2Sprite.move(Vector2f(22.66f, 0.f));
                                    break;
                                case 1:  // 아래쪽 이동
                                    player2Sprite.move(Vector2f(0.f, 22.66f));
                                    break;
                                case 2:  // 왼쪽 이동
                                    player2Sprite.move(Vector2f(-22.66f, 0.f));
                                    break;
                                case 3:  // 위쪽 이동
                                    player2Sprite.move(Vector2f(0.f, -22.66f));
                                    break;
                                }

                                //  2. 이동 후 위치 확인하여 회전 적용
                                if (player2Sprite.getPosition().x >= SCREEN_WIDTH - 50 && player2Direction == 0) {
                                    player2Direction = 1;  // 아래로 이동
                                    player2Sprite.rotate(degrees(90.f));  // 90도 회전
                                }
                                else if (player2Sprite.getPosition().y >= SCREEN_HEIGHT - 50 && player2Direction == 1) {
                                    player2Direction = 2;  // 왼쪽 이동
                                    player2Sprite.rotate(degrees(90.f));;  // 180도 회전
                                }
                                else if (player2Sprite.getPosition().x - 50 <= 0 && player2Direction == 2) {
                                    player2Direction = 3;  // 위로 이동
                                    player2Sprite.rotate(degrees(90.f));;  // 270도 회전
                                }
                                else if (player2Sprite.getPosition().y - 50 <= 0 && player2Direction == 3) {
                                    player2Direction = 0;  // 오른쪽 이동
                                    player2Sprite.rotate(degrees(90.f));;  // 0도 회전
                                }

                                //client.receiveMoves();
                                //-----추가
                                // 서버 메시지 수신 스레드 실행
                                //client.startReceiveThread();
                                 
                                //cout << "startReceiveThread() 실행" << endl;
                                //while (true) {
                                
                                //std::cout << "이동 좌표 (x, y): " << x << ", " << y << endl;
                                
                                //network.receiveMoves();
                                //cout << "receiveMoves() 실행" << endl;
                               // }
                                //------
                                player1MoveDist += 25.0f;
                                player2MoveDist += 22.66f;
                                if (player1MoveDist >= 3840.f) {
                                    cout << "player1 승리" << endl;
                                }
                                if (player2MoveDist >= 3480.f) {
                                    cout << "player2 승리" << endl;
                                }
                                
                                
                                window.draw(fadeEffect);
                                window.draw(player1Sprite);
                                window.draw(player2Sprite);
                                window.display();
                                sf::sleep(sf::milliseconds(100));
                            }
                            float x = player1Sprite.getPosition().x;
                            float y = player1Sprite.getPosition().y;
                            client.receiveMoves();
                            std::cout << "아이디 : " << game.getClientID() << ", x: " << x << ", y: " << y << std::endl;
                            client.sendMove(NetworkManager::getInstance().getSocket(), game.getClientID(), x, y);  // 이놈에서 끊겼다가
                            //client.startReceiveThread();
                            /*game.playerMove(x, y);*/
                            check++;

                            userInput.clear();
                            currentSentence.setString(sentences[currentSentenceIndex]);
                            nextSentence.setString((currentSentenceIndex + 1 < sentences.size()) ? sentences[currentSentenceIndex + 1] : L"");
                        }
                        else {
                            gameText.setString(L"게임 종료!");
                            //client.stopReceiveThread();
                            currentSentence.setString(L"");
                            nextSentence.setString(L"");
                        }
                    }
                }
                else {
                    userInput += static_cast<wchar_t>(textEntered->unicode);
                }
                userInputText.setString(converter.to_bytes(userInput));
            }
            
        }

        if (isWindowClosed) {
            break;
        }

        // 커서 깜박임 업데이트
        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
        // 서버에서 메시지 수신
        //receiveMoves();
        //window.clear(Color::White);
        window.draw(spriteLadder);
        window.draw(fadeEffect);  // 잔상 효과 추가
        window.draw(player1Sprite);
        window.draw(player2Sprite);
        window.draw(Background);
        window.draw(gameText);
        window.draw(userInputText);
        /*window.draw(ladderImgSpriteLeft);
        window.draw(ladderImgSpriteRight);*/
        window.draw(currentSentence);
        window.draw(nextSentence);
        window.draw(inputBackground);
        window.draw(userInputText);
   
        if (showCursor) {
            window.draw(cursor);
        }

        window.display();
    }

}
