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
bool loggedOut = false;  // �α׾ƿ� ���θ� Ȯ���ϴ� ����

int main() {
    int MAIN_WINDOW_WIDTH = 1280;
    int MAIN_WINDOW_HEIGHT = 720;
    sf::RenderWindow gameWindow(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"Ÿ�� ���� ����");

    // �α��� ���� ��ü
    MySQLConnector db;
    std::string userID;
    bool loggedIn = false;

    // ��ŷ UI ��ü
    RankingUI rankingUI;

    // ���� ���� ��ü��
    NetworkManager client;
    GameManager game;
    FileLoader loader;
    TextUtil util;
    TypingGame typing;

    // ��ü ���� ���
    std::string folderPath = "./assets/file";
    // �ڵ� ��� ���� ���
    std::string filePath;

    // ��Ʈ �ε�
    sf::Font font;
    if (!font.openFromFile("D2Coding.ttf")) {
        std::cerr << "��Ʈ �ε� ����!" << std::endl;
        return -1;
    }

    // ��ŷ ������ �ε�
    sf::Texture rankingIconTexture;
    if (!rankingIconTexture.loadFromFile("./assets/image/ranking_icon2.png")) {
        std::cerr << "������ �̹��� �ε� ����!" << std::endl;
        return -1;
    }
    sf::Sprite rankingIcon(rankingIconTexture);
    rankingIcon.setPosition(Vector2f(MAIN_WINDOW_WIDTH - 120, 10));
    rankingIcon.setScale({ 0.2f, 0.2f });

    // �α׾ƿ� ��ư
    sf::RectangleShape logoutButton(sf::Vector2f(200, 60));
    logoutButton.setFillColor(sf::Color(200, 50, 50));
    logoutButton.setPosition(Vector2f(MAIN_WINDOW_WIDTH / 2 - 100, MAIN_WINDOW_HEIGHT - 70));

    sf::Text logoutButtonText(font, L"�α׾ƿ�", 30);
    logoutButtonText.setFillColor(sf::Color::Black);
    logoutButtonText.setPosition(Vector2f(logoutButton.getPosition().x + 40, logoutButton.getPosition().y + 15));

    // ���� ȭ�� ���� �ؽ�ó �� ��������Ʈ
    Texture profileTexture, backGroundTexture, korButtonTexture, engButtonTexture, codeButtonTexture, matchButtonTexture,
        codeJavaButtonTexture, codeCppButtonTexture, codePyButtonTexture, fileSelectTexture;

    // ��� �̹��� �ε�
    if (!backGroundTexture.loadFromFile("./assets/image/bgimg.png")) {
        std::cerr << "��� �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    // ��� sprite ����
    Sprite backGroundSprite(backGroundTexture);
    backGroundSprite.setTexture(backGroundTexture);
    backGroundSprite.setPosition(Vector2f(0.f, 0.f));

    // ������ ��� ����
    sf::RectangleShape whiteBackground(sf::Vector2f(800.f, 400.f));
    whiteBackground.setPosition({ 200.f, 150.f });
    whiteBackground.setFillColor(sf::Color(255, 255, 255, 200)); // ������ - ����
    whiteBackground.setOutlineThickness(1.f);
    whiteBackground.setOutlineColor(sf::Color(100, 100, 100)); // �׵θ� ����

    // ������ �̹��� �ε�
    if (!profileTexture.loadFromFile("./assets/image/user.png")) {
        std::cerr << "������ �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    // ������ Sprite
    Sprite profileSprite(profileTexture);
    Vector2u profileTextureSize = profileTexture.getSize();
    float scalexX = 50.f / profileTextureSize.x;
    float scaleyY = 50.f / profileTextureSize.y;
    profileSprite.setScale(Vector2(scalexX, scaleyY));
    profileSprite.setPosition(Vector2f(20.f, 20.f));

    // ������ Text ���� (���� ���)
    Text profileText(font, "My Profile", 20);
    profileText.setPosition(Vector2f(100.f, 20.f));
    profileText.setFillColor(Color::Black);

    // �ѱ� ��� �̹��� �ε�
    if (!korButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "kor ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    // �ѱ� Sprite ����
    Sprite korButtonSprite(korButtonTexture);
    Vector2u textureSize = korButtonTexture.getSize();
    float scaleX = 50.f / textureSize.x;
    float scaleY = 50.f / textureSize.y;
    korButtonSprite.setScale(Vector2(scaleX, scaleY));
    korButtonSprite.setTexture(korButtonTexture);
    korButtonSprite.setPosition(Vector2f(320.f, 190.f));

    // �ѱ� Text ����
    Text korButtonText(font, L"�ѱ� Ÿ�� ����");
    korButtonText.setPosition(Vector2f(400.f, 200.f));
    korButtonText.setCharacterSize(30);
    korButtonText.setFillColor(Color::Black);

    // ���� ��� �̹��� �ε�
    if (!engButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "eng ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    // ���� Sprite ����
    Sprite engButtonSprite(engButtonTexture);
    engButtonSprite.setScale(Vector2(scaleX, scaleY));
    engButtonSprite.setTexture(engButtonTexture);
    engButtonSprite.setPosition(Vector2f(320.f, 290.f));

    // ���� Text ����
    Text engButtonText(font, L"���� Ÿ�� ����");
    engButtonText.setPosition(Vector2f(400.f, 300.f));
    engButtonText.setCharacterSize(30);
    engButtonText.setFillColor(Color::Black);

    // �ڵ� ��� �̹��� �ε�
    if (!codeButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "code mode ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    // �ڵ� ��� Sprite ����
    Sprite codeButtionSprite(codeButtonTexture);
    codeButtionSprite.setScale(Vector2(scaleX, scaleY));
    codeButtionSprite.setTexture(engButtonTexture);
    codeButtionSprite.setPosition(Vector2f(320.f, 390.f));

    // �ڵ� ��� Text ����
    Text codeButtonText(font, L"�ڵ� ��� ����");
    codeButtonText.setPosition(Vector2f(400.f, 400.f));
    codeButtonText.setCharacterSize(30);
    codeButtonText.setFillColor(Color::Black);

    // ��Ʋ ��� �̹��� �ε�
    if (!matchButtonTexture.loadFromFile("./assets/image/button.png")) {
        std::cerr << "Match ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    // ��Ʋ Sprite ����
    Sprite matchButtonSprite(matchButtonTexture);
    matchButtonSprite.setScale(Vector2(scaleX, scaleY));
    matchButtonSprite.setTexture(matchButtonTexture);
    matchButtonSprite.setPosition(Vector2f(320.f, 490.f));

    // ��Ʋ Text ����
    Text matchButtonText(font, L"�ڵ� ���");
    matchButtonText.setPosition(Vector2f(400.f, 500.f));
    matchButtonText.setCharacterSize(30);
    matchButtonText.setFillColor(Color::Black);

    // �ڵ� ��� ��ư �̹��� �ε�
    if (!codeJavaButtonTexture.loadFromFile("./assets/image/blueButton.png")) {
        std::cerr << "�ڵ� ��� ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    if (!codeCppButtonTexture.loadFromFile("./assets/image/blueButton2.png")) {
        std::cerr << "�ڵ� ��� ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    if (!codePyButtonTexture.loadFromFile("./assets/image/blueButton3.png")) {
        std::cerr << "�ڵ� ��� ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    // Java - �ϴû�
    Sprite codeJavaButtonSprite(codeJavaButtonTexture);
    codeJavaButtonSprite.setTexture(codeJavaButtonTexture);
    codeJavaButtonSprite.setScale(sf::Vector2f(1.2f, 1.2f));
    codeJavaButtonSprite.setPosition(sf::Vector2f(150.f, 300.f));

    Text javaButtonText(font, "Java", 35);
    javaButtonText.setFillColor(Color::Black);

    // ��ư�� �ؽ�Ʈ ��ġ ����
    sf::FloatRect javabuttonBounds = codeJavaButtonSprite.getGlobalBounds();
    sf::FloatRect javatextBounds = javaButtonText.getGlobalBounds();

    // ��ư �߾ӿ� �ؽ�Ʈ ��ġ
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

    // ��ư�� �ؽ�Ʈ ��ġ ����
    sf::FloatRect cppbuttonBounds = codeCppButtonSprite.getGlobalBounds();
    sf::FloatRect cpptextBounds = cppButtonText.getGlobalBounds();

    // ��ư �߾ӿ� �ؽ�Ʈ ��ġ
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

    // ��ư�� �ؽ�Ʈ ��ġ ����
    sf::FloatRect pybuttonBounds = codePyButtonSprite.getGlobalBounds();
    sf::FloatRect pytextBounds = pyButtonText.getGlobalBounds();

    // ��ư �߾ӿ� �ؽ�Ʈ ��ġ
    pyButtonText.setPosition(Vector2f(
        pybuttonBounds.position.x + (pybuttonBounds.size.x / 2) - (pytextBounds.size.x / 2),
        pybuttonBounds.position.y + (pybuttonBounds.size.y / 2) - (pytextBounds.size.y / 2))
    );

    // ���� ���� ��ư
    if (!fileSelectTexture.loadFromFile("./assets/image/bluebutton4.png")) {
        std::cerr << "���� ���� ��ư �̹��� �ε� ����!" << std::endl;
        return -1;
    }

    Sprite fileSelectSprite(fileSelectTexture);
    fileSelectSprite.setTexture(fileSelectTexture);
    fileSelectSprite.setScale(sf::Vector2f(0.5f, 0.5f));
    fileSelectSprite.setPosition(sf::Vector2f(1200.f, 600.f));

    Text fileSelectText(font, L"���� ����", 20);
    fileSelectText.setFillColor(Color::Black);
    fileSelectText.setPosition(sf::Vector2f(1200.f, 600.f));

    // �ڵ� ��� ���� Ȯ�� ����
    bool isCodingMode = false;

    // ���� ���� ����
    while (gameWindow.isOpen()) {
        while (const std::optional<Event> event = gameWindow.pollEvent()) {
            if (event->is<Event::Closed>()) {
                gameWindow.close();
            }

            if (event->is<Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(gameWindow);

                // �α��� ������ �� �α׾ƿ� ��ư �� ��ŷ ������ Ŭ�� ó��
                if (loggedIn) {
                    if (rankingIcon.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        showRankingUI = true;
                    }

                    // �α׾ƿ� ��ư Ŭ�� ��
                    else if (logoutButton.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        loggedIn = false;  // �α��� ���� �ʱ�ȭ
                        loggedOut = true;  // �α׾ƿ� �÷��� ����
                    }

                    // �⺻ ���� ���� ȭ�� ��ư Ŭ�� ó��
                    else if (!isCodingMode) {
                       
                        // �ѱ� Ÿ�� ����
                        if (korButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            std::wcout << L"�ѱ� Ÿ�� ���� ����!" << endl;
                            try {
                                game.handleTypingGameMode(loader, util, typing, gameWindow, font, false);
                            }
                            catch (const std::exception& e) {
                                std::cerr << "[���� �߻�] handleTypingMode: " << e.what() << std::endl;
                            }
                            catch (...) {
                                std::cerr << "[���� �߻�] �� �� ���� ���� in handleTypingMode()" << std::endl;
                            }
                        }

                        // ���� ���
                        else if (engButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            game.handleTypingGameMode(loader, util, typing, gameWindow, font, false);
                        }

                        // �ڵ� ���
                        else if (codeButtionSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = true;
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
                                game.showLoadingScreen(gameWindow, font, isMatching);
                                wcout << L"�ε�â ����!!" << endl;
                                });

                            if (client.waitForMatch(gameWindow, font)) {
                                isMatching = true;
                                if (loadingThread.joinable()) loadingThread.join();
                                wcout << L"��ġ �Ϸ�Ǿ���!!" << endl;
                                wcout << L"���� ����!!" << endl;

                                std::string filePath;
                                filePath = getRandomFile(folderPath, ".java");
                                if (filePath.empty()) {
                                    std::cerr << "[�����] Error: No file selected!" << std::endl;
                                    std::cout << filePath << std::endl;
                                }
                                else {
                                    std::cout << "[�����] Selected file: " << filePath << std::endl;
                                    // ���� ��ΰ� �ùٸ��� Ȯ��
                                    std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                    std::cout << "[�����] Absolute path: " << absPath << std::endl;

                                    // ���Ͽ��� ���� �б�
                                    std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);

                                    // ���� �ε� Ȯ��
                                    if (sentences.empty()) {
                                        std::cout << "Error: No sentences loaded!" << std::endl;
                                    }
                                    else {
                                        std::cout << "[�����] ���� ����: ���� �� = " << sentences.size() << std::endl;
                                        game.runGame2(gameWindow, font, 24, sentences);
                                    }
                                }
                            }
                        }

                    }
                    // �ڵ� ��� ȭ�� ��ư Ŭ�� ó��
                    else {

                        // Java ��ư
                        if (codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            filePath = getRandomFile(folderPath, ".java");
                            if (!filePath.empty()) {
                                std::cout << "[�����] Selected file: " << filePath << std::endl;
                                std::filesystem::path absPath = std::filesystem::absolute(filePath);
                                std::cout << "[�����] Absolute path: " << absPath << std::endl;

                                // ���Ͽ��� ���� �б�
                                std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);

                                if (!sentences.empty()) {
                                    std::cout << "[�����] ���� ����: ���� �� = " << sentences.size() << std::endl;
                                    typing.startNewGame(gameWindow, font, 24, sentences);
                                }
                            }
                        }
                        // C++ ��ư
                        else if (codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            filePath = getRandomFile(folderPath, ".cpp");
                            if (!filePath.empty()) {
                                std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);
                                if (!sentences.empty()) {
                                    typing.startNewGame(gameWindow, font, 24, sentences);
                                }
                            }
                        }
                        // Python ��ư
                        else if (codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            filePath = getRandomFile(folderPath, ".py");
                            if (!filePath.empty()) {
                                std::vector<std::vector<std::wstring>> sentences = typing.loadSentencesFromFile(filePath);
                                if (!sentences.empty()) {
                                    typing.startNewGame(gameWindow, font, 24, sentences);
                                }
                            }
                        }
                        // ���� ���� ��ư
                        else if (fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            game.handleTypingGameMode(loader, util, typing, gameWindow, font, false);
                        }
                        // ���� ȭ������ ���ư���
                        else if (!fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y)) &&
                            !codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                            isCodingMode = false; // �ڵ� ��� ����
                        }

                    }

                }
                // �α��� ���°� �ƴ� �� ��ŷ ������ Ŭ�� ó��
                else {
                    if (rankingIcon.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        showRankingUI = true;
                    }
                }

            }
        }


        gameWindow.clear();

        // �α��� ���°� �ƴ� �� �α��� UI ǥ��
        if (!loggedIn) {
            // �α��� UI�� ǥ��
            if (showLoginWindow(gameWindow, db, userID)) {
                std::cout << "�α��� ����! ���� ID: " << userID << std::endl;
                loggedIn = true;
            }
            else {
                // �α��� ȭ�鿡�� ��ŷ �����ܵ� ǥ��
                gameWindow.draw(rankingIcon);
            }
        }
        else {
            if (!isCodingMode) {
                // �⺻ ���� ���� ȭ�� ������� �׸���
                gameWindow.draw(backGroundSprite); // ��� ���� 
                gameWindow.draw(whiteBackground);  // �Ͼ�� ��� 
                gameWindow.draw(profileSprite);
                gameWindow.draw(profileText);    // ���� ��ܿ� profileText �׸���

                gameWindow.draw(korButtonSprite);  // �ѱ� ��ư �̹��� �׸���
                gameWindow.draw(korButtonText);    // �ѱ� Text �̹��� �׸���

                gameWindow.draw(engButtonSprite);  // ���� ��ư �̹��� �׸���
                gameWindow.draw(engButtonText);    // ���� Text �̹��� �׸���

                gameWindow.draw(codeButtionSprite); // �ڵ���� ��ư �̹��� �׸���
                gameWindow.draw(codeButtonText);    // �ڵ���� Text �̹��� �׸���

                gameWindow.draw(matchButtonSprite);  // 1:1 Match ��ư �̹��� �׸���
                gameWindow.draw(matchButtonText);    // match Text �׸���

                gameWindow.draw(rankingIcon);        // ��ŷ ������ �׸���
                gameWindow.draw(logoutButton);       // �α׾ƿ� ��ư �׸���
                gameWindow.draw(logoutButtonText);   // �α׾ƿ� �ؽ�Ʈ �׸���

            }
            else {

                // �ڵ� ��� ȭ�� ������� �׸���
                gameWindow.draw(backGroundSprite); // ��� ǥ��
                gameWindow.draw(whiteBackground); // ��� ���
                gameWindow.draw(codeJavaButtonSprite); // Java ��ư
                gameWindow.draw(javaButtonText);
                gameWindow.draw(codeCppButtonSprite); // C++ ��ư
                gameWindow.draw(cppButtonText);
                gameWindow.draw(codePyButtonSprite); // Python ��ư
                gameWindow.draw(pyButtonText);
                gameWindow.draw(fileSelectSprite); // ���� ���� ��ư
                gameWindow.draw(fileSelectText);

            }
            gameWindow.draw(rankingIcon); // ��ŷ ������
            gameWindow.draw(logoutButton); // �α׾ƿ� ��ư
            gameWindow.draw(logoutButtonText);
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
    }

    return 0;
}