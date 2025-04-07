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
    // 설정 객체 생성 및 초기화
    Settings settings;

    if (settings.loadSettings("settings.ini")) {
        std::cout << "설정기능--> 저장된 설정을 불러왔습니다" << std::endl;
    }
    else {
        std::cout << "기본 설정을 사용합니다" << std::endl;
    }

    // 메인 창 생성
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), L"타자연습");

    // 폰트 로드
    sf::Font font;
    if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
        std::cout << "폰트 로드 실패" << std::endl;
        return -1;
    }
    std::cout << "폰트 로드 성공" << std::endl;

    // 배경음 로드 및 설정
    sf::Music bgMusic;
    if (!bgMusic.openFromFile("assets/audio/music3.ogg")) {
        std::cout << "배경음 로드 실패" << std::endl;
        return -1;
    }
    std::cout << "배경음 로드 성공" << std::endl;
    bgMusic.setLooping(true);                                        // 반복 재생 설정
    bgMusic.setVolume(static_cast<float>(settings.getBgmVol()));    // 설정값으로 볼륨 설정
    bgMusic.play();                                                  // 재생 시작



    // 효과음 로드
   /* std::cout << "\n========== 효과음 로드 시작 ==========\n" << std::endl;
    const std::string soundEffects[] = { "win", "lose", "error", "collect" };
    for (const auto& effect : soundEffects) {
        std::string path = "assets/audio/soundEffect/" + effect;
        if (effect == "error" || effect == "collect") path += "2";
        path += ".ogg";

        bool loaded = settings.loadSound(effect, path);
        if (loaded) {
            std::cout << effect << " 효과음 로드 성공" << std::endl;
        }
        else {
            std::cout << effect << " 효과음 로드 실패" << std::endl;
        }
    }
    std::cout << "\n========== 효과음 로드 완료 ==========\n" << std::endl;*/

    // 설정 UI 생성 및 리소스 로드
    SettingsUI settingsUI(settings, font, bgMusic);
    if (!settingsUI.loadResources()) {
        std::cout << "설정 UI 리소스 로드 실패" << std::endl;
        return -1;
    }

    // 설정 아이콘 생성 및 위치 설정
    SettingsIcon settingsIcon;
    settingsIcon.setPosition(window.getSize().x, window.getSize().y);

    // 타자 연습 UI 생성
    //TypingUI typingUI(font, settings);

    // 메인 루프
    while (window.isOpen()) {
        // 이벤트 처리 루프
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                // 창 닫을 때 설정 저장 및 배경음 정지
                settings.saveSettings("settings.ini");
                bgMusic.stop();
                window.close();
            }

            // 키보드 텍스트 입력 처리
           // typingUI.handleTextInput(*event);

            // 마우스 클릭 이벤트 처리
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    // 설정 아이콘 클릭 감지
                    if (settingsIcon.contains(mousePosF) && !settingsUI.isOpen()) {
                        settingsUI.openWindow();  // 설정 창 열기
                    }
                }
            }
        }

        // 설정 UI가 열려 있으면 해당 이벤트 처리 및 그리기
        if (settingsUI.isOpen()) {
            settingsUI.processEvents();  // 설정 창 내 이벤트 처리
            settingsUI.render();         // 설정 창 그리기

            //typingUI.update();           // 설정 변경이 반영되도록 타자 UI 업데이트
        }

        // 메인 창 렌더링
        window.clear(sf::Color::Black);   // 배경색 초기화
        //typingUI.render(window);          // 타자 연습 UI 그리기
        settingsIcon.render(window);      // 설정 아이콘 그리기
        window.display();                 // 렌더링 결과 표시
    }

    return 0;
}