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
    int countdown = 5;  // ī��Ʈ�ٿ� ����
    bool countdownStarted = false;
    bool gameStartDisplayed = false;

    while (gameWindow.isOpen()) {
        // �̺�Ʈ ó�� (SFML 3.0.0 ���)
        std::optional<Event> event;
        while (const std::optional event = gameWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                gameWindow.close();
                return;
            }
        }

        // ��Ī�� �� �Ŀ��� ȭ�� Ŭ���� �� ��Ī ī��Ʈ�ٿ�
        if (isMatching) {
            gameWindow.clear(Color::Blue); // ��Ī �� ȭ���� Ŭ����
            gameWindow.draw(matchingText);  // Matching �ؽ�Ʈ ���

            //countdownStarted = true;
            //
            //if (countdownTime > 1.0f)
            // ��Ī �� ù ���� �� ī��Ʈ �ð� ����
            //if (!countdownStarted) {  // 1�ʸ��� ī��Ʈ�ٿ� ������Ʈ
            //    countdownClock.restart();
            //    countdownStarted = true;
            //    //gameWindow.draw(countdownText);  // ī��Ʈ�ٿ� �ؽ�Ʈ ���

            //}
            // ī��Ʈ�ٿ� ó��
            float countdownTime = countdownClock.getElapsedTime().asSeconds();

            //if (countdown > 0) {
            //    //countdown--;
            //    gameWindow.clear(Color::Red);
            //    gameWindow.draw(matchingText);
            //    gameWindow.draw(countdownText);  // ī��Ʈ�ٿ� �ؽ�Ʈ ���

            //}

            // 1�ʸ��� ī��Ʈ�ٿ� ����
            if (countdownTime > 1.0f) {
                countdownClock.restart();

                //countdownText.setString("Start Game!");
                if (countdown > 0) {
                    countdown--; 
                }

                if (countdown == 0) {
                    matchingText.setString("");
                    countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 135, GWINDOW_HEIGHT / 2 - 50));  // Matching! �Ʒ��� ǥ��
                    
                    //countdownText.setString("Start Game!");
                    // ī��Ʈ�ٿ� �� ���� ���� �ڵ� �߰� ����
                    return;
                }
                else {
                    countdownText.setString(to_string(countdown));
                }

                //if (countdown > 0) {
                //    gameWindow.clear(Color::Black);
                //    gameWindow.draw(matchingText);
                //    gameWindow.draw(countdownText);  // ī��Ʈ�ٿ� �ؽ�Ʈ ���

                //    ////matchingText.setString("");
                //    //countdownText.setPosition(Vector2f(GWINDOW_WIDTH / 2 - 135, GWINDOW_HEIGHT / 2 - 50));  // Matching! �Ʒ��� ǥ��
                //    //Text countdownText(font, L"���� ����!", 50);  // Matching! �ؽ�Ʈ
                //    //// ī��Ʈ�ٿ� �� ���� ���� �ڵ� �߰� ����

                //    //gameWindow.display();
                //    //std::this_thread::sleep_for(std::chrono::seconds(1));  // 1�� ���
                //    //gameWindow.close();  // �ε� â �ݱ�
                //    return;  // �Լ� ����
                //}
                //else {
                //    // ī��Ʈ�ٿ� ��, "���� ����!" ǥ��
                //    gameWindow.clear(Color::Magenta);
                //    //gameWindow.draw(startText);
                //    gameWindow.display();

                //    // 1�� ��� �� â �ݱ�
                //    std::this_thread::sleep_for(std::chrono::seconds(1));
                //    gameWindow.close();
                //    return;  // �Լ� ����
                //}
            }
            gameWindow.draw(countdownText);  // ī��Ʈ�ٿ� �ؽ�Ʈ ���
            gameWindow.display();

            /*else {
                countdownText.setString(to_string(countdown));
            }*/

            //gameWindow.draw(countdownText);  // ī��Ʈ�ٿ� �ؽ�Ʈ ���
            //gameWindow.display();

            //if (countdown == 0) {
            //    countdownText.setString(L"���� ����!");
            //    gameWindow.display();
            //    std::this_thread::sleep_for(std::chrono::seconds(1));  // 1�� ��� �� â �ݱ�

            //    gameWindow.close();  // ī��Ʈ�ٿ� ���� �� �ε�â �ݱ�
            //    return;
            //}
        }
        // ��Ī �������� ���
        else {
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

        //std::this_thread::sleep_for(std::chrono::seconds(1)); // 1�� ���

        //sf::Font font;
        //font.openFromFile("D2Coding.tff");
        //sf::Text loadingText(font, L"��Ī ��� ��...", 50);
        ////loadingText.setFont(font);
        ////loadingText.setCharacterSize(50);
        //loadingText.setFillColor(sf::Color::White);
        //loadingText.setPosition(sf::Vector2f(600.f, 300.f));

        //window.clear();
        //window.draw(loadingText);
        //window.display();
    }
}

//bool GameManager::showCountdown(sf::RenderWindow& window, sf::Font& font) {
//    //sf::Font font;
//    //font.openFromFile("D2Coding.tff");
//
//    sf::Text countdownText(font, '3', 80);
//    //countdownText.setFont(font);
//    //countdownText.setCharacterSize(80);
//    countdownText.setFillColor(sf::Color::White);
//    countdownText.setPosition(sf::Vector2f(600.f, 300.f));
//
//    for (int i = 3; i >= 1; --i) {
//        countdownText.setString(std::to_wstring(i));
//
//        window.clear();
//        window.draw(countdownText);
//        window.display();
//
//        while (std::optional event = window.pollEvent()) {
//
//            if (event->is<sf::Event::Closed>())
//            {
//                window.close();
//            }
//        }
//        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1�� ���
//    }
//
//    window.clear();
//    window.draw(countdownText);
//    window.display();
//
//    //std::this_thread::sleep_for(std::chrono::seconds(1)); // 1�� ���
//
//    //sf::Text startText(font, L"���� ����!@!@!@!!", 50);
//
//    //startText.setFillColor(sf::Color::White);
//    //startText.setPosition(sf::Vector2f(500.f, 300.f));
//
//    //window.clear();
//    //window.draw(startText);
//    //window.display();
//    //
//    return true;
//}

void GameManager::receiveUpdates() {
    //char buffer[128];
    //std::size_t received;
    //socket.setBlocking(false); // ����ŷ ��� ����

    //while (!gameOver) {
    //    sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
    //    if (sf::Socket::Done) {
    //        std::lock_guard<std::mutex> lock(mtx);
    //        std::string data(buffer, received);
    //        std::cout << "���� �޽���: " << data << "\n";

    //        if (data.rfind("ID ", 0) == 0) {
    //            myID = std::stoi(data.substr(3));
    //            std::cout << "�� ID: " << myID << "\n";
    //        }
    //        else if (data.rfind("PROGRESS ", 0) == 0) {
    //            int receivedID, newProgress;
    //            sscanf(data.c_str(), "PROGRESS %d %d", &receivedID, &newProgress);

    //            if (receivedID == myID) {
    //                myProgress = newProgress;
    //            }
    //            else {
    //                opponentProgress = newProgress;
    //            }
    //        }
    //        else if (data == "WIN") {
    //            gameOver = true;
    //            std::cout << "�¸��Դϴ�.\n";
    //        }
    //        else if (data == "LOSE") {
    //            gameOver = true;
    //            std::cout << "�й��Դϴ�.\n";
    //        }
    //    }
    //}
}

void GameManager::runGame(sf::RenderWindow& window, sf::Font& font) {
    //if (!font.openFromFile("D2Coding.tff")) { // �ѱ� ���� ��Ʈ ���
    //    std::cout << "��Ʈ �ε� ����!" << std::endl;
    //    return;
    //}

    //// ���� ����
    //if (socket.connect("127.0.0.1", 53000) == sf::Socket::Done) {
    //    std::cout << "������ �����!\n";
    //}
    //std::thread(&GameManager::receiveUpdates, this).detach();

    sf::Text gameText(font, L"���� ����~~~~~~!", 60);

    gameText.setFillColor(sf::Color::Red);
    gameText.setPosition(sf::Vector2f(500.f, 300.f));

    window.clear();
    window.draw(gameText);
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




    //window.clear();
    //window.draw(gameText);
    //window.display();

    //while (window.isOpen()) {
    //    while (std::optional event = window.pollEvent())
    //    {
    //        if (event->is<sf::Event::Closed>())
    //        {
    //            window.close();
    //        }

    //        //
    //        //            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
    //        //            {
    //        //                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
    //        //                    window.close();
    //        //            }
    //        //        }
    //        //if (!gameOver && event->is<sf::Event::TextEntered>()) {
    //        //    if (event->as<sf::Event::TextEntered>().unicode == '\b' && !userInput.empty()) {
    //        //        userInput.pop_back();  // �齺���̽� ó��
    //        //    }
    //        //    else if (event->get<sf::Event::TextEntered>().unicode < 128) {
    //        //        userInput += static_cast<char>(event->get<sf::Event::TextEntered>().unicode);  // �ؽ�Ʈ �Է�
    //        //    }
    //        //}

    //        //if (!gameOver && event.type == sf::Event::TextEntered) {
    //        //    if (event.text.unicode == '\b' && !userInput.empty()) {
    //        //        userInput.pop_back();
    //        //    }
    //        //    else if (event.text.unicode < 128) {
    //        //        userInput += static_cast<char>(event.text.unicode);
    //        //    }
    //        //}

    //        //if (!gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
    //        //    if (userInput == targetSentence) {
    //        //        myProgress++;
    //        //        std::cout << "�Է� ����! ���� �Է� Ƚ��: " << myProgress << "\n";

    //        //        std::string progressMsg = "PROGRESS " + std::to_string(myID) + " " + std::to_string(myProgress);
    //        //        socket.send(progressMsg.c_str(), progressMsg.size() + 1);

    //        //        userInput.clear();

    //        //        if (myProgress >= 3) {
    //        //            gameOver = true;
    //        //            resultText.setString("�¸��Դϴ�!!");
    //        //        }
    //        //    }
    //        //    else {
    //        //        std::cout << "�߸��� �Է�!\n";
    //        //    }
    //        //}
    //    }

    //    if (opponentProgress >= 3 && !gameOver) {
    //        gameOver = true;
    //        resultText.setString("�й��Դϴ�");
    //    }

    //    if (myID != -1) {
    //        idText.setString("ID: " + std::to_string(myID));
    //    }
    //    myProgressText.setString("�� ���൵: " + std::to_string(myProgress));
    //    opponentProgressText.setString("��� ���൵: " + std::to_string(opponentProgress));
    //    inputText.setString("�Է� ��: " + userInput);
    //    inputText.setFillColor(userInput == targetSentence.substr(0, userInput.size()) ? sf::Color::White : sf::Color::Red);

    //    window.clear();
    //    window.draw(idText);
    //    window.draw(myProgressText);
    //    window.draw(opponentProgressText);
    //    window.draw(targetText);
    //    window.draw(inputText);
    //    window.draw(resultText);
    //    window.display();
    //}
}
//void GameManager::runGame(sf::RenderWindow& window, sf::Font& font) {
//    //sf::Font font;
//    //font.openFromFile("D2Coding.tff");
//    sf::Font font;
//    if (!font.openFromFile("D2Coding.tff")) { // �ѱ� ���� ��Ʈ ���
//        std::cout << "��Ʈ �ε� ����!" << std::endl;
//        return;
//    }
//
//    //sf::Text gameText(font, L"���� ����!", 60);
//    ////gameText.setFont(font);
//    ////gameText.setString(L"���� ����!");
//    ////gameText.setCharacterSize(60);
//    //gameText.setFillColor(sf::Color::White);
//    //gameText.setPosition(sf::Vector2f(500.f, 300.f));
//
//
//    // Ÿ��Ʋ �ؽ�Ʈ
//    sf::Text gameText(font, L"Ÿ�� ���", 50);  // L�� �ѱ� ���ͷ� ó��
//    gameText.setPosition(sf::Vector2f(200, 100));
//    gameText.setFillColor(sf::Color::Black);
//
//
//    // ����� ���� (�ѱ� ����)
//    std::wstring sentence = L"Ÿ�� ����� �����մϴ�!";  // �ѱ� ���� ���
//    sf::Text sentencetext(font, sentence, 30);
//    sentencetext.setPosition(sf::Vector2f(100, 200));
//    sentencetext.setFillColor(sf::Color::Black);
//
//    // �÷��̾� �Է� �ؽ�Ʈ (�ѱ� �Է� ó��)
//    std::wstring player1input, player2input;
//    sf::Text player1text(font, player1input, 30);
//    player1text.setPosition(sf::Vector2f(100, 300));
//    player1text.setFillColor(sf::Color::Red);
//
//    sf::Text player2text(font, player2input, 30);
//    player2text.setPosition(sf::Vector2f(100, 400));
//    player2text.setFillColor(sf::Color::Blue);
//
//    // ���ư��� ��ư
//    //sf::rectangleshape backbutton = createbutton(300, 500, 200, 50, sf::color::red);
//    //sf::text backtext(l"���ư���", font, 30);  // ���ư��� ��ư�� �ѱ۷� ǥ��
//    //backtext.setposition(340, 510);
//    //backtext.setfillcolor(sf::color::white);
//
//   // Ÿ���� ������ ���� ����
//    bool isplayer1turn = true;  // ���ʸ� �����ϴ� ����
//    bool gameover = false;
//
//    while (window.isOpen()) {
//        while (std::optional event = window.pollEvent())
//        {
//            if (event->is<sf::Event::Closed>())
//            {
//                window.close();
//            }
//
//            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
//            {
//                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
//                    window.close();
//            }
//        }
//    }
//    window.clear();
//    window.draw(gameText);
//    window.display();
//}


// �������� ��Ī �Ϸ� ���� ���� ������ ���
//void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font) {
//    // ������ non-blocking ���� ��ȯ
//    socket.setBlocking(false);
//    showLoadingScreen(window, font);
//
//    char buffer[256] = { 0 };
//    std::size_t received = 0;
//    bool matched = false;
//
//    while (window.isOpen() && !matched) {
//        while (std::optional event = window.pollEvent()) {
//            if (event->is<sf::Event::Closed>()) {
//                window.close();
//                return;
//            }
//        }
//        sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
//        if (status == sf::Socket::Status::Done) {
//            std::string message(buffer, received);
//            std::cout << "Received message: " << message << std::endl;
//            if (message.find("��Ī �Ϸ�! ���� ����!") != std::string::npos) {
//                matched = true;
//                break;
//            }
//        }
//        sf::sleep(sf::milliseconds(100));
//    }
//
//    if (matched) {
//        showCountdown(window, font);
//        runGame(window, font);
//    }
//    socket.setBlocking(true);
//}