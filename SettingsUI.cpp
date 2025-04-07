#include "SettingsUI.hpp"
#include <iostream>


// ������ - ����, ��Ʈ, ���� ���� ����
SettingsUI::SettingsUI(Settings& settings, sf::Font& font, sf::Music& bgMusic)
    : settings(settings), font(font), bgMusic(bgMusic),
      confirmButtonSprite(confirmButtonTexture),  // �ؽ�ó�� �ʱ�ȭ
      cancelButtonSprite(cancelButtonTexture) {
    // �����ڿ����� sprite�� �ʱ�ȭ�� ���� ���� (loadResources���� ����)
}
    


SettingsUI::~SettingsUI() {
    close();
}
// Ȯ��/��� ��ư �̹��� �ε� �� ��������Ʈ ����
bool SettingsUI::loadResources() {
    
    if (resourcesLoaded) {
        return true;
    }

    if (!confirmButtonTexture.loadFromFile("assets/images/orange.png")) {
        std::cout << "Ȯ�� ��ư �̹��� �ε� ����" << std::endl;
        return false;
    }
    std::cout << "Ȯ�� ��ư �̹��� �ε� ����" << std::endl;

    // �ؽ�ó�� �ε�� �� ��������Ʈ �ʱ�ȭ
    confirmButtonSprite.setTexture(confirmButtonTexture);
    confirmButtonSprite.setScale({ 0.1f, 0.1f });

    if (!cancelButtonTexture.loadFromFile("assets/images/blue.png")) {
        std::cout << "��� ��ư �̹��� �ε� ����" << std::endl;
        return false;
    }
    std::cout << "��� ��ư �̹��� �ε� ����" << std::endl;

    // �ؽ�ó�� �ε�� �� ��������Ʈ �ʱ�ȭ
    cancelButtonSprite.setTexture(cancelButtonTexture);
    cancelButtonSprite.setScale({ 0.1f, 0.1f });

    resourcesLoaded = true;
    return true;
}
// ���� â ���� (���� �������� �ӽð����� �ʱ�ȭ)
void SettingsUI::openWindow() {
    if (isWindowOpen) return;

    // ���� ���ҽ��� �ε�
    if (!loadResources()) {
        std::cout << "���ҽ� �ε忡 �����Ͽ� ���� â�� �� �� �����ϴ�." << std::endl;
        return;
    }


    int actualFontSize = 18 + (settings.getFontSize() * 3);
    float newWindowHeight = 300.f + (actualFontSize - 18) * 2.f;

    settingWindow = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth),
                       static_cast<unsigned int>(newWindowHeight) }),
        L"����� ����"
    );

    isWindowOpen = true;

    // Initialize temporary settings
    settings.initTempSettings();
}

bool SettingsUI::isOpen() const {
    return isWindowOpen && settingWindow != nullptr;
}

// �̺�Ʈ ó�� ����
void SettingsUI::processEvents() {
    if (!isOpen()) return;

    while (std::optional<sf::Event> event = settingWindow->pollEvent()) {
        // ���� â �ݱ� �̺�Ʈ
        if (event->is<sf::Event::Closed>()) {
            close();
            return;
        }

        // ���콺 Ŭ�� �̺�Ʈ ó��
        if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                // Ȯ��/��� ��ư ��ġ ����
                sf::FloatRect confirmButtonArea = confirmButtonSprite.getGlobalBounds();
                sf::FloatRect cancelButtonArea = cancelButtonSprite.getGlobalBounds();

                //Ȯ�� ��ư Ŭ�� �� ���� ����
                if (confirmButtonArea.contains(mousePosF)) {
                    std::cout << "���� ��ư Ŭ�� ��: ����� " << settings.getBgmVol()
                        << "%, ȿ���� " << settings.getSfVol()
                        << "%, ���� ũ��: " << settings.getFontSize() << std::endl;

                    // ���� �������� �ӽð� �ݿ�
                    settings.confirmSettings();

                    // �۲� ũ�⿡ ���� â ���� ����
                    int actualFontSize = 18 + (settings.getFontSize() * 3);
                    float newWindowHeight = 300.f + (actualFontSize - 18) * 2.f;

                    // â ��ġ ����ϰ� �� â ����
                    sf::Vector2i currentPosition = settingWindow->getPosition();

                    // ����� ���� ����
                    settingWindow->close();
                    settingWindow.reset();

                    settingWindow = std::make_unique<sf::RenderWindow>(
                        sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth),
                                       static_cast<unsigned int>(newWindowHeight) }),
                        L"����� ����"
                    );

                    // ���Ϸ� ���� ����
                    settingWindow->setPosition(currentPosition);

                    std::cout << "���� ��: ����� " << settings.getBgmVol()
                        << "%, ȿ���� " << settings.getSfVol()
                        << "%, ���� ũ��: " << settings.getFontSize() << std::endl;

                    // ����� ���� ����
                    bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));

                    // ���Ϸ� ���� ����
                    settings.saveSettings("settings.ini");

                    // �ӽð� �ʱ�ȭ
                    settings.initTempSettings();
                }

                // ��� ��ư Ŭ�� �� �ӽð� �ǵ�����
                if (cancelButtonArea.contains(mousePosF)) {
                    std::cout << "��� ��ư Ŭ��: ���� �ӽð� = ����� " << settings.getTempBgmvol()
                        << "%, ȿ���� " << settings.getTempSfVol()
                        << "%, ���� ũ��: " << settings.getTempFontSize() << std::endl;

                    // �ӽð� �ʱ�ȭ
                    settings.cancelSettings();

                    std::cout << "��� �Ϸ�: �ӽð��� ���������� ������ = ����� " << settings.getTempBgmvol()
                        << "%, ȿ���� " << settings.getTempSfVol()
                        << "%, ���� ũ��: " << settings.getTempFontSize() << std::endl;

                    // �����̴� �巡�� ���� �ʱ�ȭ
                    isDraggingBgmSlider = isDraggingSfxSlider = isDraggingFontSizeSlider = false;

                    // ����� ���� ������� ����
                    bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));
                }

                // �����̴� �ڵ� ������
                sf::Vector2f handleSize(15.f, 20.f);

                // ����� �����̴� ���� ���
                sf::Vector2f bgmHandlePos(
                    sliderStartX + settings.getTempBgmvol() / 100.0f * sliderWidth - 7.5f,
                    70.f
                );
                sf::FloatRect bgmHandleArea(bgmHandlePos, handleSize);
                sf::FloatRect bgmSliderArea(sf::Vector2f(sliderStartX, 65.f), sf::Vector2f(sliderWidth, 20.f));

                // ȿ���� �����̴� ����
                sf::Vector2f sfxHandlePos(
                    sliderStartX + settings.getTempSfVol() / 100.0f * sliderWidth - 7.5f,
                    140.f
                );
                sf::FloatRect sfxHandleArea(sfxHandlePos, handleSize);
                sf::FloatRect sfxSliderArea(sf::Vector2f(sliderStartX, 135.f), sf::Vector2f(sliderWidth, 20.f));

                // ���� ũ�� �����̴� ����
                float fontSizePos = settings.getTempFontSize() / 8.0f;
                sf::Vector2f fontSizeHandlePos(
                    sliderStartX + fontSizePos * sliderWidth - 8.f,
                    215.f
                );
                sf::FloatRect fontSizeHandleArea(fontSizeHandlePos, sf::Vector2f(16.f, 16.f));
                sf::FloatRect fontSizeSliderArea(sf::Vector2f(sliderStartX, 210.f), sf::Vector2f(sliderWidth, 20.f));

                // ����� �����̴� Ŭ��
                if (bgmHandleArea.contains(mousePosF) || bgmSliderArea.contains(mousePosF)) {
                    isDraggingBgmSlider = true;
                    if (bgmSliderArea.contains(mousePosF)) {
                        float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                        int newVolume = static_cast<int>((newPos - sliderStartX) / sliderWidth * 100.f);
                        newVolume = std::max(0, std::min(100, newVolume));
                        settings.setTempBgmVol(newVolume);
                        bgMusic.setVolume(static_cast<float>(newVolume));
                    }
                }

                // ȿ���� �����̴� Ŭ��
                if (sfxHandleArea.contains(mousePosF) || sfxSliderArea.contains(mousePosF)) {
                    isDraggingSfxSlider = true;
                    if (sfxSliderArea.contains(mousePosF)) {
                        float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                        int newVolume = static_cast<int>((newPos - sliderStartX) / sliderWidth * 100.f);
                        newVolume = std::max(0, std::min(100, newVolume));
                        settings.setTempSfVol(newVolume);
                    }
                }

                //  ���� ũ�� �����̴� Ŭ��
                if (fontSizeHandleArea.contains(mousePosF) || fontSizeSliderArea.contains(mousePosF)) {
                    isDraggingFontSizeSlider = true;
                    if (fontSizeSliderArea.contains(mousePosF)) {
                        float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                        int newFontSize = static_cast<int>((newPos - sliderStartX) / sliderWidth * 8.f);
                        newFontSize = std::max(0, std::min(8, newFontSize));
                        settings.setTempFontSize(newFontSize);
                    }
                }

                // �Ҹ� ��ư Ŭ�� �� �Ҹ� ���
                sf::FloatRect testButtonBounds(sf::Vector2f(400.f, 140.f), sf::Vector2f(60.f, 20.f));
                if (testButtonBounds.contains(mousePosF)) {
                    //settings.playSound("win");
                }
            }
        }

        // ���콺 ��ư ���� �� �巡�� ���� ����
        if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                isDraggingBgmSlider = false;
                isDraggingSfxSlider = false;
                isDraggingFontSizeSlider = false;
            }
        }

        // ���콺 �̵� �� �����̴� �巡�� ����
        if (const auto* mouseEvent = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
            float mouseX = static_cast<float>(mousePos.x);
            float sliderPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mouseX));

            // ����� �����̴� �巡�� ó��
            if (isDraggingBgmSlider) {
                int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                newVolume = std::max(0, std::min(100, newVolume));
                settings.setTempBgmVol(newVolume);
                bgMusic.setVolume(static_cast<float>(newVolume));
            }

            // ȿ���� �����̴� �巡�� ó��
            if (isDraggingSfxSlider) {
                int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                newVolume = std::max(0, std::min(100, newVolume));
                settings.setTempSfVol(newVolume);
            }

            // �۲� ũ�� �����̴� �巡�� ó��
            if (isDraggingFontSizeSlider) {
                int newFontSize = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 8.f);
                newFontSize = std::max(0, std::min(8, newFontSize));
                settings.setTempFontSize(newFontSize);
            }
        }
    }
}

void SettingsUI::render() {
    if (!isOpen()) return;

    settingWindow->clear(sf::Color(230, 240, 255)); // â ��� ����

     // ��Ʈ�� ���� �ε�Ǿ����� Ȯ��
    bool fontLoaded = (font.getInfo().family != "");

    if (fontLoaded) {
        // ����� ����
        sf::Text bgmText(font);
        bgmText.setString(L"�����");
        bgmText.setCharacterSize(20);
        bgmText.setFillColor(sf::Color::Black);
        bgmText.setPosition({ 30.f, 70.f });
        settingWindow->draw(bgmText);

        //�����̴� ��� ��
        sf::RectangleShape bgmSliderBg(sf::Vector2f(sliderWidth, 10.f));
        bgmSliderBg.setPosition({ sliderStartX, 75.f });
        bgmSliderBg.setFillColor(sf::Color(180, 180, 180));
        bgmSliderBg.setOutlineThickness(1);
        bgmSliderBg.setOutlineColor(sf::Color::Black);
        settingWindow->draw(bgmSliderBg);

        // �����̴� �ڵ� ��ġ ���
        float bgmVolPos = settings.getTempBgmvol() / 100.0f;
        sf::RectangleShape bgmSliderHandle(sf::Vector2f(15.f, 20.f));
        bgmSliderHandle.setPosition({ sliderStartX + bgmVolPos * sliderWidth - 7.5f, 70.f });
        bgmSliderHandle.setFillColor(sf::Color(100, 100, 255));
        bgmSliderHandle.setOutlineThickness(1);
        bgmSliderHandle.setOutlineColor(sf::Color::Black);
        settingWindow->draw(bgmSliderHandle);

        // ���� �� �ؽ�Ʈ
        sf::Text bgmVolumeText(font);
        bgmVolumeText.setString(std::to_wstring(settings.getTempBgmvol()) + L"%");
        bgmVolumeText.setCharacterSize(16);
        bgmVolumeText.setFillColor(sf::Color::Black);
        bgmVolumeText.setPosition({ 330.f, 73.f });
        settingWindow->draw(bgmVolumeText);

        // ȿ���� �����̴� UI 
        // ȿ���� ����
        sf::Text sfxText(font);
        sfxText.setString(L"ȿ����");
        sfxText.setCharacterSize(20);
        sfxText.setFillColor(sf::Color::Black);
        sfxText.setPosition({ 30.f, 140.f });
        settingWindow->draw(sfxText);

        // �����̴� ��� ����
        sf::RectangleShape sfxSliderBg(sf::Vector2f(sliderWidth, 10.f));
        sfxSliderBg.setPosition({ sliderStartX, 145.f });
        sfxSliderBg.setFillColor(sf::Color(180, 180, 180));
        sfxSliderBg.setOutlineThickness(1);
        sfxSliderBg.setOutlineColor(sf::Color::Black);
        settingWindow->draw(sfxSliderBg);

        // �����̴� �ڵ� ��ġ ��� �� �׸���
        float sfxVolPos = settings.getTempSfVol() / 100.0f;
        sf::RectangleShape sfxSliderHandle(sf::Vector2f(15.f, 20.f));
        sfxSliderHandle.setPosition({ sliderStartX + sfxVolPos * sliderWidth - 7.5f, 140.f });
        sfxSliderHandle.setFillColor(sf::Color(255, 100, 100));
        sfxSliderHandle.setOutlineThickness(1);
        sfxSliderHandle.setOutlineColor(sf::Color::Black);
        settingWindow->draw(sfxSliderHandle);


        // ���� ȿ���� ���� �ؽ�Ʈ ǥ��
        sf::Text sfxVolumeText(font);
        sfxVolumeText.setString(std::to_wstring(settings.getTempSfVol()) + L"%");
        sfxVolumeText.setCharacterSize(16);
        sfxVolumeText.setFillColor(sf::Color::Black);
        sfxVolumeText.setPosition({ 330.f, 143.f });
        settingWindow->draw(sfxVolumeText);

        //ȿ���� �׽�Ʈ ��ư UI
        sf::RectangleShape sfxTestButton(sf::Vector2f(60.f, 20.f));
        sfxTestButton.setPosition({ 400.f, 140.f });
        sfxTestButton.setFillColor(sf::Color(220, 220, 220));
        sfxTestButton.setOutlineThickness(1);
        sfxTestButton.setOutlineColor(sf::Color::Black);
        settingWindow->draw(sfxTestButton);

        // �׽�Ʈ ��ư �ؽ�Ʈ
        sf::Text sfxTestText(font);
        sfxTestText.setString(L"�׽�Ʈ");
        sfxTestText.setCharacterSize(14);
        sfxTestText.setFillColor(sf::Color::Black);
        sfxTestText.setPosition({ 405.f, 143.f });
        settingWindow->draw(sfxTestText);

        //-------���� ũ�� �����̴�--------
        // �� �ؽ�Ʈ
        sf::Text fontSizeText(font);
        fontSizeText.setString(L"����ũ��");
        fontSizeText.setCharacterSize(20);
        fontSizeText.setFillColor(sf::Color::Black);
        fontSizeText.setPosition({ 30.f, 210.f });
        settingWindow->draw(fontSizeText);

        // �����̴� ��
        sf::RectangleShape fontSizeSliderLine(sf::Vector2f(sliderWidth, 2.f));
        fontSizeSliderLine.setPosition({ sliderStartX, 220.f });
        fontSizeSliderLine.setFillColor(sf::Color(100, 100, 100));
        settingWindow->draw(fontSizeSliderLine);

        // ���� ǥ�� (�� 9ĭ)
        for (int i = 0; i < 9; i++) {
            sf::RectangleShape tick(sf::Vector2f(2.f, (i % 4 == 0) ? 10.f : 5.f));
            tick.setPosition({ sliderStartX + (i * 25.f), 215.f });
            tick.setFillColor(sf::Color::Black);
            settingWindow->draw(tick);
        }

        // "�۰�", "ũ��" ���̺� �ؽ�Ʈ
        sf::Text minSizeLabel(font);
        minSizeLabel.setString(L"�۰�");
        minSizeLabel.setCharacterSize(10);
        minSizeLabel.setFillColor(sf::Color::Red);
        minSizeLabel.setPosition({ 110.f, 240.f });
        settingWindow->draw(minSizeLabel);

        sf::Text maxSizeLabel(font);
        maxSizeLabel.setString(L"ũ��");
        maxSizeLabel.setCharacterSize(10);
        maxSizeLabel.setFillColor(sf::Color::Red);
        maxSizeLabel.setPosition({ 310.f, 240.f });
        settingWindow->draw(maxSizeLabel);

        // �����̴� �ڵ� (����)
        float fontSizePos = settings.getTempFontSize() / 8.0f;
        sf::CircleShape fontSizeHandle(8.f);
        fontSizeHandle.setPosition({ sliderStartX + fontSizePos * sliderWidth - 8.f, 215.f });

        sf::Color fillColor(12, 124, 197);
        sf::Color outlineColor(255, 225, 255, 180);
        fontSizeHandle.setFillColor(fillColor);
        fontSizeHandle.setOutlineThickness(2.f);
        fontSizeHandle.setOutlineColor(outlineColor);
        settingWindow->draw(fontSizeHandle);

        // ���� ���� ũ�� ���� ("��")
        sf::Text currentSizeExample(font);
        currentSizeExample.setString(L"��");
        int actualFontSize = 18 + (settings.getTempFontSize() * 3);
        currentSizeExample.setCharacterSize(actualFontSize);
        currentSizeExample.setFillColor(sf::Color::Black);
        currentSizeExample.setPosition({ 400.f, 210.f });
        settingWindow->draw(currentSizeExample);

        // ���� �����̴� �� ���� ǥ��
        sf::Text currentValueText(font);
        currentValueText.setString(std::to_wstring(settings.getTempFontSize()));
        currentValueText.setCharacterSize(14);
        currentValueText.setFillColor(sf::Color::Black);
        currentValueText.setPosition({ 115.f + fontSizePos * sliderWidth, 190.f });
        settingWindow->draw(currentValueText);

        // ��ư �ؽ�Ʈ ��ü ����
        sf::Text cancelText(font);
        cancelText.setString(L"���");
        cancelText.setCharacterSize(12);
        cancelText.setFillColor(sf::Color::Black);

        sf::Text confirmText(font);
        confirmText.setString(L"����");
        confirmText.setCharacterSize(12);
        confirmText.setFillColor(sf::Color::Black);

        // ��ư ��ġ �� ũ�� ����
        // â ������ �������� ��ư ��ġ ���
        float windowWidth = static_cast<float>(settingWindow->getSize().x);
        float windowHeight = static_cast<float>(settingWindow->getSize().y);
        float buttonSpacing = 10.f;

        // ��ư ��ġ ���� (������ �̹� scale�� �����Ǿ����Ƿ� ���⼭�� �ٽ� �������� ����)
        cancelButtonSprite.setPosition({
            windowWidth - (confirmButtonTexture.getSize().x * 0.1f * 2) - buttonSpacing - 20.f,
            windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
            });

        confirmButtonSprite.setPosition({
            windowWidth - (confirmButtonTexture.getSize().x * 0.1f) - 20.f,
            windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
            });

        // ��ư ������
        // �ؽ�Ʈ�� ��ư �߾ӿ� ��ġ��Ű�� ���� ���
        sf::Vector2f cancelButtonSize(
            cancelButtonTexture.getSize().x * 0.1f,
            cancelButtonTexture.getSize().y * 0.1f
        );

        sf::Vector2f confirmButtonSize(
            confirmButtonTexture.getSize().x * 0.1f,
            confirmButtonTexture.getSize().y * 0.1f
        );

        // Text positioning
        float cancelCenterX = cancelButtonSprite.getPosition().x + (cancelButtonSize.x / 2.0f);
        float cancelCenterY = cancelButtonSprite.getPosition().y + (cancelButtonSize.y / 2.0f);

        cancelText.setPosition({
            cancelCenterX - static_cast<float>(cancelText.getCharacterSize()) * 1.0f,
            cancelCenterY - static_cast<float>(cancelText.getCharacterSize()) * 0.5f
            });

        float confirmCenterX = confirmButtonSprite.getPosition().x + (confirmButtonSize.x / 2.0f);
        float confirmCenterY = confirmButtonSprite.getPosition().y + (confirmButtonSize.y / 2.0f);

        confirmText.setPosition({
            confirmCenterX - static_cast<float>(confirmText.getCharacterSize()) * 1.0f,
            confirmCenterY - static_cast<float>(confirmText.getCharacterSize()) * 0.5f
            });

        // ��ư �� �ؽ�Ʈ ������
        settingWindow->draw(cancelButtonSprite);
        settingWindow->draw(confirmButtonSprite);
        settingWindow->draw(cancelText);
        settingWindow->draw(confirmText);
    }
    
    settingWindow->display();
}

void SettingsUI::close() {
    if (settingWindow) {
        settingWindow->close();
        settingWindow.reset();
        isWindowOpen = false;
    }
}