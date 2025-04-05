#pragma once
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "gameManager.h"
#include "FileLoader.hpp"
#include "TextUtil.hpp"
#include "Utils.h"
#include "TypingGame.hpp"
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <locale>
#include <codecvt>
#include <iostream>
#include <thread>
#include <filesystem> // 파일 불러오기
#include <random> // 랜덤
#include <string>
#include <chrono>
#include <sstream>  // 문자열 스트림을 사용하기 위한 헤더
#include <iomanip>  // 시간을 형식에 맞게 출력하기 위한 헤더
#include <SFML/Network.hpp>

int MAIN_WINDOW_WIDTH = 1280;
int MAIN_WINDOW_HEIGHT = 720;

using namespace sf;
using namespace std;
namespace fs = filesystem;

std::string folderPath = "./assets/file";

GameManager game;
TypingGame tpgame;
const int GWINDOW_WIDTH = 300;
const int GWINDOW_HEIGHT = 250;
const float ROTATION_SPEED = 60.f;  // 회전 속도 (1초당 n도 회전)

FileLoader loader;
TextUtil util;


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
                wcout << L"gameWindow.close()" << endl;
                gameWindow.close();
                return;
            }
        }

        // 네트워크에서 매칭 정보 및 카운트다운 시작 시간 수신 (예시)
        // 실제로는 네트워크 코드에서 이 변수들을 설정해야 합니다
        if (isMatching && !serverTimeReceived) {
            // 서버로부터 카운트다운 시작 시간 수신 (예시)
            // serverStartTime = 서버에서 받은 시간
            wcout << L"카운트다운 시작!" << endl;
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
                wcout << L"잠시후 게임 시작!!" << std::endl;
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


void GameManager::handleTypingGameMode(FileLoader& loader, TextUtil& util, TypingGame& typing, sf::RenderWindow& window, sf::Font& font, bool userGameManager) {

    // 폰트 로드 실패일시 종속성 확인해보기
    if (!font.openFromFile("D2Coding.ttf")) {
        wcerr << L"폰트 로드 실패!" << endl;
        return;
    }

    // 1. 파일 열기 테스트
    std::optional<std::wstring> path = loader.openFileDialog();
    if (!path) {
        std::wcout << L"[ERROR] 파일을 선택하지 않았습니다." << std::endl;
        system("pause");
        return;
    }

    std::wcout << L"[INFO] 파일 선택됨: " << path.value() << std::endl;

    //// 2. 텍스트 로딩 테스트
    std::wstring content = loader.loadText(path.value());
    std::wcout << L"[INFO] 파일에서 읽은 내용:\n" << content << L"...\n\n";
    //std::wcout << L"[INFO] 파일에서 읽은 내용:\n" << content.substr(0, 200) << L"...\n\n";

    //// 3. 줄 나누기 테스트
    std::vector<std::wstring> lines = util.splitStrtoVector(content);
    std::wcout << L"[INFO] 줄 개수: " << lines.size() << std::endl;

    for (int i = 0; i < lines.size(); ++i) {
        std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
    }
    /*for (int i = 0; i < std::min((size_t)5, lines.size()); ++i) {
        std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
    }*/

    // 4. 줄바꿈 테스트
    //sf::Font font;
    std::filesystem::current_path("C:/Project/gameProject/typingGame");
    if (!font.openFromFile("assets/font/D2Coding.ttf")) {
        std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
        return;
    }

    auto wrapped = util.wrapAllLinesToPixelWidth(lines, font, 30, 1280.f);
    std::wcout << L"\n[INFO] 줄바꿈 결과 (최대 3줄):" << std::endl;
    /*for (int i = 0; i < std::min((size_t)3, wrapped.size()); ++i) {
        for (const auto& sub : wrapped[i]) {
            std::wcout << L"  → " << sub << std::endl;
        }
    }*/

    for (int i = 0; i < wrapped.size(); ++i) {
        for (const auto& sub : wrapped[i]) {
            std::wcout << L"  → " << sub << std::endl;
        }
    }

    if (userGameManager) {
        game.runGame2(window, font, 18, wrapped);
    }
    else {
        typing.startNewGame(window, font, 18, wrapped);
    }

    //typing.startNewGame(window, font, 18, wrapped);
    system("pause");
}


void GameManager::runGame2(sf::RenderWindow& window, sf::Font& font, int fontSize,
    std::vector<std::vector<std::wstring>>& sentences) {


    // 선택한 파일의 총 문장 수
    size_t numSentences = sentences.size();                   // 전체 문장 개수
    size_t numLinesInLastSentence = sentences.back().size();  // 마지막 문장의 줄 수
    size_t totalLines = 0;
    size_t correctLineCount = 0;


    // 모든 문장의 줄 수 세기
    for (const auto& sentence : sentences) {
        totalLines += sentence.size();
    }
    std::wcout << L"currentSentenceIdx" << numSentences << std::endl;
    std::wcout << L"currentSentenceIdx" << numLinesInLastSentence << std::endl;

    // 로그 출력 용 나중에 삭제
    std::wstring target = sentences[numSentences - 1].back(); // 마지막 문장 줄
    for (size_t i = 0; i < target.size(); ++i) {
        std::wcout << L"[" << i << L"]: '" << target[i] << L"' (int: " << (int)target[i] << L")\n";
    }
    int currentSentenceIdx = 0;
    int currentLineIdx = 0;

    // 게임 시작 시간 기록
    auto startTime = std::chrono::high_resolution_clock::now();  // 게임 시작 시간 기록

    // userInput 초기화
    std::vector<std::vector<std::wstring>> userInputs;
    for (const auto& sentenceLine : sentences) {
        std::vector<std::wstring> inputLine(sentenceLine.size(), L"");
        userInputs.push_back(inputLine);
    }

    sf::Text gameText(font, L"게임 시작~~!", 60);

    gameText.setFillColor(Color::Red);
    gameText.setPosition(Vector2f(480.f, 50.f)); // (왼쪽 -> 오른쪽, 위 -> 아래)

    //------------------------------------타자 입력창 만들기------------------------------------

    // 현재 문장 표시
    Text currentSentence(font, L"", 50);
    FloatRect textBounds = currentSentence.getGlobalBounds();
    currentSentence.setOrigin(textBounds.getCenter());
    currentSentence.setFillColor(Color::Black);
    currentSentence.setPosition(Vector2f(1280.f / 2, 300.f)); // 가운데 정렬

    // 입력 표시
    Text userInputText(font, L"", 30);
    // userInputText도 중앙 정렬이 되도록 설정
    userInputText.setFillColor(Color::Green);

    FloatRect userInputBounds = userInputText.getGlobalBounds();

    // SFML 3.0 방식: getCenter() 사용
    sf::Vector2f center = userInputBounds.getCenter();
    // 중심점으로 원점 설정
    userInputText.setOrigin(center);
    userInputText.setPosition(Vector2f(1280.f / 2, 500.f)); // 입력창 위치와 같은 곳에 위치시킴

    //--------------------------------------------------------------------------------------------

    // 통계를 내기 위한 변수 
    double totalWpm = 0.0;          // 총 WPM (단어당 분)
    double totalAccuracy = 0.0;     // 총 정확도
    double totalTime = 0.0;         // 총 시간
    double totalTpm = 0;
    int totalKeyPress = 0;          // 키 입력 횟수
    int backspacePressCount = 0;

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
            // 키보드 입력 처리
            // 키 입력
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                wchar_t c = static_cast<wchar_t>(textEvent->unicode);   // 입력된 문자를 가져옴
                totalKeyPress++;
                std::wstring& currentSentence = userInputs[currentSentenceIdx][currentLineIdx];
                std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                if (c == 8) {  // 백스페이스 처리
                    backspacePressCount++;
                    if (!currentInput.empty()) {
                        currentInput.pop_back();  // 입력된 문자에서 마지막 문자 삭제
                    }
                }
                // 한 문장 끝까지 치면 입력 안되도록 제한
                if (currentInput.length() != sentences[currentSentenceIdx][currentLineIdx].length())
                {
                    if (iswprint(c)) {  // 출력 가능한 문자일 경우
                        currentInput += c;  // 입력 텍스트에 문자 추가
                    }
                }

                // 입력 텍스트가 변경되었으므로 위치를 업데이트
                userInputText.setString(currentInput);
                FloatRect updatedBounds = userInputText.getGlobalBounds();
                Vector2f center2 = updatedBounds.getCenter();
                userInputText.setOrigin(center2);

            }
            // 엔터 입력
            // 한줄마다 정확도, 속도 나옴
            // 한줄 정확하지 않으면 넘어가지 않도록
            // 마지막 줄 일때 엔터 누르면 결과 창 나오도록 
            // 틀리면 빨강 글시는 이거 다 하고 구현, 깜박이는 거랑
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {

                    // 여기에 엔터 처리 로직
                    std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                    // 현재는 결과가 올바를 때만 다음 문장 입력할 수 있도록 구현
                    if (currentInput == sentences[currentSentenceIdx][currentLineIdx])
                    {
                        TypingStats stats = tpgame.updateStats(startTime, sentences[currentSentenceIdx][currentLineIdx],
                            currentInput, backspacePressCount, totalKeyPress);


                        totalWpm += stats.wpm;
                        totalAccuracy += stats.accuracy;
                        totalTime += stats.elapsedTime;
                        totalTpm += stats.tmp;
                        std::wcout << L"totalTpm" << totalTpm << std::endl;
                        correctLineCount++;
                        currentLineIdx++;
                        if (currentLineIdx >= sentences[currentSentenceIdx].size())
                        {
                            // 짧은 문장일 경우 다음 문장으로 이동
                            currentLineIdx = 0;
                            currentSentenceIdx++;

                            // 전체 문장을 끝 까지 입력했을 경우
                            if (currentSentenceIdx >= sentences.size())
                            {
                                std::wcout << L"끝까지 입력함: 결과창 출력" << currentSentenceIdx << std::endl;
                                auto endTime = std::chrono::high_resolution_clock::now();  // 종료 시간 기록

                                // 전체 문장 종료 결과창 출력
                                tpgame.showResultWindow(window, font, fontSize, totalWpm / totalLines,
                                    totalAccuracy / totalLines, totalTime, totalTpm / totalLines, sentences);
                                return;
                            }
                            nextLine();
                        }

                        // 다음 라인으로 넘어갔으므로 입력 텍스트를 초기화하고 위치 재설정
                        userInputText.setString(L"");
                        FloatRect updatedBounds = userInputText.getGlobalBounds();
                        Vector2f center2 = updatedBounds.getCenter();
                        userInputText.setOrigin(center2);

                    }
                    else
                    {

                        // 로그 출력용 나중에 삭제
                        std::wcout << L"오답" << std::endl;
                        std::wcout << L"currentInput:    " << currentInput << std::endl;
                        std::wcout << L"sentences:    " << sentences[currentSentenceIdx][currentLineIdx] << std::endl;
                        std::wcout << L"오답 글자수    " << currentInput.size() << std::endl;
                        std::wcout << L"정답 글자수    " << sentences[currentSentenceIdx][currentLineIdx].size() << std::endl;

                    }
                }

            }

        }

        if (isWindowClosed) {
            break;  // 창이 닫히면 반복문을 종료하여 게임을 진행
        }

        window.clear(Color::White);
        window.draw(gameText);

        // 현재 문장 표시 업데이트
        currentSentence.setString(sentences[currentSentenceIdx][currentLineIdx]);
        FloatRect updatedTextBounds = currentSentence.getGlobalBounds();

        Vector2f center3 = updatedTextBounds.getCenter();
        currentSentence.setOrigin(center3);

        // 입력 텍스트 업데이트
        userInputText.setString(userInputs[currentSentenceIdx][currentLineIdx]);

        drawOriginTotalText(window, font, fontSize, sentences); // 전체 문장 출력
        drawOriginText(window, font, fontSize, sentences); // 문자 출력
        drawUserInputText(window, font, fontSize, userInputs, sentences); // 사용자 입력 출력

        window.display();
    }

}

// 문장 통채로 출력
void GameManager::drawOriginTotalText(sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& fileAllLines) {

    // section 10개로 나누기
    const float sectionWidth = MAIN_WINDOW_WIDTH / 10.f; 
    const float sectionHeight = MAIN_WINDOW_HEIGHT / 4.f;


    // === 디버깅용: 가로 10등분 영역 시각화 ===
    //for (int i = 0; i < 10; ++i) {
    //    sf::RectangleShape rect(sf::Vector2f(sectionWidth, MAIN_WINDOW_HEIGHT));
    //    rect.setPosition({ i * sectionWidth, 0 });

    //    // 색상은 구간마다 다르게 설정
    //    switch (i % 4) {
    //    case 0: rect.setFillColor(sf::Color(255, 0, 0, 40)); break;     // 빨강
    //    case 1: rect.setFillColor(sf::Color(0, 255, 0, 40)); break;     // 초록
    //    case 2: rect.setFillColor(sf::Color(0, 0, 255, 40)); break;     // 파랑
    //    case 3: rect.setFillColor(sf::Color(255, 255, 0, 40)); break;   // 노랑
    //    }

    //    window.draw(rect);
    //}

    // 기준 문장 위치 (화면 10/2부터 시작)
    float x = sectionWidth * 1.5;
    float y = sectionHeight * 1.5; // y를 누적해서 줄 간격 주기

    // 텍스트 영역의 너비 (10/3 크기로 제한)
    float maxWidth = sectionWidth * 3;

    // 한 글자씩 입력
    // 한글 / 영어일때 수정해줘야 됨 drawUserInputText도 같이
    float letterSpacing = fontSize * 0.50f; // 영어
    //float letterSpacing = fontSize * 0.90f; // 한글

    // 배경 크기 계산을 위한 변수
    float maxTextWidth = 0.f;  // 가장 긴 텍스트의 너비
    float totalTextHeight = 0.f; // 전체 텍스트 높이

    // 텍스트 크기 측정
    for (int i = 0; i < fileAllLines.size(); ++i) {
        for (size_t j = 0; j < fileAllLines[i].size(); ++j) {
            sf::Text text(font, fileAllLines[i][j], fontSize);
            float textWidth = text.getLocalBounds().size.x;
            maxTextWidth = std::max(maxTextWidth, textWidth);
            totalTextHeight += fontSize * 2.0f; // 줄 간격 포함
        }
    }
    // 결과 출력 (터미널에서 확인)
    //std::cout << "maxTextWidth: " << maxTextWidth << std::endl; //477

    // 배경 크기 설정 (텍스트 크기보다 여유 공간 추가)
    float padding = 20.f;  // 여백 추가
    float backgroundWidth = maxTextWidth + padding ;  // 가장 긴 텍스트 기준
    float backgroundHeight = totalTextHeight + padding;  // 전체 높이


    // 배경 박스 설정
    sf::RectangleShape OriginBackground(sf::Vector2f(backgroundWidth, backgroundHeight));
    OriginBackground.setFillColor(sf::Color(224, 224, 224, 200)); // 불투명 회색
    OriginBackground.setPosition(Vector2f(x - padding, y - padding)); // 여백 적용

    window.draw(OriginBackground); // 배경 먼저 그림

    for (int i = 0; i < fileAllLines.size(); ++i)
    {
        for (size_t j = 0; j < fileAllLines[i].size(); ++j)
        {
            // 기준 문장 출력
            sf::Text originaTotalText(font, fileAllLines[i][j], fontSize);
            originaTotalText.setFillColor(sf::Color::Black);
            originaTotalText.setPosition({ x, y });
            // 텍스트 너비가 maxWidth를 초과하는 경우를 대비한 스케일 조정 (선택적)
            float textWidth = originaTotalText.getLocalBounds().size.x;

            if (textWidth > maxWidth) {
                float scale = maxWidth / textWidth;
                originaTotalText.setScale(Vector2f(scale, 1.0f));
            }
            window.draw(originaTotalText);
            y += fontSize * 2.0f ; // 줄 간격 설정
        }
    }

    // 한글일 경우
    //for (int i = 0; i < fileAllLines.size(); ++i)
    //{
    //    for (size_t j = 0; j < fileAllLines[i].size(); ++j)
    //    {
    //        std::wstring& Line = fileAllLines[i][j];

    //        for (size_t k = 0; k < Line.size(); ++k)
    //        {
    //            sf::Text oneChar(font, Line.substr(k, 1), fontSize);

    //            oneChar.setFillColor(sf::Color::Black);

    //            oneChar.setPosition({ x + k * letterSpacing, y + fontSize + 10.f });


    //            window.draw(oneChar);
    //        }

    //        //sf::Text inputText(font, userInputs[i][j], fontSize);
    //        //inputText.setFillColor(correct ? sf::Color::Green : sf::Color::Red);  // 정확하면 Green, 아니면 Red
    //        ////inputText.setFillColor(sf::Color::Green);
    //        //inputText.setPosition({ x, y + fontSize + 10.f });

    //        //window.draw(inputText);


    //        y += fontSize * 3; // 줄 간격 설정

    //    }
    //}

}

// 본래 TypingGame의 drawOriginText
// ui 생성 함수 - 기준 문장
void GameManager::drawOriginText(
    sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& fileAllLines)
{
    if (currentLineIndex >= fileAllLines.size()) return; // 모든 줄 출력 완료
    const float sectionWidth = MAIN_WINDOW_WIDTH / 10.f; // = 72.0f
    const float sectionHeight = MAIN_WINDOW_HEIGHT / 4.f;

    //float x = 200.f;
    float x = sectionWidth * 7.5f; // 오른쪽 중앙
    // 화면의 1/6 영역으로 제한 (4/6 ~ 5/6)
    float areaWidth = sectionWidth * 3;
    float y = sectionHeight * 1.5; // y를 누적해서 줄 간격 주기
    
    //한 글자씩 입력
    //한글 / 영어일때 수정해줘야 됨 drawUserInputText도 같이
    float letterSpacing = fontSize * 0.50f; // 영어
    //float letterSpacing = fontSize * 0.90f; // 한글

    // 현재 문장 출력
    if (currentLineIndex < fileAllLines.size()) {
        for (size_t j = 0; j < fileAllLines[currentLineIndex].size(); ++j)
        {
            // 현재 문장 출력
            sf::Text currentSentence(font, fileAllLines[currentLineIndex][0], fontSize);
            currentSentence.setFillColor(sf::Color::Black);
            FloatRect textBounds = currentSentence.getGlobalBounds();
            currentSentence.setOrigin(textBounds.getCenter());
            currentSentence.setPosition({ x, y });

            // 텍스트 너비가 할당된 영역을 초과하면 스케일 조정
            if (textBounds.size.x > areaWidth) {
                float scale = areaWidth / textBounds.size.x;
                currentSentence.setScale(Vector2f(scale, 1.0f));
            }
            window.draw(currentSentence);
        }
    }
    // 다음 문장 출력 (현재 줄 아래)
    if (currentLineIndex + 1 < fileAllLines.size()) { // 다음 문장이 있을 때만 출력
        //float nextLineY = y + fontSize * 2; // 줄 간격 추가
        for (size_t j = 0; j < fileAllLines[currentLineIndex + 1].size(); ++j)
        {

            // 불투명 배경 설정
            sf::RectangleShape Background(sf::Vector2f(250.f, 55.f));
            // 배경 크기 가져오기
            FloatRect bgBounds = Background.getGlobalBounds();
            Background.setOrigin(bgBounds.getCenter()); // 중앙 기준 정렬

            Background.setFillColor(sf::Color(96, 96, 96, 200)); // 마지막 - 투명도
            Background.setPosition(Vector2f(x, y + 100.f));
             
            sf::Text nextSentence(font, fileAllLines[currentLineIndex + 1][0], fontSize / 2);
            nextSentence.setFillColor(sf::Color(32, 32, 32)); // 어두운 회색
            FloatRect nextTextBounds = nextSentence.getGlobalBounds();
            nextSentence.setOrigin(nextTextBounds.getCenter());
            nextSentence.setPosition({ x, y + 100.f }); // 현재 문장 아래 위치
            // 텍스트 너비가 할당된 영역을 초과하면 스케일 조정
            if (nextTextBounds.size.x > areaWidth) {
                float scale = areaWidth / nextTextBounds.size.x;
                nextSentence.setScale(Vector2f(scale, 1.0f));
            }
			window.draw(Background); // 배경 그리기
            window.draw(nextSentence);
        }
    }
}

void GameManager::nextLine() {
    currentLineIndex++; // 다음 줄로 이동
}

bool userInputCompleted(const std::wstring& userInput, const std::wstring& correctAnswer) {
    return userInput == correctAnswer;
}

// ui 생성 함수 - 입력 문장
void GameManager::drawUserInputText(
    sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& userInputs,
    std::vector<std::vector<std::wstring>>& sentences)
{
    if (currentLineIndex >= userInputs.size()) return; // 모든 입력이 끝나면 종료
    const float sectionWidth = MAIN_WINDOW_WIDTH / 10.f; // = 72.0f
    const float sectionHeight = MAIN_WINDOW_HEIGHT / 4.f;
    //float x = 200.f;

    float x = sectionWidth * 7.5f;
    float areaWidth = sectionWidth * 4;
    float y = sectionHeight * 3; // y를 누적해서 줄 간격 주기

    // 한 글자씩 입력
    // 한글 / 영어일때 수정해줘야 됨 drawOriginalText도 같이
    float letterSpacing = fontSize * 0.50f; // 영어
    //float letterSpacing = fontSize * 0.90f; // 한글

    std::wstring& inputLine = userInputs[currentLineIndex][0];
    std::wstring& targetLine = sentences[currentLineIndex][0];  // 현재 문장

    //// 입력창 배경 생성
    sf::RectangleShape inputBackground(sf::Vector2f(500.f, 40.f)); // 입력창 크기
    inputBackground.setFillColor(sf::Color(240, 240, 240)); // 밝은 회색 배경
    inputBackground.setOutlineThickness(2.f);
    inputBackground.setOutlineColor(sf::Color(100, 100, 100)); // 테두리 색상

    FloatRect inputTextBounds = inputBackground.getGlobalBounds();
    inputBackground.setOrigin(inputTextBounds.getCenter());
    inputBackground.setPosition({ x , y });

    window.draw(inputBackground);

    // 입력 텍스트가 너무 길면 화면에 맞게 조정할 스케일 계산
    float maxChars = areaWidth / letterSpacing;
    float scale = 1.0f;
    if (inputLine.size() > maxChars) {
        scale = maxChars / static_cast<float>(inputLine.size());
    }

    // 입력 텍스트 출력
    for (size_t k = 0; k < inputLine.size(); ++k)
    {
        Text inputText(font, inputLine.substr(k, 1), fontSize) ;

        if (k < targetLine.size() && inputLine[k] == targetLine[k]) {
            inputText.setFillColor(sf::Color::Green); // 맞으면 초록색
        }
        else {
            inputText.setFillColor(sf::Color::Red); // 틀리면 빨간색
        }
        // **텍스트의 원점을 왼쪽 상단으로 설정**
        inputText.setOrigin(Vector2f(0.0f, 0.0f));

        // **입력창 안에서 한 글자씩 배치**
        float textX = x - (inputLine.size() * letterSpacing * scale) / 2 + k * letterSpacing * scale;
        //float textX = x - (inputLine.size() * letterSpacing) / 2 + k * letterSpacing;
        float textY = y - fontSize / 2; // 입력창 중앙에 오도록 조정
        inputText.setPosition(Vector2f(textX, textY));
        inputText.setScale(Vector2f(scale, scale)); // 필요시 스케일 조정

        window.draw(inputText);
    }
    //// 커서 추가 (깜박이기 기능)
    static sf::Clock cursorClock;
    static bool showCursor = true;

    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }

    if (showCursor) {
        sf::RectangleShape cursor(sf::Vector2f(2.f, fontSize));
        cursor.setFillColor(sf::Color::Black);

        float inputCenterX = inputBackground.getPosition().x;

        //float cursorX = x - (inputLine.size() * letterSpacing) / 2 + inputLine.size() * letterSpacing + 3.f;
        float cursorX = x - (inputLine.size() * letterSpacing * scale) / 2 + inputLine.size() * letterSpacing * scale + 3.f;

        //float cursorX = inputCenterX - (inputLine.size() * letterSpacing) / 2 + inputLine.size() * letterSpacing + 3.f;
        float cursorY = y - fontSize / 2;
        cursor.setPosition(Vector2f(cursorX, cursorY));

        window.draw(cursor);
    }
}
