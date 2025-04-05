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
    font.openFromFile("D2Coding.ttf");

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

    RenderWindow signupWindow(VideoMode({ 500, 400 }), L"ȸ������");

    while (signupWindow.isOpen()) {
        float deltaTime = 1.f / 60.f;  // FPS�� ���� �ð� ���� (60 FPS ����)

        while (const optional<Event> event = signupWindow.pollEvent()) {
            if (event->is<Event::Closed>())
                signupWindow.close();

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
        }

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
    }
}


bool showLoginWindow(RenderWindow& window, MySQLConnector& db, std::string& userID) {
    string id, password;
    bool isIdActive = false, isPassActive = false, showPassword = false;

    Font font;
    font.openFromFile("D2Coding.ttf");

    // â ũ�� ����
    Vector2f windowSize(1280, 720);
    window.create(VideoMode({ static_cast<unsigned int>(windowSize.x),
        static_cast<unsigned int>(windowSize.y) }),
        L"Ÿ�ڿ��� ����");

    // �߾� ���� ���� (X �߾� ��ġ)
    float centerX = windowSize.x / 2;

    Text title(font, L"�α���", 40);
    title.setPosition({ centerX - 60, 100 });
    title.setFillColor(Color::White);

    Text status(font, "", 24);
    status.setPosition({ centerX - 130, 150 });
    status.setFillColor(Color::Red);

    RectangleShape inputBox(Vector2f(400, 50));
    inputBox.setFillColor(Color::White);
    inputBox.setOutlineColor(Color::Black);
    inputBox.setOutlineThickness(2);

    RectangleShape idBox = inputBox, passBox = inputBox;
    idBox.setPosition({ centerX - 200, 200 });
    passBox.setPosition({ centerX - 200, 280 });

    Text idLabel(font, "ID:", 24), passLabel(font, "PW:", 24);
    idLabel.setPosition({ centerX - 250, 210 });
    passLabel.setPosition({ centerX - 250, 290 });
    idLabel.setFillColor(Color::White);
    passLabel.setFillColor(Color::White);

    Text idText(font, "", 24), passText(font, "", 24);
    idText.setPosition({ centerX - 190, 210 });
    passText.setPosition({ centerX - 190, 290 });
    idText.setFillColor(Color::Black);
    passText.setFillColor(Color::Black);

    RectangleShape loginButton(Vector2f(250, 60));
    loginButton.setPosition({ centerX - 125, 380 });
    loginButton.setFillColor(Color::Green);

    Text loginText(font, "Login", 28);
    loginText.setPosition({ centerX - 40, 395 });
    loginText.setFillColor(Color::White);

    RectangleShape signupButton(Vector2f(250, 60));
    signupButton.setPosition({ centerX - 125, 460 });
    signupButton.setFillColor(Color::Blue);

    Text signupText(font, "Sign Up", 28);
    signupText.setPosition({ centerX - 50, 475 });
    signupText.setFillColor(Color::White);

    RectangleShape checkbox(Vector2f(25, 25));
    checkbox.setPosition({ centerX - 200, 340 });
    checkbox.setFillColor(Color::White);

    Text checkboxLabel(font, "Show Password", 24);
    checkboxLabel.setPosition({ centerX - 160, 340 });
    checkboxLabel.setFillColor(Color::White);

    Text checkMark(font, "V", 24);
    checkMark.setPosition({ centerX - 190, 337 });
    checkMark.setFillColor(Color::Black);
    checkMark.setString("");

    RectangleShape cursor(Vector2f(2, 35));
    cursor.setFillColor(Color::Black);
    float cursorTimer = 0.f;
    float cursorBlinkRate = 10.f;
    bool cursorVisible = true;

    //  "��ŷ ����" ��ư �߰� (�ϴ� ��ġ)
    RectangleShape rankingButton(Vector2f(250, 60));
    rankingButton.setPosition({ centerX - 125, 540 });
    rankingButton.setFillColor(Color::Yellow);

    Text rankingText(font, "View Ranking", 28);
    rankingText.setPosition({ centerX - 80, 555 });
    rankingText.setFillColor(Color::Black);

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

                        // �α��� ���� �� ���� ȭ�� ����
                        return true;  // �α��� ���� �� ���� ȭ�� �������� ��ȯ
                    }
                    else {
                        status.setFillColor(Color::Red);
                        status.setString("Invalid ID or Password!");
                    }
                }
                else if (signupButton.getGlobalBounds().contains(mousePos)) {
                    showSignupWindow(window, db);
                }
                else if (rankingButton.getGlobalBounds().contains(mousePos)) {
                    RankingUI rankingUI;
                    rankingUI.show();
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
                    if (isIdActive) {
                        isIdActive = false;
                        isPassActive = true;
                    }
                    else if (isPassActive) {
                        isPassActive = false;
                        isIdActive = true;
                    }
                }
            }
        }

        cursorTimer += deltaTime;
        if (cursorTimer >= cursorBlinkRate) {
            cursorVisible = !cursorVisible;
            cursorTimer = 0.f;
        }

        if (cursorVisible && isIdActive) {
            cursor.setPosition({ centerX - 190 + idText.getLocalBounds().size.x, 210 });
        }
        else if (cursorVisible && isPassActive) {
            cursor.setPosition({ centerX - 190 + passText.getLocalBounds().size.x, 290 });
        }

        window.clear(Color::Black);
        window.draw(title);
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
        window.draw(rankingButton);
        window.draw(rankingText);
        if (cursorVisible) window.draw(cursor);
        window.display();
    }

    return false;
}
