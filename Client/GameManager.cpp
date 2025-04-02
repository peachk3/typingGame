#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
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



// ��Ʈ NanumGothic //

// ���� ����
void GameManager::runGame(sf::RenderWindow& window, sf::Font& font) {


    std::cout << "���� ����â �̵� �Ϸ�!!" << std::endl;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    Texture ladderImg;

    // �̹��� �ε�
    if (!ladderImg.loadFromFile("./assets/image/ladder3.png")) {
        cerr << "��ٸ� �̹��� �ε� ����!" << endl;
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

    // ��ٸ� 2 �̹��� ����
    Sprite ladderImgSpriteRight(ladderImg);
    //Vector2u ladderImgSize = ladderImg.getSize();
    //// ���ϴ� ũ��� ���� (��: 200x200)
    //float scaleX = 30.f / ladderImgSize.x;
    //float scaleY = 50.f / ladderImgSize.y;
    ladderImgSpriteRight.setTexture(ladderImg);
    ladderImgSpriteRight.setPosition(Vector2f(1080.f, 150.f));     // ������ 

    sf::Text gameText(font, L"���� ����~~!", 60);

    gameText.setFillColor(Color::Red);
    gameText.setPosition(Vector2f(480.f, 50.f)); // (���� -> ������, �� -> �Ʒ�)

    //------------------------------------Ÿ�� �Է�â �����------------------------------------
    // Ÿ�� ���� ���� ����Ʈ
    vector<wstring> sentences = { L"Hello", L"Welcome", L"Bye" };
    size_t currentSentenceIndex = 0;
    wstring userInput;

    // ���� ���� ǥ��
    Text currentSentence(font, sentences[currentSentenceIndex], 50);
    FloatRect textBounds = currentSentence.getGlobalBounds();
    currentSentence.setOrigin(textBounds.getCenter());
    currentSentence.setFillColor(Color::Black);
    currentSentence.setPosition(Vector2f(1280.f / 2, 300.f)); // ��� ����

    // �Է�â ��� ����
    sf::RectangleShape inputBackground(sf::Vector2f(500.f, 40.f)); // �Է�â ũ��
    FloatRect inputTextBounds = inputBackground.getGlobalBounds();
    inputBackground.setOrigin(inputTextBounds.getCenter());

    inputBackground.setPosition({ 1280.f / 2, 500.f });
    inputBackground.setFillColor(sf::Color(240, 240, 240)); // ���� ȸ�� ���
    inputBackground.setOutlineThickness(2.f);
    inputBackground.setOutlineColor(sf::Color(100, 100, 100)); // �׵θ� ����

    // ���� ���� ǥ��
    Text nextSentence(font, L"", 20);
    if (currentSentenceIndex + 1 < sentences.size()) {
        nextSentence.setString(sentences[currentSentenceIndex + 1]);
    }
    FloatRect nextTextBounds = nextSentence.getGlobalBounds();
    nextSentence.setOrigin(nextTextBounds.getCenter());
    nextSentence.setFillColor(Color(32, 32, 32)); // �������� ����� ȸ��
    nextSentence.setPosition(Vector2f(1280.f / 2, 400.f)); // ���

    // ������ ��� ����
    sf::RectangleShape Background(sf::Vector2f(400.f, 70.f));
    // ��� ũ�� ��������
    FloatRect bgBounds = Background.getGlobalBounds();
    Background.setOrigin(bgBounds.getCenter()); // �߾� ���� ����

    Background.setFillColor(sf::Color(224, 224, 224, 200)); // ������ - ����
    Background.setPosition(Vector2f(1280.f / 2, 400.f));
    //Background.setOutlineThickness(1.f);
    //Background.setOutlineColor(sf::Color(139, 0, 0)); // �׵θ� ����

    // �Է� ǥ��
    Text userInputText(font, L"", 30);
    userInputText.setFillColor(Color::Green);
    userInputText.setPosition(Vector2f(400.f, 485.f)); // ȭ�� ���߾�

    //--------------------------------------------------------------------------------------------

    // userID �޾ƿͼ� ��� (
    wstring user1ID = L"����� 1"; // id �޾ƿͼ� �ֱ�
    wstring user2ID = L"����� 2"; // id �޾ƿͼ� �ֱ�

    Text user1Text(font, user1ID, 20);
    Text user2Text(font, user2ID, 20); 
    user1Text.setFillColor(Color::Black);
    user2Text.setFillColor(Color::Black);


    user1Text.setPosition(Vector2f(600.f, 500.f)); // (���� -> ������, �� -> �Ʒ�)
    user2Text.setPosition(Vector2f(600.f, 100.f)); // (���� -> ������, �� -> �Ʒ�)


    window.clear(Color::White);
    window.draw(Background);
    window.draw(gameText);
    window.draw(userInputText);
    window.draw(ladderImgSpriteLeft);       // ����
    window.draw(ladderImgSpriteRight);      // ������
    window.draw(currentSentence);
    window.draw(nextSentence);
    //window.draw(user1Text);
    //window.draw(user2Text);
    window.draw(inputBackground);
    window.display();

    // Ŀ�� ���� �߰�
    sf::Clock cursorClock;
    bool showCursor = true;
    sf::RectangleShape cursor(sf::Vector2f(2.f, 30.f));
    cursor.setFillColor(sf::Color::Black);
    cursor.setPosition({ 400.f, 485.f });


    while (window.isOpen()) {
        bool isWindowClosed = false;

        while (std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                isWindowClosed = true;
                break;
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    isWindowClosed = true;
                    break;
                }
            }
            // �ؽ�Ʈ �Է� ó��
            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                if (textEntered->unicode == 8) { // �齺���̽� ó��
                    if (!userInput.empty()) {
                        userInput.pop_back();
                    }
                }
                else if (textEntered->unicode == 13) { // ���� �Է� �� ����
                    if (userInput == sentences[currentSentenceIndex]) {
                        currentSentenceIndex++;
                        if (currentSentenceIndex < sentences.size()) {
                            userInput.clear();
                            currentSentence.setString(sentences[currentSentenceIndex]);
                            nextSentence.setString((currentSentenceIndex + 1 < sentences.size()) ? sentences[currentSentenceIndex + 1] : L"");
                        }
                        else {
                            gameText.setString(L"���� ����!");
                            currentSentence.setString(L"");
                            nextSentence.setString(L"");
                        }
                    }
                }
                else {
                    userInput += static_cast<wchar_t>(textEntered->unicode);
                    std::wcout << L"�Էµ� ����: " << static_cast<wchar_t>(textEntered->unicode) << std::endl;
               
                }
                // UTF-8�� ��ȯ�Ͽ� SFML Text�� ����
                userInputText.setString(converter.to_bytes(userInput));
                //userInputText.setString(userInput);
            }
        }

        if (isWindowClosed) {
            break;  // â�� ������ �ݺ����� �����Ͽ� ������ ����
        }

        // Ŀ�� ������ ������Ʈ
        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
        }

        //sf::Vector2u textureSize = texture.getSize();
        //float scaledWidth = textureSize.x * 0.2f;
        //float scaledHeight = textureSize.y * 0.2f;
        //sprite.setPosition({
        //    window.getSize().x - scaledWidth - 20.f,
        //    window.getSize().y - scaledHeight - 20.f
        //    });

        //// Ŀ�� ��ġ ������Ʈ
        //float textWidth = userInputText.getLocalBounds().width;
        //cursor.setPosition({ 300.f + textWidth, 350.f });


        if (isWindowClosed) {
            break;  // â�� ������ �ݺ����� �����Ͽ� ������ ����
        }


        window.clear(Color::White);
        window.draw(Background);
        window.draw(gameText);
        window.draw(userInputText);
        window.draw(ladderImgSpriteLeft);
        window.draw(ladderImgSpriteRight);
        window.draw(currentSentence);
        window.draw(nextSentence);
        window.draw(inputBackground);  // ���� ����� �׸�
        window.draw(userInputText);    // �� ���� �ؽ�Ʈ�� �׸�
        //window.draw(user1Text);
        //window.draw(user2Text);

        // Ŀ�� ǥ�� ���� ����
        if (showCursor) {
            window.draw(cursor);
        }

        //window.draw(inputBackground);
        window.display();
    }

}
