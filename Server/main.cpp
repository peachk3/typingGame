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

    //_setmode(_fileno(stdout), _O_U16TEXT);  // UTF-16 ��� Ȱ��ȭ
    //_setmode(_fileno(stderr), _O_U16TEXT);  // ���� ��µ� UTF-16 ��� ����
    // �̰� ���� cout 
    //std::wcout << L"�����ڵ� ��� �׽�Ʈ: �ȳ��ϼ���! " << std::endl;
    //std::wcerr << L"��Ʈ �ε� ����!" << std::endl;  // ���� �޽��� ���


    // windowâ 3.0 ����
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"���� ���� ȭ��");
    sf::Font font;

    NetworkManager client;
    GameManager game;
    FileLoader loader;
    TextUtil util;
    TypingGame typing;
    //handleTpMode typinghandler;

    // ������ , ���, �ѱ�, ����, �ڵ�, ��Ʋ
    Texture profileTexture, backGroundTexture, korButtonTexture, engButtonTexture, codeButtonTexture, matchButtonTexture,
        codeJavaButtonTexture, codeCppButtonTexture, codePyButtonTexture, fileSelectTexture;

    // ��Ʈ �ε� �����Ͻ� ���Ӽ� Ȯ���غ���
    if (!font.openFromFile("D2Coding.ttf")) {
        wcerr << L"��Ʈ �ε� ����!" << endl;
        return -1;
    }

    // ��� �̹��� �ε�
    if (!backGroundTexture.loadFromFile("./assets/image/bgimg.png")) {
        wcerr << L"��� �̹��� �ε� ����!" << endl;
        return -1;
    }

    // ��� sprite ����
    Sprite backGroundSprite(backGroundTexture);
    Vector2u backTextureSize = backGroundTexture.getSize();
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
        wcerr << L"������ �̹��� �ε� ����!" << endl;
        return -1;
    }

    // ������ Sprite
    Sprite profileSprite(profileTexture);
    Vector2u profileTextureSize = profileTexture.getSize();
    float scalexX = 50.f / profileTextureSize.x;  // ���� ũ�⸦ 50px�� ����
    float scaleyY = 50.f / profileTextureSize.y;   // ���� ũ�⸦ 50px�� ����
    profileSprite.setScale(Vector2(scalexX, scaleyY));
    profileSprite.setPosition(Vector2f(20.f, 20.f));  // ȭ�� ���� ��ܿ� ��ġ

    // ������ Text ���� (���� ���)
    Text profileText(font, "My Profile", 20);
    profileText.setPosition(Vector2f(100.f, 20.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    profileText.setFillColor(Color::Black);

    // �ѱ� ��� �̹��� �ε�
    if (!korButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"koreng ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }

    // �ѱ� Sprite ����
    Sprite korButtonSprite(korButtonTexture);
    Vector2u textureSize = korButtonTexture.getSize();
    float scaleX = 50.f / textureSize.x;  // ���� ũ�⸦ 50px�� ����
    float scaleY = 50.f / textureSize.y;   // ���� ũ�⸦ 50px�� ����
    korButtonSprite.setScale(Vector2(scaleX, scaleY));
    korButtonSprite.setTexture(korButtonTexture);
    korButtonSprite.setPosition(Vector2f(320.f, 190.f));


    // �ѱ� Text ����
    Text korButtonText(font, L"�ѱ� Ÿ�� ����");
    korButtonText.setPosition(Vector2f(400.f, 200.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    korButtonText.setCharacterSize(30);
    korButtonText.setFillColor(Color::Black);

    // ���� ��� �̹��� �ε�
    if (!engButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"Typing ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }

    // ���� Sprite ����
    Sprite engButtonSprite(engButtonTexture);
    engButtonSprite.setScale(Vector2(scaleX, scaleY));
    engButtonSprite.setTexture(engButtonTexture);
    engButtonSprite.setPosition(Vector2f(320.f, 290.f)); // x,y


    // ���� Text ����
    Text engButtonText(font, L"���� Ÿ�� ����");
    engButtonText.setPosition(Vector2f(400.f, 300.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    engButtonText.setCharacterSize(30);
    engButtonText.setFillColor(Color::Black);

    // �ڵ� ��� �̹��� �ε�
    if (!codeButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"Typing ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }

    // �ڵ� ��� Sprite ����
    Sprite codeButtionSprite(codeButtonTexture);
    codeButtionSprite.setScale(Vector2(scaleX, scaleY));
    //typingButtonSprite.setScale(Vector2f(0.5f, 0.5f));
    codeButtionSprite.setTexture(engButtonTexture);
    codeButtionSprite.setPosition(Vector2f(320.f, 390.f)); // x,y

    // �ڵ� ��� Text ����
    Text codeButtonText(font, L"�ڵ� ��� ����");
    codeButtonText.setPosition(Vector2f(400.f, 400.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    codeButtonText.setCharacterSize(30);
    codeButtonText.setFillColor(Color::Black);

    // ��Ʋ ��� �̹��� �ε�
    if (!matchButtonTexture.loadFromFile("./assets/image/button.png")) {
        wcerr << L"Match ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }

    // ��Ʋ Sprite ����
    Sprite matchButtonSprite(matchButtonTexture);
    matchButtonSprite.setScale(Vector2(scaleX, scaleY));
    matchButtonSprite.setTexture(matchButtonTexture);
    matchButtonSprite.setPosition(Vector2f(320.f, 490.f));


    // ��Ʋ Text ����
    Text matchButtonText(font, L"�ڵ� ���");
    matchButtonText.setPosition(Vector2f(400.f, 500.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    matchButtonText.setCharacterSize(30);
    matchButtonText.setFillColor(Color::Black);

    // �ڵ� ��� ��ư �̹��� �ε�
    if (!codeJavaButtonTexture.loadFromFile("./assets/image/blueButton.png")) {
        wcerr << L"�ڵ� ��� ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }

    if (!codeCppButtonTexture.loadFromFile("./assets/image/blueButton2.png")) {
        wcerr << L"�ڵ� ��� ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }

    if (!codePyButtonTexture.loadFromFile("./assets/image/blueButton3.png")) {
        wcerr << L"�ڵ� ��� ��ư �̹��� �ε� ����!" << endl;
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

    // ��ư�� �ؽ�Ʈ ��ġ ����
    sf::FloatRect cppbuttonBounds = codeCppButtonSprite.getGlobalBounds();
    sf::FloatRect cpptextBounds = cppButtonText.getGlobalBounds();

    // ��ư �߾ӿ� �ؽ�Ʈ ��ġ
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

    // ��ư�� �ؽ�Ʈ ��ġ ����
    sf::FloatRect pybuttonBounds = codePyButtonSprite.getGlobalBounds();
    sf::FloatRect pytextBounds = pyButtonText.getGlobalBounds();

    // ��ư �߾ӿ� �ؽ�Ʈ ��ġ
    pyButtonText.setPosition(Vector2
    (pybuttonBounds.position.x + (pybuttonBounds.size.x / 2) - (pytextBounds.size.x / 2),
        pybuttonBounds.position.y + (pybuttonBounds.size.y / 2) - (pytextBounds.size.y / 2))
    );

    // ���� �ҷ�����

    // ���� �ҷ����� �̹��� �ε�
    if (!fileSelectTexture.loadFromFile("./assets/image/bluebutton4.png")) {
        wcerr << L"Typing ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }

    Sprite fileSelectSprite(fileSelectTexture);
	fileSelectSprite.setTexture(fileSelectTexture);
	fileSelectSprite.setScale(sf::Vector2(0.5f, 0.5f));
	fileSelectSprite.setPosition(sf::Vector2f(1200.f, 600.f));

	Text fileSelectText(font, L"���� ����", 20);
	fileSelectText.setFillColor(Color::Black); 
    fileSelectText.setPosition(sf::Vector2f(1200.f, 600.f));


    bool isCodingMode = false; // �ڵ� ��� ���� Ȯ��
    std::string folderPath = "./assets/file";

    while (window.isOpen()) {
        // ���ο� ����� �̺�Ʈ ó��
        //std::optional<Event> event;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // ���콺 Ŭ�� �� ��ư Ŭ�� Ȯ��
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(window);
                //sf::Vector2f worldPos(mousePos.x, mousePos.y);

                if (!isCodingMode) {
                    std::wcout << L"�ڵ� ��� �ƴ� " << std::endl;

                    // �ѱ� Ÿ�� ����
                    if (korButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {

                        std::wcout << L"�ѱ� Ÿ�� ���� ����!" << endl;
                        //typing.startNewGame(window, font, 18, wrapped);
                        // �Լ� ����
                        try {
                            game.handleTypingGameMode(loader, util, typing, window, font, false);
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

                        // �Լ� ����
                        //game.handleTypingMode(loader, util, typing, window, font);
                        game.handleTypingGameMode(loader, util, typing, window, font, false);


                    }

                    else if (codeButtionSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        isCodingMode = true;

                    }

                    // ��ġ
                    else if (matchButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        wcout << L"1:1 Match Button Clicked!" << endl;

                        if (!client.connectToServer()) {
                            return -1;
                        }

                        // �ε�â ���� ���� ������
                        bool isMatching = false;
                        std::thread loadingThread([&]() {
                            game.showLoadingScreen(window, font, isMatching);
                            wcout << L"�ε�â ����!!" << endl;
                            });

                        if (client.waitForMatch(window, font)) {
                            isMatching = true;
                            if (loadingThread.joinable()) loadingThread.join();
                            //loadingThread.join();
                            wcout << L"��ġ �Ϸ�Ǿ���!!" << endl;
                            //game.showLoadingScreen(window, font, isMatching);  // ��Ī ���� ī��Ʈ�ٿ� ����
                            //if (game.showCountdown(window, font)) {
                            wcout << L"���� ����!!" << endl;
                            // �ϴ� ���� �ҷ��ͼ� �����ϴ� �ɷ� �����ϰ� ����
                            // -- ���� �ʿ�
                            //game.handleTypingGameMode(loader, util, typing, window, font, true);
                            //game.runGame(window, font); 

                            std::string filePath;

                            //wcout << L"���� ����â �̵� �Ϸ�!!" << std::endl;
                            //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

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
                                    game.runGame2(window, font, 24, sentences);
                                }
                            }

                        }

                    }
                }
                // �ڵ����
                else {
                    std::wcout << L"�ڵ� ��� ����!" << std::endl;
                    std::string filePath;
                    // java
                    if (codeJavaButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        filePath = getRandomFile(folderPath, ".java");
                        std::cout << "Selected file: " << filePath << std::endl;
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
                                typing.startNewGame(window, font, 24, sentences);
                            }
                        }
                    }
                    // C++
                    else if (codeCppButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        std::cout << "[�����] C++ ��ư�� Ŭ����!" << std::endl;
                        filePath = getRandomFile(folderPath, ".cpp");
                        std::cout << "[�����] getRandomFile() ȣ�� �Ϸ�" << std::endl;
                        //std::cout << "Selected file: " << filePath << std::endl;
                        // ���� ��� ����� ���
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
                                typing.startNewGame(window, font, 24, sentences);
                            }
                        }

                    }
                    // ���̽� 
                    else if (codePyButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        filePath = getRandomFile(folderPath, ".py");
                        std::cout << "Selected file: " << filePath << std::endl;

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
                                typing.startNewGame(window, font, 24, sentences);
                            }
                        }
                    }
                    // ���� ����
                    else if (fileSelectSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                        //game.handleTypingMode(loader, util, typing, window, font);
                        game.handleTypingGameMode(loader, util, typing, window, font, false);

                    }
                    //if (!filePath.empty()) {
                    //    std::filesystem::path absPath = std::filesystem::absolute(filePath);
                    //    std::cout << "���� ���� ���: " << absPath << std::endl;

                    //    auto sentences = typing.loadSentencesFromFile(filePath);
                    //    if (sentences.empty()) {
                    //        std::wcout << L"Error: ������ �ҷ����� ���߽��ϴ�!" << std::endl;
                    //    }
                    //    else {
                    //        typing.startNewGame(window, font, 24, sentences);
                    //        isCodingMode = false; // ���� ���� �� �ٽ� �Ϲ� ���� ��ȯ
                    //    }
                    //}

                }
            }
        }

        // ȭ�� �׸���
        window.clear(Color::White);
        if (!isCodingMode) {
            window.draw(backGroundSprite); // ��� ���� 
            window.draw(whiteBackground);  // �Ͼ�� ��� 
            window.draw(profileSprite);
            window.draw(profileText);    // ���� ��ܿ� profileText �׸���

            window.draw(korButtonSprite);  // �ѱ� ��ư �̹��� �׸���
            window.draw(korButtonText);     // �ѱ� Text �̹��� �׸���

            window.draw(engButtonSprite);  // ���� ��ư �̹��� �׸���
            window.draw(engButtonText);  // ���� Text �̹��� �׸���

            window.draw(codeButtionSprite); // �ڵ���� ��ư �̹��� �׸���
            window.draw(codeButtonText);      // �ڵ���� Text �̹��� �׸���

            window.draw(matchButtonSprite);  // 1:1 Match ��ư �̹��� �׸���
            window.draw(matchButtonText);  // match Text �׸���
            //window.draw(button);
            //window.draw(buttonText);
        }
        else {
            // �ڵ� ��� ȭ�� �׸���
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