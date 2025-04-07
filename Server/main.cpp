#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <iostream>
#include <locale>
#include <filesystem>
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
// 이름 수정 예정
#include "UIManager.hpp"
//---- 추가
#include <sstream>
#include "GameState.hpp"
#include "Scene.hpp"
#include "UIAlign.hpp"
#include "DrawUI.hpp"
#include "ResultScreen.hpp"
#include "DrawUIR.hpp"
#include "FileSelectList.hpp"
#include "UserProfileUI.hpp"
#include "UserProfileSelectUI.hpp"
#include <iomanip>  // std::hex, std::setw
#include <codecvt>  // wide → utf8 변환 (선택)

using namespace std;
using namespace sf;

bool showStartScreen(sf::RenderWindow& window);  // 함수 원형 선언
bool showRankingUI = false;
bool loggedOut = false;  // 로그아웃 여부를 확인하는 변수


int main() {
    float MAIN_WINDOW_WIDTH = 1280.f;
    float MAIN_WINDOW_HEIGHT = 720.f;

    sf::RenderWindow gameWindow(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"타자 연습 게임");
    // 회원가입창
    RenderWindow signupWindow(VideoMode({ 500, 400 }), L"회원가입");


    std::wcout.imbue(std::locale(""));         // 유니코드 로케일 설정

    // 로그인 관련 객체
    MySQLConnector db;
    std::string userID;
    bool loggedIn = false;

    // 랭킹 UI 객체
    RankingUI rankingUI;

    // 게임 관련 객체들
    NetworkManager client;
    GameManager gm;
    //FileLoader loader;
    //TextUtil util;
    //TypingGame typing;

    // UI 관련 객체
    UIManager ui;

    // --------------------------- 추가
    GameState game;


    // 테스트 용 임시 유저 정보
    //game.user.id = L"test_user_001";
    game.user.nickname = L"타자마스터";
    game.user.profileImagePath = L"assets/profile_img/default_avatar.png";

    int fontSize = game.user.fontSize;

    game.selectPath = L"assets/hangleFile/애국가.txt";


    // 게임 기록 - 일단 임의로 초기화
    game.user.point = 1234;
    game.user.bestWPM = 87.5f;
    game.user.bestAccuracy = 96.2f;
    game.user.totalPlayCount = 12;
    game.user.totalPlayTime = 983.7f;

    // 현제 플레이어의 게임 화면 저장
    //game.currentScene = Scene::MAIN_MENU; // 초기값 줬으므로 필요 x 나중에 복사용으로 남겨뒀음

    game.currentScene = Scene::MAIN_MENU; // 초기값 줬으므로 필요 x 나중에 복사용으로 남겨뒀음
    //TYPING_GAME

    // 플레이 타임 기록용(?)
    sf::Clock clock;


    // 파일 목록용 테스트 변수
    std::vector<FileOption> fileOptions;
    std::wstring selectMod = L"한글";
    std::vector<std::wstring> typingFilePath;

    typingFilePath = {
            L"./assets/typing/한글1.txt",
            L"assets/typing/한글2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/한글1.txt",
            L"assets/typing/한글2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/coding.cpp",
            L"assets/typing/한글1.txt",

    };

    // --------------------------------------

    // 전체 폴더 경로
    std::wstring folderPath = L"./assets/file";
    // 코딩 모드 폴더 경로
    std::wstring filePath;

    if (!ui.loadFont("./assets/font/D2Coding.ttf")) {
        std::cerr << "폰트 로드 실패!" << std::endl;
        return -1;
    }
    Font font = ui.getFont();

    //------------
        // 배경 이미지 로딩
    Texture bgTexture;
    if (!bgTexture.loadFromFile("./assets/image/login.png")) {
        cerr << "로그인 배경 이미지 로딩 실패!" << endl;
        return false;
    }
    Sprite background(bgTexture);
    background.setScale(Vector2f(
        MAIN_WINDOW_WIDTH / static_cast<float>(bgTexture.getSize().x),
        MAIN_WINDOW_HEIGHT / static_cast<float>(bgTexture.getSize().y)
    ));
    //----------------
    // 수정: 로그아웃 버튼 이미지
    sf::Texture logoutButtonTexture;
    if (!logoutButtonTexture.loadFromFile("assets/image/logout.png")) {
        std::cerr << "로그아웃 버튼 이미지 로드 실패!" << std::endl;
        return -1;
    }
    sf::Sprite logoutButton(logoutButtonTexture);
    // 스케일 먼저 적용
    logoutButton.setScale({ 0.1f, 0.1f });  // 필요시 크기 조절

    // 스케일 적용 후 정확한 크기 가져오기
    FloatRect buttonBounds = logoutButton.getGlobalBounds();

    // 위치: 화면 하단 중앙에, 아래에서 안 띄우기
    logoutButton.setPosition({
        MAIN_WINDOW_WIDTH / 2.f - buttonBounds.size.x / 2.f,
        MAIN_WINDOW_HEIGHT - buttonBounds.size.y
        });

    sf::Text logoutButtonText = ui.createText(L"로그아웃", 30, { logoutButton.getPosition().x + 40, logoutButton.getPosition().y + 15 }, sf::Color::Black);

    if (!ui.loadTexture("ranking", "./assets/image/ranking_icon2.png")) return -1;

    // 랭킹 아이콘
    sf::Sprite rankingIcon = ui.createIcon("./assets/image/ranking_icon2.png",
        Vector2f(MAIN_WINDOW_WIDTH - 120, 10),
        Vector2f(0.2f, 0.2f));

    // 게임 화면 관련 텍스처 및 스프라이트
    Texture profileTexture, backGroundTexture, korButtonTexture, engButtonTexture, codeButtonTexture, matchButtonTexture,
        codeJavaButtonTexture, codeCppButtonTexture, codePyButtonTexture, fileSelectTexture;

    // 배경 이미지 로드
    if (!ui.loadTexture("background", "./assets/image/backGround.png")) {

        return -1;
    }

    ui.loadTexture("background", "./assets/image/backGround.png");
    sf::Sprite backGroundSprite = ui.createSprite("background", { 0, 0 }, { MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT }); \
        //backGroundSprite.setPosition(Vector2f(0.f, 0.f));

        // 불투명 배경 설정
        sf::RectangleShape whiteBackground(sf::Vector2f(800.f, 400.f));
    whiteBackground.setPosition({ 200.f, 150.f });
    whiteBackground.setFillColor(sf::Color(255, 255, 255, 200)); // 마지막 - 투명도
    whiteBackground.setOutlineThickness(1.f);
    whiteBackground.setOutlineColor(sf::Color(100, 100, 100)); // 테두리 색상

    // 프로필 이미지 로드
    if (!ui.loadTexture("profile", "./assets/image/user.png")) return -1;

    sf::Sprite profileSprite = ui.createSprite("profile", { 20.f, 20.f }, { 50.f, 50.f });
    sf::Text profileText = ui.createText(L"내 프로필", 20, { 100.f, 20.f }, Color::Black);


    // 공통 스케일링을 위한 버튼 크기
    sf::Vector2f buttonSize(50.f, 50.f);

    // 한글 이미지 

    if (!ui.loadTexture("korButton", "./assets/image/button.png")) return -1;

    // 버튼 스프라이트 생성
    sf::Sprite korButtonSprite = ui.createSprite("korButton", { 320.f, 190.f }, buttonSize);
    sf::Text korButtonText = ui.createText(L"한글 타자 연습", 30, { 400.f, 200.f }, sf::Color::Black);

    if (!ui.loadTexture("engButton", "./assets/image/button.png")) return -1;
    // 버튼 텍스트 생성 (한글은 L"..." 사용)
    sf::Sprite engButtonSprite = ui.createSprite("engButton", { 320.f, 290.f }, buttonSize);
    sf::Text engButtonText = ui.createText(L"영어 타자 연습", 30, { 400.f, 290.f }, sf::Color::Black);

    // 이미지 로드
    if (!ui.loadTexture("codeButton", "./assets/image/button.png")) return -1;
    if (!ui.loadTexture("matchButton", "./assets/image/button.png")) return -1;

    // 코딩 모드 버튼
    sf::Sprite codeButtonSprite = ui.createSprite("codeButton", { 320.f, 390.f }, buttonSize);
    sf::Text codeButtonText = ui.createText(L"코딩 모드 연습", 30, { 400.f, 400.f }, sf::Color::Black);

    // 배틀 모드 버튼
    sf::Sprite matchButtonSprite = ui.createSprite("matchButton", { 320.f, 490.f }, buttonSize);
    sf::Text matchButtonText = ui.createText(L"코딩 대결", 30, { 400.f, 500.f }, sf::Color::Black);


    // ---------- 로그인 창
    float leftX = 330;
    float shiftY = 100;

    RectangleShape loginButton(Vector2f(250, 60));
    loginButton.setPosition({ leftX, 370 + shiftY });
    loginButton.setFillColor(Color::Green);

    Text loginText(font, "Login", 28);
    loginText.setPosition({ leftX + 85, 385 + shiftY });
    loginText.setFillColor(Color::White);

    //  ----------------------

    // ---------- 회원가입 창
    string nickname, id, password;
    bool isNicknameActive = true, isIdActive = false, isPassActive = false, showPassword = false;

    Text title(font, L"회원가입", 30);
    title.setPosition({ 200, 50 });
    title.setFillColor(Color::White);

    Text status(font, "", 20);
    status.setPosition({ 50, 85 });
    status.setFillColor(Color::Red);

    RectangleShape inputBox(Vector2f(350, 40)); // 입력 박스 크기 조정
    inputBox.setFillColor(Color::White);
    inputBox.setOutlineColor(Color::Black);
    inputBox.setOutlineThickness(2);

    RectangleShape nicknameBox = inputBox, idBox = inputBox, passBox = inputBox;
    nicknameBox.setPosition({ 100, 120 });
    idBox.setPosition({ 100, 180 });
    passBox.setPosition({ 100, 240 });

    Text nicknameLabel(font, "Name:", 20), idLabel(font, "ID:", 20), passLabel(font, "PW:", 20);
    nicknameLabel.setPosition({ 35, 125 });
    idLabel.setPosition({ 50, 185 });
    passLabel.setPosition({ 50, 245 });
    nicknameLabel.setFillColor(Color::White);
    idLabel.setFillColor(Color::White);
    passLabel.setFillColor(Color::White);

    Text nicknameText(font, "", 20), idText(font, "", 20), passText(font, "", 20);
    nicknameText.setPosition({ 110, 125 });
    idText.setPosition({ 110, 185 });
    passText.setPosition({ 110, 245 });
    nicknameText.setFillColor(Color::Black);
    idText.setFillColor(Color::Black);
    passText.setFillColor(Color::Black);

    // 커서 관련 변수
    RectangleShape cursor(Vector2f(2, 30));  // 커서 크기
    cursor.setFillColor(Color::Black);
    float cursorTimer = 0.f;  // 커서 깜빡임 시간
    float cursorBlinkRate = 10.f;  // 커서 깜빡임 주기 (초)
    bool cursorVisible = true;  // 커서 가시성 상태

    // 회원가입 버튼
    RectangleShape signupButton(Vector2f(200, 50));
    signupButton.setPosition({ 150, 330 });
    signupButton.setFillColor(Color::Blue);

    Text signupText(font, "Sign Up", 24);
    signupText.setPosition({ 210, 340 });
    signupText.setFillColor(Color::White);

    // 비밀번호 표시 체크박스 추가
    RectangleShape checkbox(Vector2f(20, 20));
    checkbox.setPosition({ 100, 290 });
    checkbox.setFillColor(Color::White);

    Text checkboxLabel(font, "Show Password", 20);
    checkboxLabel.setPosition({ 130, 290 });
    checkboxLabel.setFillColor(Color::White);

    Text checkMark(font, "V", 20);
    checkMark.setPosition({ 105, 285 });
    checkMark.setFillColor(Color::Black);
    checkMark.setString("");
    // -----------------
    // 텍스처 로딩
    if (!ui.loadTexture("codeJavaButton", "./assets/image/javaButton.png")) return -1;
    if (!ui.loadTexture("codeCppButton", "./assets/image/cppButton.png")) return -1;
    if (!ui.loadTexture("codePyButton", "./assets/image/pyButton.png")) return -1;
    if (!ui.loadTexture("fileSelectButton", "./assets/image/bluebutton4.png")) return -1;

    // 버튼 공통 스케일
    sf::Vector2f buttonScale(180.f, 120.f);
    sf::Vector2f fileButtonScale(100.f, 60.f);

    // Java 버튼
    sf::Sprite codeJavaButtonSprite = ui.createSprite("codeJavaButton", { MAIN_WINDOW_WIDTH / 3 - 150 , MAIN_WINDOW_HEIGHT / 2 - 70 }, buttonScale);
    //sf::Text javaButtonText = ui.createText(L"Java", 35, { 150.f, 300.f }, sf::Color::Black);
    //ui.centerTextOnSprite(codeJavaButtonSprite, javaButtonText);

    // C++ 버튼
    sf::Sprite codeCppButtonSprite = ui.createSprite("codeCppButton", { MAIN_WINDOW_WIDTH / 3 + 100 , MAIN_WINDOW_HEIGHT / 2 - 70 }, buttonScale);
 /*   sf::Text cppButtonText = ui.createText(L"C++", 35, { 450.f, 300.f }, sf::Color::Black);
    ui.centerTextOnSprite(codeCppButtonSprite, cppButtonText);*/

    // Python 버튼
    sf::Sprite codePyButtonSprite = ui.createSprite("codePyButton", { MAIN_WINDOW_WIDTH / 3 + 350 , MAIN_WINDOW_HEIGHT / 2 - 70 }, buttonScale);
    //sf::Text pyButtonText = ui.createText(L"Python", 35, { 750.f, 300.f }, sf::Color::Black);
    //ui.centerTextOnSprite(codePyButtonSprite, pyButtonText);

    // 파일 선택 버튼
    sf::Sprite fileSelectSprite = ui.createSprite("fileSelectButton", { MAIN_WINDOW_WIDTH - 150, MAIN_WINDOW_HEIGHT - 120 }, fileButtonScale);
    sf::Text fileSelectText = ui.createText(L"파일 선택", 20, { 1200.f, 600.f }, sf::Color::Black);
    ui.centerTextOnSprite(fileSelectSprite, fileSelectText);

    // 코딩 모드 여부 확인 변수
    bool isCodingMode = false;

    // 로그인 창 실행 전에 추가
    if (!showStartScreen(gameWindow)) {
        return 0;  // 사용자가 창을 닫은 경우 종료
    }

    // 추가 할 것들
    std::vector<sf::Sprite> sprites;
    std::vector<ImageOption> imageOptions;
    std::vector<sf::Text> profileTexts;

    // 프로필 선택 hover 효과
    sf::RectangleShape thumbnailHoverOutline;
    thumbnailHoverOutline.setSize({ 200, 200 }); // 썸네일 크기에 맞게
    thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
    thumbnailHoverOutline.setOutlineColor(sf::Color::Red);
    thumbnailHoverOutline.setOutlineThickness(4.f);

    // 기본 프로필 이미지 로드
    sf::Image img = loadImg(game.user.profileImagePath);
    game.user.profileTexture = sf::Texture(resizeImageKeepAspect(img));
    sf::Sprite userImage(game.user.profileTexture);

    // 메인 게임 루프
    while (gameWindow.isOpen()) {
        float deltaTime = 1.f / 60.f;
        std::wcout << L"[DEBUG] 현재 Scene: " << static_cast<int>(game.currentScene) << std::endl;
        sf::Vector2i mousePos = sf::Mouse::getPosition(gameWindow);
        sf::Vector2f worldPos = gameWindow.mapPixelToCoords(mousePos);

        while (const std::optional<Event> event = gameWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                gameWindow.close();

            switch (game.currentScene) {
            case Scene::PROFILE: {
                if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouse->button == sf::Mouse::Button::Left) {
                        std::wcout << sprites.size() << std::endl;
                        if (game.showImageOverlay) {
                            for (int i = 0; i < sprites.size(); ++i) {
                                if (sprites[i].getGlobalBounds().contains(worldPos)) {
                                    std::wcout << L"[debug!!]" << std::endl;

                                    imageOptions[i].onClick();

                                }
                            }

                            // 불러오기 버튼 클릭 확인
                            if (game.btn.loadImgBtnBounds.contains(worldPos)) {
                                std::wcout << L"[불러오기 버튼 클릭됨]" << std::endl;
                                auto originalPath = std::filesystem::current_path();	// 현재 폴더 경로 저장
                                std::wstring selectedImagePath = openImageFileDialog();

                                std::filesystem::current_path(originalPath);  // 다시 원래 경로로 되돌림 - 선택된 폴더 경로로 바뀌는 거 방지
                                sf::Image image;
                                if (image.loadFromFile(selectedImagePath)) {
                                    // 성공 처리
                                    std::wcout << L"[이미지를 성공적으로 불러왔습니다]" << std::endl;
                                    std::wcout << selectedImagePath << std::endl;
                                    game.user.profileImagePath = selectedImagePath;
                                    updateProfileImage(selectedImagePath, game, userImage);
                                    game.showImageOverlay = false;
                                    game.currentScene = Scene::PROFILE;


                                }
                                else {
                                    std::wcout << L"[이미지를 불러오지 못했습니다]" << std::endl;
                                    return -1;
                                }

                            }   // HOVER 이벤트

                        }
                        if (game.btn.selectImgBtnBounds.contains(worldPos)) {
                            game.showImageOverlay = true;
                            std::cout << 1 << std::endl;
                        }
                    }
                }
                else if (const auto* move = event->getIf<sf::Event::MouseMoved>()) {
                    sf::Vector2f mousePos = gameWindow.mapPixelToCoords(move->position);
                    game.bHoveringThumbnail = false;

                    for (int i = 0; i < sprites.size(); ++i) {
                        if (sprites[i].getGlobalBounds().contains(mousePos)) {
                            sf::FloatRect bounds = sprites[i].getGlobalBounds();
                            thumbnailHoverOutline.setPosition(bounds.position);
                            thumbnailHoverOutline.setSize(bounds.size);
                            game.bHoveringThumbnail = true;
                            break;
                        }
                    }
                }
                break;
            }
            case Scene::LOGIN: {
                std::wcout << L"[DEBUG] 현재 Scene: " << static_cast<int>(game.currentScene) << std::endl;
                if (event->is<Event::MouseButtonPressed>()) {
                    Vector2f mousePos = Vector2f(Mouse::getPosition(gameWindow));
                    if (rankingIcon.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        showRankingUI = true;
                    }
                    if (checkbox.getGlobalBounds().contains(mousePos)) {
                        showPassword = !showPassword;
                        passText.setString(showPassword ? password : string(password.size(), '*'));
                        checkMark.setString(showPassword ? "V" : "");
                    }

                    if (loginButton.getGlobalBounds().contains(mousePos)) {
                        if (db.checkLogin(id, password)) {
                            status.setFillColor(Color::Green);
                            status.setString("Login successful!");
                            userID = id;
                            gameWindow.display();
                            this_thread::sleep_for(chrono::seconds(2));
                            return true;
                        }
                        else {
                            status.setFillColor(Color::Red);
                            status.setString("Invalid ID or Password!");
                        }
                    }
                    else if (signupButton.getGlobalBounds().contains(mousePos)) {
                        showSignupWindow(gameWindow, db);
                    }

                    isIdActive = idBox.getGlobalBounds().contains(mousePos);
                    isPassActive = passBox.getGlobalBounds().contains(mousePos);
                }

                if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                    wchar_t c = static_cast<wchar_t>(textEvent->unicode);
                    if (isIdActive) {
                        if (c == '\b' && !id.empty()) id.pop_back();
                        else if (isalnum(c)) id += static_cast<char>(c);
                        idText.setString(id);
                    }
                    else if (isPassActive) {
                        if (c == '\b' && !password.empty()) password.pop_back();
                        else if (isalnum(c)) password += static_cast<char>(c);
                        passText.setString(showPassword ? password : string(password.size(), '*'));
                    }
                }

                if (event->is<Event::KeyPressed>()) {
                    if (Keyboard::isKeyPressed(Keyboard::Key::Tab)) {
                        isIdActive = !isIdActive;
                        isPassActive = !isPassActive;
                    }
                }
                break;
            }
            case Scene::SIGNUP: {
                if (event->is<Event::MouseButtonPressed>()) {
                    Vector2f mousePos = Vector2f(Mouse::getPosition(signupWindow));

                    // 체크박스 클릭
                    if (checkbox.getGlobalBounds().contains(mousePos)) {
                        showPassword = !showPassword;  // 비밀번호 표시 여부 토글
                        passText.setString(showPassword ? password : string(password.size(), '*'));

                        // 체크박스 내부에 V 표시 추가/제거
                        if (showPassword) {
                            checkMark.setString("V");
                        }
                        else {
                            checkMark.setString("");
                        }
                    }

                    isNicknameActive = nicknameBox.getGlobalBounds().contains(mousePos);
                    isIdActive = idBox.getGlobalBounds().contains(mousePos);
                    isPassActive = passBox.getGlobalBounds().contains(mousePos);
                    if (signupButton.getGlobalBounds().contains(mousePos)) {
                        // 필수 입력 체크
                        if (nickname.empty() || id.empty() || password.empty()) {
                            status.setFillColor(Color::Red);
                            status.setString("All fields must be filled!");
                        }
                        else {
                            if (db.registerUser(id, password, nickname)) {
                                status.setFillColor(Color::Green);
                                status.setString("Sign Up successful! Returning to login...");
                                signupWindow.display();
                                this_thread::sleep_for(chrono::seconds(2));
                                signupWindow.close();
                            }
                            else {
                                status.setFillColor(Color::Red);
                                status.setString("ID already exists.");
                            }
                        }

                        signupWindow.clear(Color::Black);
                        signupWindow.draw(title);
                        signupWindow.draw(nicknameLabel);
                        signupWindow.draw(idLabel);
                        signupWindow.draw(passLabel);
                        signupWindow.draw(nicknameBox);
                        signupWindow.draw(idBox);
                        signupWindow.draw(passBox);
                        signupWindow.draw(nicknameText);
                        signupWindow.draw(idText);
                        signupWindow.draw(passText);
                        signupWindow.draw(signupButton);
                        signupWindow.draw(signupText);
                        signupWindow.draw(status);
                        signupWindow.draw(checkbox);
                        signupWindow.draw(checkboxLabel);
                        signupWindow.draw(checkMark);
                        signupWindow.display();

                        this_thread::sleep_for(chrono::seconds(2));
                        signupWindow.close();
                    }
                }

                if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                    wchar_t c = static_cast<wchar_t>(textEvent->unicode);
                    if (isNicknameActive) {
                        if (c == '\b' && !nickname.empty()) nickname.pop_back();
                        else if (isalnum(c)) nickname += static_cast<char>(c);
                        nicknameText.setString(nickname);
                    }
                    else if (isIdActive) {
                        if (c == '\b' && !id.empty()) id.pop_back();
                        else if (isalnum(c)) id += static_cast<char>(c);
                        idText.setString(id);
                    }
                    else if (isPassActive) {
                        if (c == '\b' && !password.empty()) password.pop_back();
                        else if (isalnum(c)) password += static_cast<char>(c);
                        passText.setString(showPassword ? password : string(password.size(), '*'));
                    }
                }

                if (event->is<Event::KeyPressed>()) {
                    if (Keyboard::isKeyPressed(Keyboard::Key::Tab)) {
                        if (isNicknameActive) {
                            isNicknameActive = false;
                            isIdActive = true;
                        }
                        else if (isIdActive) {
                            isIdActive = false;
                            isPassActive = true;
                        }
                        else if (isPassActive) {
                            isPassActive = false;
                            isNicknameActive = true;
                        }
                    }
                }
                break;
            }

            case Scene::MAIN_MENU: {

                if (event->is<Event::MouseButtonPressed>()) {
                    //std::wcout << L"[DEBUG] 현재 Scene: " << static_cast<int>(game.currentScene) << std::endl;
                    // 로그아웃 버튼 클릭 시
                    if (logoutButton.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        loggedIn = false;  // 로그인 상태 초기화
                        loggedOut = true;  // 로그아웃 플래그 설정
                    }
                    // 한글 타자 연습
                    if (korButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        std::wcout << L"한글 타자 연습 시작!" << endl;
                        //game.currentScene = Scene::FILE_SELECT;
                        fileOptions.clear();
                        selectMod = L"한글";
                        game.bHangle = true;
                        game.currentScene = Scene::PROFILE;
                    }


                    // 영어 모드
                    else if (engButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        std::wcout << L"영어 타자 연습 시작!" << endl;
                        selectMod = L"영어";
                        game.bHangle = false;
                        game.currentScene = Scene::FILE_SELECT;
                    }

                    // 코딩 모드
                    else if (codeButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        game.bHangle = false;
                        isCodingMode = true;
                        game.currentScene = Scene::FILE_SELECT;
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
                            gm.showLoadingScreen(gameWindow, font, isMatching);
                            wcout << L"로딩창 띄우기!!" << endl;
                            });

                        if (client.waitForMatch(gameWindow, font)) {
                            isMatching = true;
                            if (loadingThread.joinable()) loadingThread.join();
                            wcout << L"매치 완료되었슴!!" << endl;
                            wcout << L"게임 시작!!" << endl;

                            std::wstring filePath;
                            filePath = getRandomFile(folderPath, L".java");
                            if (filePath.empty()) {
                                std::wcerr << "[디버깅] Error: No file selected!" << std::endl;
                                std::wcout << filePath << std::endl;
                            }
                            else {
                                std::wcout << "[디버깅] Selected file: " << filePath << std::endl;
                                // 파일 경로가 올바른지 확인
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << "[디버깅] Absolute path: " << absPath << std::endl;

                                // 파일에서 문장 읽기
                                //std::vector<std::vector<std::wstring>> sentences = loadText(filePath);
                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);


                                // 문장 로딩 확인
                                if (game.sentences.empty()) {
                                    std::cout << "Error: No sentences loaded!" << std::endl;
                                }
                                else {
                                    std::cout << "[디버깅] 게임 시작: 문장 수 = " << game.sentences.size() << std::endl;
                                    gm.runGame2(gameWindow, font, 24, game.sentences);
                                }
                            }
                        }
                    }
                }

                break;
            }
            case Scene::FILE_SELECT: {
                if (isCodingMode) {
                    // 마우스 클릭 처리
                    if (event->is<Event::MouseButtonPressed>()) {
                        // Java 버튼
                        if (codeJavaButtonSprite.getGlobalBounds().contains(worldPos)) {
                            std::cout << "[디버깅] FILE_SELECT Scene, isCodingMode: " << (isCodingMode ? "true" : "false") << std::endl;
                            filePath = getRandomFile(folderPath, L".java");
                            if (!filePath.empty()) {
                                std::wcout << L"[디버깅] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << L"[디버깅] Absolute path: " << absPath << std::endl;

                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                                resetGameResult(game);
                                game.currentScene = Scene::TYPING_GAME;

                            }
                        }
                        // C++ 버튼
                        else if (codeCppButtonSprite.getGlobalBounds().contains(worldPos)) {
                            filePath = getRandomFile(folderPath, L".cpp");
                            if (!filePath.empty()) {
                                std::wcout << L"[디버깅] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << L"[디버깅] Absolute path: " << absPath << std::endl;

                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                                resetGameResult(game);
                                game.currentScene = Scene::TYPING_GAME;

                            }
                        }
                        // Python 버튼
                        else if (codePyButtonSprite.getGlobalBounds().contains(worldPos)) {
                            filePath = getRandomFile(folderPath, L".py");
                            if (!filePath.empty()) {
                                std::wcout << L"[디버깅] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << L"[디버깅] Absolute path: " << absPath << std::endl;

                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                                resetGameResult(game);
                                game.currentScene = Scene::TYPING_GAME;
                            }
                        }
                        // 파일 선택 버튼
                        else if (fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = false;
                            game.currentScene = Scene::FILE_SELECT;
                            //handleFileClick(game, *event, worldPos, fileOptions, font);
                        }
                        // 메인 화면으로 돌아가기
                        else if (!fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = false; // 코딩 모드 해제
                            game.currentScene = Scene::MAIN_MENU;
                        }

                    }
                }
                else {
                    handleFileClick(game, *event, worldPos, fileOptions, font);
                }
                break;
            }
            case Scene::TYPING_GAME:
            {
                // Scene별 입력 처리
                handleInputGame(game, *event);
                break;
            }
            }

        }

        // HOVER 효과
        if (game.currentScene == Scene::FILE_SELECT) {
            hoverText(game, fileOptions, worldPos);
        }

        gameWindow.clear(Color::White);


        // 렌더링
        switch (game.currentScene)
        {
        case Scene::PROFILE: {
            renderProfile(gameWindow, game, font, fontSize, userImage, profileTexts);

            if (game.showImageOverlay) {
                renderSelectImage(gameWindow, game, font, imageOptions, sprites, userImage, thumbnailHoverOutline);  // 프로필 이미지 선택 화면 겹쳐서 그림
            }
            break;
        }

        case Scene::LOGIN: {
            cursorTimer += deltaTime;
            if (cursorTimer >= cursorBlinkRate) {
                cursorVisible = !cursorVisible;
                cursorTimer = 0.f;
            }

            if (cursorVisible && isIdActive) {
                cursor.setPosition({ leftX + 10 + idText.getLocalBounds().size.x , 210 + shiftY });
            }
            else if (cursorVisible && isPassActive) {
                cursor.setPosition({ leftX + 10 + passText.getLocalBounds().size.x, 290 + shiftY });
            }

            gameWindow.clear(Color::Black);
            gameWindow.draw(background);
            gameWindow.draw(idLabel);
            gameWindow.draw(passLabel);
            gameWindow.draw(idBox);
            gameWindow.draw(passBox);
            gameWindow.draw(idText);
            gameWindow.draw(passText);
            gameWindow.draw(loginButton);
            gameWindow.draw(loginText);
            gameWindow.draw(signupButton);
            gameWindow.draw(signupText);
            gameWindow.draw(status);
            gameWindow.draw(checkbox);
            gameWindow.draw(checkboxLabel);
            gameWindow.draw(checkMark);
            if (cursorVisible) gameWindow.draw(cursor);
            gameWindow.display();
            break;
        }
        case Scene::SIGNUP: {
            // 커서 깜빡이기
            cursorTimer += deltaTime;
            if (cursorTimer >= cursorBlinkRate) {
                cursorVisible = !cursorVisible;
                cursorTimer = 0.f;
            }

            if (cursorVisible && isNicknameActive) {
                cursor.setPosition({ 110 + nicknameText.getLocalBounds().size.x, 125 });  // 커서 X 위치는 nicknameText의 길이에 따라 결정
            }
            else if (cursorVisible && isIdActive) {
                cursor.setPosition({ 110 + idText.getLocalBounds().size.x, 185 });  // 커서 X 위치는 idText의 길이에 따라 결정
            }
            else if (cursorVisible && isPassActive) {
                cursor.setPosition({ 110 + passText.getLocalBounds().size.x, 245 });  // 커서 X 위치는 passText의 길이에 따라 결정
            }

            signupWindow.clear(Color::Black);
            signupWindow.draw(title);
            signupWindow.draw(nicknameLabel);
            signupWindow.draw(idLabel);
            signupWindow.draw(passLabel);
            signupWindow.draw(nicknameBox);
            signupWindow.draw(idBox);
            signupWindow.draw(passBox);
            signupWindow.draw(nicknameText);
            signupWindow.draw(idText);
            signupWindow.draw(passText);
            signupWindow.draw(signupButton);
            signupWindow.draw(signupText);
            signupWindow.draw(status);
            signupWindow.draw(checkbox);
            signupWindow.draw(checkboxLabel);
            signupWindow.draw(checkMark);
            if (cursorVisible) {
                signupWindow.draw(cursor);  // 커서 표시
            }
            signupWindow.display();
            break;
        }
        case Scene::MAIN_MENU: {
            // 기본 게임 메인 화면 구성요소 그리기
            gameWindow.draw(backGroundSprite); // 배경 사진 
            gameWindow.draw(whiteBackground);  // 하얀색 배경 
            gameWindow.draw(profileSprite);
            gameWindow.draw(profileText);    // 왼쪽 상단에 profileText 그리기

            gameWindow.draw(korButtonSprite);  // 한글 버튼 이미지 그리기
            gameWindow.draw(korButtonText);    // 한글 Text 이미지 그리기

            gameWindow.draw(engButtonSprite);  // 영어 버튼 이미지 그리기
            gameWindow.draw(engButtonText);    // 영어 Text 이미지 그리기

            gameWindow.draw(codeButtonSprite); // 코딩모드 버튼 이미지 그리기
            gameWindow.draw(codeButtonText);    // 코딩모드 Text 이미지 그리기

            gameWindow.draw(matchButtonSprite);  // 1:1 Match 버튼 이미지 그리기
            gameWindow.draw(matchButtonText);    // match Text 그리기

            gameWindow.draw(rankingIcon);        // 랭킹 아이콘 그리기
            gameWindow.draw(logoutButton);       // 로그아웃 버튼 그리기

            break;
        }
        case Scene::FILE_SELECT: {

            if (isCodingMode) {
                gameWindow.draw(backGroundSprite); // 배경 표시
                gameWindow.draw(whiteBackground); // 흰색 배경
                gameWindow.draw(codeJavaButtonSprite); // Java 버튼
                //gameWindow.draw(javaButtonText);
                gameWindow.draw(codeCppButtonSprite); // C++ 버튼
                //gameWindow.draw(cppButtonText);
                gameWindow.draw(codePyButtonSprite); // Python 버튼
                //gameWindow.draw(pyButtonText);
                gameWindow.draw(fileSelectSprite); // 파일 선택 버튼
                gameWindow.draw(fileSelectText);

            }
            else {
                renderFileList(gameWindow, game, font, fontSize, typingFilePath, fileOptions, selectMod);
            }
            break;
        }
        case Scene::TYPING_GAME: {
            if (game.currentScene == Scene::TYPING_GAME && !game.started) {
                game.startTime = std::chrono::high_resolution_clock::now();     // 게임 시작 시간 저장
                game.started = true;                    // 한 번만 초기화 하도록
            }
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float>(now - game.startTime).count();
            game.elapsedSeconds = elapsed;

            if (game.readyToShowResult)
                //game.progress = 100.f;
                game.currentScene = Scene::RESULT;
   
            renderGame(gameWindow, game, font, fontSize);         // UI 렌더링
            updateTypingStats(game, elapsed);           // 실시간 통계 업데이트
            break;
        }
        case Scene::RESULT: {
            std::wcout << L"[DEBUG] 렌더링 RESULT 로 변경합니다." << std::endl;

            bool bRstart = showResultWindow(game, font, fontSize, game.sentences);
            if (bRstart) {
                resetGameResult(game);  // 값 초기화
                game.currentScene = Scene::TYPING_GAME;
            }
            else {
                game.currentScene = Scene::FILE_SELECT;
            }

            break;
        }
        }

        // 로그인 상태가 아닐 때 로그인 UI 표시
        if (!loggedIn) {
            // 로그인 UI를 표시
            if (showLoginWindow(gameWindow, db, userID)) {
                std::cout << "로그인 성공! 유저 ID: " << userID << std::endl;
                loggedIn = true;
                game.currentScene = Scene::MAIN_MENU;

            }

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


    } // while

    return 0;
} // main