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

// 텍스처, 폰트, 음악 선언
sf::Texture texture;
sf::Texture confirmButtonTexture;
sf::Texture cancelButtonTexture;
sf::Font settingsFont;
sf::Music bgMusic;
Settings settings;

//타자 연습 UI 초기화 함수 --> 주석처리 할 예정~~!!!
sf::Text practiceText(settingsFont, "", 15);      // 연습할 텍스트
sf::Text userInputText(settingsFont, "");     // 사용자 입력 텍스트
sf::RectangleShape inputBox; // 입력 박스
std::string currentInput = "";  // 현재 입력 내용
std::wstring practiceContent = L"타자 연습을 시작합니다. 이 텍스트의 크기는 설정에서 변경할 수 있습니다."; // 연습 내용

// 타자 연습 UI 초기화 함수
//void initTypingUI() {
//    // 연습 텍스트 설정
//    practiceText.setFont(settingsFont);
//    practiceText.setString(practiceContent);
//    practiceText.setCharacterSize(settings.getActualFontSize()); // 설정된 폰트 크기 적용
//    practiceText.setFillColor(sf::Color::White);
//    practiceText.setPosition({ 100.f, 200.f });
//
//    // 사용자 입력 텍스트 설정
//    userInputText.setFont(settingsFont);
//    userInputText.setString(currentInput);
//    userInputText.setCharacterSize(settings.getActualFontSize()); // 설정된 폰트 크기 적용
//    userInputText.setFillColor(sf::Color::Green);
//    userInputText.setPosition({ 100.f, 250.f });
//
//    // 입력 박스 설정 (고정 크기)
//    inputBox.setSize(sf::Vector2f(600.f, 50.f));
//    inputBox.setPosition({ 90.f, 245.f });
//    inputBox.setFillColor(sf::Color(30, 30, 30));
//    inputBox.setOutlineThickness(2.f);
//    inputBox.setOutlineColor(sf::Color(100, 100, 100));
//}

// 타자 연습 UI 업데이트 함수 (폰트 크기 변경 시 호출)
//void updateTypingUI() {
//    // 폰트 크기 업데이트
//    practiceText.setCharacterSize(settings.getActualFontSize());
//    userInputText.setCharacterSize(settings.getActualFontSize());
//
//    // 위치는 그대로 유지하되, 텍스트 크기만 변경
//    practiceText.setPosition({ 100.f, 200.f });
//    userInputText.setPosition({ 100.f, 250.f });
//
//    // 위치 재조정 (필요시)
//    practiceText.setPosition({ 100.f, 200.f });
//    userInputText.setPosition({ 100.f, 250.f + settings.getActualFontSize() });
//    inputBox.setPosition({ 90.f, 245.f + settings.getActualFontSize() });
//}

int main() {
   

    // 설정창 및 상수 선언
    const float settingWindowWidth = 500.f;  // 설정 창 너비
    const float sliderStartX = 120.f;        // 슬라이더 시작 X 좌표
    const float sliderWidth = 200.f;         // 슬라이더 너비

    // 프로그램 시작 시 설정 파일 로드
    if (settings.loadSettings("settings.ini")) {
        cout << "설정기능--> 저장된 설정을 불러왔습니다" << endl;
    }
    else {
        cout << "기본 설정을 사용합니다" << endl;
    }

    // (1) 메인 창 생성
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), L"타자연습");

    // (2) 리소스 로드
    // 2.1 설정 아이콘 로드
    if (!texture.loadFromFile("assets/images/setting_icon.png")) {
        cout << "설정 아이콘 로드 실패" << endl;
        return -1;
    }
    cout << "설정 아이콘 로드 성공" << endl;
    sf::Sprite sprite(texture); // 스프라이트 생성

    // 2.2 폰트 로드
    if (!settingsFont.openFromFile("assets/fonts/D2Coding.ttf")) {
        cout << "폰트 로드 실패" << endl;
    }
    else {
        cout << "폰트 로드 성공" << endl;

        // Text 객체 초기화
        practiceText.setFont(settingsFont);
        userInputText.setFont(settingsFont);


		//initTypingUI(); // 타자 연습 UI 초기화 ----------> 주석처리 예정
    }

    // 2.3 배경음 로드
    if (!bgMusic.openFromFile("assets/audio/music3.ogg")) {
        cout << "배경음 로드 실패" << endl;
        return -1;
    }
    cout << "배경음 로드 성공" << endl;
    bgMusic.setLooping(true); // 반복 재생
    bgMusic.setVolume(static_cast<float>(settings.getBgmVol())); // 저장된 볼륨값 적용
    bgMusic.play(); // 재생 시작

    // 2.4 효과음 로드
    cout << "\n========== 효과음 로드 시작 ==========\n" << endl;
    const string soundEffects[] = { "win", "lose", "error", "collect" };
    for (const auto& effect : soundEffects) {
        string path = "assets/audio/soundEffect/" + effect;
        if (effect == "error" || effect == "collect") path += "2";
        path += ".ogg";

        bool loaded = settings.loadSound(effect, path);
        if (loaded) {
            cout << effect << " 효과음 로드 성공" << endl;
        }
        else {
            cout << effect << " 효과음 로드 실패" << endl;
        }
    }
    cout << "\n========== 효과음 로드 완료 ==========\n" << endl;

    // 2.5 버튼 이미지 로드
    cout << "\n========== 버튼 이미지 로드 시작 ==========\n" << endl;

    if (!confirmButtonTexture.loadFromFile("assets/images/orange.png")) {
        cout << "확인 버튼 이미지 로드 실패" << endl;
    }
    else {
        cout << "확인 버튼 이미지 로드 성공" << endl;
    }
    sf::Sprite confirmButtonSprite(confirmButtonTexture);
    confirmButtonSprite.setScale({ 0.5f, 0.5f });

    if (!cancelButtonTexture.loadFromFile("assets/images/blue.png")) {
        cout << "취소 버튼 이미지 로드 실패" << endl;
    }
    else {
        cout << "취소 버튼 이미지 로드 성공" << endl;
    }
    sf::Sprite cancelButtonSprite(cancelButtonTexture);
    cancelButtonSprite.setScale({ 0.5f, 0.5f });

    cout << "\n========== 버튼 이미지 로드 완료 ==========\n" << endl;

    // (3) 스프라이트 설정 및 배치
    sprite.setScale({ 0.2f, 0.2f });

    // 고정 값 사용 (실제 아이콘 크기에 맞게 조정)
    float fixedWidth = 100.0f;  // 아이콘의 예상 너비
    float fixedHeight = 100.0f; // 아이콘의 예상 높이

    // 아이콘을 창의 오른쪽 하단에 배치
    sprite.setPosition({
        window.getSize().x - fixedWidth - 30.f,  // 오른쪽 가장자리에서 30픽셀 안쪽
        window.getSize().y - fixedHeight - 30.f  // 하단 가장자리에서 30픽셀 위
        });

    // (4) 상태 변수 선언
    bool settingWindowOpen = false;
    std::unique_ptr<sf::RenderWindow> settingWindow = nullptr;

    // 슬라이더 드래그 상태 변수
    bool isDraggingBgmSlider = false;
    bool isDraggingSfxSlider = false;
    bool isDraggingFontSizeSlider = false;

    // 버튼 상태 변수
    bool isConfirmButtonHovered = false;
    bool isCancelButtonHovered = false;
    bool isConfirmButtonPressed = false;
    bool isCancelButtonPressed = false;

    // (5) 메인루프
    while (window.isOpen()) {
        // 5.1 메인 창 이벤트 처리
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                settings.saveSettings("settings.ini");
                bgMusic.stop();
                window.close();
            }

            // 마우스 클릭 감지 및 설정창 열기
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    // 설정 아이콘 클릭 시 설정 창 열기
                    if (sprite.getGlobalBounds().contains(mousePosF) && !settingWindowOpen) {
                        settingWindowOpen = true;
                        settingWindow = std::make_unique<sf::RenderWindow>(
                            sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth), 300 }),
                            L"사용자 설정"
                        );
                        //임시 설정값 초기화
                        settings.initTempSettings();
                    }
                }
            }
        }

           


        // 5.2 설정 창 이벤트 처리
        if (settingWindowOpen && settingWindow) {
            while (std::optional<sf::Event> settingsEvent = settingWindow->pollEvent()) {
                // 설정 창 닫기 버튼 처리
                if (settingsEvent->is<sf::Event::Closed>()) {
                    settingWindow->close();
                    settingWindow.reset();
                    settingWindowOpen = false;
                    break;
                }

                // 마우스 버튼 누름 이벤트 처리
                if (const auto* mouseEvent = settingsEvent->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
                        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                        // 버튼 영역 계산
                        sf::FloatRect confirmButtonArea = confirmButtonSprite.getGlobalBounds();
                        sf::FloatRect cancelButtonArea = cancelButtonSprite.getGlobalBounds();

                        // 확인(저장) 버튼 클릭 처리
                        if (confirmButtonArea.contains(mousePosF)) {

                            // 변경 전 값 로그 출력 (디버깅용)
                            cout << "저장 버튼 클릭 전: 배경음 " << settings.getBgmVol()
                                << "%, 효과음 " << settings.getSfVol()
                                << "%, 글자 크기: " << settings.getFontSize() << endl;

                            // 설정값 저장
                            settings.confirmSettings();
                            //updateTypingUI(); // 폰트 크기 변경 후 UI 업데이트

                            // 글자 크기에 따라 설정창 높이 동적 조정
                            int actualFontSize = 18 + (settings.getFontSize() * 3);
                            float newWindowHeight = 300.f + (actualFontSize - 18) * 2.f; // 기본 높이에 추가 높이 더하기

                            // 현재 설정창의 위치 저장
                            sf::Vector2i currentPosition = settingWindow->getPosition();

                            // 설정창 닫기
                            settingWindow->close();
                            settingWindow.reset();


                            // 새 설정창 생성
                            settingWindow = std::make_unique<sf::RenderWindow>(
                                sf::VideoMode({ static_cast<unsigned int>(settingWindowWidth),
                                                static_cast<unsigned int>(newWindowHeight) }),
                                L"사용자 설정"
                            );

                            // 이전 위치로 새 창 이동
                            settingWindow->setPosition(currentPosition);

                            // 변경 후 값 로그 출력 (디버깅용)
                            cout << "저장 후: 배경음 " << settings.getBgmVol()
                                << "%, 효과음 " << settings.getSfVol()
                                << "%, 글자 크기: " << settings.getFontSize() << endl;

                            // 배경음 볼륨 설정 적용
                            bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));

                            // 설정 파일에 저장
                            settings.saveSettings("settings.ini");

                            // 임시 설정값 초기화
                            settings.initTempSettings();

                            settingWindowOpen = true;
                        }

                        // 취소 버튼 클릭 처리
                        if (cancelButtonArea.contains(mousePosF)) {
                            cout << "취소 버튼 클릭: 현재 임시값 = 배경음 " << settings.getTempBgmvol()
                                << "%, 효과음 " << settings.getTempSfVol()
                                << "%, 글자 크기: " << settings.getTempFontSize() << endl;

                            // 설정값을 이전 저장값으로 되돌림 (실제값 -> 임시값)
                            settings.cancelSettings();

                            cout << "취소 완료: 임시값이 원래값으로 복원됨 = 배경음 " << settings.getTempBgmvol()
                                << "%, 효과음 " << settings.getTempSfVol()
                                << "%, 글자 크기: " << settings.getTempFontSize() << endl;

                            // 드래그 상태 초기화
                            isDraggingBgmSlider = isDraggingSfxSlider = isDraggingFontSizeSlider = false;

                            // 배경음 볼륨 실시간 업데이트
                            bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));
                        }

                        // 슬라이더 핸들 영역 계산
                        sf::Vector2f handleSize(15.f, 20.f);

                        // 배경음 슬라이더 영역
                        sf::Vector2f bgmHandlePos(
                            sliderStartX + settings.getTempBgmvol() / 100.0f * sliderWidth - 7.5f, // getBgmVol -> getTempBgmvol
                            70.f
                        );
                        sf::FloatRect bgmHandleArea(bgmHandlePos, handleSize);
                        sf::FloatRect bgmSliderArea(sf::Vector2f(sliderStartX, 65.f), sf::Vector2f(sliderWidth, 20.f));

                        // 효과음 슬라이더 영역
                        sf::Vector2f sfxHandlePos(
                            sliderStartX + settings.getTempSfVol() / 100.0f * sliderWidth - 7.5f, // getSfVol -> getTempSfVol
                            140.f
                        );
                        sf::FloatRect sfxHandleArea(sfxHandlePos, handleSize);
                        sf::FloatRect sfxSliderArea(sf::Vector2f(sliderStartX, 135.f), sf::Vector2f(sliderWidth, 20.f));

                        // 글자 크기 슬라이더 영역
                        float fontSizePos = settings.getTempFontSize() / 8.0f; // getFontSize -> getTempFontSize
                        sf::Vector2f fontSizeHandlePos(
                            sliderStartX + fontSizePos * sliderWidth - 8.f,
                            215.f
                        );
                        sf::FloatRect fontSizeHandleArea(fontSizeHandlePos, sf::Vector2f(16.f, 16.f));
                        sf::FloatRect fontSizeSliderArea(sf::Vector2f(sliderStartX, 210.f), sf::Vector2f(sliderWidth, 20.f));

                        // 배경음 슬라이더 클릭 처리
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

                        // 효과음 슬라이더 클릭 처리
                        if (sfxHandleArea.contains(mousePosF) || sfxSliderArea.contains(mousePosF)) {
                            isDraggingSfxSlider = true;
                            if (sfxSliderArea.contains(mousePosF)) {
                                float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                                int newVolume = static_cast<int>((newPos - sliderStartX) / sliderWidth * 100.f);
                                newVolume = std::max(0, std::min(100, newVolume));
                                settings.setTempSfVol(newVolume); // setSfVol -> setTempSfVol
                            }
                        }

                        // 글자 크기 슬라이더 클릭 처리
                        if (fontSizeHandleArea.contains(mousePosF) || fontSizeSliderArea.contains(mousePosF)) {
                            isDraggingFontSizeSlider = true;
                            if (fontSizeSliderArea.contains(mousePosF)) {
                                float newPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mousePosF.x));
                                int newFontSize = static_cast<int>((newPos - sliderStartX) / sliderWidth * 8.f);
                                newFontSize = std::max(0, std::min(8, newFontSize));
                                settings.setTempFontSize(newFontSize); // setFontSize -> setTempFontSize
                            }
                        }

                        // 효과음 테스트 버튼 클릭 처리
                        sf::FloatRect testButtonBounds(sf::Vector2f(400.f, 140.f), sf::Vector2f(60.f, 20.f));
                        if (testButtonBounds.contains(mousePosF)) {
                            settings.playSound("win");
                        }
                    }
                }

                // 마우스 버튼 놓음 이벤트 처리
                if (const auto* mouseEvent = settingsEvent->getIf<sf::Event::MouseButtonReleased>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        isDraggingBgmSlider = false;
                        isDraggingSfxSlider = false;
                        isDraggingFontSizeSlider = false;
                    }
                }

                // 마우스 이동 이벤트 처리
                if (const auto* mouseEvent = settingsEvent->getIf<sf::Event::MouseMoved>()) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(*settingWindow);
                    float mouseX = static_cast<float>(mousePos.x);
                    float sliderPos = std::max(sliderStartX, std::min(sliderStartX + sliderWidth, mouseX));

                    // 배경음 슬라이더 드래그 처리
                    if (isDraggingBgmSlider) {
                        int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                        newVolume = std::max(0, std::min(100, newVolume));
                        settings.setTempBgmVol(newVolume); // setBgmVol -> setTempBgmVol
                        bgMusic.setVolume(static_cast<float>(newVolume));
                    }

                    // 효과음 슬라이더 드래그 처리
                    if (isDraggingSfxSlider) {
                        int newVolume = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 100.f);
                        newVolume = std::max(0, std::min(100, newVolume));
                        settings.setTempSfVol(newVolume); // setSfVol -> setTempSfVol
                    }

                    // 글자 크기 슬라이더 드래그 처리
                    if (isDraggingFontSizeSlider) {
                        int newFontSize = static_cast<int>((sliderPos - sliderStartX) / sliderWidth * 8.f);
                        newFontSize = std::max(0, std::min(8, newFontSize));
                        settings.setTempFontSize(newFontSize); // setFontSize -> setTempFontSize
                    }
                }
            }

            // 5.3 설정 창 렌더링
            if (settingWindow) {
                settingWindow->clear(sf::Color(230, 240, 255));

                // 폰트 사용 가능 여부 확인 (한 번만)
                static bool fontChecked = false;
                static bool fontLoaded = false;

                if (!fontChecked) {
                    fontLoaded = (settingsFont.getInfo().family != "");
                    fontChecked = true;
                    cout << (fontLoaded ? "폰트 사용 가능" : "폰트 사용 불가") << endl;
                }

                if (fontLoaded) {
                    // 배경음 슬라이더 렌더링
                    sf::Text bgmText(settingsFont);
                    bgmText.setString(L"배경음");
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

                    // 효과음 슬라이더 렌더링
                    sf::Text sfxText(settingsFont);
                    sfxText.setString(L"효과음");
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
                    sfxTestText.setString(L"테스트");
                    sfxTestText.setCharacterSize(14);
                    sfxTestText.setFillColor(sf::Color::Black);
                    sfxTestText.setPosition({ 405.f, 143.f });
                    settingWindow->draw(sfxTestText);

                    // 글자 크기 슬라이더 렌더링
                    sf::Text fontSizeText(settingsFont);
                    fontSizeText.setString(L"글자크기");
                    fontSizeText.setCharacterSize(20);
                    fontSizeText.setFillColor(sf::Color::Black);
                    fontSizeText.setPosition({ 30.f, 210.f });
                    settingWindow->draw(fontSizeText);

                    sf::RectangleShape fontSizeSliderLine(sf::Vector2f(sliderWidth, 2.f));
                    fontSizeSliderLine.setPosition({ sliderStartX, 220.f });
                    fontSizeSliderLine.setFillColor(sf::Color(100, 100, 100));
                    settingWindow->draw(fontSizeSliderLine);

                    // 눈금 표시 (9단계)
                    for (int i = 0; i < 9; i++) {
                        sf::RectangleShape tick(sf::Vector2f(2.f, (i % 4 == 0) ? 10.f : 5.f));
                        tick.setPosition({ sliderStartX + (i * 25.f), 215.f });
                        tick.setFillColor(sf::Color::Black);
                        settingWindow->draw(tick);
                    }

                    // 최소/최대 레이블
                    sf::Text minSizeLabel(settingsFont);
                    minSizeLabel.setString(L"작게");
                    minSizeLabel.setCharacterSize(10);
                    minSizeLabel.setFillColor(sf::Color::Red);
                    minSizeLabel.setPosition({ 110.f, 240.f });
                    settingWindow->draw(minSizeLabel);

                    sf::Text maxSizeLabel(settingsFont);
                    maxSizeLabel.setString(L"크게");
                    maxSizeLabel.setCharacterSize(10);
                    maxSizeLabel.setFillColor(sf::Color::Red);
                    maxSizeLabel.setPosition({ 310.f, 240.f });
                    settingWindow->draw(maxSizeLabel);

                    // 글자 크기 슬라이더 핸들
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

                    // 현재 글자 크기 예시
                    sf::Text currentSizeExample(settingsFont);
                    currentSizeExample.setString(L"가");
                    // getFontSize -> getTempFontSize
                    int actualFontSize = 18 + (settings.getTempFontSize() * 3);
                    currentSizeExample.setCharacterSize(actualFontSize);
                    currentSizeExample.setFillColor(sf::Color::Black);
                    currentSizeExample.setPosition({ 400.f, 210.f });
                    settingWindow->draw(currentSizeExample);

                    // 현재 선택된 값 표시
                    sf::Text currentValueText(settingsFont);
                    // getFontSize -> getTempFontSize
                    currentValueText.setString(std::to_wstring(settings.getTempFontSize()));
                    currentValueText.setCharacterSize(14);
                    currentValueText.setFillColor(sf::Color::Black);
                    currentValueText.setPosition({ 115.f + fontSizePos * sliderWidth, 190.f });
                    settingWindow->draw(currentValueText);

                    //취소 버튼 텍스트
                    sf::Text cancelText(settingsFont);
                    cancelText.setString(L"취소");
                    cancelText.setCharacterSize(12);
                    cancelText.setFillColor(sf::Color::Black);

                    //저장 버튼 텍스트
                    sf::Text confirmText(settingsFont);
                    confirmText.setString(L"저장");
                    confirmText.setCharacterSize(12);
                    confirmText.setFillColor(sf::Color::Black);

                    // 저장/취소 버튼 렌더링
                    // 창 크기 가져오기
                    float windowWidth = static_cast<float>(settingWindow->getSize().x);
                    float windowHeight = static_cast<float>(settingWindow->getSize().y);
                    float buttonSpacing = 10.f; // 버튼 간격

                    cancelButtonSprite.setScale({ 0.1f, 0.1f });
                    confirmButtonSprite.setScale({ 0.1f, 0.1f });

                    // 버튼 위치
                    // 버튼 위치 계산
                    cancelButtonSprite.setPosition({
                        windowWidth - (confirmButtonTexture.getSize().x * 0.1f * 2) - buttonSpacing - 20.f,
                        windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
                        });

                    confirmButtonSprite.setPosition({
                        windowWidth - (confirmButtonTexture.getSize().x * 0.1f) - 20.f,
                        windowHeight - (confirmButtonTexture.getSize().y * 0.1f) - 10.f
                        });

                    // 버튼 크기 계산
                    sf::Vector2f cancelButtonSize(
                        cancelButtonTexture.getSize().x * 0.1f,
                        cancelButtonTexture.getSize().y * 0.1f
                    );

                    sf::Vector2f confirmButtonSize(
                        confirmButtonTexture.getSize().x * 0.1f,
                        confirmButtonTexture.getSize().y * 0.1f
                    );

                    // 한글 취소 텍스트(2글자)를 버튼 중앙에 배치
                    float cancelCenterX = cancelButtonSprite.getPosition().x + (cancelButtonSize.x / 2.0f);
                    float cancelCenterY = cancelButtonSprite.getPosition().y + (cancelButtonSize.y / 2.0f);

                    // 한글은 문자 크기의 배수로 조정
                    cancelText.setPosition({
                        cancelCenterX - static_cast<float>(cancelText.getCharacterSize()) * 1.0f,
                        cancelCenterY - static_cast<float>(cancelText.getCharacterSize()) * 0.5f
                        });

                    // 저장 텍스트(2글자)를 버튼 중앙에 배치 
                    float confirmCenterX = confirmButtonSprite.getPosition().x + (confirmButtonSize.x / 2.0f);
                    float confirmCenterY = confirmButtonSprite.getPosition().y + (confirmButtonSize.y / 2.0f);

                    // 한글은 문자 크기의 배수로 조정
                    confirmText.setPosition({
                        confirmCenterX - static_cast<float>(confirmText.getCharacterSize()) * 1.0f,
                        confirmCenterY - static_cast<float>(confirmText.getCharacterSize()) * 0.5f
                        });

                    // 버튼 렌더링
                    settingWindow->draw(cancelButtonSprite);
                    settingWindow->draw(confirmButtonSprite);
                    settingWindow->draw(cancelText);
                    settingWindow->draw(confirmText);
                }

                settingWindow->display();
            }
        }

        // 5.4 메인 창 렌더링
        window.clear(sf::Color::Black);
        window.draw(sprite);

        // 타자 연습 텍스트 렌더링 추가
        window.draw(practiceText);
        window.draw(userInputText);
        window.draw(inputBox);

        window.display();
    }

    return 0;
}