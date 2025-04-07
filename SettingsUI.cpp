#include "SettingsUI.hpp"
#include <iostream>


// 생성자 - 설정, 폰트, 음악 참조 저장
SettingsUI::SettingsUI(Settings& settings, sf::Font& font, sf::Music& bgMusic)
    : settings(settings), font(font), bgMusic(bgMusic),
      confirmButtonSprite(confirmButtonTexture),  // 텍스처로 초기화
      cancelButtonSprite(cancelButtonTexture) {
    // 생성자에서는 sprite의 초기화를 하지 않음 (loadResources에서 수행)
}
    


SettingsUI::~SettingsUI() {
    close();
}
// 확인/취소 버튼 이미지 로드 및 스프라이트 설정
bool SettingsUI::loadResources() {
    
    if (resourcesLoaded) {
        return true;
    }

    if (!confirmButtonTexture.loadFromFile("assets/images/orange.png")) {
        std::cout << "확인 버튼 이미지 로드 실패" << std::endl;
        return false;
    }
    std::cout << "확인 버튼 이미지 로드 성공" << std::endl;

    // 텍스처가 로드된 후 스프라이트 초기화
    confirmButtonSprite.setTexture(confirmButtonTexture);
    confirmButtonSprite.setScale({ 0.1f, 0.1f });

    if (!cancelButtonTexture.loadFromFile("assets/images/blue.png")) {
        std::cout << "취소 버튼 이미지 로드 실패" << std::endl;
        return false;
    }
    std::cout << "취소 버튼 이미지 로드 성공" << std::endl;

    // 텍스처가 로드된 후 스프라이트 초기화
    cancelButtonSprite.setTexture(cancelButtonTexture);
    cancelButtonSprite.setScale({ 0.1f, 0.1f });

    resourcesLoaded = true;
    return true;
}
// 설정 창 열기 (현재 설정값을 임시값으로 초기화)
void SettingsUI::openWindow() {
    if (isWindowOpen) return;

    // 먼저 리소스를 로드
    if (!loadResources()) {
        std::cout << "리소스 로드에 실패하여 설정 창을 열 수 없습니다." << std::endl;
        return;
    }


    int actualFontSize = 18 + (settings.getFontSize() * 3);
    float newWindowHeight = 300.f + (actualFontSize - 18) * 2.f;

    settingWindow = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth),
                       static_cast<unsigned int>(newWindowHeight) }),
        L"사용자 설정"
    );

    isWindowOpen = true;

    // Initialize temporary settings
    settings.initTempSettings();
}

bool SettingsUI::isOpen() const {
    return isWindowOpen && settingWindow != nullptr;
}

// 이벤트 처리 루프
void SettingsUI::processEvents() {
    if (!isOpen()) return;

    while (std::optional<sf::Event> event = settingWindow->pollEvent()) {
        // 설정 창 닫기 이벤트
        if (event->is<sf::Event::Closed>()) {
            close();
            return;
        }

        // 마우스 클릭 이벤트 처리
        if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                // 확인/취소 버튼 위치 정보
                sf::FloatRect confirmButtonArea = confirmButtonSprite.getGlobalBounds();
                sf::FloatRect cancelButtonArea = cancelButtonSprite.getGlobalBounds();

                //확인 버튼 클릭 시 설정 적용
                if (confirmButtonArea.contains(mousePosF)) {
                    std::cout << "저장 버튼 클릭 전: 배경음 " << settings.getBgmVol()
                        << "%, 효과음 " << settings.getSfVol()
                        << "%, 글자 크기: " << settings.getFontSize() << std::endl;

                    // 실제 설정값에 임시값 반영
                    settings.confirmSettings();

                    // 글꼴 크기에 맞춰 창 높이 재계산
                    int actualFontSize = 18 + (settings.getFontSize() * 3);
                    float newWindowHeight = 300.f + (actualFontSize - 18) * 2.f;

                    // 창 위치 기억하고 새 창 생성
                    sf::Vector2i currentPosition = settingWindow->getPosition();

                    // 배경음 볼륨 적용
                    settingWindow->close();
                    settingWindow.reset();

                    settingWindow = std::make_unique<sf::RenderWindow>(
                        sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth),
                                       static_cast<unsigned int>(newWindowHeight) }),
                        L"사용자 설정"
                    );

                    // 파일로 설정 저장
                    settingWindow->setPosition(currentPosition);

                    std::cout << "저장 후: 배경음 " << settings.getBgmVol()
                        << "%, 효과음 " << settings.getSfVol()
                        << "%, 글자 크기: " << settings.getFontSize() << std::endl;

                    // 배경음 볼륨 적용
                    bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));

                    // 파일로 설정 저장
                    settings.saveSettings("settings.ini");

                    // 임시값 초기화
                    settings.initTempSettings();
                }

                // 취소 버튼 클릭 시 임시값 되돌리기
                if (cancelButtonArea.contains(mousePosF)) {
                    std::cout << "취소 버튼 클릭: 현재 임시값 = 배경음 " << settings.getTempBgmvol()
                        << "%, 효과음 " << settings.getTempSfVol()
                        << "%, 글자 크기: " << settings.getTempFontSize() << std::endl;

                    // 임시값 초기화
                    settings.cancelSettings();

                    std::cout << "취소 완료: 임시값이 원래값으로 복원됨 = 배경음 " << settings.getTempBgmvol()
                        << "%, 효과음 " << settings.getTempSfVol()
                        << "%, 글자 크기: " << settings.getTempFontSize() << std::endl;

                    // 슬라이더 드래그 상태 초기화
                    isDraggingBgmSlider = isDraggingSfxSlider = isDraggingFontSizeSlider = false;

                    // 배경음 볼륨 원래대로 적용
                    bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));
                }

                // 슬라이더 핸들 사이즈
                sf::Vector2f handleSize(15.f, 20.f);

                // 배경음 슬라이더 영역 계산
                sf::Vector2f bgmHandlePos(
                    sliderStartX + settings.getTempBgmvol() / 100.0f * sliderWidth - 7.5f,
                    70.f
                );
                sf::FloatRect bgmHandleArea(bgmHandlePos, handleSize);
                sf::FloatRect bgmSliderArea(sf::Vector2f(sliderStartX, 65.f), sf::Vector2f(sliderWidth, 20.f));

                // 효과음 슬라이더 영역
                sf::Vector2f sfxHandlePos(
                    sliderStartX + settings.getTempSfVol() / 100.0f * sliderWidth - 7.5f,
                    140.f
                );
                sf::FloatRect sfxHandleArea(sfxHandlePos, handleSize);
                sf::FloatRect sfxSliderArea(sf::Vector2f(sliderStartX, 135.f), sf::Vector2f(sliderWidth, 20.f));

                // 글자 크기 슬라이더 영역
                float fontSizePos = settings.getTempFontSize() / 8.0f;
                sf::Vector2f fontSizeHandlePos(
                    sliderStartX + fontSizePos * sliderWidth - 8.f,
                    215.f
                );
                sf::FloatRect fontSizeHandleArea(fontSizeHandlePos, sf::Vector2f(16.f, 16.f));
                sf::FloatRect fontSizeSliderArea(sf::Vector2f(sliderStartX, 210.f), sf::Vector2f(sliderWidth, 20.f));

                // 배경음 슬라이더 클릭
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

                // 효과음 슬라이더 클릭
                if (sfxHandleArea.contains(mousePosF) || sfxSliderArea.contains(mousePosF)) {
                    isDraggingSfxSlider = true;
                    if (sfxSliderArea.contains(mousePosF)) {
                        float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                        int newVolume = static_cast<int>((newPos - sliderStartX) / sliderWidth * 100.f);
                        newVolume = std::max(0, std::min(100, newVolume));
                        settings.setTempSfVol(newVolume);
                    }
                }

                //  글자 크기 슬라이더 클릭
                if (fontSizeHandleArea.contains(mousePosF) || fontSizeSliderArea.contains(mousePosF)) {
                    isDraggingFontSizeSlider = true;
                    if (fontSizeSliderArea.contains(mousePosF)) {
                        float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                        int newFontSize = static_cast<int>((newPos - sliderStartX) / sliderWidth * 8.f);
                        newFontSize = std::max(0, std::min(8, newFontSize));
                        settings.setTempFontSize(newFontSize);
                    }
                }

                // 소리 버튼 클릭 시 소리 재생
                sf::FloatRect testButtonBounds(sf::Vector2f(400.f, 140.f), sf::Vector2f(60.f, 20.f));
                if (testButtonBounds.contains(mousePosF)) {
                    //settings.playSound("win");
                }
            }
        }

        // 마우스 버튼 뗐을 때 드래그 상태 해제
        if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                isDraggingBgmSlider = false;
                isDraggingSfxSlider = false;
                isDraggingFontSizeSlider = false;
            }
        }

        // 마우스 이동 시 슬라이더 드래그 반응
        if (const auto* mouseEvent = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
            float mouseX = static_cast<float>(mousePos.x);
            float sliderPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mouseX));

            // 배경음 슬라이더 드래그 처리
            if (isDraggingBgmSlider) {
                int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                newVolume = std::max(0, std::min(100, newVolume));
                settings.setTempBgmVol(newVolume);
                bgMusic.setVolume(static_cast<float>(newVolume));
            }

            // 효과음 슬라이더 드래그 처리
            if (isDraggingSfxSlider) {
                int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                newVolume = std::max(0, std::min(100, newVolume));
                settings.setTempSfVol(newVolume);
            }

            // 글꼴 크기 슬라이더 드래그 처리
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

    settingWindow->clear(sf::Color(230, 240, 255)); // 창 배경 색상

     // 폰트가 정상 로드되었는지 확인
    bool fontLoaded = (font.getInfo().family != "");

    if (fontLoaded) {
        // 배경음 글자
        sf::Text bgmText(font);
        bgmText.setString(L"배경음");
        bgmText.setCharacterSize(20);
        bgmText.setFillColor(sf::Color::Black);
        bgmText.setPosition({ 30.f, 70.f });
        settingWindow->draw(bgmText);

        //슬라이더 배경 바
        sf::RectangleShape bgmSliderBg(sf::Vector2f(sliderWidth, 10.f));
        bgmSliderBg.setPosition({ sliderStartX, 75.f });
        bgmSliderBg.setFillColor(sf::Color(180, 180, 180));
        bgmSliderBg.setOutlineThickness(1);
        bgmSliderBg.setOutlineColor(sf::Color::Black);
        settingWindow->draw(bgmSliderBg);

        // 슬라이더 핸들 위치 계산
        float bgmVolPos = settings.getTempBgmvol() / 100.0f;
        sf::RectangleShape bgmSliderHandle(sf::Vector2f(15.f, 20.f));
        bgmSliderHandle.setPosition({ sliderStartX + bgmVolPos * sliderWidth - 7.5f, 70.f });
        bgmSliderHandle.setFillColor(sf::Color(100, 100, 255));
        bgmSliderHandle.setOutlineThickness(1);
        bgmSliderHandle.setOutlineColor(sf::Color::Black);
        settingWindow->draw(bgmSliderHandle);

        // 볼륨 값 텍스트
        sf::Text bgmVolumeText(font);
        bgmVolumeText.setString(std::to_wstring(settings.getTempBgmvol()) + L"%");
        bgmVolumeText.setCharacterSize(16);
        bgmVolumeText.setFillColor(sf::Color::Black);
        bgmVolumeText.setPosition({ 330.f, 73.f });
        settingWindow->draw(bgmVolumeText);

        // 효과음 슬라이더 UI 
        // 효과음 글자
        sf::Text sfxText(font);
        sfxText.setString(L"효과음");
        sfxText.setCharacterSize(20);
        sfxText.setFillColor(sf::Color::Black);
        sfxText.setPosition({ 30.f, 140.f });
        settingWindow->draw(sfxText);

        // 슬라이더 배경 막대
        sf::RectangleShape sfxSliderBg(sf::Vector2f(sliderWidth, 10.f));
        sfxSliderBg.setPosition({ sliderStartX, 145.f });
        sfxSliderBg.setFillColor(sf::Color(180, 180, 180));
        sfxSliderBg.setOutlineThickness(1);
        sfxSliderBg.setOutlineColor(sf::Color::Black);
        settingWindow->draw(sfxSliderBg);

        // 슬라이더 핸들 위치 계산 및 그리기
        float sfxVolPos = settings.getTempSfVol() / 100.0f;
        sf::RectangleShape sfxSliderHandle(sf::Vector2f(15.f, 20.f));
        sfxSliderHandle.setPosition({ sliderStartX + sfxVolPos * sliderWidth - 7.5f, 140.f });
        sfxSliderHandle.setFillColor(sf::Color(255, 100, 100));
        sfxSliderHandle.setOutlineThickness(1);
        sfxSliderHandle.setOutlineColor(sf::Color::Black);
        settingWindow->draw(sfxSliderHandle);


        // 현재 효과음 볼륨 텍스트 표시
        sf::Text sfxVolumeText(font);
        sfxVolumeText.setString(std::to_wstring(settings.getTempSfVol()) + L"%");
        sfxVolumeText.setCharacterSize(16);
        sfxVolumeText.setFillColor(sf::Color::Black);
        sfxVolumeText.setPosition({ 330.f, 143.f });
        settingWindow->draw(sfxVolumeText);

        //효과음 테스트 버튼 UI
        sf::RectangleShape sfxTestButton(sf::Vector2f(60.f, 20.f));
        sfxTestButton.setPosition({ 400.f, 140.f });
        sfxTestButton.setFillColor(sf::Color(220, 220, 220));
        sfxTestButton.setOutlineThickness(1);
        sfxTestButton.setOutlineColor(sf::Color::Black);
        settingWindow->draw(sfxTestButton);

        // 테스트 버튼 텍스트
        sf::Text sfxTestText(font);
        sfxTestText.setString(L"테스트");
        sfxTestText.setCharacterSize(14);
        sfxTestText.setFillColor(sf::Color::Black);
        sfxTestText.setPosition({ 405.f, 143.f });
        settingWindow->draw(sfxTestText);

        //-------글자 크기 슬라이더--------
        // 라벨 텍스트
        sf::Text fontSizeText(font);
        fontSizeText.setString(L"글자크기");
        fontSizeText.setCharacterSize(20);
        fontSizeText.setFillColor(sf::Color::Black);
        fontSizeText.setPosition({ 30.f, 210.f });
        settingWindow->draw(fontSizeText);

        // 슬라이더 선
        sf::RectangleShape fontSizeSliderLine(sf::Vector2f(sliderWidth, 2.f));
        fontSizeSliderLine.setPosition({ sliderStartX, 220.f });
        fontSizeSliderLine.setFillColor(sf::Color(100, 100, 100));
        settingWindow->draw(fontSizeSliderLine);

        // 눈금 표시 (총 9칸)
        for (int i = 0; i < 9; i++) {
            sf::RectangleShape tick(sf::Vector2f(2.f, (i % 4 == 0) ? 10.f : 5.f));
            tick.setPosition({ sliderStartX + (i * 25.f), 215.f });
            tick.setFillColor(sf::Color::Black);
            settingWindow->draw(tick);
        }

        // "작게", "크게" 레이블 텍스트
        sf::Text minSizeLabel(font);
        minSizeLabel.setString(L"작게");
        minSizeLabel.setCharacterSize(10);
        minSizeLabel.setFillColor(sf::Color::Red);
        minSizeLabel.setPosition({ 110.f, 240.f });
        settingWindow->draw(minSizeLabel);

        sf::Text maxSizeLabel(font);
        maxSizeLabel.setString(L"크게");
        maxSizeLabel.setCharacterSize(10);
        maxSizeLabel.setFillColor(sf::Color::Red);
        maxSizeLabel.setPosition({ 310.f, 240.f });
        settingWindow->draw(maxSizeLabel);

        // 슬라이더 핸들 (원형)
        float fontSizePos = settings.getTempFontSize() / 8.0f;
        sf::CircleShape fontSizeHandle(8.f);
        fontSizeHandle.setPosition({ sliderStartX + fontSizePos * sliderWidth - 8.f, 215.f });

        sf::Color fillColor(12, 124, 197);
        sf::Color outlineColor(255, 225, 255, 180);
        fontSizeHandle.setFillColor(fillColor);
        fontSizeHandle.setOutlineThickness(2.f);
        fontSizeHandle.setOutlineColor(outlineColor);
        settingWindow->draw(fontSizeHandle);

        // 현재 글자 크기 예시 ("가")
        sf::Text currentSizeExample(font);
        currentSizeExample.setString(L"가");
        int actualFontSize = 18 + (settings.getTempFontSize() * 3);
        currentSizeExample.setCharacterSize(actualFontSize);
        currentSizeExample.setFillColor(sf::Color::Black);
        currentSizeExample.setPosition({ 400.f, 210.f });
        settingWindow->draw(currentSizeExample);

        // 현재 슬라이더 값 숫자 표시
        sf::Text currentValueText(font);
        currentValueText.setString(std::to_wstring(settings.getTempFontSize()));
        currentValueText.setCharacterSize(14);
        currentValueText.setFillColor(sf::Color::Black);
        currentValueText.setPosition({ 115.f + fontSizePos * sliderWidth, 190.f });
        settingWindow->draw(currentValueText);

        // 버튼 텍스트 객체 생성
        sf::Text cancelText(font);
        cancelText.setString(L"취소");
        cancelText.setCharacterSize(12);
        cancelText.setFillColor(sf::Color::Black);

        sf::Text confirmText(font);
        confirmText.setString(L"저장");
        confirmText.setCharacterSize(12);
        confirmText.setFillColor(sf::Color::Black);

        // 버튼 위치 및 크기 설정
        // 창 사이즈 기준으로 버튼 위치 계산
        float windowWidth = static_cast<float>(settingWindow->getSize().x);
        float windowHeight = static_cast<float>(settingWindow->getSize().y);
        float buttonSpacing = 10.f;

        // 버튼 위치 설정 (위에서 이미 scale이 설정되었으므로 여기서는 다시 설정하지 않음)
        cancelButtonSprite.setPosition({
            windowWidth - (confirmButtonTexture.getSize().x * 0.1f * 2) - buttonSpacing - 20.f,
            windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
            });

        confirmButtonSprite.setPosition({
            windowWidth - (confirmButtonTexture.getSize().x * 0.1f) - 20.f,
            windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
            });

        // 버튼 사이즈
        // 텍스트를 버튼 중앙에 위치시키기 위한 계산
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

        // 버튼 및 텍스트 렌더링
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