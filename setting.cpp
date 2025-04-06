#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Settings.h"
#include <iostream>
#include <memory>
#include <filesystem>
using namespace std;

void renderSettingUI(sf::RenderWindow* window, Settings& settings, sf::Font& font, float windowWidth) {
    window->clear(sf::Color(230, 240, 255));
}

// �ؽ�ó, ��Ʈ, ���� ����
sf::Texture texture;
sf::Texture confirmButtonTexture;
sf::Texture cancelButtonTexture;
sf::Font settingsFont;
sf::Music bgMusic;
Settings settings;

//Ÿ�� ���� UI �ʱ�ȭ �Լ� --> �ּ�ó�� �� ����~~!!!
sf::Text practiceText(settingsFont, "", 15);      // ������ �ؽ�Ʈ
sf::Text userInputText(settingsFont, "");     // ����� �Է� �ؽ�Ʈ
sf::RectangleShape inputBox; // �Է� �ڽ�
std::string currentInput = "";  // ���� �Է� ����
std::wstring practiceContent = L"Ÿ�� ������ �����մϴ�. �� �ؽ�Ʈ�� ũ��� �������� ������ �� �ֽ��ϴ�."; // ���� ����

// Ÿ�� ���� UI �ʱ�ȭ �Լ�
//void initTypingUI() {
//    // ���� �ؽ�Ʈ ����
//    practiceText.setFont(settingsFont);
//    practiceText.setString(practiceContent);
//    practiceText.setCharacterSize(settings.getActualFontSize()); // ������ ��Ʈ ũ�� ����
//    practiceText.setFillColor(sf::Color::White);
//    practiceText.setPosition({ 100.f, 200.f });
//
//    // ����� �Է� �ؽ�Ʈ ����
//    userInputText.setFont(settingsFont);
//    userInputText.setString(currentInput);
//    userInputText.setCharacterSize(settings.getActualFontSize()); // ������ ��Ʈ ũ�� ����
//    userInputText.setFillColor(sf::Color::Green);
//    userInputText.setPosition({ 100.f, 250.f });
//
//    // �Է� �ڽ� ���� (���� ũ��)
//    inputBox.setSize(sf::Vector2f(600.f, 50.f));
//    inputBox.setPosition({ 90.f, 245.f });
//    inputBox.setFillColor(sf::Color(30, 30, 30));
//    inputBox.setOutlineThickness(2.f);
//    inputBox.setOutlineColor(sf::Color(100, 100, 100));
//}

// Ÿ�� ���� UI ������Ʈ �Լ� (��Ʈ ũ�� ���� �� ȣ��)
//void updateTypingUI() {
//    // ��Ʈ ũ�� ������Ʈ
//    practiceText.setCharacterSize(settings.getActualFontSize());
//    userInputText.setCharacterSize(settings.getActualFontSize());
//
//    // ��ġ�� �״�� �����ϵ�, �ؽ�Ʈ ũ�⸸ ����
//    practiceText.setPosition({ 100.f, 200.f });
//    userInputText.setPosition({ 100.f, 250.f });
//
//    // ��ġ ������ (�ʿ��)
//    practiceText.setPosition({ 100.f, 200.f });
//    userInputText.setPosition({ 100.f, 250.f + settings.getActualFontSize() });
//    inputBox.setPosition({ 90.f, 245.f + settings.getActualFontSize() });
//}

int main() {
   

    // ����â �� ��� ����
    const float settingWindowWidth = 500.f;  // ���� â �ʺ�
    const float sliderStartX = 120.f;        // �����̴� ���� X ��ǥ
    const float sliderWidth = 200.f;         // �����̴� �ʺ�

    // ���α׷� ���� �� ���� ���� �ε�
    if (settings.loadSettings("settings.ini")) {
        cout << "�������--> ����� ������ �ҷ��Խ��ϴ�" << endl;
    }
    else {
        cout << "�⺻ ������ ����մϴ�" << endl;
    }

    // (1) ���� â ����
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), L"Ÿ�ڿ���");

    // (2) ���ҽ� �ε�
    // 2.1 ���� ������ �ε�
    if (!texture.loadFromFile("assets/images/setting_icon.png")) {
        cout << "���� ������ �ε� ����" << endl;
        return -1;
    }
    cout << "���� ������ �ε� ����" << endl;
    sf::Sprite sprite(texture); // ��������Ʈ ����

    // 2.2 ��Ʈ �ε�
    if (!settingsFont.openFromFile("assets/fonts/D2Coding.ttf")) {
        cout << "��Ʈ �ε� ����" << endl;
    }
    else {
        cout << "��Ʈ �ε� ����" << endl;

        // Text ��ü �ʱ�ȭ
        practiceText.setFont(settingsFont);
        userInputText.setFont(settingsFont);


		//initTypingUI(); // Ÿ�� ���� UI �ʱ�ȭ ----------> �ּ�ó�� ����
    }

    // 2.3 ����� �ε�
    if (!bgMusic.openFromFile("assets/audio/music3.ogg")) {
        cout << "����� �ε� ����" << endl;
        return -1;
    }
    cout << "����� �ε� ����" << endl;
    bgMusic.setLooping(true); // �ݺ� ���
    bgMusic.setVolume(static_cast<float>(settings.getBgmVol())); // ����� ������ ����
    bgMusic.play(); // ��� ����

    // 2.4 ȿ���� �ε�
    cout << "\n========== ȿ���� �ε� ���� ==========\n" << endl;
    const string soundEffects[] = { "win", "lose", "error", "collect" };
    for (const auto& effect : soundEffects) {
        string path = "assets/audio/soundEffect/" + effect;
        if (effect == "error" || effect == "collect") path += "2";
        path += ".ogg";

        bool loaded = settings.loadSound(effect, path);
        if (loaded) {
            cout << effect << " ȿ���� �ε� ����" << endl;
        }
        else {
            cout << effect << " ȿ���� �ε� ����" << endl;
        }
    }
    cout << "\n========== ȿ���� �ε� �Ϸ� ==========\n" << endl;

    // 2.5 ��ư �̹��� �ε�
    cout << "\n========== ��ư �̹��� �ε� ���� ==========\n" << endl;

    if (!confirmButtonTexture.loadFromFile("assets/images/orange.png")) {
        cout << "Ȯ�� ��ư �̹��� �ε� ����" << endl;
    }
    else {
        cout << "Ȯ�� ��ư �̹��� �ε� ����" << endl;
    }
    sf::Sprite confirmButtonSprite(confirmButtonTexture);
    confirmButtonSprite.setScale({ 0.5f, 0.5f });

    if (!cancelButtonTexture.loadFromFile("assets/images/blue.png")) {
        cout << "��� ��ư �̹��� �ε� ����" << endl;
    }
    else {
        cout << "��� ��ư �̹��� �ε� ����" << endl;
    }
    sf::Sprite cancelButtonSprite(cancelButtonTexture);
    cancelButtonSprite.setScale({ 0.5f, 0.5f });

    cout << "\n========== ��ư �̹��� �ε� �Ϸ� ==========\n" << endl;

    // (3) ��������Ʈ ���� �� ��ġ
    sprite.setScale({ 0.2f, 0.2f });

    // ���� �� ��� (���� ������ ũ�⿡ �°� ����)
    float fixedWidth = 100.0f;  // �������� ���� �ʺ�
    float fixedHeight = 100.0f; // �������� ���� ����

    // �������� â�� ������ �ϴܿ� ��ġ
    sprite.setPosition({
        window.getSize().x - fixedWidth - 30.f,  // ������ �����ڸ����� 30�ȼ� ����
        window.getSize().y - fixedHeight - 30.f  // �ϴ� �����ڸ����� 30�ȼ� ��
        });

    // (4) ���� ���� ����
    bool settingWindowOpen = false;
    std::unique_ptr<sf::RenderWindow> settingWindow = nullptr;

    // �����̴� �巡�� ���� ����
    bool isDraggingBgmSlider = false;
    bool isDraggingSfxSlider = false;
    bool isDraggingFontSizeSlider = false;

    // ��ư ���� ����
    bool isConfirmButtonHovered = false;
    bool isCancelButtonHovered = false;
    bool isConfirmButtonPressed = false;
    bool isCancelButtonPressed = false;

    // (5) ���η���
    while (window.isOpen()) {
        // 5.1 ���� â �̺�Ʈ ó��
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                settings.saveSettings("settings.ini");
                bgMusic.stop();
                window.close();
            }

            // ���콺 Ŭ�� ���� �� ����â ����
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    // ���� ������ Ŭ�� �� ���� â ����
                    if (sprite.getGlobalBounds().contains(mousePosF) && !settingWindowOpen) {
                        settingWindowOpen = true;
                        settingWindow = std::make_unique<sf::RenderWindow>(
                            sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth), 300 }),
                            L"����� ����"
                        );
                        //�ӽ� ������ �ʱ�ȭ
                        settings.initTempSettings();
                    }
                }
            }
        }

           


        // 5.2 ���� â �̺�Ʈ ó��
        if (settingWindowOpen && settingWindow) {
            while (std::optional<sf::Event> settingsEvent = settingWindow->pollEvent()) {
                // ���� â �ݱ� ��ư ó��
                if (settingsEvent->is<sf::Event::Closed>()) {
                    settingWindow->close();
                    settingWindow.reset();
                    settingWindowOpen = false;
                    break;
                }

                // ���콺 ��ư ���� �̺�Ʈ ó��
                if (const auto* mouseEvent = settingsEvent->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
                        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                        // ��ư ���� ���
                        sf::FloatRect confirmButtonArea = confirmButtonSprite.getGlobalBounds();
                        sf::FloatRect cancelButtonArea = cancelButtonSprite.getGlobalBounds();

                        // Ȯ��(����) ��ư Ŭ�� ó��
                        if (confirmButtonArea.contains(mousePosF)) {

                            // ���� �� �� �α� ��� (������)
                            cout << "���� ��ư Ŭ�� ��: ����� " << settings.getBgmVol()
                                << "%, ȿ���� " << settings.getSfVol()
                                << "%, ���� ũ��: " << settings.getFontSize() << endl;

                            // ������ ����
                            settings.confirmSettings();
                            //updateTypingUI(); // ��Ʈ ũ�� ���� �� UI ������Ʈ

                            // ���� ũ�⿡ ���� ����â ���� ���� ����
                            int actualFontSize = 18 + (settings.getFontSize() * 3);
                            float newWindowHeight = 300.f + (actualFontSize - 18) * 2.f; // �⺻ ���̿� �߰� ���� ���ϱ�

                            // ���� ����â�� ��ġ ����
                            sf::Vector2i currentPosition = settingWindow->getPosition();

                            // ����â �ݱ�
                            settingWindow->close();
                            settingWindow.reset();


                            // �� ����â ����
                            settingWindow = std::make_unique<sf::RenderWindow>(
                                sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth),
                                                static_cast<unsigned int>(newWindowHeight) }),
                                L"����� ����"
                            );

                            // ���� ��ġ�� �� â �̵�
                            settingWindow->setPosition(currentPosition);

                            // ���� �� �� �α� ��� (������)
                            cout << "���� ��: ����� " << settings.getBgmVol()
                                << "%, ȿ���� " << settings.getSfVol()
                                << "%, ���� ũ��: " << settings.getFontSize() << endl;

                            // ����� ���� ���� ����
                            bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));

                            // ���� ���Ͽ� ����
                            settings.saveSettings("settings.ini");

                            // �ӽ� ������ �ʱ�ȭ
                            settings.initTempSettings();

                            settingWindowOpen = true;
                        }

                        // ��� ��ư Ŭ�� ó��
                        if (cancelButtonArea.contains(mousePosF)) {
                            cout << "��� ��ư Ŭ��: ���� �ӽð� = ����� " << settings.getTempBgmvol()
                                << "%, ȿ���� " << settings.getTempSfVol()
                                << "%, ���� ũ��: " << settings.getTempFontSize() << endl;

                            // �������� ���� ���尪���� �ǵ��� (������ -> �ӽð�)
                            settings.cancelSettings();

                            cout << "��� �Ϸ�: �ӽð��� ���������� ������ = ����� " << settings.getTempBgmvol()
                                << "%, ȿ���� " << settings.getTempSfVol()
                                << "%, ���� ũ��: " << settings.getTempFontSize() << endl;

                            // �巡�� ���� �ʱ�ȭ
                            isDraggingBgmSlider = isDraggingSfxSlider = isDraggingFontSizeSlider = false;

                            // ����� ���� �ǽð� ������Ʈ
                            bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));
                        }

                        // �����̴� �ڵ� ���� ���
                        sf::Vector2f handleSize(15.f, 20.f);

                        // ����� �����̴� ����
                        sf::Vector2f bgmHandlePos(
                            sliderStartX + settings.getTempBgmvol() / 100.0f * sliderWidth - 7.5f, // getBgmVol -> getTempBgmvol
                            70.f
                        );
                        sf::FloatRect bgmHandleArea(bgmHandlePos, handleSize);
                        sf::FloatRect bgmSliderArea(sf::Vector2f(sliderStartX, 65.f), sf::Vector2f(sliderWidth, 20.f));

                        // ȿ���� �����̴� ����
                        sf::Vector2f sfxHandlePos(
                            sliderStartX + settings.getTempSfVol() / 100.0f * sliderWidth - 7.5f, // getSfVol -> getTempSfVol
                            140.f
                        );
                        sf::FloatRect sfxHandleArea(sfxHandlePos, handleSize);
                        sf::FloatRect sfxSliderArea(sf::Vector2f(sliderStartX, 135.f), sf::Vector2f(sliderWidth, 20.f));

                        // ���� ũ�� �����̴� ����
                        float fontSizePos = settings.getTempFontSize() / 8.0f; // getFontSize -> getTempFontSize
                        sf::Vector2f fontSizeHandlePos(
                            sliderStartX + fontSizePos * sliderWidth - 8.f,
                            215.f
                        );
                        sf::FloatRect fontSizeHandleArea(fontSizeHandlePos, sf::Vector2f(16.f, 16.f));
                        sf::FloatRect fontSizeSliderArea(sf::Vector2f(sliderStartX, 210.f), sf::Vector2f(sliderWidth, 20.f));

                        // ����� �����̴� Ŭ�� ó��
                        if (bgmHandleArea.contains(mousePosF) || bgmSliderArea.contains(mousePosF)) {
                            isDraggingBgmSlider = true;
                            if (bgmSliderArea.contains(mousePosF)) {
                                float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                                int newVolume = static_cast<int>((newPos - sliderStartX) / sliderWidth * 100.f);
                                newVolume = std::max(0, std::min(100, newVolume));
                                settings.setTempBgmVol(newVolume); // setBgmVol -> setTempBgmVol
                                bgMusic.setVolume(static_cast<float>(newVolume));
                            }
                        }

                        // ȿ���� �����̴� Ŭ�� ó��
                        if (sfxHandleArea.contains(mousePosF) || sfxSliderArea.contains(mousePosF)) {
                            isDraggingSfxSlider = true;
                            if (sfxSliderArea.contains(mousePosF)) {
                                float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                                int newVolume = static_cast<int>((newPos - sliderStartX) / sliderWidth * 100.f);
                                newVolume = std::max(0, std::min(100, newVolume));
                                settings.setTempSfVol(newVolume); // setSfVol -> setTempSfVol
                            }
                        }

                        // ���� ũ�� �����̴� Ŭ�� ó��
                        if (fontSizeHandleArea.contains(mousePosF) || fontSizeSliderArea.contains(mousePosF)) {
                            isDraggingFontSizeSlider = true;
                            if (fontSizeSliderArea.contains(mousePosF)) {
                                float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                                int newFontSize = static_cast<int>((newPos - sliderStartX) / sliderWidth * 8.f);
                                newFontSize = std::max(0, std::min(8, newFontSize));
                                settings.setTempFontSize(newFontSize); // setFontSize -> setTempFontSize
                            }
                        }

                        // ȿ���� �׽�Ʈ ��ư Ŭ�� ó��
                        sf::FloatRect testButtonBounds(sf::Vector2f(400.f, 140.f), sf::Vector2f(60.f, 20.f));
                        if (testButtonBounds.contains(mousePosF)) {
                            settings.playSound("win");
                        }
                    }
                }

                // ���콺 ��ư ���� �̺�Ʈ ó��
                if (const auto* mouseEvent = settingsEvent->getIf<sf::Event::MouseButtonReleased>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        isDraggingBgmSlider = false;
                        isDraggingSfxSlider = false;
                        isDraggingFontSizeSlider = false;
                    }
                }

                // ���콺 �̵� �̺�Ʈ ó��
                if (const auto* mouseEvent = settingsEvent->getIf<sf::Event::MouseMoved>()) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
                    float mouseX = static_cast<float>(mousePos.x);
                    float sliderPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mouseX));

                    // ����� �����̴� �巡�� ó��
                    if (isDraggingBgmSlider) {
                        int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                        newVolume = std::max(0, std::min(100, newVolume));
                        settings.setTempBgmVol(newVolume); // setBgmVol -> setTempBgmVol
                        bgMusic.setVolume(static_cast<float>(newVolume));
                    }

                    // ȿ���� �����̴� �巡�� ó��
                    if (isDraggingSfxSlider) {
                        int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                        newVolume = std::max(0, std::min(100, newVolume));
                        settings.setTempSfVol(newVolume); // setSfVol -> setTempSfVol
                    }

                    // ���� ũ�� �����̴� �巡�� ó��
                    if (isDraggingFontSizeSlider) {
                        int newFontSize = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 8.f);
                        newFontSize = std::max(0, std::min(8, newFontSize));
                        settings.setTempFontSize(newFontSize); // setFontSize -> setTempFontSize
                    }
                }
            }

            // 5.3 ���� â ������
            if (settingWindow) {
                settingWindow->clear(sf::Color(230, 240, 255));

                // ��Ʈ ��� ���� ���� Ȯ�� (�� ����)
                static bool fontChecked = false;
                static bool fontLoaded = false;

                if (!fontChecked) {
                    fontLoaded = (settingsFont.getInfo().family != "");
                    fontChecked = true;
                    cout << (fontLoaded ? "��Ʈ ��� ����" : "��Ʈ ��� �Ұ�") << endl;
                }

                if (fontLoaded) {
                    // ����� �����̴� ������
                    sf::Text bgmText(settingsFont);
                    bgmText.setString(L"�����");
                    bgmText.setCharacterSize(20);
                    bgmText.setFillColor(sf::Color::Black);
                    bgmText.setPosition({ 30.f, 70.f });
                    settingWindow->draw(bgmText);

                    sf::RectangleShape bgmSliderBg(sf::Vector2f(sliderWidth, 10.f));
                    bgmSliderBg.setPosition({ sliderStartX, 75.f });
                    bgmSliderBg.setFillColor(sf::Color(180, 180, 180));
                    bgmSliderBg.setOutlineThickness(1);
                    bgmSliderBg.setOutlineColor(sf::Color::Black);
                    settingWindow->draw(bgmSliderBg);

                    // getBgmVol -> getTempBgmvol
                    float bgmVolPos = settings.getTempBgmvol() / 100.0f;
                    sf::RectangleShape bgmSliderHandle(sf::Vector2f(15.f, 20.f));
                    bgmSliderHandle.setPosition({ sliderStartX + bgmVolPos * sliderWidth - 7.5f, 70.f });
                    bgmSliderHandle.setFillColor(sf::Color(100, 100, 255));
                    bgmSliderHandle.setOutlineThickness(1);
                    bgmSliderHandle.setOutlineColor(sf::Color::Black);
                    settingWindow->draw(bgmSliderHandle);

                    sf::Text bgmVolumeText(settingsFont);
                    // getBgmVol -> getTempBgmvol
                    bgmVolumeText.setString(std::to_wstring(settings.getTempBgmvol()) + L"%");
                    bgmVolumeText.setCharacterSize(16);
                    bgmVolumeText.setFillColor(sf::Color::Black);
                    bgmVolumeText.setPosition({ 330.f, 73.f });
                    settingWindow->draw(bgmVolumeText);

                    // ȿ���� �����̴� ������
                    sf::Text sfxText(settingsFont);
                    sfxText.setString(L"ȿ����");
                    sfxText.setCharacterSize(20);
                    sfxText.setFillColor(sf::Color::Black);
                    sfxText.setPosition({ 30.f, 140.f });
                    settingWindow->draw(sfxText);

                    sf::RectangleShape sfxSliderBg(sf::Vector2f(sliderWidth, 10.f));
                    sfxSliderBg.setPosition({ sliderStartX, 145.f });
                    sfxSliderBg.setFillColor(sf::Color(180, 180, 180));
                    sfxSliderBg.setOutlineThickness(1);
                    sfxSliderBg.setOutlineColor(sf::Color::Black);
                    settingWindow->draw(sfxSliderBg);

                    // getSfVol -> getTempSfVol
                    float sfxVolPos = settings.getTempSfVol() / 100.0f;
                    sf::RectangleShape sfxSliderHandle(sf::Vector2f(15.f, 20.f));
                    sfxSliderHandle.setPosition({ sliderStartX + sfxVolPos * sliderWidth - 7.5f, 140.f });
                    sfxSliderHandle.setFillColor(sf::Color(255, 100, 100));
                    sfxSliderHandle.setOutlineThickness(1);
                    sfxSliderHandle.setOutlineColor(sf::Color::Black);
                    settingWindow->draw(sfxSliderHandle);

                    sf::Text sfxVolumeText(settingsFont);
                    // getSfVol -> getTempSfVol
                    sfxVolumeText.setString(std::to_wstring(settings.getTempSfVol()) + L"%");
                    sfxVolumeText.setCharacterSize(16);
                    sfxVolumeText.setFillColor(sf::Color::Black);
                    sfxVolumeText.setPosition({ 330.f, 143.f });
                    settingWindow->draw(sfxVolumeText);

                    sf::RectangleShape sfxTestButton(sf::Vector2f(60.f, 20.f));
                    sfxTestButton.setPosition({ 400.f, 140.f });
                    sfxTestButton.setFillColor(sf::Color(220, 220, 220));
                    sfxTestButton.setOutlineThickness(1);
                    sfxTestButton.setOutlineColor(sf::Color::Black);
                    settingWindow->draw(sfxTestButton);

                    sf::Text sfxTestText(settingsFont);
                    sfxTestText.setString(L"�׽�Ʈ");
                    sfxTestText.setCharacterSize(14);
                    sfxTestText.setFillColor(sf::Color::Black);
                    sfxTestText.setPosition({ 405.f, 143.f });
                    settingWindow->draw(sfxTestText);

                    // ���� ũ�� �����̴� ������
                    sf::Text fontSizeText(settingsFont);
                    fontSizeText.setString(L"����ũ��");
                    fontSizeText.setCharacterSize(20);
                    fontSizeText.setFillColor(sf::Color::Black);
                    fontSizeText.setPosition({ 30.f, 210.f });
                    settingWindow->draw(fontSizeText);

                    sf::RectangleShape fontSizeSliderLine(sf::Vector2f(sliderWidth, 2.f));
                    fontSizeSliderLine.setPosition({ sliderStartX, 220.f });
                    fontSizeSliderLine.setFillColor(sf::Color(100, 100, 100));
                    settingWindow->draw(fontSizeSliderLine);

                    // ���� ǥ�� (9�ܰ�)
                    for (int i = 0; i < 9; i++) {
                        sf::RectangleShape tick(sf::Vector2f(2.f, (i % 4 == 0) ? 10.f : 5.f));
                        tick.setPosition({ sliderStartX + (i * 25.f), 215.f });
                        tick.setFillColor(sf::Color::Black);
                        settingWindow->draw(tick);
                    }

                    // �ּ�/�ִ� ���̺�
                    sf::Text minSizeLabel(settingsFont);
                    minSizeLabel.setString(L"�۰�");
                    minSizeLabel.setCharacterSize(10);
                    minSizeLabel.setFillColor(sf::Color::Red);
                    minSizeLabel.setPosition({ 110.f, 240.f });
                    settingWindow->draw(minSizeLabel);

                    sf::Text maxSizeLabel(settingsFont);
                    maxSizeLabel.setString(L"ũ��");
                    maxSizeLabel.setCharacterSize(10);
                    maxSizeLabel.setFillColor(sf::Color::Red);
                    maxSizeLabel.setPosition({ 310.f, 240.f });
                    settingWindow->draw(maxSizeLabel);

                    // ���� ũ�� �����̴� �ڵ�
                    // getFontSize -> getTempFontSize
                    float fontSizePos = settings.getTempFontSize() / 8.0f;
                    sf::CircleShape fontSizeHandle(8.f);
                    fontSizeHandle.setPosition({ sliderStartX + fontSizePos * sliderWidth - 8.f, 215.f });

                    sf::Color fillColor(12, 124, 197);
                    sf::Color outlineColor(255, 225, 255, 180);
                    fontSizeHandle.setFillColor(fillColor);
                    fontSizeHandle.setOutlineThickness(2.f);
                    fontSizeHandle.setOutlineColor(outlineColor);
                    settingWindow->draw(fontSizeHandle);

                    // ���� ���� ũ�� ����
                    sf::Text currentSizeExample(settingsFont);
                    currentSizeExample.setString(L"��");
                    // getFontSize -> getTempFontSize
                    int actualFontSize = 18 + (settings.getTempFontSize() * 3);
                    currentSizeExample.setCharacterSize(actualFontSize);
                    currentSizeExample.setFillColor(sf::Color::Black);
                    currentSizeExample.setPosition({ 400.f, 210.f });
                    settingWindow->draw(currentSizeExample);

                    // ���� ���õ� �� ǥ��
                    sf::Text currentValueText(settingsFont);
                    // getFontSize -> getTempFontSize
                    currentValueText.setString(std::to_wstring(settings.getTempFontSize()));
                    currentValueText.setCharacterSize(14);
                    currentValueText.setFillColor(sf::Color::Black);
                    currentValueText.setPosition({ 115.f + fontSizePos * sliderWidth, 190.f });
                    settingWindow->draw(currentValueText);

                    //��� ��ư �ؽ�Ʈ
                    sf::Text cancelText(settingsFont);
                    cancelText.setString(L"���");
                    cancelText.setCharacterSize(12);
                    cancelText.setFillColor(sf::Color::Black);

                    //���� ��ư �ؽ�Ʈ
                    sf::Text confirmText(settingsFont);
                    confirmText.setString(L"����");
                    confirmText.setCharacterSize(12);
                    confirmText.setFillColor(sf::Color::Black);

                    // ����/��� ��ư ������
                    // â ũ�� ��������
                    float windowWidth = static_cast<float>(settingWindow->getSize().x);
                    float windowHeight = static_cast<float>(settingWindow->getSize().y);
                    float buttonSpacing = 10.f; // ��ư ����

                    cancelButtonSprite.setScale({ 0.1f, 0.1f });
                    confirmButtonSprite.setScale({ 0.1f, 0.1f });

                    // ��ư ��ġ
                    // ��ư ��ġ ���
                    cancelButtonSprite.setPosition({
                        windowWidth - (confirmButtonTexture.getSize().x * 0.1f * 2) - buttonSpacing - 20.f,
                        windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
                        });

                    confirmButtonSprite.setPosition({
                        windowWidth - (confirmButtonTexture.getSize().x * 0.1f) - 20.f,
                        windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
                        });

                    // ��ư ũ�� ���
                    sf::Vector2f cancelButtonSize(
                        cancelButtonTexture.getSize().x * 0.1f,
                        cancelButtonTexture.getSize().y * 0.1f
                    );

                    sf::Vector2f confirmButtonSize(
                        confirmButtonTexture.getSize().x * 0.1f,
                        confirmButtonTexture.getSize().y * 0.1f
                    );

                    // �ѱ� ��� �ؽ�Ʈ(2����)�� ��ư �߾ӿ� ��ġ
                    float cancelCenterX = cancelButtonSprite.getPosition().x + (cancelButtonSize.x / 2.0f);
                    float cancelCenterY = cancelButtonSprite.getPosition().y + (cancelButtonSize.y / 2.0f);

                    // �ѱ��� ���� ũ���� ����� ����
                    cancelText.setPosition({
                        cancelCenterX - static_cast<float>(cancelText.getCharacterSize()) * 1.0f,
                        cancelCenterY - static_cast<float>(cancelText.getCharacterSize()) * 0.5f
                        });

                    // ���� �ؽ�Ʈ(2����)�� ��ư �߾ӿ� ��ġ 
                    float confirmCenterX = confirmButtonSprite.getPosition().x + (confirmButtonSize.x / 2.0f);
                    float confirmCenterY = confirmButtonSprite.getPosition().y + (confirmButtonSize.y / 2.0f);

                    // �ѱ��� ���� ũ���� ����� ����
                    confirmText.setPosition({
                        confirmCenterX - static_cast<float>(confirmText.getCharacterSize()) * 1.0f,
                        confirmCenterY - static_cast<float>(confirmText.getCharacterSize()) * 0.5f
                        });

                    // ��ư ������
                    settingWindow->draw(cancelButtonSprite);
                    settingWindow->draw(confirmButtonSprite);
                    settingWindow->draw(cancelText);
                    settingWindow->draw(confirmText);
                }

                settingWindow->display();
            }
        }

        // 5.4 ���� â ������
        window.clear(sf::Color::Black);
        window.draw(sprite);

        // Ÿ�� ���� �ؽ�Ʈ ������ �߰�
        window.draw(practiceText);
        window.draw(userInputText);
        window.draw(inputBox);

        window.display();
    }

    return 0;
}