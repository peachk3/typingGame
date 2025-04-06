// loginUI.cpp
#include "userManager.hpp"
#include "RankingUI.hpp"  
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace sf;
using namespace chrono;

void showSignupWindow(RenderWindow& window, MySQLConnector& db) {
    string nickname, id, password;
    bool isNicknameActive = true, isIdActive = false, isPassActive = false, showPassword = false;
    Font font;
    font.openFromFile("assets/font/D2Coding.ttf");

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

    RenderWindow signupWindow(VideoMode({ 500, 400 }), L"회원가입");

    while (signupWindow.isOpen()) {
        float deltaTime = 1.f / 60.f;  // FPS에 맞춘 시간 간격 (60 FPS 가정)

        while (const optional<Event> event = signupWindow.pollEvent()) {
            if (event->is<Event::Closed>())
                signupWindow.close();

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
        }

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
    }
}

bool showLoginWindow(RenderWindow& window, MySQLConnector& db, std::string& userID) {
    string id, password;
    bool isIdActive = false, isPassActive = false, showPassword = false;

    Font font;
    font.openFromFile("assets/font/D2Coding.ttf");

    // 창 크기 설정
    Vector2f windowSize(1280, 720);
    window.create(VideoMode({ static_cast<unsigned int>(windowSize.x),
        static_cast<unsigned int>(windowSize.y) }),
        L"타자연습 게임");

    // 배경 이미지 로딩
    Texture bgTexture;
    if (!bgTexture.loadFromFile("assets/image/login.png")) {
        cerr << "로그인 배경 이미지 로딩 실패!" << endl;
        return false;
    }
    Sprite background(bgTexture);
    background.setScale(Vector2f(
        windowSize.x / static_cast<float>(bgTexture.getSize().x),
        windowSize.y / static_cast<float>(bgTexture.getSize().y)
    ));

    // 왼쪽 정렬 기준 X값으로 이동
    float leftX = 330;
    float shiftY = 100;

    Text status(font, "", 24);
    status.setPosition({ leftX, 160 + shiftY });
    status.setFillColor(Color::Red);

    RectangleShape inputBox(Vector2f(400, 50));
    inputBox.setFillColor(Color::White);
    inputBox.setOutlineColor(Color::Black);
    inputBox.setOutlineThickness(2);

    RectangleShape idBox = inputBox, passBox = inputBox;
    idBox.setPosition({ leftX, 200 + shiftY });
    passBox.setPosition({ leftX, 280 + shiftY });

    Text idLabel(font, "ID:", 24), passLabel(font, "PW:", 24);
    idLabel.setPosition({ leftX - 50, 210 + shiftY });
    passLabel.setPosition({ leftX - 50, 290 + shiftY });
    idLabel.setFillColor(Color::White);
    passLabel.setFillColor(Color::White);

    Text idText(font, "", 24), passText(font, "", 24);
    idText.setPosition({ leftX + 10, 210 + shiftY });
    passText.setPosition({ leftX + 10, 290 + shiftY });
    idText.setFillColor(Color::Black);
    passText.setFillColor(Color::Black);

    RectangleShape loginButton(Vector2f(250, 60));
    loginButton.setPosition({ leftX, 370 + shiftY });
    loginButton.setFillColor(Color::Green);

    Text loginText(font, "Login", 28);
    loginText.setPosition({ leftX + 85, 385 + shiftY });
    loginText.setFillColor(Color::White);

    RectangleShape signupButton(Vector2f(250, 60));
    signupButton.setPosition({ leftX, 450 + shiftY });
    signupButton.setFillColor(Color::Blue);

    Text signupText(font, "Sign Up", 28);
    signupText.setPosition({ leftX + 75, 465 + shiftY });
    signupText.setFillColor(Color::White);

    RectangleShape checkbox(Vector2f(25, 25));
    checkbox.setPosition({ leftX, 335 + shiftY });
    checkbox.setFillColor(Color::White);

    Text checkboxLabel(font, "Show Password", 24);
    checkboxLabel.setPosition({ leftX + 40, 330 + shiftY });
    checkboxLabel.setFillColor(Color::White);

    Text checkMark(font, "V", 24);
    checkMark.setPosition({ leftX + 5, 332 + shiftY });
    checkMark.setFillColor(Color::Black);
    checkMark.setString("");

    RectangleShape cursor(Vector2f(2, 35));
    cursor.setFillColor(Color::Black);
    float cursorTimer = 0.f;
    float cursorBlinkRate = 10.f;
    bool cursorVisible = true;

    while (window.isOpen()) {
        float deltaTime = 1.f / 60.f;

        while (const optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
                return false;
            }

            if (event->is<Event::MouseButtonPressed>()) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
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
                        window.display();
                        this_thread::sleep_for(chrono::seconds(2));
                        return true;
                    }
                    else {
                        status.setFillColor(Color::Red);
                        status.setString("Invalid ID or Password!");
                    }
                }
                else if (signupButton.getGlobalBounds().contains(mousePos)) {
                    showSignupWindow(window, db);
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
        }

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

        window.clear(Color::Black);
        window.draw(background);
        window.draw(idLabel);
        window.draw(passLabel);
        window.draw(idBox);
        window.draw(passBox);
        window.draw(idText);
        window.draw(passText);
        window.draw(loginButton);
        window.draw(loginText);
        window.draw(signupButton);
        window.draw(signupText);
        window.draw(status);
        window.draw(checkbox);
        window.draw(checkboxLabel);
        window.draw(checkMark);
        if (cursorVisible) window.draw(cursor);
        window.display();
    }

    return false;
}