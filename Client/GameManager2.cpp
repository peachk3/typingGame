//#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
//#include "gameManager.h"
//#include "FileLoader.hpp"
//#include "TextUtil.hpp"
//#include "TypingGame.hpp"
//#include <Windows.h>
//#include <fcntl.h>
//#include <io.h>
//#include <locale>
//#include <codecvt>
//#include <iostream>
//#include <thread>
//#include <chrono>
//#include <sstream>  // 문자열 스트림을 사용하기 위한 헤더
//#include <iomanip>  // 시간을 형식에 맞게 출력하기 위한 헤더
//#include <SFML/Network.hpp>
//
//using namespace sf;
//using namespace std;
//GameManager game;
//TypingGame tpgame;
//
//const int GWINDOW_WIDTH = 300;
//const int GWINDOW_HEIGHT = 250;
//const float ROTATION_SPEED = 60.f;  // 회전 속도 (1초당 n도 회전)
//FileLoader loader;
//TextUtil util;
//TypingGame typing;
//
//// ID 입력창
//void showIDInputScreen(sf::RenderWindow& window, sf::Font& font, std::string& clientID) {
//    sf::Text idText(font, L"아이디를 입력하세요: ", 30);
//    idText.setFillColor(sf::Color::White);
//    idText.setPosition(sf::Vector2f(200.f, 300.f));
//
//    sf::Text inputText(font, "", 30);
//    inputText.setFillColor(sf::Color::White);
//    inputText.setPosition(sf::Vector2f(200.f, 350.f));
//
//    window.clear();
//    window.draw(idText);
//    window.draw(inputText);
//    window.display();
//
//    bool inputComplete = false;
//    while (window.isOpen()) {
//        while (std::optional event = window.pollEvent()) {
//            if (event->is<sf::Event::Closed>()) {
//                window.close();
//            }
//            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
//                if (textEntered->unicode == 8 && clientID.length() > 0) {  // 백스페이스
//                    clientID.pop_back();
//                }
//                else if (textEntered->unicode < 128) {  // 문자 입력
//                    clientID += static_cast<char>(textEntered->unicode);
//                }
//            }
//            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
//                if (keyPressed->scancode == sf::Keyboard::Scancode::Enter) {
//                    inputComplete = true;  // Enter 키로 입력 완료
//                }
//            }
//        }
//        inputText.setString(clientID);  // 입력된 아이디 화면에 표시
//        window.clear();
//        window.draw(idText);
//        window.draw(inputText);
//        window.display();
//        if (inputComplete) {
//            break;
//        }
//    }
//}
//
////// 로딩창 설정
////void GameManager::showLoadingScreen(sf::RenderWindow& window, sf::Font& font, bool& isMatching) {
////
////    RenderWindow gameWindow(VideoMode({ GWINDOW_WIDTH, GWINDOW_HEIGHT }), L"게임 화면");
////
////    // 텍스처와 스프라이트 로드
////    Texture texture;
////    if (!texture.loadFromFile("./loading.png")) {  // "loading.png"는 준비해야 함
////        cerr << "이미지 로드 실패!" << endl;
////        return;
////    }
////
////    Sprite sprite(texture);
////
////    //sprite.setPosition(Vector2f(float(GWINDOW_WIDTH / 2), float(GWINDOW_HEIGHT / 2))); // 초기 위치 설정
////
////    // 스프라이트 원점 설정 (중앙으로 회전하도록)
////    sprite.setOrigin(Vector2f(texture.getSize().x / 2.f, texture.getSize().y / 2.f));
////    sprite.setPosition(Vector2f(GWINDOW_WIDTH / 2, GWINDOW_HEIGHT / 2));  // 화면 중앙으로 이동
////
////    Text statusText(font, L"상대 검색중...", 30);
////    statusText.setFillColor(Color::Black);
////    statusText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 75, (GWINDOW_HEIGHT / 2) - 100));
////
////    Text timerText(font, "00:00", 30);  // 시간 표시 텍스트
////    timerText.setFillColor(Color::Black);
////    timerText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 35, (GWINDOW_HEIGHT / 2) - 70));
////
////    Text matchingText(font, L"상대 연결 완료!", 30);  // Matching! 텍스트
////    matchingText.setFillColor(Color::Black);
////    matchingText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 100, (GWINDOW_HEIGHT / 2) - 100));
////
////    Text countdownText(font, "", 50);  // 카운트다운 텍스트
////    countdownText.setFillColor(Color::Black);
////    countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 10, (GWINDOW_HEIGHT / 2 - 20)));  // Matching! 아래에 표시
////
////    Clock clock;  // 게임 실행 시간 측정을 위한 시계
////    Clock countdownClock;  // 카운트다운을 위한 시계
////    Time elapsedTime;
////
////    //bool isMatching = false;  // 매칭 상태 추적
////    int countdown = 3;  // 카운트다운 시작
////    bool countdownStarted = false;
////    bool gameStartDisplayed = false;
////
////
////    // 서버에서 받은 카운트다운 시작 시각 (서버 시간)
////    // 이 값은 서버와 연결된 네트워크 코드에서 설정해야 함
////    Time serverStartTime;
////    bool serverTimeReceived = false;
////    bool gameStarted = false;
////
////    while (gameWindow.isOpen()) {
////        // 이벤트 처리 (SFML 3.0.0 방식)
////        std::optional<Event> event;
////        while (const std::optional event = gameWindow.pollEvent()) {
////            if (event->is<sf::Event::Closed>()) {
////                wcout << L"gameWindow.close()" << endl;
////                gameWindow.close();
////                return;
////            }
////        }
////
////        // 네트워크에서 매칭 정보 및 카운트다운 시작 시간 수신 (예시)
////        // 실제로는 네트워크 코드에서 이 변수들을 설정해야 합니다
////        if (isMatching && !serverTimeReceived) {
////            // 서버로부터 카운트다운 시작 시간 수신 (예시)
////            // serverStartTime = 서버에서 받은 시간
////            wcout << L"카운트다운 시작!" << endl;
////            serverTimeReceived = true;
////            countdownClock.restart(); // 서버 시간을 받은 시점부터 로컬 시계 시작
////        }
////        // 매칭이 된 경우 카운트다운 처리
////        if (isMatching && serverTimeReceived) {
////            //cout << "isMatching: " << isMatching << "serverTimeReceived:" << serverTimeReceived << endl;
////            gameWindow.clear(Color::White);
////            gameWindow.draw(matchingText);
////
////            // 서버 시간에 맞춰 로컬 카운트다운 계산
////            float elapsedCountdownTime = countdownClock.getElapsedTime().asSeconds();
////
////            // 카운트다운 값 계산 (서버 시간 기준)
////            int currentCountdown = countdown - static_cast<int>(elapsedCountdownTime);
////            //std::cout << "currentCountdown: " << currentCountdown << std::endl;
////            if (currentCountdown <= 0 && !gameStarted) {
////                // 카운트다운 종료, 게임 시작
////                countdownText.setString("Start Game!");
////                countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 135, GWINDOW_HEIGHT / 2 - 50));
////                countdown = 0;  // 카운트다운 시작
////                gameWindow.draw(countdownText);
////                //gameWindow.draw(countdownText);
////                //gameWindow.display();
////                wcout << L"잠시후 게임 시작!!" << std::endl;
////                gameStarted = true;  // 게임 시작 플래그 설정
////                isMatching = 0;  // 매칭 상태 변경 (중복 실행 방지)
////                // 잠시 대기 후 게임 시작
////                std::this_thread::sleep_for(std::chrono::seconds(1));
////                gameWindow.close();
////                return;
////            }
////            else {
////                // 카운트다운 표시
////                if (gameStarted != true) {
////                    countdownText.setString(to_string(currentCountdown));
////                    gameWindow.draw(countdownText);
////                }
////                else {
////                    gameWindow.close();
////                    return;
////                }
////            }
////
////            gameWindow.display();
////        }
////
////        // 매칭 실패했을 경우
////        else if (gameStarted != true) {
////            // deltaTime을 계산하여 회전 속도 적용
////            Time deltaTime = clock.restart();  // 이전 프레임에서 경과한 시간
////            elapsedTime += deltaTime;  // 전체 경과 시간 누적
////
////            // 이미지 360도 회전
////            sprite.rotate(degrees(ROTATION_SPEED) * deltaTime.asSeconds());  // 초당 ROTATION_SPEED도씩 회전
////
////            // 경과된 시간 계산 (초 단위로)
////            int seconds = static_cast<int>(elapsedTime.asSeconds());
////            int minutes = seconds / 60;
////            seconds = seconds % 60;
////
////            // 00:00 형식으로 시간 텍스트 업데이트
////            stringstream ss;
////            ss << setw(2) << setfill('0') << minutes << ":" << setw(2) << setfill('0') << seconds;
////            timerText.setString(ss.str());
////
////            // 화면 그리기
////            gameWindow.clear(Color::White);
////            gameWindow.draw(sprite);  // 회전하는 이미지 그리기
////            gameWindow.draw(statusText);  // 연결 상태 메시지 그리기
////            gameWindow.draw(timerText);   // 시간 텍스트 그리기
////            gameWindow.display();
////
////        }
////    }
////}
////
//
//
//// 폰트 NanumGothic //
//
//void GameManager::runGame2(sf::RenderWindow& window, sf::Font& font, int fontSize,
//    std::vector<std::vector<std::wstring>>& sentences) {
//
//    wcout << L"게임 시작창 이동 완료!!" << std::endl;
//    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//
//    // 선택한 파일의 총 문장 수
//    size_t numSentences = sentences.size();                   // 전체 문장 개수
//    size_t numLinesInLastSentence = sentences.back().size();  // 마지막 문장의 줄 수
//    size_t totalLines = 0;
//    size_t correctLineCount = 0;
//
//
//    // 모든 문장의 줄 수 세기
//    for (const auto& sentence : sentences) {
//        totalLines += sentence.size();
//    }
//    std::wcout << L"currentSentenceIdx" << numSentences << std::endl;
//    std::wcout << L"currentSentenceIdx" << numLinesInLastSentence << std::endl;
//
//    // 로그 출력 용 나중에 삭제
//    std::wstring target = sentences[numSentences - 1].back(); // 마지막 문장 줄
//    for (size_t i = 0; i < target.size(); ++i) {
//        std::wcout << L"[" << i << L"]: '" << target[i] << L"' (int: " << (int)target[i] << L")\n";
//    }
//    int currentSentenceIdx = 0;
//    int currentLineIdx = 0;
//
//
//    // 게임 시작 시간 기록
//    auto startTime = std::chrono::high_resolution_clock::now();  // 게임 시작 시간 기록
//
//
//    // userInput 초기화
//    std::vector<std::vector<std::wstring>> userInputs;
//    for (const auto& sentenceLine : sentences) {
//        std::vector<std::wstring> inputLine(sentenceLine.size(), L"");
//        userInputs.push_back(inputLine);
//    }
//    
//    Texture ladderImg;
//
//
//    // 이미지 로드
//    if (!ladderImg.loadFromFile("./assets/image/ladder3.png")) {
//        wcerr << L"사다리 이미지 로드 실패!" << endl;
//        //return;
//    }
//
//    // 플레이어 이미지 로드
//    Texture player1Texture, player2Texture;
//    if (!player1Texture.loadFromFile("./assets/image/player1_1.png") || !player1Texture.loadFromFile("./assets/image/player1_2.png")) {
//        wcout << L"플레이어 이미지 로드 실패!" << endl;
//        //return -1;
//    }
//    if (!player2Texture.loadFromFile("./assets/image/player2_1.png") || !player2Texture.loadFromFile("./assets/image/player2_2.png")) {
//        wcout << L"플레이어 이미지 로드 실패!" << endl;
//        //return -1;
//    }
//
//    // 초기 위치 설정
//    // 플레이어1 애니메이션 스프라이트 로드
//    std::vector<Texture> player1Textures(3);
//    player1Textures[0].loadFromFile("./assets/image/player1_1.png");
//    player1Textures[1].loadFromFile("./assets/image/player1_2.png");
//    // player1Textures[2].loadFromFile("player1_3.png");
//    Sprite player1Sprite(player1Textures[0]);
//    player1Sprite.setPosition(Vector2f(20.f, 10.f));
//    int player1Frame = 0;
//
//    // 플레이어2 애니메이션 스프라이트 로드
//    std::vector<Texture> player2Textures(3);
//    player2Textures[0].loadFromFile("./assets/image/player2_1.png");
//    player2Textures[1].loadFromFile("./assets/image/player2_2.png");
//    //player2Textures[2].loadFromFile("player1_3.png");
//    Sprite player2Sprite(player2Textures[0]);
//    player2Sprite.setPosition(Vector2f(20.f, 60.f));
//    int player2Frame = 0;
//
//
//    sf::Sprite sprite(ladderImg);
//
//    // 사다리 1 이미지 설정
//    Sprite ladderImgSpriteLeft(ladderImg);
//    //Vector2u ladderImgSize = ladderImg.getSize();
//    //// 원하는 크기로 조정 (예: 200x200)
//    //float scaleX = 30.f / ladderImgSize.x;
//    //float scaleY = 50.f / ladderImgSize.y;
//    ladderImgSpriteLeft.setTexture(ladderImg);
//    ladderImgSpriteLeft.setPosition(Vector2f(100.f, 150.f));     // 왼쪽
//
//    // 사다리 2 이미지 설정
//    Sprite ladderImgSpriteRight(ladderImg);
//    //Vector2u ladderImgSize = ladderImg.getSize();
//    //// 원하는 크기로 조정 (예: 200x200)
//    //float scaleX = 30.f / ladderImgSize.x;
//    //float scaleY = 50.f / ladderImgSize.y;
//    ladderImgSpriteRight.setTexture(ladderImg);
//    ladderImgSpriteRight.setPosition(Vector2f(1080.f, 150.f));     // 오른쪽 
//
//    sf::Text gameText(font, L"게임 시작~~!", 60);
//
//    gameText.setFillColor(Color::Red);
//    gameText.setPosition(Vector2f(480.f, 50.f)); // (왼쪽 -> 오른쪽, 위 -> 아래)
//
//    //------------------------------------타자 입력창 만들기------------------------------------
//    // 타자 연습 문장 리스트
//    //vector<wstring> sentences = { L"Hello", L"Welcome", L"World", L"AB", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"A", L"Bye" };
//    //size_t currentSentenceIndex = 0;
//    //wstring userInput;
//
//    // 현재 문장 표시
//    Text currentSentence(font, L"", 50);
//    FloatRect textBounds = currentSentence.getGlobalBounds();
//    currentSentence.setOrigin(textBounds.getCenter());
//    currentSentence.setFillColor(Color::Black);
//    currentSentence.setPosition(Vector2f(1280.f / 2, 300.f)); // 가운데 정렬
//
//    // 입력창 배경 생성
//    sf::RectangleShape inputBackground(sf::Vector2f(500.f, 40.f)); // 입력창 크기
//    FloatRect inputTextBounds = inputBackground.getGlobalBounds();
//    inputBackground.setOrigin(inputTextBounds.getCenter());
//
//    inputBackground.setPosition({ 1280.f / 2, 500.f });
//    inputBackground.setFillColor(sf::Color(240, 240, 240)); // 밝은 회색 배경
//    inputBackground.setOutlineThickness(2.f);
//    inputBackground.setOutlineColor(sf::Color(100, 100, 100)); // 테두리 색상
//
//    // 다음 문장 표시
//    //Text nextSentence(font, L"", 20);
//    //if (currentSentenceIndex + 1 < sentences.size()) {
//    //    nextSentence.setString(sentences[currentSentenceIndex + 1]);
//    //}
//    //FloatRect nextTextBounds = nextSentence.getGlobalBounds();
//    //nextSentence.setOrigin(nextTextBounds.getCenter());
//    //nextSentence.setFillColor(Color(32, 32, 32)); // 검은색에 가까운 회색
//    //nextSentence.setPosition(Vector2f(1280.f / 2, 400.f)); // 가운데
//
//    // 불투명 배경 설정
//    sf::RectangleShape Background(sf::Vector2f(400.f, 70.f));
//    // 배경 크기 가져오기
//    FloatRect bgBounds = Background.getGlobalBounds();
//    Background.setOrigin(bgBounds.getCenter()); // 중앙 기준 정렬
//
//    Background.setFillColor(sf::Color(224, 224, 224, 200)); // 마지막 - 투명도
//    Background.setPosition(Vector2f(1280.f / 2, 400.f));
//    //Background.setOutlineThickness(1.f);
//    //Background.setOutlineColor(sf::Color(139, 0, 0)); // 테두리 색상
//
//    // 입력 표시
//    Text userInputText(font, L"", 30);
//    userInputText.setFillColor(Color::Green);
//    userInputText.setPosition(Vector2f(400.f, 485.f)); // 화면 정중앙
//
//    //--------------------------------------------------------------------------------------------
//
//    // userID 받아와서 출력 (
//    wstring user1ID = L"사용자 1"; // id 받아와서 넣기
//    wstring user2ID = L"사용자 2"; // id 받아와서 넣기
//
//    Text user1Text(font, user1ID, 20);
//    Text user2Text(font, user2ID, 20);
//    user1Text.setFillColor(Color::Black);
//    user2Text.setFillColor(Color::Black);
//
//
//    user1Text.setPosition(Vector2f(600.f, 500.f)); // (왼쪽 -> 오른쪽, 위 -> 아래)
//    user2Text.setPosition(Vector2f(600.f, 100.f)); // (왼쪽 -> 오른쪽, 위 -> 아래)
//
//
//    window.clear(Color::White);
//    window.draw(player1Sprite);             // player1
//    window.draw(player2Sprite);             // player2
//    window.draw(Background);
//    window.draw(gameText);
//    window.draw(userInputText);
//    window.draw(ladderImgSpriteLeft);       // 왼쪽
//    window.draw(ladderImgSpriteRight);      // 오른쪽
//    window.draw(currentSentence);
//    //window.draw(nextSentence);
//    //window.draw(user1Text);
//    //window.draw(user2Text);
//    window.draw(inputBackground);
//    window.display();
//
//    // 커서 설정 추가
//    sf::Clock cursorClock;
//    bool showCursor = true;
//    sf::RectangleShape cursor(sf::Vector2f(2.f, 30.f));
//    cursor.setFillColor(sf::Color::Black);
//    cursor.setPosition({ 400.f, 485.f });
//
//    // 통계를 내기 위한 변수 
//    double totalWpm = 0.0;          // 총 WPM (단어당 분)
//    double totalAccuracy = 0.0;     // 총 정확도
//    double totalTime = 0.0;         // 총 시간
//    double totalTpm = 0;
//    int totalKeyPress = 0;          // 키 입력 횟수
//    int backspacePressCount = 0;
//
//    while (window.isOpen()) {
//        bool isWindowClosed = false;
//
//        while (std::optional event = window.pollEvent()) {
//            if (event->is<sf::Event::Closed>()) {
//                isWindowClosed = true;
//                break;
//            }
//            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
//                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
//                    isWindowClosed = true;
//                    break;
//                }
//            }
//            // 텍스트 입력 처리
//                        // 키보드 입력 처리
//            // 키 입력
//            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
//                wchar_t c = static_cast<wchar_t>(textEvent->unicode);   // 입력된 문자를 가져옴
//                totalKeyPress++;
//                std::wstring& currentSentence = userInputs[currentSentenceIdx][currentLineIdx];
//                std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
//                if (c == 8) {  // 백스페이스 처리
//                    backspacePressCount++;
//                    if (!currentInput.empty()) {
//                        currentInput.pop_back();  // 입력된 문자에서 마지막 문자 삭제
//                    }
//                }
//                // 한 문장 끝까지 치면 입력 안되도록 제한
//                if (currentInput.length() != sentences[currentSentenceIdx][currentLineIdx].length())
//                {
//                    if (iswprint(c)) {  // 출력 가능한 문자일 경우
//                        currentInput += c;  // 입력 텍스트에 문자 추가
//                    }
//                }
//
//            }
//            // 엔터 입력
//            // 한줄마다 정확도, 속도 나옴
//            // 한줄 정확하지 않으면 넘어가지 않도록
//            // 마지막 줄 일때 엔터 누르면 결과 창 나오도록 
//            // 틀리면 빨강 글시는 이거 다 하고 구현, 깜박이는 거랑
//            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
//                if (keyEvent->code == sf::Keyboard::Key::Enter) {
//
//                    // 여기에 엔터 처리 로직
//                    std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
//                    // 현재는 결과가 올바를 때만 다음 문장 입력할 수 있도록 구현
//                    if (currentInput == sentences[currentSentenceIdx][currentLineIdx])
//                    {
//                        TypingStats stats = tpgame.updateStats(startTime, sentences[currentSentenceIdx][currentLineIdx],
//                            currentInput, backspacePressCount, totalKeyPress);
//
//
//                        totalWpm += stats.wpm;
//                        totalAccuracy += stats.accuracy;
//                        totalTime += stats.elapsedTime;
//                        totalTpm += stats.tmp;
//                        std::wcout << L"totalTpm" << totalTpm << std::endl;
//                        correctLineCount++;
//                        currentLineIdx++;
//
//                        if (currentLineIdx >= sentences[currentSentenceIdx].size())
//                        {
//                            // 짧은 문장일 경우 다음 문장으로 이동
//                            currentLineIdx = 0;
//                            currentSentenceIdx++;
//
//                            // 전체 문장을 끝 까지 입력했을 경우
//                            if (currentSentenceIdx >= sentences.size())
//                            {
//                                std::wcout << L"끝까지 입력함: 결과창 출력" << currentSentenceIdx << std::endl;
//                                auto endTime = std::chrono::high_resolution_clock::now();  // 종료 시간 기록
//
//                                // 전체 문장 종료 결과창 출력
//                                tpgame.showResultWindow(window, font, fontSize, totalWpm / totalLines,
//                                    totalAccuracy / totalLines, totalTime, totalTpm / totalLines, sentences);
//                                return;
//                            }
//                        }
//                    }
//                    else
//                    {
//
//                        // 로그 출력용 나중에 삭제
//                        std::wcout << L"오답" << std::endl;
//                        std::wcout << L"currentInput:    " << currentInput << std::endl;
//                        std::wcout << L"sentences:    " << sentences[currentSentenceIdx][currentLineIdx] << std::endl;
//                        std::wcout << L"오답 글자수    " << currentInput.size() << std::endl;
//                        std::wcout << L"정답 글자수    " << sentences[currentSentenceIdx][currentLineIdx].size() << std::endl;
//
//                    }
//                }
//
//            }
//
//
//            //else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
//            //    if (textEntered->unicode == 8) { // 백스페이스 처리
//            //        if (!userInput.empty()) {
//            //            userInput.pop_back();
//            //        }
//            //    }
//            //    else if (textEntered->unicode == 13) { // 엔터 입력 시 검증
//            //        if (userInput == sentences[currentSentenceIndex]) {
//            //            currentSentenceIndex++;
//            //            int check = 0;
//            //            if (currentSentenceIndex < sentences.size()) {
//            //                for (int i = 0; i < 5; i++) {
//            //                    player1Sprite.setColor(sf::Color(255, 255, 255, 255)); // 
//
//            //                    window.draw(player1Sprite);
//
//            //                    player1Sprite.setTexture(player1Textures[i % 2]);
//            //                    player1Sprite.move(Vector2f(20.f, 0.f));  // 오른쪽으로 이동
//            //                    if (i == 2) {
//            //                        if (check % 2 == 1) {
//            //                            player1Textures[1].loadFromFile("./assets/image/player1_1.png");
//            //                            player1Textures[0].loadFromFile("./assets/image/player1_2.png");
//            //                        }
//            //                        else {
//            //                            player1Textures[0].loadFromFile("./assets/image/player1_1.png");
//            //                            player1Textures[1].loadFromFile("./assets/image/player1_2.png");
//            //                        }
//            //                    }
//
//            //                    //window.clear(Color::White); 
//            //                    //player1Sprite.setColor(sf::Color(255, 255, 255, 0)); // 완전 투명
//            //                    player1Sprite.setColor(sf::Color(255, 255, 255, 255)); // 
//
//            //                    window.draw(player1Sprite);
//            //                    window.draw(player2Sprite);
//            //                    //window.draw(inputText);
//            //                    window.display();
//            //                    sf::sleep(sf::milliseconds(100));  // 애니메이션 속도 조절
//            //                }
//            //                check++;
//
//            //                userInput.clear();
//            //                currentSentence.setString(sentences[currentSentenceIndex]);
//            //                nextSentence.setString((currentSentenceIndex + 1 < sentences.size()) ? sentences[currentSentenceIndex + 1] : L"");
//            //            }
//            //            else {
//            //                gameText.setString(L"게임 종료!");
//            //                currentSentence.setString(L"");
//            //                nextSentence.setString(L"");
//            //            }
//            //        }
//            //    }
//            //    else {
//            //        userInput += static_cast<wchar_t>(textEntered->unicode);
//            //        wcout << L"입력된 문자: " << static_cast<wchar_t>(textEntered->unicode) << endl;
//
//            //    }
//            //    // UTF-8로 변환하여 SFML Text에 설정
//            //    userInputText.setString(converter.to_bytes(userInput));
//            //    //userInputText.setString(userInput);
//            //}
//        }
//
//        if (isWindowClosed) {
//            break;  // 창이 닫히면 반복문을 종료하여 게임을 진행
//        }
//
//        // 커서 깜박임 업데이트
//        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
//            showCursor = !showCursor;
//            cursorClock.restart();
//        }
//
//        //sf::Vector2u textureSize = texture.getSize();
//        //float scaledWidth = textureSize.x * 0.2f;
//        //float scaledHeight = textureSize.y * 0.2f;
//        //sprite.setPosition({
//        //    window.getSize().x - scaledWidth - 20.f,
//        //    window.getSize().y - scaledHeight - 20.f
//        //    });
//
//        //// 커서 위치 업데이트
//        //float textWidth = userInputText.getLocalBounds().width;
//        //cursor.setPosition({ 300.f + textWidth, 350.f });
//
//
//        if (isWindowClosed) {
//            break;  // 창이 닫히면 반복문을 종료하여 게임을 진행
//        }
//
//
//        window.clear(Color::White);
//        window.draw(player1Sprite);             // player1
//        window.draw(player2Sprite);             // player2
//        window.draw(Background);
//        window.draw(gameText);
//        window.draw(userInputText);
//        window.draw(ladderImgSpriteLeft);
//        window.draw(ladderImgSpriteRight);
//        window.draw(currentSentence);
//        //window.draw(nextSentence);
//        window.draw(inputBackground);  // 먼저 배경을 그림
//        window.draw(userInputText);    // 그 다음 텍스트를 그림
//        //window.draw(user1Text);
//        //window.draw(user2Text);
//
//        // 커서 표시 여부 결정
//        if (showCursor) {
//            window.draw(cursor);
//        }
//
//        //window.draw(inputBackground);
//        window.display();
//    }
//
//}
// //게임 시작
//
//
////void GameManager::handleTypingMode(FileLoader& loader, TextUtil& util, TypingGame& typing, sf::RenderWindow& window, sf::Font& font) {
////    //sf::Font font;
////
////    // 폰트 로드 실패일시 종속성 확인해보기
////    if (!font.openFromFile("D2Coding.ttf")) {
////        wcerr << L"폰트 로드 실패!" << endl;
////        return;
////    }
////
////    // 1. 파일 열기 테스트
////    std::optional<std::wstring> path = loader.openFileDialog();
////    if (!path) {
////        std::wcout << L"[ERROR] 파일을 선택하지 않았습니다." << std::endl;
////        system("pause");
////        return;
////    }
////
////    std::wcout << L"[INFO] 파일 선택됨: " << path.value() << std::endl;
////
////    //// 2. 텍스트 로딩 테스트
////    std::wstring content = loader.loadText(path.value());
////    std::wcout << L"[INFO] 파일에서 읽은 내용:\n" << content << L"...\n\n";
////    //std::wcout << L"[INFO] 파일에서 읽은 내용:\n" << content.substr(0, 200) << L"...\n\n";
////
////    //// 3. 줄 나누기 테스트
////    std::vector<std::wstring> lines = util.splitStrtoVector(content);
////    std::wcout << L"[INFO] 줄 개수: " << lines.size() << std::endl;
////
////    for (int i = 0; i < lines.size(); ++i) {
////        std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
////    }
////    /*for (int i = 0; i < std::min((size_t)5, lines.size()); ++i) {
////        std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
////    }*/
////
////    // 4. 줄바꿈 테스트
////    //sf::Font font;
////    std::filesystem::current_path("C:/Project/gameProject/typingGame");
////    if (!font.openFromFile("assets/font/D2Coding.ttf")) {
////        std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
////        return;
////    }
////
////    auto wrapped = util.wrapAllLinesToPixelWidth(lines, font, 30, 1280.f);
////    std::wcout << L"\n[INFO] 줄바꿈 결과 (최대 3줄):" << std::endl;
////    /*for (int i = 0; i < std::min((size_t)3, wrapped.size()); ++i) {
////        for (const auto& sub : wrapped[i]) {
////            std::wcout << L"  → " << sub << std::endl;
////        }
////    }*/
////
////    for (int i = 0; i < wrapped.size(); ++i) {
////        for (const auto& sub : wrapped[i]) {
////            std::wcout << L"  → " << sub << std::endl;
////        }
////    }
////
////    //typing.startNewGame(window, font, 18, wrapped);
////    typing.startNewGame(window, font, 18, wrapped);
////    system("pause");
////}