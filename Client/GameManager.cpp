#include "gameManager.h"
#include <locale>
#include <codecvt>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>  // ���ڿ� ��Ʈ���� ����ϱ� ���� ���
#include <iomanip>  // �ð��� ���Ŀ� �°� ����ϱ� ���� ���
#include <SFML/Network.hpp>

using namespace sf;
using namespace std;
GameManager game;

const int GWINDOW_WIDTH = 300;
const int GWINDOW_HEIGHT = 250;
const float ROTATION_SPEED = 60.f;  // ȸ�� �ӵ� (1�ʴ� n�� ȸ��)

// ID �Է�â
void showIDInputScreen(sf::RenderWindow& window, sf::Font& font, std::string& clientID) {
    sf::Text idText(font, L"���̵� �Է��ϼ���: ", 30);
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
                if (textEntered->unicode == 8 && clientID.length() > 0) {  // �齺���̽�
                    clientID.pop_back();
                }
                else if (textEntered->unicode < 128) {  // ���� �Է�
                    clientID += static_cast<char>(textEntered->unicode);
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Enter) {
                    inputComplete = true;  // Enter Ű�� �Է� �Ϸ�
                }
            }
        }
        inputText.setString(clientID);  // �Էµ� ���̵� ȭ�鿡 ǥ��
        window.clear();
        window.draw(idText);
        window.draw(inputText);
        window.display();
        if (inputComplete) {
            break;
        }
    }
}

// �ε�â ����
void GameManager::showLoadingScreen(sf::RenderWindow& window, sf::Font& font, bool& isMatching) {

    RenderWindow gameWindow(VideoMode({ GWINDOW_WIDTH, GWINDOW_HEIGHT }), L"���� ȭ��");

    // �ؽ�ó�� ��������Ʈ �ε�
    Texture texture;
    if (!texture.loadFromFile("./loading.png")) {  // "loading.png"�� �غ��ؾ� ��
        cerr << "�̹��� �ε� ����!" << endl;
        return;
    }

    Sprite sprite(texture);

    //sprite.setPosition(Vector2f(float(GWINDOW_WIDTH / 2), float(GWINDOW_HEIGHT / 2))); // �ʱ� ��ġ ����

    // ��������Ʈ ���� ���� (�߾����� ȸ���ϵ���)
    sprite.setOrigin(Vector2f(texture.getSize().x / 2.f, texture.getSize().y / 2.f));
    sprite.setPosition(Vector2f(GWINDOW_WIDTH / 2, GWINDOW_HEIGHT / 2));  // ȭ�� �߾����� �̵�

    Text statusText(font, L"��� �˻���...", 30);
    statusText.setFillColor(Color::Black);
    statusText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 75, (GWINDOW_HEIGHT / 2) - 100));

    Text timerText(font, "00:00", 30);  // �ð� ǥ�� �ؽ�Ʈ
    timerText.setFillColor(Color::Black);
    timerText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 35, (GWINDOW_HEIGHT / 2) - 70));

    Text matchingText(font, L"��� ���� �Ϸ�!", 30);  // Matching! �ؽ�Ʈ
    matchingText.setFillColor(Color::Black);
    matchingText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 100, (GWINDOW_HEIGHT / 2) - 100));

    Text countdownText(font, "", 50);  // ī��Ʈ�ٿ� �ؽ�Ʈ
    countdownText.setFillColor(Color::Black);
    countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 10, (GWINDOW_HEIGHT / 2 - 20)));  // Matching! �Ʒ��� ǥ��

    Clock clock;  // ���� ���� �ð� ������ ���� �ð�
    Clock countdownClock;  // ī��Ʈ�ٿ��� ���� �ð�
    Time elapsedTime;

    //bool isMatching = false;  // ��Ī ���� ����
    int countdown = 3;  // ī��Ʈ�ٿ� ����
    bool countdownStarted = false;
    bool gameStartDisplayed = false;


    // �������� ���� ī��Ʈ�ٿ� ���� �ð� (���� �ð�)
    // �� ���� ������ ����� ��Ʈ��ũ �ڵ忡�� �����ؾ� ��
    Time serverStartTime;
    bool serverTimeReceived = false;
    bool gameStarted = false;

    while (gameWindow.isOpen()) {
        // �̺�Ʈ ó�� (SFML 3.0.0 ���)
        std::optional<Event> event;
        while (const std::optional event = gameWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                std::cout << "gameWindow.close()" << std::endl;
                gameWindow.close();
                return;
            }
        }

        // ��Ʈ��ũ���� ��Ī ���� �� ī��Ʈ�ٿ� ���� �ð� ���� (����)
        // �����δ� ��Ʈ��ũ �ڵ忡�� �� �������� �����ؾ� �մϴ�
        if (isMatching && !serverTimeReceived) {
            // �����κ��� ī��Ʈ�ٿ� ���� �ð� ���� (����)
            // serverStartTime = �������� ���� �ð�
            cout << "ī��Ʈ�ٿ� ����!" << endl;
            serverTimeReceived = true;
            countdownClock.restart(); // ���� �ð��� ���� �������� ���� �ð� ����
        }
        // ��Ī�� �� ��� ī��Ʈ�ٿ� ó��
        if (isMatching && serverTimeReceived) {
            //cout << "isMatching: " << isMatching << "serverTimeReceived:" << serverTimeReceived << endl;
            gameWindow.clear(Color::White);
            gameWindow.draw(matchingText);

            // ���� �ð��� ���� ���� ī��Ʈ�ٿ� ���
            float elapsedCountdownTime = countdownClock.getElapsedTime().asSeconds();

            // ī��Ʈ�ٿ� �� ��� (���� �ð� ����)
            int currentCountdown = countdown - static_cast<int>(elapsedCountdownTime);
            //std::cout << "currentCountdown: " << currentCountdown << std::endl;
            if (currentCountdown <= 0 && !gameStarted) {
                // ī��Ʈ�ٿ� ����, ���� ����
                countdownText.setString("Start Game!");
                countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 135, GWINDOW_HEIGHT / 2 - 50));
                countdown = 0;  // ī��Ʈ�ٿ� ����
                gameWindow.draw(countdownText);
                //gameWindow.draw(countdownText);
                //gameWindow.display();
                std::cout << "����� ���� ����!!" << std::endl;
                gameStarted = true;  // ���� ���� �÷��� ����
                isMatching = 0;  // ��Ī ���� ���� (�ߺ� ���� ����)
                // ��� ��� �� ���� ����
                std::this_thread::sleep_for(std::chrono::seconds(1));
                gameWindow.close();
                return;
            }
            else {
                // ī��Ʈ�ٿ� ǥ��
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

        // ��Ī �������� ���
        else if (gameStarted != true) {
            // deltaTime�� ����Ͽ� ȸ�� �ӵ� ����
            Time deltaTime = clock.restart();  // ���� �����ӿ��� ����� �ð�
            elapsedTime += deltaTime;  // ��ü ��� �ð� ����

            // �̹��� 360�� ȸ��
            sprite.rotate(degrees(ROTATION_SPEED) * deltaTime.asSeconds());  // �ʴ� ROTATION_SPEED���� ȸ��

            // ����� �ð� ��� (�� ������)
            int seconds = static_cast<int>(elapsedTime.asSeconds());
            int minutes = seconds / 60;
            seconds = seconds % 60;

            // 00:00 �������� �ð� �ؽ�Ʈ ������Ʈ
            stringstream ss;
            ss << setw(2) << setfill('0') << minutes << ":" << setw(2) << setfill('0') << seconds;
            timerText.setString(ss.str());

            // ȭ�� �׸���
            gameWindow.clear(Color::White);
            gameWindow.draw(sprite);  // ȸ���ϴ� �̹��� �׸���
            gameWindow.draw(statusText);  // ���� ���� �޽��� �׸���
            gameWindow.draw(timerText);   // �ð� �ؽ�Ʈ �׸���
            gameWindow.display();

        }
    }
}

void GameManager::runGame(sf::RenderWindow& window, sf::Font& font) {
    std::cout << "���� ����â �̵� �Ϸ�!!" << std::endl;

    Texture ladderImg;

    // �̹��� �ε�
    if (!ladderImg.loadFromFile("./assets/image/ladder3.png")) {
        cerr << "Typing ��ư �̹��� �ε� ����!" << endl;
        //return;
    }

    sf::Sprite sprite(ladderImg);

    // ��ٸ� 1 �̹��� ����
    Sprite ladderImgSpriteLeft(ladderImg);
    //Vector2u ladderImgSize = ladderImg.getSize();
    //// ���ϴ� ũ��� ���� (��: 200x200)
    //float scaleX = 30.f / ladderImgSize.x;
    //float scaleY = 50.f / ladderImgSize.y;
    ladderImgSpriteLeft.setTexture(ladderImg);
    ladderImgSpriteLeft.setPosition(Vector2f(100.f, 150.f));     // ����
    //ladderImgSpriteLeft.setPosition(Vec)

    // ��ٸ� 2 �̹��� ����
    Sprite ladderImgSpriteRight(ladderImg);
    //Vector2u ladderImgSize = ladderImg.getSize();
    //// ���ϴ� ũ��� ���� (��: 200x200)
    //float scaleX = 30.f / ladderImgSize.x;
    //float scaleY = 50.f / ladderImgSize.y;
    ladderImgSpriteRight.setTexture(ladderImg);
    ladderImgSpriteRight.setPosition(Vector2f(1080.f, 150.f));     // ������


    //sf::FloatRect bounds = ladderImgSprite.getLocalBounds();

    //sprite.setScale(scaleX, scaleY);

    sf::Text gameText(font, L"���� ����~~!", 60);

    gameText.setFillColor(Color::Red);
    gameText.setPosition(Vector2f(480.f, 50.f)); // (���� -> ������, �� -> �Ʒ�)

    window.clear(Color::White);
    window.draw(gameText);
    window.draw(ladderImgSpriteLeft);       // ����
    window.draw(ladderImgSpriteRight);      // ������
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
            break;  // â�� ������ �ݺ����� �����Ͽ� ������ ����
        }
    }
}
