#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "networkManager.h"
#include "gameManager.h"
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
    
    // windowâ 3.0 ����
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT)), L"���� ���� ȭ��");
    sf::Font font;
   
    NetworkManager client;
    GameManager game;
    
    Texture profileTexture, backGroundTexture, typingButtonTexture, matchButtonTexture, korButtonTexture;

    // ��Ʈ �ε� �����Ͻ� ���Ӽ� Ȯ���غ���
    if (!font.openFromFile("D2Coding.ttf")) {
        std::cerr << "��Ʈ �ε� ����!" << std::endl;
        return -1;
    }

    // ��� �̹��� �ε�
    if (!backGroundTexture.loadFromFile("./assets/image/bgimg.png")) {
        cerr << "��� �̹��� �ε� ����!" << endl;
        return -1;
    }

    // �̹��� �ε�
    if (!typingButtonTexture.loadFromFile("./assets/image/button.png")) {
        cerr << "Typing ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }
    if (!matchButtonTexture.loadFromFile("./assets/image/button.png")) {
        cerr << "Match ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }
    if (!korButtonTexture.loadFromFile("./assets/image/button.png")) {
        cerr << "koreng ��ư �̹��� �ε� ����!" << endl;
        return -1;
    }
    if (!profileTexture.loadFromFile("./assets/image/user.png")) {
        cerr << "������ �̹��� �ε� ����!" << endl;
        return -1;
    }

    // ������ Sprite
    Sprite profileSprite(profileTexture);
    Vector2u profileTextureSize = profileTexture.getSize();
    float scalexX = 50.f / profileTextureSize.x;  // ���� ũ�⸦ 50px�� ����
    float scaleyY = 50.f / profileTextureSize.y;   // ���� ũ�⸦ 50px�� ����
    profileSprite.setScale(Vector2(scalexX, scaleyY));
    //profileSprite.setTexture(profileTexture);
    profileSprite.setPosition(Vector2f(20.f, 20.f));  // ȭ�� ���� ��ܿ� ��ġ


    // ������ Text ���� (���� ���)
    Text profileText(font, "My Profile", 20);
    profileText.setPosition(Vector2f(100.f, 20.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    //profileText.setCharacterSize(30);
    profileText.setFillColor(Color::Black);

    // ��� sprite ����
    Sprite backGroundSprite(backGroundTexture);
    Vector2u backTextureSize = backGroundTexture.getSize();
    //backGroundSprite.setScale(Vector2(scaleX, scaleY));
    backGroundSprite.setTexture(backGroundTexture);
    backGroundSprite.setPosition(Vector2f(0.f, 0.f));

    // ������ ��� ����
    sf::RectangleShape whiteBackground(sf::Vector2f(800.f, 400.f));
    whiteBackground.setPosition({ 200.f, 150.f });
    whiteBackground.setFillColor(sf::Color(255, 255, 255, 200)); // ������ - ����
    whiteBackground.setOutlineThickness(1.f);
    whiteBackground.setOutlineColor(sf::Color(100, 100, 100)); // �׵θ� ����

    // �ѱ� Sprite ����
    Sprite korengButtonSprite(typingButtonTexture);
    Vector2u textureSize = typingButtonTexture.getSize();
    float scaleX = 50.f / textureSize.x;  // ���� ũ�⸦ 50px�� ����
    float scaleY = 50.f / textureSize.y;   // ���� ũ�⸦ 50px�� ����
    korengButtonSprite.setScale(Vector2(scaleX, scaleY));
    korengButtonSprite.setTexture(typingButtonTexture);
    korengButtonSprite.setPosition(Vector2f(320.f, 190.f));


    // �ѱ� Text ����
    Text korengButtonText(font, L"�ѱ� Ÿ�� ����");
    korengButtonText.setPosition(Vector2f(400.f, 200.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    korengButtonText.setCharacterSize(30);
    korengButtonText.setFillColor(Color::Black);

    // ���� Sprite ����
    Sprite typingButtonSprite(typingButtonTexture);
    //Vector2u textureSize = typingButtonTexture.getSize();
    //float scaleX = 100.f / textureSize.x;  // ���� ũ�⸦ 100px�� ����
    //float scaleY = 50.f / textureSize.y;   // ���� ũ�⸦ 50px�� ����
    typingButtonSprite.setScale(Vector2(scaleX, scaleY));
    //typingButtonSprite.setScale(Vector2f(0.5f, 0.5f));
    typingButtonSprite.setTexture(typingButtonTexture);
    typingButtonSprite.setPosition(Vector2f(320.f, 290.f)); // x,y

     
    // ���� Text ����
    Text typingButtonText(font, L"���� Ÿ�� ����");
    typingButtonText.setPosition(Vector2f(400.f, 300.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    typingButtonText.setCharacterSize(30);
    typingButtonText.setFillColor(Color::Black);

    // �ڵ� ��� Sprite ����
    Sprite matchButtonSprite(typingButtonTexture);
    //Vector2u textureSize = typingButtonTexture.getSize();
    //float scaleX = 100.f / textureSize.x;  // ���� ũ�⸦ 100px�� ���� 
    //float scaleY = 50.f / textureSize.y;   // ���� ũ�⸦ 50px�� ����
    matchButtonSprite.setScale(Vector2(scaleX, scaleY));
    //matchButtonSprite.setScale(Vector2f(0.5f, 0.5f));
    matchButtonSprite.setTexture(typingButtonTexture);
    matchButtonSprite.setPosition(Vector2f(320.f, 390.f));


    // �ڵ� ��� Text ����
    Text matchButtonText(font, L"�ڵ� ���");
    matchButtonText.setPosition(Vector2f(400.f, 400.f));  // �̹��� �����ʿ� �ؽ�Ʈ ��ġ
    matchButtonText.setCharacterSize(30);
    matchButtonText.setFillColor(Color::Black);


    // ��������Ʈ ����
    typingButtonSprite.setTexture(typingButtonTexture);
    matchButtonSprite.setTexture(matchButtonTexture);
    korengButtonSprite.setTexture(korButtonTexture);
    //profileSprite.setTexture(profileTexture);

    Text buttonText(font, L"���� ����", 40);
    buttonText.setPosition(Vector2f(300.f, 250.f));
    buttonText.setFillColor(Color::White);

    // ���� �Ķ��� ��ư
    //RectangleShape button(Vector2f(200, 80));
    //button.setPosition(Vector2f(300.f, 250.f));
    //button.setFillColor(Color::Blue);

    while (window.isOpen()) {
        // ���ο� ����� �̺�Ʈ ó��
        std::optional<Event> event;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // ���콺 Ŭ�� �� ��ư Ŭ�� Ȯ��
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (matchButtonSprite.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {
                    cout << "1:1 Match Button Clicked!" << endl;

                    if (!client.connectToServer()) {
                        return -1;
                    }

                    // �ε�â ���� ���� ������
                    bool isMatching = false;
                    std::thread loadingThread([&]() {
                        game.showLoadingScreen(window, font, isMatching);
                        std::cout << "�ε�â ����!!" << std::endl;
                    });

                    if (client.waitForMatch(window, font)) {
                        isMatching = true;
                        if (loadingThread.joinable()) loadingThread.join();
                        //loadingThread.join();
                        std::cout << "��ġ �Ϸ�Ǿ���!!" << std::endl;
                        //game.showLoadingScreen(window, font, isMatching);  // ��Ī ���� ī��Ʈ�ٿ� ����
                        //if (game.showCountdown(window, font)) {
                        std::cout << "���� ����!!" << std::endl;
                        game.runGame(window, font);
                        //}
                    }


                }
            }
        }

        // ȭ�� �׸���
        window.clear(Color::White);
        window.draw(backGroundSprite); // ��� ���� 
        window.draw(whiteBackground);  // �Ͼ�� ��� 
        window.draw(profileSprite);
        window.draw(profileText);    // ���� ��ܿ� profileText �׸���
        window.draw(typingButtonSprite);  // Typing ��ư �̹��� �׸���
        window.draw(typingButtonText);  // Typing Text �̹��� �׸���
        window.draw(matchButtonSprite);  // 1:1 Match ��ư �̹��� �׸���
        window.draw(matchButtonText);  // match Text �׸���
        window.draw(korengButtonSprite);  // �ѿ� ��ư �̹��� �׸���
        window.draw(korengButtonText);  // �ѿ� Text �̹��� �׸���
        //window.draw(button);
        //window.draw(buttonText);
        window.display();
    }
    return 0;
}