#pragma once
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "gameManager.h"
#include "FileLoader.hpp"
#include "TextUtil.hpp"
#include "Utils.h"
#include "TypingGame.hpp"
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <locale>
#include <codecvt>
#include <iostream>
#include <thread>
#include <filesystem> // ���� �ҷ�����
#include <random> // ����
#include <string>
#include <chrono>
#include <sstream>  // ���ڿ� ��Ʈ���� ����ϱ� ���� ���
#include <iomanip>  // �ð��� ���Ŀ� �°� ����ϱ� ���� ���
#include <SFML/Network.hpp>

int MAIN_WINDOW_WIDTH = 1280;
int MAIN_WINDOW_HEIGHT = 720;

using namespace sf;
using namespace std;
namespace fs = filesystem;

std::string folderPath = "./assets/file";

GameManager game;
TypingGame tpgame;
const int GWINDOW_WIDTH = 300;
const int GWINDOW_HEIGHT = 250;
const float ROTATION_SPEED = 60.f;  // ȸ�� �ӵ� (1�ʴ� n�� ȸ��)

FileLoader loader;
TextUtil util;


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
                wcout << L"gameWindow.close()" << endl;
                gameWindow.close();
                return;
            }
        }

        // ��Ʈ��ũ���� ��Ī ���� �� ī��Ʈ�ٿ� ���� �ð� ���� (����)
        // �����δ� ��Ʈ��ũ �ڵ忡�� �� �������� �����ؾ� �մϴ�
        if (isMatching && !serverTimeReceived) {
            // �����κ��� ī��Ʈ�ٿ� ���� �ð� ���� (����)
            // serverStartTime = �������� ���� �ð�
            wcout << L"ī��Ʈ�ٿ� ����!" << endl;
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
                wcout << L"����� ���� ����!!" << std::endl;
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


void GameManager::handleTypingGameMode(FileLoader& loader, TextUtil& util, TypingGame& typing, sf::RenderWindow& window, sf::Font& font, bool userGameManager) {

    // ��Ʈ �ε� �����Ͻ� ���Ӽ� Ȯ���غ���
    if (!font.openFromFile("D2Coding.ttf")) {
        wcerr << L"��Ʈ �ε� ����!" << endl;
        return;
    }

    // 1. ���� ���� �׽�Ʈ
    std::optional<std::wstring> path = loader.openFileDialog();
    if (!path) {
        std::wcout << L"[ERROR] ������ �������� �ʾҽ��ϴ�." << std::endl;
        system("pause");
        return;
    }

    std::wcout << L"[INFO] ���� ���õ�: " << path.value() << std::endl;

    //// 2. �ؽ�Ʈ �ε� �׽�Ʈ
    std::wstring content = loader.loadText(path.value());
    std::wcout << L"[INFO] ���Ͽ��� ���� ����:\n" << content << L"...\n\n";
    //std::wcout << L"[INFO] ���Ͽ��� ���� ����:\n" << content.substr(0, 200) << L"...\n\n";

    //// 3. �� ������ �׽�Ʈ
    std::vector<std::wstring> lines = util.splitStrtoVector(content);
    std::wcout << L"[INFO] �� ����: " << lines.size() << std::endl;

    for (int i = 0; i < lines.size(); ++i) {
        std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
    }
    /*for (int i = 0; i < std::min((size_t)5, lines.size()); ++i) {
        std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
    }*/

    // 4. �ٹٲ� �׽�Ʈ
    //sf::Font font;
    std::filesystem::current_path("C:/Project/gameProject/typingGame");
    if (!font.openFromFile("assets/font/D2Coding.ttf")) {
        std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
        return;
    }

    auto wrapped = util.wrapAllLinesToPixelWidth(lines, font, 30, 1280.f);
    std::wcout << L"\n[INFO] �ٹٲ� ��� (�ִ� 3��):" << std::endl;
    /*for (int i = 0; i < std::min((size_t)3, wrapped.size()); ++i) {
        for (const auto& sub : wrapped[i]) {
            std::wcout << L"  �� " << sub << std::endl;
        }
    }*/

    for (int i = 0; i < wrapped.size(); ++i) {
        for (const auto& sub : wrapped[i]) {
            std::wcout << L"  �� " << sub << std::endl;
        }
    }

    if (userGameManager) {
        game.runGame2(window, font, 18, wrapped);
    }
    else {
        typing.startNewGame(window, font, 18, wrapped);
    }

    //typing.startNewGame(window, font, 18, wrapped);
    system("pause");
}


void GameManager::runGame2(sf::RenderWindow& window, sf::Font& font, int fontSize,
    std::vector<std::vector<std::wstring>>& sentences) {


    // ������ ������ �� ���� ��
    size_t numSentences = sentences.size();                   // ��ü ���� ����
    size_t numLinesInLastSentence = sentences.back().size();  // ������ ������ �� ��
    size_t totalLines = 0;
    size_t correctLineCount = 0;


    // ��� ������ �� �� ����
    for (const auto& sentence : sentences) {
        totalLines += sentence.size();
    }
    std::wcout << L"currentSentenceIdx" << numSentences << std::endl;
    std::wcout << L"currentSentenceIdx" << numLinesInLastSentence << std::endl;

    // �α� ��� �� ���߿� ����
    std::wstring target = sentences[numSentences - 1].back(); // ������ ���� ��
    for (size_t i = 0; i < target.size(); ++i) {
        std::wcout << L"[" << i << L"]: '" << target[i] << L"' (int: " << (int)target[i] << L")\n";
    }
    int currentSentenceIdx = 0;
    int currentLineIdx = 0;

    // ���� ���� �ð� ���
    auto startTime = std::chrono::high_resolution_clock::now();  // ���� ���� �ð� ���

    // userInput �ʱ�ȭ
    std::vector<std::vector<std::wstring>> userInputs;
    for (const auto& sentenceLine : sentences) {
        std::vector<std::wstring> inputLine(sentenceLine.size(), L"");
        userInputs.push_back(inputLine);
    }

    sf::Text gameText(font, L"���� ����~~!", 60);

    gameText.setFillColor(Color::Red);
    gameText.setPosition(Vector2f(480.f, 50.f)); // (���� -> ������, �� -> �Ʒ�)

    //------------------------------------Ÿ�� �Է�â �����------------------------------------

    // ���� ���� ǥ��
    Text currentSentence(font, L"", 50);
    FloatRect textBounds = currentSentence.getGlobalBounds();
    currentSentence.setOrigin(textBounds.getCenter());
    currentSentence.setFillColor(Color::Black);
    currentSentence.setPosition(Vector2f(1280.f / 2, 300.f)); // ��� ����

    // �Է� ǥ��
    Text userInputText(font, L"", 30);
    // userInputText�� �߾� ������ �ǵ��� ����
    userInputText.setFillColor(Color::Green);

    FloatRect userInputBounds = userInputText.getGlobalBounds();

    // SFML 3.0 ���: getCenter() ���
    sf::Vector2f center = userInputBounds.getCenter();
    // �߽������� ���� ����
    userInputText.setOrigin(center);
    userInputText.setPosition(Vector2f(1280.f / 2, 500.f)); // �Է�â ��ġ�� ���� ���� ��ġ��Ŵ

    //--------------------------------------------------------------------------------------------

    // ��踦 ���� ���� ���� 
    double totalWpm = 0.0;          // �� WPM (�ܾ�� ��)
    double totalAccuracy = 0.0;     // �� ��Ȯ��
    double totalTime = 0.0;         // �� �ð�
    double totalTpm = 0;
    int totalKeyPress = 0;          // Ű �Է� Ƚ��
    int backspacePressCount = 0;

    while (window.isOpen()) {
        bool isWindowClosed = false;

        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                isWindowClosed = true;
                break;
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    isWindowClosed = true;
                    break;
                }
            }
            // �ؽ�Ʈ �Է� ó��
            // Ű���� �Է� ó��
            // Ű �Է�
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                wchar_t c = static_cast<wchar_t>(textEvent->unicode);   // �Էµ� ���ڸ� ������
                totalKeyPress++;
                std::wstring& currentSentence = userInputs[currentSentenceIdx][currentLineIdx];
                std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                if (c == 8) {  // �齺���̽� ó��
                    backspacePressCount++;
                    if (!currentInput.empty()) {
                        currentInput.pop_back();  // �Էµ� ���ڿ��� ������ ���� ����
                    }
                }
                // �� ���� ������ ġ�� �Է� �ȵǵ��� ����
                if (currentInput.length() != sentences[currentSentenceIdx][currentLineIdx].length())
                {
                    if (iswprint(c)) {  // ��� ������ ������ ���
                        currentInput += c;  // �Է� �ؽ�Ʈ�� ���� �߰�
                    }
                }

                // �Է� �ؽ�Ʈ�� ����Ǿ����Ƿ� ��ġ�� ������Ʈ
                userInputText.setString(currentInput);
                FloatRect updatedBounds = userInputText.getGlobalBounds();
                Vector2f center2 = updatedBounds.getCenter();
                userInputText.setOrigin(center2);

            }
            // ���� �Է�
            // ���ٸ��� ��Ȯ��, �ӵ� ����
            // ���� ��Ȯ���� ������ �Ѿ�� �ʵ���
            // ������ �� �϶� ���� ������ ��� â �������� 
            // Ʋ���� ���� �۽ô� �̰� �� �ϰ� ����, �����̴� �Ŷ�
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {

                    // ���⿡ ���� ó�� ����
                    std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                    // ����� ����� �ùٸ� ���� ���� ���� �Է��� �� �ֵ��� ����
                    if (currentInput == sentences[currentSentenceIdx][currentLineIdx])
                    {
                        TypingStats stats = tpgame.updateStats(startTime, sentences[currentSentenceIdx][currentLineIdx],
                            currentInput, backspacePressCount, totalKeyPress);


                        totalWpm += stats.wpm;
                        totalAccuracy += stats.accuracy;
                        totalTime += stats.elapsedTime;
                        totalTpm += stats.tmp;
                        std::wcout << L"totalTpm" << totalTpm << std::endl;
                        correctLineCount++;
                        currentLineIdx++;
                        if (currentLineIdx >= sentences[currentSentenceIdx].size())
                        {
                            // ª�� ������ ��� ���� �������� �̵�
                            currentLineIdx = 0;
                            currentSentenceIdx++;

                            // ��ü ������ �� ���� �Է����� ���
                            if (currentSentenceIdx >= sentences.size())
                            {
                                std::wcout << L"������ �Է���: ���â ���" << currentSentenceIdx << std::endl;
                                auto endTime = std::chrono::high_resolution_clock::now();  // ���� �ð� ���

                                // ��ü ���� ���� ���â ���
                                tpgame.showResultWindow(window, font, fontSize, totalWpm / totalLines,
                                    totalAccuracy / totalLines, totalTime, totalTpm / totalLines, sentences);
                                return;
                            }
                            nextLine();
                        }

                        // ���� �������� �Ѿ���Ƿ� �Է� �ؽ�Ʈ�� �ʱ�ȭ�ϰ� ��ġ �缳��
                        userInputText.setString(L"");
                        FloatRect updatedBounds = userInputText.getGlobalBounds();
                        Vector2f center2 = updatedBounds.getCenter();
                        userInputText.setOrigin(center2);

                    }
                    else
                    {

                        // �α� ��¿� ���߿� ����
                        std::wcout << L"����" << std::endl;
                        std::wcout << L"currentInput:    " << currentInput << std::endl;
                        std::wcout << L"sentences:    " << sentences[currentSentenceIdx][currentLineIdx] << std::endl;
                        std::wcout << L"���� ���ڼ�    " << currentInput.size() << std::endl;
                        std::wcout << L"���� ���ڼ�    " << sentences[currentSentenceIdx][currentLineIdx].size() << std::endl;

                    }
                }

            }

        }

        if (isWindowClosed) {
            break;  // â�� ������ �ݺ����� �����Ͽ� ������ ����
        }

        window.clear(Color::White);
        window.draw(gameText);

        // ���� ���� ǥ�� ������Ʈ
        currentSentence.setString(sentences[currentSentenceIdx][currentLineIdx]);
        FloatRect updatedTextBounds = currentSentence.getGlobalBounds();

        Vector2f center3 = updatedTextBounds.getCenter();
        currentSentence.setOrigin(center3);

        // �Է� �ؽ�Ʈ ������Ʈ
        userInputText.setString(userInputs[currentSentenceIdx][currentLineIdx]);

        drawOriginTotalText(window, font, fontSize, sentences); // ��ü ���� ���
        drawOriginText(window, font, fontSize, sentences); // ���� ���
        drawUserInputText(window, font, fontSize, userInputs, sentences); // ����� �Է� ���

        window.display();
    }

}

// ���� ��ä�� ���
void GameManager::drawOriginTotalText(sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& fileAllLines) {

    // section 10���� ������
    const float sectionWidth = MAIN_WINDOW_WIDTH / 10.f; 
    const float sectionHeight = MAIN_WINDOW_HEIGHT / 4.f;


    // === ������: ���� 10��� ���� �ð�ȭ ===
    //for (int i = 0; i < 10; ++i) {
    //    sf::RectangleShape rect(sf::Vector2f(sectionWidth, MAIN_WINDOW_HEIGHT));
    //    rect.setPosition({ i * sectionWidth, 0 });

    //    // ������ �������� �ٸ��� ����
    //    switch (i % 4) {
    //    case 0: rect.setFillColor(sf::Color(255, 0, 0, 40)); break;     // ����
    //    case 1: rect.setFillColor(sf::Color(0, 255, 0, 40)); break;     // �ʷ�
    //    case 2: rect.setFillColor(sf::Color(0, 0, 255, 40)); break;     // �Ķ�
    //    case 3: rect.setFillColor(sf::Color(255, 255, 0, 40)); break;   // ���
    //    }

    //    window.draw(rect);
    //}

    // ���� ���� ��ġ (ȭ�� 10/2���� ����)
    float x = sectionWidth * 1.5;
    float y = sectionHeight * 1.5; // y�� �����ؼ� �� ���� �ֱ�

    // �ؽ�Ʈ ������ �ʺ� (10/3 ũ��� ����)
    float maxWidth = sectionWidth * 3;

    // �� ���ھ� �Է�
    // �ѱ� / �����϶� ��������� �� drawUserInputText�� ����
    float letterSpacing = fontSize * 0.50f; // ����
    //float letterSpacing = fontSize * 0.90f; // �ѱ�

    // ��� ũ�� ����� ���� ����
    float maxTextWidth = 0.f;  // ���� �� �ؽ�Ʈ�� �ʺ�
    float totalTextHeight = 0.f; // ��ü �ؽ�Ʈ ����

    // �ؽ�Ʈ ũ�� ����
    for (int i = 0; i < fileAllLines.size(); ++i) {
        for (size_t j = 0; j < fileAllLines[i].size(); ++j) {
            sf::Text text(font, fileAllLines[i][j], fontSize);
            float textWidth = text.getLocalBounds().size.x;
            maxTextWidth = std::max(maxTextWidth, textWidth);
            totalTextHeight += fontSize * 2.0f; // �� ���� ����
        }
    }
    // ��� ��� (�͹̳ο��� Ȯ��)
    //std::cout << "maxTextWidth: " << maxTextWidth << std::endl; //477

    // ��� ũ�� ���� (�ؽ�Ʈ ũ�⺸�� ���� ���� �߰�)
    float padding = 20.f;  // ���� �߰�
    float backgroundWidth = maxTextWidth + padding ;  // ���� �� �ؽ�Ʈ ����
    float backgroundHeight = totalTextHeight + padding;  // ��ü ����


    // ��� �ڽ� ����
    sf::RectangleShape OriginBackground(sf::Vector2f(backgroundWidth, backgroundHeight));
    OriginBackground.setFillColor(sf::Color(224, 224, 224, 200)); // ������ ȸ��
    OriginBackground.setPosition(Vector2f(x - padding, y - padding)); // ���� ����

    window.draw(OriginBackground); // ��� ���� �׸�

    for (int i = 0; i < fileAllLines.size(); ++i)
    {
        for (size_t j = 0; j < fileAllLines[i].size(); ++j)
        {
            // ���� ���� ���
            sf::Text originaTotalText(font, fileAllLines[i][j], fontSize);
            originaTotalText.setFillColor(sf::Color::Black);
            originaTotalText.setPosition({ x, y });
            // �ؽ�Ʈ �ʺ� maxWidth�� �ʰ��ϴ� ��츦 ����� ������ ���� (������)
            float textWidth = originaTotalText.getLocalBounds().size.x;

            if (textWidth > maxWidth) {
                float scale = maxWidth / textWidth;
                originaTotalText.setScale(Vector2f(scale, 1.0f));
            }
            window.draw(originaTotalText);
            y += fontSize * 2.0f ; // �� ���� ����
        }
    }

    // �ѱ��� ���
    //for (int i = 0; i < fileAllLines.size(); ++i)
    //{
    //    for (size_t j = 0; j < fileAllLines[i].size(); ++j)
    //    {
    //        std::wstring& Line = fileAllLines[i][j];

    //        for (size_t k = 0; k < Line.size(); ++k)
    //        {
    //            sf::Text oneChar(font, Line.substr(k, 1), fontSize);

    //            oneChar.setFillColor(sf::Color::Black);

    //            oneChar.setPosition({ x + k * letterSpacing, y + fontSize + 10.f });


    //            window.draw(oneChar);
    //        }

    //        //sf::Text inputText(font, userInputs[i][j], fontSize);
    //        //inputText.setFillColor(correct ? sf::Color::Green : sf::Color::Red);  // ��Ȯ�ϸ� Green, �ƴϸ� Red
    //        ////inputText.setFillColor(sf::Color::Green);
    //        //inputText.setPosition({ x, y + fontSize + 10.f });

    //        //window.draw(inputText);


    //        y += fontSize * 3; // �� ���� ����

    //    }
    //}

}

// ���� TypingGame�� drawOriginText
// ui ���� �Լ� - ���� ����
void GameManager::drawOriginText(
    sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& fileAllLines)
{
    if (currentLineIndex >= fileAllLines.size()) return; // ��� �� ��� �Ϸ�
    const float sectionWidth = MAIN_WINDOW_WIDTH / 10.f; // = 72.0f
    const float sectionHeight = MAIN_WINDOW_HEIGHT / 4.f;

    //float x = 200.f;
    float x = sectionWidth * 7.5f; // ������ �߾�
    // ȭ���� 1/6 �������� ���� (4/6 ~ 5/6)
    float areaWidth = sectionWidth * 3;
    float y = sectionHeight * 1.5; // y�� �����ؼ� �� ���� �ֱ�
    
    //�� ���ھ� �Է�
    //�ѱ� / �����϶� ��������� �� drawUserInputText�� ����
    float letterSpacing = fontSize * 0.50f; // ����
    //float letterSpacing = fontSize * 0.90f; // �ѱ�

    // ���� ���� ���
    if (currentLineIndex < fileAllLines.size()) {
        for (size_t j = 0; j < fileAllLines[currentLineIndex].size(); ++j)
        {
            // ���� ���� ���
            sf::Text currentSentence(font, fileAllLines[currentLineIndex][0], fontSize);
            currentSentence.setFillColor(sf::Color::Black);
            FloatRect textBounds = currentSentence.getGlobalBounds();
            currentSentence.setOrigin(textBounds.getCenter());
            currentSentence.setPosition({ x, y });

            // �ؽ�Ʈ �ʺ� �Ҵ�� ������ �ʰ��ϸ� ������ ����
            if (textBounds.size.x > areaWidth) {
                float scale = areaWidth / textBounds.size.x;
                currentSentence.setScale(Vector2f(scale, 1.0f));
            }
            window.draw(currentSentence);
        }
    }
    // ���� ���� ��� (���� �� �Ʒ�)
    if (currentLineIndex + 1 < fileAllLines.size()) { // ���� ������ ���� ���� ���
        //float nextLineY = y + fontSize * 2; // �� ���� �߰�
        for (size_t j = 0; j < fileAllLines[currentLineIndex + 1].size(); ++j)
        {

            // ������ ��� ����
            sf::RectangleShape Background(sf::Vector2f(250.f, 55.f));
            // ��� ũ�� ��������
            FloatRect bgBounds = Background.getGlobalBounds();
            Background.setOrigin(bgBounds.getCenter()); // �߾� ���� ����

            Background.setFillColor(sf::Color(96, 96, 96, 200)); // ������ - ����
            Background.setPosition(Vector2f(x, y + 100.f));
             
            sf::Text nextSentence(font, fileAllLines[currentLineIndex + 1][0], fontSize / 2);
            nextSentence.setFillColor(sf::Color(32, 32, 32)); // ��ο� ȸ��
            FloatRect nextTextBounds = nextSentence.getGlobalBounds();
            nextSentence.setOrigin(nextTextBounds.getCenter());
            nextSentence.setPosition({ x, y + 100.f }); // ���� ���� �Ʒ� ��ġ
            // �ؽ�Ʈ �ʺ� �Ҵ�� ������ �ʰ��ϸ� ������ ����
            if (nextTextBounds.size.x > areaWidth) {
                float scale = areaWidth / nextTextBounds.size.x;
                nextSentence.setScale(Vector2f(scale, 1.0f));
            }
			window.draw(Background); // ��� �׸���
            window.draw(nextSentence);
        }
    }
}

void GameManager::nextLine() {
    currentLineIndex++; // ���� �ٷ� �̵�
}

bool userInputCompleted(const std::wstring& userInput, const std::wstring& correctAnswer) {
    return userInput == correctAnswer;
}

// ui ���� �Լ� - �Է� ����
void GameManager::drawUserInputText(
    sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& userInputs,
    std::vector<std::vector<std::wstring>>& sentences)
{
    if (currentLineIndex >= userInputs.size()) return; // ��� �Է��� ������ ����
    const float sectionWidth = MAIN_WINDOW_WIDTH / 10.f; // = 72.0f
    const float sectionHeight = MAIN_WINDOW_HEIGHT / 4.f;
    //float x = 200.f;

    float x = sectionWidth * 7.5f;
    float areaWidth = sectionWidth * 4;
    float y = sectionHeight * 3; // y�� �����ؼ� �� ���� �ֱ�

    // �� ���ھ� �Է�
    // �ѱ� / �����϶� ��������� �� drawOriginalText�� ����
    float letterSpacing = fontSize * 0.50f; // ����
    //float letterSpacing = fontSize * 0.90f; // �ѱ�

    std::wstring& inputLine = userInputs[currentLineIndex][0];
    std::wstring& targetLine = sentences[currentLineIndex][0];  // ���� ����

    //// �Է�â ��� ����
    sf::RectangleShape inputBackground(sf::Vector2f(500.f, 40.f)); // �Է�â ũ��
    inputBackground.setFillColor(sf::Color(240, 240, 240)); // ���� ȸ�� ���
    inputBackground.setOutlineThickness(2.f);
    inputBackground.setOutlineColor(sf::Color(100, 100, 100)); // �׵θ� ����

    FloatRect inputTextBounds = inputBackground.getGlobalBounds();
    inputBackground.setOrigin(inputTextBounds.getCenter());
    inputBackground.setPosition({ x , y });

    window.draw(inputBackground);

    // �Է� �ؽ�Ʈ�� �ʹ� ��� ȭ�鿡 �°� ������ ������ ���
    float maxChars = areaWidth / letterSpacing;
    float scale = 1.0f;
    if (inputLine.size() > maxChars) {
        scale = maxChars / static_cast<float>(inputLine.size());
    }

    // �Է� �ؽ�Ʈ ���
    for (size_t k = 0; k < inputLine.size(); ++k)
    {
        Text inputText(font, inputLine.substr(k, 1), fontSize) ;

        if (k < targetLine.size() && inputLine[k] == targetLine[k]) {
            inputText.setFillColor(sf::Color::Green); // ������ �ʷϻ�
        }
        else {
            inputText.setFillColor(sf::Color::Red); // Ʋ���� ������
        }
        // **�ؽ�Ʈ�� ������ ���� ������� ����**
        inputText.setOrigin(Vector2f(0.0f, 0.0f));

        // **�Է�â �ȿ��� �� ���ھ� ��ġ**
        float textX = x - (inputLine.size() * letterSpacing * scale) / 2 + k * letterSpacing * scale;
        //float textX = x - (inputLine.size() * letterSpacing) / 2 + k * letterSpacing;
        float textY = y - fontSize / 2; // �Է�â �߾ӿ� ������ ����
        inputText.setPosition(Vector2f(textX, textY));
        inputText.setScale(Vector2f(scale, scale)); // �ʿ�� ������ ����

        window.draw(inputText);
    }
    //// Ŀ�� �߰� (�����̱� ���)
    static sf::Clock cursorClock;
    static bool showCursor = true;

    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }

    if (showCursor) {
        sf::RectangleShape cursor(sf::Vector2f(2.f, fontSize));
        cursor.setFillColor(sf::Color::Black);

        float inputCenterX = inputBackground.getPosition().x;

        //float cursorX = x - (inputLine.size() * letterSpacing) / 2 + inputLine.size() * letterSpacing + 3.f;
        float cursorX = x - (inputLine.size() * letterSpacing * scale) / 2 + inputLine.size() * letterSpacing * scale + 3.f;

        //float cursorX = inputCenterX - (inputLine.size() * letterSpacing) / 2 + inputLine.size() * letterSpacing + 3.f;
        float cursorY = y - fontSize / 2;
        cursor.setPosition(Vector2f(cursorX, cursorY));

        window.draw(cursor);
    }
}
