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
// �̸� ���� ����
#include "UIManager.hpp"
//---- �߰�
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
#include <codecvt>  // wide �� utf8 ��ȯ (����)

using namespace std;
using namespace sf;

bool showStartScreen(sf::RenderWindow& window);  // �Լ� ���� ����
bool showRankingUI = false;
bool loggedOut = false;  // �α׾ƿ� ���θ� Ȯ���ϴ� ����


int main() {
    float MAIN_WINDOW_WIDTH = 1280.f;
    float MAIN_WINDOW_HEIGHT = 720.f;

    sf::RenderWindow gameWindow(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"Ÿ�� ���� ����");
    // ȸ������â
    RenderWindow signupWindow(VideoMode({ 500, 400 }), L"ȸ������");


    std::wcout.imbue(std::locale(""));         // �����ڵ� ������ ����

    // �α��� ���� ��ü
    MySQLConnector db;
    std::string userID;
    bool loggedIn = false;

    // ��ŷ UI ��ü
    RankingUI rankingUI;

    // ���� ���� ��ü��
    NetworkManager client;
    GameManager gm;
    //FileLoader loader;
    //TextUtil util;
    //TypingGame typing;

    // UI ���� ��ü
    UIManager ui;

    // --------------------------- �߰�
    GameState game;


    // �׽�Ʈ �� �ӽ� ���� ����
    //game.user.id = L"test_user_001";
    game.user.nickname = L"Ÿ�ڸ�����";
    game.user.profileImagePath = L"assets/profile_img/default_avatar.png";

    int fontSize = game.user.fontSize;

    game.selectPath = L"assets/hangleFile/�ֱ���.txt";


    // ���� ��� - �ϴ� ���Ƿ� �ʱ�ȭ
    game.user.point = 1234;
    game.user.bestWPM = 87.5f;
    game.user.bestAccuracy = 96.2f;
    game.user.totalPlayCount = 12;
    game.user.totalPlayTime = 983.7f;

    // ���� �÷��̾��� ���� ȭ�� ����
    //game.currentScene = Scene::MAIN_MENU; // �ʱⰪ �����Ƿ� �ʿ� x ���߿� ��������� ���ܵ���

    game.currentScene = Scene::MAIN_MENU; // �ʱⰪ �����Ƿ� �ʿ� x ���߿� ��������� ���ܵ���
    //TYPING_GAME

    // �÷��� Ÿ�� ��Ͽ�(?)
    sf::Clock clock;


    // ���� ��Ͽ� �׽�Ʈ ����
    std::vector<FileOption> fileOptions;
    std::wstring selectMod = L"�ѱ�";
    std::vector<std::wstring> typingFilePath;

    typingFilePath = {
            L"./assets/typing/�ѱ�1.txt",
            L"assets/typing/�ѱ�2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/�ѱ�1.txt",
            L"assets/typing/�ѱ�2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/coding.cpp",
            L"assets/typing/�ѱ�1.txt",

    };

    // --------------------------------------

    // ��ü ���� ���
    std::wstring folderPath = L"./assets/file";
    // �ڵ� ��� ���� ���
    std::wstring filePath;

    if (!ui.loadFont("./assets/font/D2Coding.ttf")) {
        std::cerr << "��Ʈ �ε� ����!" << std::endl;
        return -1;
    }
    Font font = ui.getFont();

    //------------
        // ��� �̹��� �ε�
    Texture bgTexture;
    if (!bgTexture.loadFromFile("./assets/image/login.png")) {
        cerr << "�α��� ��� �̹��� �ε� ����!" << endl;
        return false;
    }
    Sprite background(bgTexture);
    background.setScale(Vector2f(
        MAIN_WINDOW_WIDTH / static_cast<float>(bgTexture.getSize().x),
        MAIN_WINDOW_HEIGHT / static_cast<float>(bgTexture.getSize().y)
    ));
    //----------------
    // ����: �α׾ƿ� ��ư �̹���
    sf::Texture logoutButtonTexture;
    if (!logoutButtonTexture.loadFromFile("assets/image/logout.png")) {
        std::cerr << "�α׾ƿ� ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }
    sf::Sprite logoutButton(logoutButtonTexture);
    // ������ ���� ����
    logoutButton.setScale({ 0.1f, 0.1f });  // �ʿ�� ũ�� ����

    // ������ ���� �� ��Ȯ�� ũ�� ��������
    FloatRect buttonBounds = logoutButton.getGlobalBounds();

    // ��ġ: ȭ�� �ϴ� �߾ӿ�, �Ʒ����� �� ����
    logoutButton.setPosition({
        MAIN_WINDOW_WIDTH / 2.f - buttonBounds.size.x / 2.f,
        MAIN_WINDOW_HEIGHT - buttonBounds.size.y
        });

    sf::Text logoutButtonText = ui.createText(L"�α׾ƿ�", 30, { logoutButton.getPosition().x + 40, logoutButton.getPosition().y + 15 }, sf::Color::Black);

    if (!ui.loadTexture("ranking", "./assets/image/ranking_icon2.png")) return -1;

    // ��ŷ ������
    sf::Sprite rankingIcon = ui.createIcon("./assets/image/ranking_icon2.png",
        Vector2f(MAIN_WINDOW_WIDTH - 120, 10),
        Vector2f(0.2f, 0.2f));

    // ���� ȭ�� ���� �ؽ�ó �� ��������Ʈ
    Texture profileTexture, backGroundTexture, korButtonTexture, engButtonTexture, codeButtonTexture, matchButtonTexture,
        codeJavaButtonTexture, codeCppButtonTexture, codePyButtonTexture, fileSelectTexture;

    // ��� �̹��� �ε�
    if (!ui.loadTexture("background", "./assets/image/backGround.png")) {

        return -1;
    }

    ui.loadTexture("background", "./assets/image/backGround.png");
    sf::Sprite backGroundSprite = ui.createSprite("background", { 0, 0 }, { MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT }); \
        //backGroundSprite.setPosition(Vector2f(0.f, 0.f));

        // ������ ��� ����
        sf::RectangleShape whiteBackground(sf::Vector2f(800.f, 400.f));
    whiteBackground.setPosition({ 200.f, 150.f });
    whiteBackground.setFillColor(sf::Color(255, 255, 255, 200)); // ������ - ����
    whiteBackground.setOutlineThickness(1.f);
    whiteBackground.setOutlineColor(sf::Color(100, 100, 100)); // �׵θ� ����

    // ������ �̹��� �ε�
    if (!ui.loadTexture("profile", "./assets/image/user.png")) return -1;

    sf::Sprite profileSprite = ui.createSprite("profile", { 20.f, 20.f }, { 50.f, 50.f });
    sf::Text profileText = ui.createText(L"�� ������", 20, { 100.f, 20.f }, Color::Black);


    // ���� �����ϸ��� ���� ��ư ũ��
    sf::Vector2f buttonSize(50.f, 50.f);

    // �ѱ� �̹��� 

    if (!ui.loadTexture("korButton", "./assets/image/button.png")) return -1;

    // ��ư ��������Ʈ ����
    sf::Sprite korButtonSprite = ui.createSprite("korButton", { 320.f, 190.f }, buttonSize);
    sf::Text korButtonText = ui.createText(L"�ѱ� Ÿ�� ����", 30, { 400.f, 200.f }, sf::Color::Black);

    if (!ui.loadTexture("engButton", "./assets/image/button.png")) return -1;
    // ��ư �ؽ�Ʈ ���� (�ѱ��� L"..." ���)
    sf::Sprite engButtonSprite = ui.createSprite("engButton", { 320.f, 290.f }, buttonSize);
    sf::Text engButtonText = ui.createText(L"���� Ÿ�� ����", 30, { 400.f, 290.f }, sf::Color::Black);

    // �̹��� �ε�
    if (!ui.loadTexture("codeButton", "./assets/image/button.png")) return -1;
    if (!ui.loadTexture("matchButton", "./assets/image/button.png")) return -1;

    // �ڵ� ��� ��ư
    sf::Sprite codeButtonSprite = ui.createSprite("codeButton", { 320.f, 390.f }, buttonSize);
    sf::Text codeButtonText = ui.createText(L"�ڵ� ��� ����", 30, { 400.f, 400.f }, sf::Color::Black);

    // ��Ʋ ��� ��ư
    sf::Sprite matchButtonSprite = ui.createSprite("matchButton", { 320.f, 490.f }, buttonSize);
    sf::Text matchButtonText = ui.createText(L"�ڵ� ���", 30, { 400.f, 500.f }, sf::Color::Black);


    // ---------- �α��� â
    float leftX = 330;
    float shiftY = 100;

    RectangleShape loginButton(Vector2f(250, 60));
    loginButton.setPosition({ leftX, 370 + shiftY });
    loginButton.setFillColor(Color::Green);

    Text loginText(font, "Login", 28);
    loginText.setPosition({ leftX + 85, 385 + shiftY });
    loginText.setFillColor(Color::White);

    //  ----------------------

    // ---------- ȸ������ â
    string nickname, id, password;
    bool isNicknameActive = true, isIdActive = false, isPassActive = false, showPassword = false;

    Text title(font, L"ȸ������", 30);
    title.setPosition({ 200, 50 });
    title.setFillColor(Color::White);

    Text status(font, "", 20);
    status.setPosition({ 50, 85 });
    status.setFillColor(Color::Red);

    RectangleShape inputBox(Vector2f(350, 40)); // �Է� �ڽ� ũ�� ����
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

    // Ŀ�� ���� ����
    RectangleShape cursor(Vector2f(2, 30));  // Ŀ�� ũ��
    cursor.setFillColor(Color::Black);
    float cursorTimer = 0.f;  // Ŀ�� ������ �ð�
    float cursorBlinkRate = 10.f;  // Ŀ�� ������ �ֱ� (��)
    bool cursorVisible = true;  // Ŀ�� ���ü� ����

    // ȸ������ ��ư
    RectangleShape signupButton(Vector2f(200, 50));
    signupButton.setPosition({ 150, 330 });
    signupButton.setFillColor(Color::Blue);

    Text signupText(font, "Sign Up", 24);
    signupText.setPosition({ 210, 340 });
    signupText.setFillColor(Color::White);

    // ��й�ȣ ǥ�� üũ�ڽ� �߰�
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
    // �ؽ�ó �ε�
    if (!ui.loadTexture("codeJavaButton", "./assets/image/javaButton.png")) return -1;
    if (!ui.loadTexture("codeCppButton", "./assets/image/cppButton.png")) return -1;
    if (!ui.loadTexture("codePyButton", "./assets/image/pyButton.png")) return -1;
    if (!ui.loadTexture("fileSelectButton", "./assets/image/bluebutton4.png")) return -1;

    // ��ư ���� ������
    sf::Vector2f buttonScale(180.f, 120.f);
    sf::Vector2f fileButtonScale(100.f, 60.f);

    // Java ��ư
    sf::Sprite codeJavaButtonSprite = ui.createSprite("codeJavaButton", { MAIN_WINDOW_WIDTH / 3 - 150 , MAIN_WINDOW_HEIGHT / 2 - 70 }, buttonScale);
    //sf::Text javaButtonText = ui.createText(L"Java", 35, { 150.f, 300.f }, sf::Color::Black);
    //ui.centerTextOnSprite(codeJavaButtonSprite, javaButtonText);

    // C++ ��ư
    sf::Sprite codeCppButtonSprite = ui.createSprite("codeCppButton", { MAIN_WINDOW_WIDTH / 3 + 100 , MAIN_WINDOW_HEIGHT / 2 - 70 }, buttonScale);
 /*   sf::Text cppButtonText = ui.createText(L"C++", 35, { 450.f, 300.f }, sf::Color::Black);
    ui.centerTextOnSprite(codeCppButtonSprite, cppButtonText);*/

    // Python ��ư
    sf::Sprite codePyButtonSprite = ui.createSprite("codePyButton", { MAIN_WINDOW_WIDTH / 3 + 350 , MAIN_WINDOW_HEIGHT / 2 - 70 }, buttonScale);
    //sf::Text pyButtonText = ui.createText(L"Python", 35, { 750.f, 300.f }, sf::Color::Black);
    //ui.centerTextOnSprite(codePyButtonSprite, pyButtonText);

    // ���� ���� ��ư
    sf::Sprite fileSelectSprite = ui.createSprite("fileSelectButton", { MAIN_WINDOW_WIDTH - 150, MAIN_WINDOW_HEIGHT - 120 }, fileButtonScale);
    sf::Text fileSelectText = ui.createText(L"���� ����", 20, { 1200.f, 600.f }, sf::Color::Black);
    ui.centerTextOnSprite(fileSelectSprite, fileSelectText);

    // �ڵ� ��� ���� Ȯ�� ����
    bool isCodingMode = false;

    // �α��� â ���� ���� �߰�
    if (!showStartScreen(gameWindow)) {
        return 0;  // ����ڰ� â�� ���� ��� ����
    }

    // �߰� �� �͵�
    std::vector<sf::Sprite> sprites;
    std::vector<ImageOption> imageOptions;
    std::vector<sf::Text> profileTexts;

    // ������ ���� hover ȿ��
    sf::RectangleShape thumbnailHoverOutline;
    thumbnailHoverOutline.setSize({ 200, 200 }); // ����� ũ�⿡ �°�
    thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
    thumbnailHoverOutline.setOutlineColor(sf::Color::Red);
    thumbnailHoverOutline.setOutlineThickness(4.f);

    // �⺻ ������ �̹��� �ε�
    sf::Image img = loadImg(game.user.profileImagePath);
    game.user.profileTexture = sf::Texture(resizeImageKeepAspect(img));
    sf::Sprite userImage(game.user.profileTexture);

    // ���� ���� ����
    while (gameWindow.isOpen()) {
        float deltaTime = 1.f / 60.f;
        std::wcout << L"[DEBUG] ���� Scene: " << static_cast<int>(game.currentScene) << std::endl;
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

                            // �ҷ����� ��ư Ŭ�� Ȯ��
                            if (game.btn.loadImgBtnBounds.contains(worldPos)) {
                                std::wcout << L"[�ҷ����� ��ư Ŭ����]" << std::endl;
                                auto originalPath = std::filesystem::current_path();	// ���� ���� ��� ����
                                std::wstring selectedImagePath = openImageFileDialog();

                                std::filesystem::current_path(originalPath);  // �ٽ� ���� ��η� �ǵ��� - ���õ� ���� ��η� �ٲ�� �� ����
                                sf::Image image;
                                if (image.loadFromFile(selectedImagePath)) {
                                    // ���� ó��
                                    std::wcout << L"[�̹����� ���������� �ҷ��Խ��ϴ�]" << std::endl;
                                    std::wcout << selectedImagePath << std::endl;
                                    game.user.profileImagePath = selectedImagePath;
                                    updateProfileImage(selectedImagePath, game, userImage);
                                    game.showImageOverlay = false;
                                    game.currentScene = Scene::PROFILE;


                                }
                                else {
                                    std::wcout << L"[�̹����� �ҷ����� ���߽��ϴ�]" << std::endl;
                                    return -1;
                                }

                            }   // HOVER �̺�Ʈ

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
                std::wcout << L"[DEBUG] ���� Scene: " << static_cast<int>(game.currentScene) << std::endl;
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

                    // üũ�ڽ� Ŭ��
                    if (checkbox.getGlobalBounds().contains(mousePos)) {
                        showPassword = !showPassword;  // ��й�ȣ ǥ�� ���� ���
                        passText.setString(showPassword ? password : string(password.size(), '*'));

                        // üũ�ڽ� ���ο� V ǥ�� �߰�/����
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
                        // �ʼ� �Է� üũ
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
                    //std::wcout << L"[DEBUG] ���� Scene: " << static_cast<int>(game.currentScene) << std::endl;
                    // �α׾ƿ� ��ư Ŭ�� ��
                    if (logoutButton.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        loggedIn = false;  // �α��� ���� �ʱ�ȭ
                        loggedOut = true;  // �α׾ƿ� �÷��� ����
                    }
                    // �ѱ� Ÿ�� ����
                    if (korButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        std::wcout << L"�ѱ� Ÿ�� ���� ����!" << endl;
                        //game.currentScene = Scene::FILE_SELECT;
                        fileOptions.clear();
                        selectMod = L"�ѱ�";
                        game.bHangle = true;
                        game.currentScene = Scene::PROFILE;
                    }


                    // ���� ���
                    else if (engButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        std::wcout << L"���� Ÿ�� ���� ����!" << endl;
                        selectMod = L"����";
                        game.bHangle = false;
                        game.currentScene = Scene::FILE_SELECT;
                    }

                    // �ڵ� ���
                    else if (codeButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        game.bHangle = false;
                        isCodingMode = true;
                        game.currentScene = Scene::FILE_SELECT;
                    }

                    // ��ġ ���
                    else if (matchButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        wcout << L"1:1 Match Button Clicked!" << endl;

                        if (!client.connectToServer()) {
                            return -1;
                        }

                        // �ε�â ���� ���� ������
                        bool isMatching = false;
                        std::thread loadingThread([&]() {
                            gm.showLoadingScreen(gameWindow, font, isMatching);
                            wcout << L"�ε�â ����!!" << endl;
                            });

                        if (client.waitForMatch(gameWindow, font)) {
                            isMatching = true;
                            if (loadingThread.joinable()) loadingThread.join();
                            wcout << L"��ġ �Ϸ�Ǿ���!!" << endl;
                            wcout << L"���� ����!!" << endl;

                            std::wstring filePath;
                            filePath = getRandomFile(folderPath, L".java");
                            if (filePath.empty()) {
                                std::wcerr << "[�����] Error: No file selected!" << std::endl;
                                std::wcout << filePath << std::endl;
                            }
                            else {
                                std::wcout << "[�����] Selected file: " << filePath << std::endl;
                                // ���� ��ΰ� �ùٸ��� Ȯ��
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << "[�����] Absolute path: " << absPath << std::endl;

                                // ���Ͽ��� ���� �б�
                                //std::vector<std::vector<std::wstring>> sentences = loadText(filePath);
                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);


                                // ���� �ε� Ȯ��
                                if (game.sentences.empty()) {
                                    std::cout << "Error: No sentences loaded!" << std::endl;
                                }
                                else {
                                    std::cout << "[�����] ���� ����: ���� �� = " << game.sentences.size() << std::endl;
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
                    // ���콺 Ŭ�� ó��
                    if (event->is<Event::MouseButtonPressed>()) {
                        // Java ��ư
                        if (codeJavaButtonSprite.getGlobalBounds().contains(worldPos)) {
                            std::cout << "[�����] FILE_SELECT Scene, isCodingMode: " << (isCodingMode ? "true" : "false") << std::endl;
                            filePath = getRandomFile(folderPath, L".java");
                            if (!filePath.empty()) {
                                std::wcout << L"[�����] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << L"[�����] Absolute path: " << absPath << std::endl;

                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                                resetGameResult(game);
                                game.currentScene = Scene::TYPING_GAME;

                            }
                        }
                        // C++ ��ư
                        else if (codeCppButtonSprite.getGlobalBounds().contains(worldPos)) {
                            filePath = getRandomFile(folderPath, L".cpp");
                            if (!filePath.empty()) {
                                std::wcout << L"[�����] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << L"[�����] Absolute path: " << absPath << std::endl;

                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                                resetGameResult(game);
                                game.currentScene = Scene::TYPING_GAME;

                            }
                        }
                        // Python ��ư
                        else if (codePyButtonSprite.getGlobalBounds().contains(worldPos)) {
                            filePath = getRandomFile(folderPath, L".py");
                            if (!filePath.empty()) {
                                std::wcout << L"[�����] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::wcout << L"[�����] Absolute path: " << absPath << std::endl;

                                game.selectPath = loadText(filePath);
                                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                                resetGameResult(game);
                                game.currentScene = Scene::TYPING_GAME;
                            }
                        }
                        // ���� ���� ��ư
                        else if (fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = false;
                            game.currentScene = Scene::FILE_SELECT;
                            //handleFileClick(game, *event, worldPos, fileOptions, font);
                        }
                        // ���� ȭ������ ���ư���
                        else if (!fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = false; // �ڵ� ��� ����
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
                // Scene�� �Է� ó��
                handleInputGame(game, *event);
                break;
            }
            }

        }

        // HOVER ȿ��
        if (game.currentScene == Scene::FILE_SELECT) {
            hoverText(game, fileOptions, worldPos);
        }

        gameWindow.clear(Color::White);


        // ������
        switch (game.currentScene)
        {
        case Scene::PROFILE: {
            renderProfile(gameWindow, game, font, fontSize, userImage, profileTexts);

            if (game.showImageOverlay) {
                renderSelectImage(gameWindow, game, font, imageOptions, sprites, userImage, thumbnailHoverOutline);  // ������ �̹��� ���� ȭ�� ���ļ� �׸�
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
            // Ŀ�� �����̱�
            cursorTimer += deltaTime;
            if (cursorTimer >= cursorBlinkRate) {
                cursorVisible = !cursorVisible;
                cursorTimer = 0.f;
            }

            if (cursorVisible && isNicknameActive) {
                cursor.setPosition({ 110 + nicknameText.getLocalBounds().size.x, 125 });  // Ŀ�� X ��ġ�� nicknameText�� ���̿� ���� ����
            }
            else if (cursorVisible && isIdActive) {
                cursor.setPosition({ 110 + idText.getLocalBounds().size.x, 185 });  // Ŀ�� X ��ġ�� idText�� ���̿� ���� ����
            }
            else if (cursorVisible && isPassActive) {
                cursor.setPosition({ 110 + passText.getLocalBounds().size.x, 245 });  // Ŀ�� X ��ġ�� passText�� ���̿� ���� ����
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
                signupWindow.draw(cursor);  // Ŀ�� ǥ��
            }
            signupWindow.display();
            break;
        }
        case Scene::MAIN_MENU: {
            // �⺻ ���� ���� ȭ�� ������� �׸���
            gameWindow.draw(backGroundSprite); // ��� ���� 
            gameWindow.draw(whiteBackground);  // �Ͼ�� ��� 
            gameWindow.draw(profileSprite);
            gameWindow.draw(profileText);    // ���� ��ܿ� profileText �׸���

            gameWindow.draw(korButtonSprite);  // �ѱ� ��ư �̹��� �׸���
            gameWindow.draw(korButtonText);    // �ѱ� Text �̹��� �׸���

            gameWindow.draw(engButtonSprite);  // ���� ��ư �̹��� �׸���
            gameWindow.draw(engButtonText);    // ���� Text �̹��� �׸���

            gameWindow.draw(codeButtonSprite); // �ڵ���� ��ư �̹��� �׸���
            gameWindow.draw(codeButtonText);    // �ڵ���� Text �̹��� �׸���

            gameWindow.draw(matchButtonSprite);  // 1:1 Match ��ư �̹��� �׸���
            gameWindow.draw(matchButtonText);    // match Text �׸���

            gameWindow.draw(rankingIcon);        // ��ŷ ������ �׸���
            gameWindow.draw(logoutButton);       // �α׾ƿ� ��ư �׸���

            break;
        }
        case Scene::FILE_SELECT: {

            if (isCodingMode) {
                gameWindow.draw(backGroundSprite); // ��� ǥ��
                gameWindow.draw(whiteBackground); // ��� ���
                gameWindow.draw(codeJavaButtonSprite); // Java ��ư
                //gameWindow.draw(javaButtonText);
                gameWindow.draw(codeCppButtonSprite); // C++ ��ư
                //gameWindow.draw(cppButtonText);
                gameWindow.draw(codePyButtonSprite); // Python ��ư
                //gameWindow.draw(pyButtonText);
                gameWindow.draw(fileSelectSprite); // ���� ���� ��ư
                gameWindow.draw(fileSelectText);

            }
            else {
                renderFileList(gameWindow, game, font, fontSize, typingFilePath, fileOptions, selectMod);
            }
            break;
        }
        case Scene::TYPING_GAME: {
            if (game.currentScene == Scene::TYPING_GAME && !game.started) {
                game.startTime = std::chrono::high_resolution_clock::now();     // ���� ���� �ð� ����
                game.started = true;                    // �� ���� �ʱ�ȭ �ϵ���
            }
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float>(now - game.startTime).count();
            game.elapsedSeconds = elapsed;

            if (game.readyToShowResult)
                //game.progress = 100.f;
                game.currentScene = Scene::RESULT;
   
            renderGame(gameWindow, game, font, fontSize);         // UI ������
            updateTypingStats(game, elapsed);           // �ǽð� ��� ������Ʈ
            break;
        }
        case Scene::RESULT: {
            std::wcout << L"[DEBUG] ������ RESULT �� �����մϴ�." << std::endl;

            bool bRstart = showResultWindow(game, font, fontSize, game.sentences);
            if (bRstart) {
                resetGameResult(game);  // �� �ʱ�ȭ
                game.currentScene = Scene::TYPING_GAME;
            }
            else {
                game.currentScene = Scene::FILE_SELECT;
            }

            break;
        }
        }

        // �α��� ���°� �ƴ� �� �α��� UI ǥ��
        if (!loggedIn) {
            // �α��� UI�� ǥ��
            if (showLoginWindow(gameWindow, db, userID)) {
                std::cout << "�α��� ����! ���� ID: " << userID << std::endl;
                loggedIn = true;
                game.currentScene = Scene::MAIN_MENU;

            }

        }

        gameWindow.display();

        // ��ŷ UI�� ���� �����忡�� ����
        if (showRankingUI) {
            std::thread rankingThread([&]() {
                rankingUI.show();  // ��ŷ UI�� ������ �����忡�� ����
                });
            rankingThread.detach();  // �����尡 ����� ������ ��ٸ��� �ʵ��� �и�
            showRankingUI = false;
        }

        // �α׾ƿ� �÷��װ� true�̸� �α��� ȭ������ ���ư�
        if (loggedOut) {
            gameWindow.clear();
            loggedOut = false;  // �α׾ƿ� ���� �ʱ�ȭ
        }


    } // while

    return 0;
} // main