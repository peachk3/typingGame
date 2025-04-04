#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Utils.h"
#include "FileLoader.hpp"
#include "TextUtil.hpp"
#include "networkManager.h"
#include "gameManager.h"
#include "TypingGame.hpp"
//#include "typingModeHandler.hpp"
#include <filesystem>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
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

    //_setmode(_fileno(stdout), _O_U16TEXT);  // UTF-16 출력 활성화
    //_setmode(_fileno(stderr), _O_U16TEXT);  // 오류 출력도 UTF-16 모드 설정
    // 이거 사용시 cout 
    //std::wcout << L"유니코드 출력 테스트: 안녕하세요! " << std::endl;
    //std::wcerr << L"폰트 로드 실패!" << std::endl;  // 오류 메시지 출력


    // window창 3.0 버전
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"게임 메인 화면");
    sf::Font font;

    NetworkManager client;
    GameManager game;
    FileLoader loader;
    TextUtil util;
    TypingGame typing;
    //handleTpMode typinghandler;

    // 프로필 , 배경, 한글, 영어, 코딩, 배틀
    Texture profileTexture, backGroundTexture, korButtonTexture, engButtonTexture, codeButtonTexture, matchButtonTexture,
        codeJavaButtonTexture, codeCppButtonTexture, codePyButtonTexture, fileSelectTexture;

    // 폰트 로드 실패일시 종속성 확인해보기
    if (!font.openFromFile("D2Coding.ttf")) {
        wcerr << L"폰트 로드 실패!" << endl;
        return -1;
    }

    // 배경 이미지 로드
    if (!backGroundTexture.loadFromFile("./assets/image/bgimg.png")) {
        wcerr << L"배경 이미지 로드 실패!" << endl;
        return -1;
    }

    // 배경 sprite 설정
    Sprite backGroundSprite(backGroundTexture);
    Vector2u backTextureSize = backGroundTexture.getSize();
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
        wcerr << L"프로필 이미지 로드 실패!" << endl;
        return -1;
    }

    // 프로필 Sprite
    Sprite profileSprite(profileTexture);
    Vector2u profileTextureSize = profileTexture.getSize();
    float scalexX = 50.f / profileTextureSize.x;  // 가로 크기를 50px로 조정
    float scaleyY = 50.f / profileTextureSize.y;   // 세로 크기를 50px로 조정
    profileSprite.setScale(Vector2(scalexX, scaleyY));
    profileSprite.setPosition(Vector2f(20.f, 20.f));  // 화면 왼쪽 상단에 위치

    // 프로필 Text 설정 (왼쪽 상단)
    Text profileText(font, "My Profile", 20);
    profileText.setPosition(Vector2f(100.f, 20.f));  // 이미지 오른쪽에 텍스트 배치
    profileText.setFillColor(Color::Black);

    // 한글 모드 이미지 로드
    if (!korButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"koreng 버튼 이미지 로드 실패!" << endl;
        return -1;
    }

    // 한글 Sprite 설정
    Sprite korButtonSprite(korButtonTexture);
    Vector2u textureSize = korButtonTexture.getSize();
    float scaleX = 50.f / textureSize.x;  // 가로 크기를 50px로 조정
    float scaleY = 50.f / textureSize.y;   // 세로 크기를 50px로 조정
    korButtonSprite.setScale(Vector2(scaleX, scaleY));
    korButtonSprite.setTexture(korButtonTexture);
    korButtonSprite.setPosition(Vector2f(320.f, 190.f));


    // 한글 Text 설정
    Text korButtonText(font, L"한글 타자 연습");
    korButtonText.setPosition(Vector2f(400.f, 200.f));  // 이미지 오른쪽에 텍스트 배치
    korButtonText.setCharacterSize(30);
    korButtonText.setFillColor(Color::Black);

    // 영어 모드 이미지 로드
    if (!engButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"Typing 버튼 이미지 로드 실패!" << endl;
        return -1;
    }

    // 영문 Sprite 설정
    Sprite engButtonSprite(engButtonTexture);
    engButtonSprite.setScale(Vector2(scaleX, scaleY));
    engButtonSprite.setTexture(engButtonTexture);
    engButtonSprite.setPosition(Vector2f(320.f, 290.f)); // x,y


    // 영문 Text 설정
    Text engButtonText(font, L"영문 타자 연습");
    engButtonText.setPosition(Vector2f(400.f, 300.f));  // 이미지 오른쪽에 텍스트 배치
    engButtonText.setCharacterSize(30);
    engButtonText.setFillColor(Color::Black);

    // 코딩 모드 이미지 로드
    if (!codeButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"Typing 버튼 이미지 로드 실패!" << endl;
        return -1;
    }

    // 코딩 모드 Sprite 설정
    Sprite codeButtionSprite(codeButtonTexture);
    codeButtionSprite.setScale(Vector2(scaleX, scaleY));
    //typingButtonSprite.setScale(Vector2f(0.5f, 0.5f));
    codeButtionSprite.setTexture(engButtonTexture);
    codeButtionSprite.setPosition(Vector2f(320.f, 390.f)); // x,y

    // 코딩 모드 Text 설정
    Text codeButtonText(font, L"코딩 모드 연습");
    codeButtonText.setPosition(Vector2f(400.f, 400.f));  // 이미지 오른쪽에 텍스트 배치
    codeButtonText.setCharacterSize(30);
    codeButtonText.setFillColor(Color::Black);

    // 배틀 모드 이미지 로드
    if (!matchButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"Match 버튼 이미지 로드 실패!" << endl;
        return -1;
    }

    // 배틀 Sprite 설정
    Sprite matchButtonSprite(matchButtonTexture);
    matchButtonSprite.setScale(Vector2(scaleX, scaleY));
    matchButtonSprite.setTexture(matchButtonTexture);
    matchButtonSprite.setPosition(Vector2f(320.f, 490.f));


    // 배틀 Text 설정
    Text matchButtonText(font, L"코딩 대결");
    matchButtonText.setPosition(Vector2f(400.f, 500.f));  // 이미지 오른쪽에 텍스트 배치
    matchButtonText.setCharacterSize(30);
    matchButtonText.setFillColor(Color::Black);

    // 코딩 모드 버튼 이미지 로드
    if (!codeJavaButtonTexture.loadFromFile("./assets/image/blueButton.png")) {
        wcerr << L"코딩 모드 버튼 이미지 로드 실패!" << endl;
        return -1;
    }

    if (!codeCppButtonTexture.loadFromFile("./assets/image/blueButton2.png")) {
        wcerr << L"코딩 모드 버튼 이미지 로드 실패!" << endl;
        return -1;
    }

    if (!codePyButtonTexture.loadFromFile("./assets/image/blueButton3.png")) {
        wcerr << L"코딩 모드 버튼 이미지 로드 실패!" << endl;
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
    javaButtonText.setPosition(Vector2
    (javabuttonBounds.position.x + (javabuttonBounds.size.x / 2) - (javatextBounds.size.x / 2),
        javabuttonBounds.position.y + (javabuttonBounds.size.y / 2) - (javatextBounds.size.y / 2))
    );
    //javaButtonText.setPosition(Vector2f(150.f, 300.f));

    // C++
    Sprite codeCppButtonSprite(codeCppButtonTexture);
    codeCppButtonSprite.setTexture(codeCppButtonTexture);
    codeCppButtonSprite.setScale(Vector2(1.2f, 1.2f));
    codeCppButtonSprite.setPosition(Vector2f(450.f, 300.f));

    Text cppButtonText(font, "C++", 35);
    cppButtonText.setPosition(Vector2f(450.f, 300.f)); 
    cppButtonText.setFillColor(Color::Black);

    // 버튼과 텍스트 위치 조정
    sf::FloatRect cppbuttonBounds = codeCppButtonSprite.getGlobalBounds();
    sf::FloatRect cpptextBounds = cppButtonText.getGlobalBounds();

    // 버튼 중앙에 텍스트 배치
    cppButtonText.setPosition(Vector2
    (cppbuttonBounds.position.x + (cppbuttonBounds.size.x / 2) - (cpptextBounds.size.x / 2),
        cppbuttonBounds.position.y + (cppbuttonBounds.size.y / 2) - (cpptextBounds.size.y / 2))
    );

    // Python
    Sprite codePyButtonSprite(codePyButtonTexture);
    codePyButtonSprite.setTexture(codePyButtonTexture);
    codePyButtonSprite.setScale(Vector2(1.2f, 1.2f));
    codePyButtonSprite.setPosition(Vector2f(750.f, 300.f));

    Text pyButtonText(font, "Python", 35);
    pyButtonText.setPosition(Vector2f(750.f, 300.f)); 
    pyButtonText.setFillColor(Color::Black);

    // 버튼과 텍스트 위치 조정
    sf::FloatRect pybuttonBounds = codePyButtonSprite.getGlobalBounds();
    sf::FloatRect pytextBounds = pyButtonText.getGlobalBounds();

    // 버튼 중앙에 텍스트 배치
    pyButtonText.setPosition(Vector2
    (pybuttonBounds.position.x + (pybuttonBounds.size.x / 2) - (pytextBounds.size.x / 2),
        pybuttonBounds.position.y + (pybuttonBounds.size.y / 2) - (pytextBounds.size.y / 2))
    );

    // 파일 불러오기

    // 파일 불러오기 이미지 로드
    if (!fileSelectTexture.loadFromFile("./assets/image/bluebutton4.png")) {
        wcerr << L"Typing 버튼 이미지 로드 실패!" << endl;
        return -1;
    }

    Sprite fileSelectSprite(fileSelectTexture);
	fileSelectSprite.setTexture(fileSelectTexture);
	fileSelectSprite.setScale(sf::Vector2(0.5f, 0.5f));
	fileSelectSprite.setPosition(sf::Vector2f(1200.f, 600.f));

	Text fileSelectText(font, L"파일 선택", 20);
	fileSelectText.setFillColor(Color::Black); 
    fileSelectText.setPosition(sf::Vector2f(1200.f, 600.f));


    bool isCodingMode = false; // 코딩 모드 여부 확인
    std::string folderPath = "./assets/file";

    while (window.isOpen()) {
        // 새로운 방식의 이벤트 처리
        //std::optional<Event> event;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // 마우스 클릭 시 버튼 클릭 확인
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(window);
                //sf::Vector2f worldPos(mousePos.x, mousePos.y);

                if (!isCodingMode) {
                    std::wcout << L"코딩 모드 아님 " << std::endl;

                    // 한글 타자 연습
                    if (korButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {

                        std::wcout << L"한글 타자 연습 시작!" << endl;
                        //typing.startNewGame(window, font, 18, wrapped);
                        // 함수 실행
                        try {
                            game.handleTypingGameMode(loader, util, typing, window, font, false);
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

                        // 함수 실행
                        //game.handleTypingMode(loader, util, typing, window, font);
                        game.handleTypingGameMode(loader, util, typing, window, font, false);


                    }

                    else if (codeButtionSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        isCodingMode = true;

                    }

                    // 매치
                    else if (matchButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        wcout << L"1:1 Match Button Clicked!" << endl;

                        if (!client.connectToServer()) {
                            return -1;
                        }

                        // 로딩창 실행 별도 스레드
                        bool isMatching = false;
                        std::thread loadingThread([&]() {
                            game.showLoadingScreen(window, font, isMatching);
                            wcout << L"로딩창 띄우기!!" << endl;
                            });

                        if (client.waitForMatch(window, font)) {
                            isMatching = true;
                            if (loadingThread.joinable()) loadingThread.join();
                            //loadingThread.join();
                            wcout << L"매치 완료되었슴!!" << endl;
                            //game.showLoadingScreen(window, font, isMatching);  // 매칭 이후 카운트다운 실행
                            //if (game.showCountdown(window, font)) {
                            wcout << L"게임 시작!!" << endl;
                            // 일단 파일 불러와서 게임하는 걸로 진행하고 있음
                            // -- 수정 필요
                            //game.handleTypingGameMode(loader, util, typing, window, font, true);
                            //game.runGame(window, font); 

                            std::string filePath;

                            //wcout << L"게임 시작창 이동 완료!!" << std::endl;
                            //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

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
                                    game.runGame2(window, font, 24, sentences);
                                }
                            }

                        }

                    }
                }
                // 코딩모드
                else {
                    std::wcout << L"코딩 모드 시작!" << std::endl;
                    std::string filePath;
                    // java
                    if (codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        filePath = getRandomFile(folderPath, ".java");
                        std::cout << "Selected file: " << filePath << std::endl;
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
                                typing.startNewGame(window, font, 24, sentences);
                            }
                        }
                    }
                    // C++
                    else if (codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        std::cout << "[디버깅] C++ 버튼이 클릭됨!" << std::endl;
                        filePath = getRandomFile(folderPath, ".cpp");
                        std::cout << "[디버깅] getRandomFile() 호출 완료" << std::endl;
                        //std::cout << "Selected file: " << filePath << std::endl;
                        // 파일 경로 디버깅 출력
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
                                typing.startNewGame(window, font, 24, sentences);
                            }
                        }

                    }
                    // 파이썬 
                    else if (codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        filePath = getRandomFile(folderPath, ".py");
                        std::cout << "Selected file: " << filePath << std::endl;

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
                                typing.startNewGame(window, font, 24, sentences);
                            }
                        }
                    }
                    // 파일 선택
                    else if (fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        //game.handleTypingMode(loader, util, typing, window, font);
                        game.handleTypingGameMode(loader, util, typing, window, font, false);

                    }
                    //if (!filePath.empty()) {
                    //    std::filesystem::path absPath = std::filesystem::absolute(filePath);
                    //    std::cout << "파일 절대 경로: " << absPath << std::endl;

                    //    auto sentences = typing.loadSentencesFromFile(filePath);
                    //    if (sentences.empty()) {
                    //        std::wcout << L"Error: 문장을 불러오지 못했습니다!" << std::endl;
                    //    }
                    //    else {
                    //        typing.startNewGame(window, font, 24, sentences);
                    //        isCodingMode = false; // 연습 시작 후 다시 일반 모드로 전환
                    //    }
                    //}

                }
            }
        }

        // 화면 그리기
        window.clear(Color::White);
        if (!isCodingMode) {
            window.draw(backGroundSprite); // 배경 사진 
            window.draw(whiteBackground);  // 하얀색 배경 
            window.draw(profileSprite);
            window.draw(profileText);    // 왼쪽 상단에 profileText 그리기

            window.draw(korButtonSprite);  // 한글 버튼 이미지 그리기
            window.draw(korButtonText);     // 한글 Text 이미지 그리기

            window.draw(engButtonSprite);  // 영어 버튼 이미지 그리기
            window.draw(engButtonText);  // 영어 Text 이미지 그리기

            window.draw(codeButtionSprite); // 코딩모드 버튼 이미지 그리기
            window.draw(codeButtonText);      // 코딩모드 Text 이미지 그리기

            window.draw(matchButtonSprite);  // 1:1 Match 버튼 이미지 그리기
            window.draw(matchButtonText);  // match Text 그리기
            //window.draw(button);
            //window.draw(buttonText);
        }
        else {
            // 코딩 모드 화면 그리기
            window.draw(codeJavaButtonSprite);
            window.draw(javaButtonText);
            window.draw(codeCppButtonSprite);
            window.draw(cppButtonText);
            window.draw(codePyButtonSprite);
            window.draw(pyButtonText);
			window.draw(fileSelectSprite);
			window.draw(fileSelectText);
        }
        window.display();

    }// while window open
    return 0;


}// main()