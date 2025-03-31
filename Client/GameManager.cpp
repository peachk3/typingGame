#include "gameManager.h"
#include <locale>
#include <codecvt>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>  // 문자열 스트림을 사용하기 위한 헤더
#include <iomanip>  // 시간을 형식에 맞게 출력하기 위한 헤더
#include <SFML/Network.hpp>

using namespace sf;
using namespace std;

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
    if (!texture.loadFromFile("./loading.png")) {  // "loading.png"는 준비해야 함
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
    int countdown = 5;  // 카운트다운 시작
    bool countdownStarted = false;
    bool gameStartDisplayed = false;

    while (gameWindow.isOpen()) {
        // 이벤트 처리 (SFML 3.0.0 방식)
        std::optional<Event> event;
        while (const std::optional event = gameWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                gameWindow.close();
                return;
            }
        }

        // 매칭이 된 후에만 화면 클리어 및 매칭 카운트다운
        if (isMatching) {
            gameWindow.clear(Color::Blue); // 매칭 후 화면을 클리어
            gameWindow.draw(matchingText);  // Matching 텍스트 출력

            //countdownStarted = true;
            //
            //if (countdownTime > 1.0f)
            // 매칭 후 첫 실행 시 카운트 시계 시작
            //if (!countdownStarted) {  // 1초마다 카운트다운 업데이트
            //    countdownClock.restart();
            //    countdownStarted = true;
            //    //gameWindow.draw(countdownText);  // 카운트다운 텍스트 출력

            //}
            // 카운트다운 처리
            float countdownTime = countdownClock.getElapsedTime().asSeconds();

            //if (countdown > 0) {
            //    //countdown--;
            //    gameWindow.clear(Color::Red);
            //    gameWindow.draw(matchingText);
            //    gameWindow.draw(countdownText);  // 카운트다운 텍스트 출력

            //}

            // 1초마다 카운트다운 감소
            if (countdownTime > 1.0f) {
                countdownClock.restart();

                //countdownText.setString("Start Game!");
                if (countdown > 0) {
                    countdown--; 
                }

                if (countdown == 0) {
                    matchingText.setString("");
                    countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 135, GWINDOW_HEIGHT / 2 - 50));  // Matching! 아래에 표시
                    
                    //countdownText.setString("Start Game!");
                    // 카운트다운 후 게임 시작 코드 추가 가능
                    return;
                }
                else {
                    countdownText.setString(to_string(countdown));
                }

                //if (countdown > 0) {
                //    gameWindow.clear(Color::Black);
                //    gameWindow.draw(matchingText);
                //    gameWindow.draw(countdownText);  // 카운트다운 텍스트 출력

                //    ////matchingText.setString("");
                //    //countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 135, GWINDOW_HEIGHT / 2 - 50));  // Matching! 아래에 표시
                //    //Text countdownText(font, L"게임 시작!", 50);  // Matching! 텍스트
                //    //// 카운트다운 후 게임 시작 코드 추가 가능

                //    //gameWindow.display();
                //    //std::this_thread::sleep_for(std::chrono::seconds(1));  // 1초 대기
                //    //gameWindow.close();  // 로딩 창 닫기
                //    return;  // 함수 종료
                //}
                //else {
                //    // 카운트다운 끝, "게임 시작!" 표시
                //    gameWindow.clear(Color::Magenta);
                //    //gameWindow.draw(startText);
                //    gameWindow.display();

                //    // 1초 대기 후 창 닫기
                //    std::this_thread::sleep_for(std::chrono::seconds(1));
                //    gameWindow.close();
                //    return;  // 함수 종료
                //}
            }
            gameWindow.draw(countdownText);  // 카운트다운 텍스트 출력
            gameWindow.display();

            /*else {
                countdownText.setString(to_string(countdown));
            }*/

            //gameWindow.draw(countdownText);  // 카운트다운 텍스트 출력
            //gameWindow.display();

            //if (countdown == 0) {
            //    countdownText.setString(L"게임 시작!");
            //    gameWindow.display();
            //    std::this_thread::sleep_for(std::chrono::seconds(1));  // 1초 대기 후 창 닫기

            //    gameWindow.close();  // 카운트다운 종료 후 로딩창 닫기
            //    return;
            //}
        }
        // 매칭 실패했을 경우
        else {
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

        //std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 대기

        //sf::Font font;
        //font.openFromFile("D2Coding.tff");
        //sf::Text loadingText(font, L"매칭 대기 중...", 50);
        ////loadingText.setFont(font);
        ////loadingText.setCharacterSize(50);
        //loadingText.setFillColor(sf::Color::White);
        //loadingText.setPosition(sf::Vector2f(600.f, 300.f));

        //window.clear();
        //window.draw(loadingText);
        //window.display();
    }
}

//bool GameManager::showCountdown(sf::RenderWindow& window, sf::Font& font) {
//    //sf::Font font;
//    //font.openFromFile("D2Coding.tff");
//
//    sf::Text countdownText(font, '3', 80);
//    //countdownText.setFont(font);
//    //countdownText.setCharacterSize(80);
//    countdownText.setFillColor(sf::Color::White);
//    countdownText.setPosition(sf::Vector2f(600.f, 300.f));
//
//    for (int i = 3; i >= 1; --i) {
//        countdownText.setString(std::to_wstring(i));
//
//        window.clear();
//        window.draw(countdownText);
//        window.display();
//
//        while (std::optional event = window.pollEvent()) {
//
//            if (event->is<sf::Event::Closed>())
//            {
//                window.close();
//            }
//        }
//        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 대기
//    }
//
//    window.clear();
//    window.draw(countdownText);
//    window.display();
//
//    //std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 대기
//
//    //sf::Text startText(font, L"게임 시작!@!@!@!!", 50);
//
//    //startText.setFillColor(sf::Color::White);
//    //startText.setPosition(sf::Vector2f(500.f, 300.f));
//
//    //window.clear();
//    //window.draw(startText);
//    //window.display();
//    //
//    return true;
//}

void GameManager::receiveUpdates() {
    //char buffer[128];
    //std::size_t received;
    //socket.setBlocking(false); // 논블로킹 모드 설정

    //while (!gameOver) {
    //    sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
    //    if (sf::Socket::Done) {
    //        std::lock_guard<std::mutex> lock(mtx);
    //        std::string data(buffer, received);
    //        std::cout << "받은 메시지: " << data << "\n";

    //        if (data.rfind("ID ", 0) == 0) {
    //            myID = std::stoi(data.substr(3));
    //            std::cout << "내 ID: " << myID << "\n";
    //        }
    //        else if (data.rfind("PROGRESS ", 0) == 0) {
    //            int receivedID, newProgress;
    //            sscanf(data.c_str(), "PROGRESS %d %d", &receivedID, &newProgress);

    //            if (receivedID == myID) {
    //                myProgress = newProgress;
    //            }
    //            else {
    //                opponentProgress = newProgress;
    //            }
    //        }
    //        else if (data == "WIN") {
    //            gameOver = true;
    //            std::cout << "승리입니다.\n";
    //        }
    //        else if (data == "LOSE") {
    //            gameOver = true;
    //            std::cout << "패배입니다.\n";
    //        }
    //    }
    //}
}

void GameManager::runGame(sf::RenderWindow& window, sf::Font& font) {
    //if (!font.openFromFile("D2Coding.tff")) { // 한글 지원 폰트 사용
    //    std::cout << "폰트 로드 실패!" << std::endl;
    //    return;
    //}

    //// 서버 연결
    //if (socket.connect("127.0.0.1", 53000) == sf::Socket::Done) {
    //    std::cout << "서버에 연결됨!\n";
    //}
    //std::thread(&GameManager::receiveUpdates, this).detach();

    sf::Text gameText(font, L"게임 시작~~~~~~!", 60);

    gameText.setFillColor(sf::Color::Red);
    gameText.setPosition(sf::Vector2f(500.f, 300.f));

    window.clear();
    window.draw(gameText);
    window.display();

    while (window.isOpen()) {
        while (std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                isWindowClosed = true;
                break;
            }

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    isWindowClosed = true;
                break;

            }
        }
        if (isWindowClosed) {
            break;  // 창이 닫히면 반복문을 종료하여 게임을 진행
        }
    }




    //window.clear();
    //window.draw(gameText);
    //window.display();

    //while (window.isOpen()) {
    //    while (std::optional event = window.pollEvent())
    //    {
    //        if (event->is<sf::Event::Closed>())
    //        {
    //            window.close();
    //        }

    //        //
    //        //            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
    //        //            {
    //        //                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
    //        //                    window.close();
    //        //            }
    //        //        }
    //        //if (!gameOver && event->is<sf::Event::TextEntered>()) {
    //        //    if (event->as<sf::Event::TextEntered>().unicode == '\b' && !userInput.empty()) {
    //        //        userInput.pop_back();  // 백스페이스 처리
    //        //    }
    //        //    else if (event->get<sf::Event::TextEntered>().unicode < 128) {
    //        //        userInput += static_cast<char>(event->get<sf::Event::TextEntered>().unicode);  // 텍스트 입력
    //        //    }
    //        //}

    //        //if (!gameOver && event.type == sf::Event::TextEntered) {
    //        //    if (event.text.unicode == '\b' && !userInput.empty()) {
    //        //        userInput.pop_back();
    //        //    }
    //        //    else if (event.text.unicode < 128) {
    //        //        userInput += static_cast<char>(event.text.unicode);
    //        //    }
    //        //}

    //        //if (!gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
    //        //    if (userInput == targetSentence) {
    //        //        myProgress++;
    //        //        std::cout << "입력 성공! 현재 입력 횟수: " << myProgress << "\n";

    //        //        std::string progressMsg = "PROGRESS " + std::to_string(myID) + " " + std::to_string(myProgress);
    //        //        socket.send(progressMsg.c_str(), progressMsg.size() + 1);

    //        //        userInput.clear();

    //        //        if (myProgress >= 3) {
    //        //            gameOver = true;
    //        //            resultText.setString("승리입니다!!");
    //        //        }
    //        //    }
    //        //    else {
    //        //        std::cout << "잘못된 입력!\n";
    //        //    }
    //        //}
    //    }

    //    if (opponentProgress >= 3 && !gameOver) {
    //        gameOver = true;
    //        resultText.setString("패배입니다");
    //    }

    //    if (myID != -1) {
    //        idText.setString("ID: " + std::to_string(myID));
    //    }
    //    myProgressText.setString("내 진행도: " + std::to_string(myProgress));
    //    opponentProgressText.setString("상대 진행도: " + std::to_string(opponentProgress));
    //    inputText.setString("입력 중: " + userInput);
    //    inputText.setFillColor(userInput == targetSentence.substr(0, userInput.size()) ? sf::Color::White : sf::Color::Red);

    //    window.clear();
    //    window.draw(idText);
    //    window.draw(myProgressText);
    //    window.draw(opponentProgressText);
    //    window.draw(targetText);
    //    window.draw(inputText);
    //    window.draw(resultText);
    //    window.display();
    //}
}
//void GameManager::runGame(sf::RenderWindow& window, sf::Font& font) {
//    //sf::Font font;
//    //font.openFromFile("D2Coding.tff");
//    sf::Font font;
//    if (!font.openFromFile("D2Coding.tff")) { // 한글 지원 폰트 사용
//        std::cout << "폰트 로드 실패!" << std::endl;
//        return;
//    }
//
//    //sf::Text gameText(font, L"게임 시작!", 60);
//    ////gameText.setFont(font);
//    ////gameText.setString(L"게임 시작!");
//    ////gameText.setCharacterSize(60);
//    //gameText.setFillColor(sf::Color::White);
//    //gameText.setPosition(sf::Vector2f(500.f, 300.f));
//
//
//    // 타이틀 텍스트
//    sf::Text gameText(font, L"타자 대결", 50);  // L로 한글 리터럴 처리
//    gameText.setPosition(sf::Vector2f(200, 100));
//    gameText.setFillColor(sf::Color::Black);
//
//
//    // 대결할 문장 (한글 문장)
//    std::wstring sentence = L"타자 대결을 시작합니다!";  // 한글 문장 사용
//    sf::Text sentencetext(font, sentence, 30);
//    sentencetext.setPosition(sf::Vector2f(100, 200));
//    sentencetext.setFillColor(sf::Color::Black);
//
//    // 플레이어 입력 텍스트 (한글 입력 처리)
//    std::wstring player1input, player2input;
//    sf::Text player1text(font, player1input, 30);
//    player1text.setPosition(sf::Vector2f(100, 300));
//    player1text.setFillColor(sf::Color::Red);
//
//    sf::Text player2text(font, player2input, 30);
//    player2text.setPosition(sf::Vector2f(100, 400));
//    player2text.setFillColor(sf::Color::Blue);
//
//    // 돌아가기 버튼
//    //sf::rectangleshape backbutton = createbutton(300, 500, 200, 50, sf::color::red);
//    //sf::text backtext(l"돌아가기", font, 30);  // 돌아가기 버튼도 한글로 표시
//    //backtext.setposition(340, 510);
//    //backtext.setfillcolor(sf::color::white);
//
//   // 타이핑 진행을 위한 변수
//    bool isplayer1turn = true;  // 차례를 결정하는 변수
//    bool gameover = false;
//
//    while (window.isOpen()) {
//        while (std::optional event = window.pollEvent())
//        {
//            if (event->is<sf::Event::Closed>())
//            {
//                window.close();
//            }
//
//            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
//            {
//                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
//                    window.close();
//            }
//        }
//    }
//    window.clear();
//    window.draw(gameText);
//    window.display();
//}


// 서버에서 매칭 완료 응답 받을 때까지 대기
//void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font) {
//    // 소켓을 non-blocking 모드로 전환
//    socket.setBlocking(false);
//    showLoadingScreen(window, font);
//
//    char buffer[256] = { 0 };
//    std::size_t received = 0;
//    bool matched = false;
//
//    while (window.isOpen() && !matched) {
//        while (std::optional event = window.pollEvent()) {
//            if (event->is<sf::Event::Closed>()) {
//                window.close();
//                return;
//            }
//        }
//        sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
//        if (status == sf::Socket::Status::Done) {
//            std::string message(buffer, received);
//            std::cout << "Received message: " << message << std::endl;
//            if (message.find("매칭 완료! 게임 시작!") != std::string::npos) {
//                matched = true;
//                break;
//            }
//        }
//        sf::sleep(sf::milliseconds(100));
//    }
//
//    if (matched) {
//        showCountdown(window, font);
//        runGame(window, font);
//    }
//    socket.setBlocking(true);
//}