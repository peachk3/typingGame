#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <iostream>
#include <memory>
#include <filesystem>

#include "Settings.hpp"
#include "SettingsUI.hpp"
#include "SettingsIcon.hpp"
//#include "TypingUI.hpp"

using namespace std;

int main() {
    // ���� ��ü ���� �� �ʱ�ȭ
    Settings settings;

    if (settings.loadSettings("settings.ini")) {
        std::cout << "�������--> ����� ������ �ҷ��Խ��ϴ�" << std::endl;
    }
    else {
        std::cout << "�⺻ ������ ����մϴ�" << std::endl;
    }

    // ���� â ����
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), L"Ÿ�ڿ���");

    // ��Ʈ �ε�
    sf::Font font;
    if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
        std::cout << "��Ʈ �ε� ����" << std::endl;
        return -1;
    }
    std::cout << "��Ʈ �ε� ����" << std::endl;

    // ����� �ε� �� ����
    sf::Music bgMusic;
    if (!bgMusic.openFromFile("assets/audio/music3.ogg")) {
        std::cout << "����� �ε� ����" << std::endl;
        return -1;
    }
    std::cout << "����� �ε� ����" << std::endl;
    bgMusic.setLooping(true);                                        // �ݺ� ��� ����
    bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));    // ���������� ���� ����
    bgMusic.play();                                                  // ��� ����



    // ȿ���� �ε�
   /* std::cout << "\n========== ȿ���� �ε� ���� ==========\n" << std::endl;
    const std::string soundEffects[] = { "win", "lose", "error", "collect" };
    for (const auto& effect : soundEffects) {
        std::string path = "assets/audio/soundEffect/" + effect;
        if (effect == "error" || effect == "collect") path += "2";
        path += ".ogg";

        bool loaded = settings.loadSound(effect, path);
        if (loaded) {
            std::cout << effect << " ȿ���� �ε� ����" << std::endl;
        }
        else {
            std::cout << effect << " ȿ���� �ε� ����" << std::endl;
        }
    }
    std::cout << "\n========== ȿ���� �ε� �Ϸ� ==========\n" << std::endl;*/

    // ���� UI ���� �� ���ҽ� �ε�
    SettingsUI settingsUI(settings, font, bgMusic);
    if (!settingsUI.loadResources()) {
        std::cout << "���� UI ���ҽ� �ε� ����" << std::endl;
        return -1;
    }

    // ���� ������ ���� �� ��ġ ����
    SettingsIcon settingsIcon;
    settingsIcon.setPosition(window.getSize().x, window.getSize().y);

    // Ÿ�� ���� UI ����
    //TypingUI typingUI(font, settings);

    // ���� ����
    while (window.isOpen()) {
        // �̺�Ʈ ó�� ����
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                // â ���� �� ���� ���� �� ����� ����
                settings.saveSettings("settings.ini");
                bgMusic.stop();
                window.close();
            }

            // Ű���� �ؽ�Ʈ �Է� ó��
           // typingUI.handleTextInput(*event);

            // ���콺 Ŭ�� �̺�Ʈ ó��
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    // ���� ������ Ŭ�� ����
                    if (settingsIcon.contains(mousePosF) && !settingsUI.isOpen()) {
                        settingsUI.openWindow();  // ���� â ����
                    }
                }
            }
        }

        // ���� UI�� ���� ������ �ش� �̺�Ʈ ó�� �� �׸���
        if (settingsUI.isOpen()) {
            settingsUI.processEvents();  // ���� â �� �̺�Ʈ ó��
            settingsUI.render();         // ���� â �׸���

            //typingUI.update();           // ���� ������ �ݿ��ǵ��� Ÿ�� UI ������Ʈ
        }

        // ���� â ������
        window.clear(sf::Color::Black);   // ���� �ʱ�ȭ
        //typingUI.render(window);          // Ÿ�� ���� UI �׸���
        settingsIcon.render(window);      // ���� ������ �׸���
        window.display();                 // ������ ��� ǥ��
    }

    return 0;
}