#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <iostream>
#include <locale>
#include <filesystem>
#include <codecvt>
#include <chrono>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <optional>
#include "userManager.hpp"
#include "LoginUI.hpp"
#include "RankingUI.hpp"
#include "Utils.h"
#include "FileLoader.hpp"
#include "TextUtil.hpp"
#include "networkManager.h"
#include "gameManager.h"
#include "TypingGame.hpp"

using namespace std;
using namespace sf;

bool showRankingUI = false;
bool loggedOut = false;  // 로그아웃 여부를 확인하는 변수

int main() {
    int MAIN_WINDOW_WIDTH = 1280;
    int MAIN_WINDOW_HEIGHT = 720;
    sf::RenderWindow gameWindow(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"타자 연습 게임");

    // 로그인 관련 객체
    MySQLConnector db;
    std::string userID;
    bool loggedIn = false;

    // 랭킹 UI 객체
    RankingUI rankingUI;

    // 게임 관련 객체들
    NetworkManager client;
    GameManager game;
    FileLoader loader;
    TextUtil util;
    TypingGame typing;

    // 전체 폴더 경로
    std::string folderPath = "./assets/file";
    // 코딩 모드 폴더 경로
    std::string filePath;

    // 폰트 로드
    sf::Font font;
    if (!font.openFromFile("D2Coding.ttf")) {
        std::cerr << "폰트 로드 실패!" << std::endl;
        return -1;
    }

    // 랭킹 아이콘 로드
    sf::Texture rankingIconTexture;
    if (!rankingIconTexture.loadFromFile("./assets/image/ranking_icon2.png")) {
        std::cerr << "아이콘 이미지 로드 실패!" << std::endl;
        return -1;
    }
    sf::Sprite rankingIcon(rankingIconTexture);
    rankingIcon.setPosition(Vector2f(MAIN_WINDOW_WIDTH - 120, 10));
    rankingIcon.setScale({ 0.2f, 0.2f });

    // 로그아웃 버튼
    sf::RectangleShape logoutButton(sf::Vector2f(200, 60));
    logoutButton.setFillColor(sf::Color(200, 50, 50));
    logoutButton.setPosition(Vector2f(MAIN_WINDOW_WIDTH / 2 - 100, MAIN_WINDOW_HEIGHT - 70));

    sf::Text logoutButtonText(font, L"로그아웃", 30);
    logoutButtonText.setFillColor(sf::Color::Black);
    logoutButtonText.setPosition(Vector2f(logoutButton.getPosition().x + 40, logoutButton.getPosition().y + 15));

    // 게임 화면 관련 텍스처 및 스프라이트
    Texture profileTexture, backGroundTexture, korButtonTexture, engButtonTexture, codeButtonTexture, matchButtonTexture,
        codeJavaButtonTexture, codeCppButtonTexture, codePyButtonTexture, fileSelectTexture;

    // 배경 이미지 로드
    if (!backGroundTexture.loadFromFile("./assets/image/bgimg.png")) {
        std::cerr << "배경 이미지 로드 실패!" << std::endl;
        return -1;
    }

    // 배경 sprite 설정
    Sprite backGroundSprite(backGroundTexture);
    backGroundSprite.setTexture(backGroundTexture);
    backGroundSprite.setPosition(Vector2f(0.f, 0.f));

    // 불투명 배경 설정
    sf::RectangleShape whiteBackground(sf::Vector2f(800.f, 400.f));
    whiteBackground.setPosition({ 200.f, 150.f });
    whiteBackground.setFillColor(sf::Color(255, 255, 255, 200)); // 마지막 - 투명도
    whiteBackground.setOutlineThickness(1.f);
    whiteBackground.setOutlineColor(sf::Color(100, 100, 100)); // 테두리 색상

    // 프로필 이미지 로드
    if (!profileTexture.loadFromFile("./assets/image/user.png")) {
        std::cerr << "프로필 이미지 로드 실패!" << std::endl;
        return -1;
    }

    // 프로필 Sprite
    Sprite profileSprite(profileTexture);
    Vector2u profileTextureSize = profileTexture.getSize();
    float scalexX = 50.f / profileTextureSize.x;
    float scaleyY = 50.f / profileTextureSize.y;
    profileSprite.setScale(Vector2(scalexX, scaleyY));
    profileSprite.setPosition(Vector2f(20.f, 20.f));

    // 프로필 Text 설정 (왼쪽 상단)
    Text profileText(font, "My Profile", 20);
    profileText.setPosition(Vector2f(100.f, 20.f));
    profileText.setFillColor(Color::Black);

    // 한글 모드 이미지 로드
    if (!korButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "kor 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    // 한글 Sprite 설정
    Sprite korButtonSprite(korButtonTexture);
    Vector2u textureSize = korButtonTexture.getSize();
    float scaleX = 50.f / textureSize.x;
    float scaleY = 50.f / textureSize.y;
    korButtonSprite.setScale(Vector2(scaleX, scaleY));
    korButtonSprite.setTexture(korButtonTexture);
    korButtonSprite.setPosition(Vector2f(320.f, 190.f));

    // 한글 Text 설정
    Text korButtonText(font, L"한글 타자 연습");
    korButtonText.setPosition(Vector2f(400.f, 200.f));
    korButtonText.setCharacterSize(30);
    korButtonText.setFillColor(Color::Black);

    // 영어 모드 이미지 로드
    if (!engButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "eng 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    // 영문 Sprite 설정
    Sprite engButtonSprite(engButtonTexture);
    engButtonSprite.setScale(Vector2(scaleX, scaleY));
    engButtonSprite.setTexture(engButtonTexture);
    engButtonSprite.setPosition(Vector2f(320.f, 290.f));

    // 영문 Text 설정
    Text engButtonText(font, L"영문 타자 연습");
    engButtonText.setPosition(Vector2f(400.f, 300.f));
    engButtonText.setCharacterSize(30);
    engButtonText.setFillColor(Color::Black);

    // 코딩 모드 이미지 로드
    if (!codeButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "code mode 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    // 코딩 모드 Sprite 설정
    Sprite codeButtionSprite(codeButtonTexture);
    codeButtionSprite.setScale(Vector2(scaleX, scaleY));
    codeButtionSprite.setTexture(engButtonTexture);
    codeButtionSprite.setPosition(Vector2f(320.f, 390.f));

    // 코딩 모드 Text 설정
    Text codeButtonText(font, L"코딩 모드 연습");
    codeButtonText.setPosition(Vector2f(400.f, 400.f));
    codeButtonText.setCharacterSize(30);
    codeButtonText.setFillColor(Color::Black);

    // 배틀 모드 이미지 로드
    if (!matchButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "Match 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    // 배틀 Sprite 설정
    Sprite matchButtonSprite(matchButtonTexture);
    matchButtonSprite.setScale(Vector2(scaleX, scaleY));
    matchButtonSprite.setTexture(matchButtonTexture);
    matchButtonSprite.setPosition(Vector2f(320.f, 490.f));

    // 배틀 Text 설정
    Text matchButtonText(font, L"코딩 대결");
    matchButtonText.setPosition(Vector2f(400.f, 500.f));
    matchButtonText.setCharacterSize(30);
    matchButtonText.setFillColor(Color::Black);

    // 코딩 모드 버튼 이미지 로드
    if (!codeJavaButtonTexture.loadFromFile("./assets/image/blueButton.png")) {
        std::cerr << "코딩 모드 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    if (!codeCppButtonTexture.loadFromFile("./assets/image/blueButton2.png")) {
        std::cerr << "코딩 모드 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    if (!codePyButtonTexture.loadFromFile("./assets/image/blueButton3.png")) {
        std::cerr << "코딩 모드 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    // Java - 하늘색
    Sprite codeJavaButtonSprite(codeJavaButtonTexture);
    codeJavaButtonSprite.setTexture(codeJavaButtonTexture);
    codeJavaButtonSprite.setScale(sf::Vector2f(1.2f, 1.2f));
    codeJavaButtonSprite.setPosition(sf::Vector2f(150.f, 300.f));

    Text javaButtonText(font, "Java", 35);
    javaButtonText.setFillColor(Color::Black);

    // 버튼과 텍스트 위치 조정
    sf::FloatRect javabuttonBounds = codeJavaButtonSprite.getGlobalBounds();
    sf::FloatRect javatextBounds = javaButtonText.getGlobalBounds();

    // 버튼 중앙에 텍스트 배치
    javaButtonText.setPosition(Vector2f(
        javabuttonBounds.position.x + (javabuttonBounds.size.x / 2) - (javatextBounds.size.x / 2),
        javabuttonBounds.position.y + (javabuttonBounds.size.y / 2) - (javatextBounds.size.y / 2))
    );

    // C++
    Sprite codeCppButtonSprite(codeCppButtonTexture);
    codeCppButtonSprite.setTexture(codeCppButtonTexture);
    codeCppButtonSprite.setScale(Vector2f(1.2f, 1.2f));
    codeCppButtonSprite.setPosition(Vector2f(450.f, 300.f));

    Text cppButtonText(font, "C++", 35);
    cppButtonText.setPosition(Vector2f(450.f, 300.f));
    cppButtonText.setFillColor(Color::Black);

    // 버튼과 텍스트 위치 조정
    sf::FloatRect cppbuttonBounds = codeCppButtonSprite.getGlobalBounds();
    sf::FloatRect cpptextBounds = cppButtonText.getGlobalBounds();

    // 버튼 중앙에 텍스트 배치
    cppButtonText.setPosition(Vector2f(
        cppbuttonBounds.position.x + (cppbuttonBounds.size.x / 2) - (cpptextBounds.size.x / 2),
        cppbuttonBounds.position.y + (cppbuttonBounds.size.y / 2) - (cpptextBounds.size.y / 2))
    );

    // Python
    Sprite codePyButtonSprite(codePyButtonTexture);
    codePyButtonSprite.setTexture(codePyButtonTexture);
    codePyButtonSprite.setScale(Vector2f(1.2f, 1.2f));
    codePyButtonSprite.setPosition(Vector2f(750.f, 300.f));

    Text pyButtonText(font, "Python", 35);
    pyButtonText.setPosition(Vector2f(750.f, 300.f));
    pyButtonText.setFillColor(Color::Black);

    // 버튼과 텍스트 위치 조정
    sf::FloatRect pybuttonBounds = codePyButtonSprite.getGlobalBounds();
    sf::FloatRect pytextBounds = pyButtonText.getGlobalBounds();

    // 버튼 중앙에 텍스트 배치
    pyButtonText.setPosition(Vector2f(
        pybuttonBounds.position.x + (pybuttonBounds.size.x / 2) - (pytextBounds.size.x / 2),
        pybuttonBounds.position.y + (pybuttonBounds.size.y / 2) - (pytextBounds.size.y / 2))
    );

    // 파일 선택 버튼
    if (!fileSelectTexture.loadFromFile("./assets/image/bluebutton4.png")) {
        std::cerr << "파일 선택 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }

    Sprite fileSelectSprite(fileSelectTexture);
    fileSelectSprite.setTexture(fileSelectTexture);
    fileSelectSprite.setScale(sf::Vector2f(0.5f, 0.5f));
    fileSelectSprite.setPosition(sf::Vector2f(1200.f, 600.f));

    Text fileSelectText(font, L"파일 선택", 20);
    fileSelectText.setFillColor(Color::Black);
    fileSelectText.setPosition(sf::Vector2f(1200.f, 600.f));

    // 코딩 모드 여부 확인 변수
    bool isCodingMode = false;

    // 메인 게임 루프
    while (gameWindow.isOpen()) {
        while (const std::optional<Event> event = gameWindow.pollEvent()) {
            if (event->is<Event::Closed>()) {
                gameWindow.close();
            }

            if (event->is<Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(gameWindow);

                // 로그인 상태일 때 로그아웃 버튼 및 랭킹 아이콘 클릭 처리
                if (loggedIn) {
                    if (rankingIcon.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        showRankingUI = true;
                    }

                    // 로그아웃 버튼 클릭 시
                    else if (logoutButton.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        loggedIn = false;  // 로그인 상태 초기화
                        loggedOut = true;  // 로그아웃 플래그 설정
                    }

                    // 기본 게임 메인 화면 버튼 클릭 처리
                    else if (!isCodingMode) {
                       
                        // 한글 타자 연습
                        if (korButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            std::wcout << L"한글 타자 연습 시작!" << endl;
                            try {
                                game.handleTypingGameMode(loader, util, typing, gameWindow, font, false);
                            }
                            catch (const std::exception& e) {
                                std::cerr << "[예외 발생] handleTypingMode: " << e.what() << std::endl;
                            }
                            catch (...) {
                                std::cerr << "[예외 발생] 알 수 없는 예외 in handleTypingMode()" << std::endl;
                            }
                        }

                        // 영어 모드
                        else if (engButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            game.handleTypingGameMode(loader, util, typing, gameWindow, font, false);
                        }

                        // 코딩 모드
                        else if (codeButtionSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = true;
                        }

                        // 매치 모드
                        else if (matchButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            wcout << L"1:1 Match Button Clicked!" << endl;

                            if (!client.connectToServer()) {
                                return -1;
                            }

                            // 로딩창 실행 별도 스레드
                            bool isMatching = false;
                            std::thread loadingThread([&]() {
                                game.showLoadingScreen(gameWindow, font, isMatching);
                                wcout << L"로딩창 띄우기!!" << endl;
                                });

                            if (client.waitForMatch(gameWindow, font)) {
                                isMatching = true;
                                if (loadingThread.joinable()) loadingThread.join();
                                wcout << L"매치 완료되었슴!!" << endl;
                                wcout << L"게임 시작!!" << endl;

                                std::string filePath;
                                filePath = getRandomFile(folderPath, ".java");
                                if (filePath.empty()) {
                                    std::cerr << "[디버깅] Error: No file selected!" << std::endl;
                                    std::cout << filePath << std::endl;
                                }
                                else {
                                    std::cout << "[디버깅] Selected file: " << filePath << std::endl;
                                    // 파일 경로가 올바른지 확인
                                    std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                    std::cout << "[디버깅] Absolute path: " << absPath << std::endl;

                                    // 파일에서 문장 읽기
                                    std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);

                                    // 문장 로딩 확인
                                    if (sentences.empty()) {
                                        std::cout << "Error: No sentences loaded!" << std::endl;
                                    }
                                    else {
                                        std::cout << "[디버깅] 게임 시작: 문장 수 = " << sentences.size() << std::endl;
                                        game.runGame2(gameWindow, font, 24, sentences);
                                    }
                                }
                            }
                        }

                    }
                    // 코딩 모드 화면 버튼 클릭 처리
                    else {

                        // Java 버튼
                        if (codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            filePath = getRandomFile(folderPath, ".java");
                            if (!filePath.empty()) {
                                std::cout << "[디버깅] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::cout << "[디버깅] Absolute path: " << absPath << std::endl;

                                // 파일에서 문장 읽기
                                std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);

                                if (!sentences.empty()) {
                                    std::cout << "[디버깅] 게임 시작: 문장 수 = " << sentences.size() << std::endl;
                                    typing.startNewGame(gameWindow, font, 24, sentences);
                                }
                            }
                        }
                        // C++ 버튼
                        else if (codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            filePath = getRandomFile(folderPath, ".cpp");
                            if (!filePath.empty()) {
                                std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);
                                if (!sentences.empty()) {
                                    typing.startNewGame(gameWindow, font, 24, sentences);
                                }
                            }
                        }
                        // Python 버튼
                        else if (codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            filePath = getRandomFile(folderPath, ".py");
                            if (!filePath.empty()) {
                                std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);
                                if (!sentences.empty()) {
                                    typing.startNewGame(gameWindow, font, 24, sentences);
                                }
                            }
                        }
                        // 파일 선택 버튼
                        else if (fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            game.handleTypingGameMode(loader, util, typing, gameWindow, font, false);
                        }
                        // 메인 화면으로 돌아가기
                        else if (!fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = false; // 코딩 모드 해제
                        }

                    }

                }
                // 로그인 상태가 아닐 때 랭킹 아이콘 클릭 처리
                else {
                    if (rankingIcon.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        showRankingUI = true;
                    }
                }

            }
        }


        gameWindow.clear();

        // 로그인 상태가 아닐 때 로그인 UI 표시
        if (!loggedIn) {
            // 로그인 UI를 표시
            if (showLoginWindow(gameWindow, db, userID)) {
                std::cout << "로그인 성공! 유저 ID: " << userID << std::endl;
                loggedIn = true;
            }
            else {
                // 로그인 화면에서 랭킹 아이콘도 표시
                gameWindow.draw(rankingIcon);
            }
        }
        else {
            if (!isCodingMode) {
                // 기본 게임 메인 화면 구성요소 그리기
                gameWindow.draw(backGroundSprite); // 배경 사진 
                gameWindow.draw(whiteBackground);  // 하얀색 배경 
                gameWindow.draw(profileSprite);
                gameWindow.draw(profileText);    // 왼쪽 상단에 profileText 그리기

                gameWindow.draw(korButtonSprite);  // 한글 버튼 이미지 그리기
                gameWindow.draw(korButtonText);    // 한글 Text 이미지 그리기

                gameWindow.draw(engButtonSprite);  // 영어 버튼 이미지 그리기
                gameWindow.draw(engButtonText);    // 영어 Text 이미지 그리기

                gameWindow.draw(codeButtionSprite); // 코딩모드 버튼 이미지 그리기
                gameWindow.draw(codeButtonText);    // 코딩모드 Text 이미지 그리기

                gameWindow.draw(matchButtonSprite);  // 1:1 Match 버튼 이미지 그리기
                gameWindow.draw(matchButtonText);    // match Text 그리기

                gameWindow.draw(rankingIcon);        // 랭킹 아이콘 그리기
                gameWindow.draw(logoutButton);       // 로그아웃 버튼 그리기
                gameWindow.draw(logoutButtonText);   // 로그아웃 텍스트 그리기

            }
            else {

                // 코딩 모드 화면 구성요소 그리기
                gameWindow.draw(backGroundSprite); // 배경 표시
                gameWindow.draw(whiteBackground); // 흰색 배경
                gameWindow.draw(codeJavaButtonSprite); // Java 버튼
                gameWindow.draw(javaButtonText);
                gameWindow.draw(codeCppButtonSprite); // C++ 버튼
                gameWindow.draw(cppButtonText);
                gameWindow.draw(codePyButtonSprite); // Python 버튼
                gameWindow.draw(pyButtonText);
                gameWindow.draw(fileSelectSprite); // 파일 선택 버튼
                gameWindow.draw(fileSelectText);

            }
            gameWindow.draw(rankingIcon); // 랭킹 아이콘
            gameWindow.draw(logoutButton); // 로그아웃 버튼
            gameWindow.draw(logoutButtonText);
        }

        gameWindow.display();

        // 랭킹 UI를 별도 스레드에서 실행
        if (showRankingUI) {
            std::thread rankingThread([&]() {
                rankingUI.show();  // 랭킹 UI를 별도의 스레드에서 실행
                });
            rankingThread.detach();  // 스레드가 종료될 때까지 기다리지 않도록 분리
            showRankingUI = false;
        }

        // 로그아웃 플래그가 true이면 로그인 화면으로 돌아감
        if (loggedOut) {
            gameWindow.clear();
            loggedOut = false;  // 로그아웃 상태 초기화
        }
    }

    return 0;
}