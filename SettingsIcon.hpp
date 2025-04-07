#ifndef SETTINGS_ICON_HPP
#define SETTINGS_ICON_HPP

#include <SFML/Graphics.hpp>
#include <iostream>

class SettingsIcon {
private:
    // 설정 아이콘 텍스처와 스프라이트를 스마트 포인터로 관리
    std::unique_ptr<sf::Texture> texture; // 아이콘 이미지 텍스처
    std::unique_ptr<sf::Sprite> sprite;   // 텍스처를 화면에 그릴 스프라이트

public:
    // 생성자: 아이콘 이미지 로드 및 스프라이트 설정
    SettingsIcon() {
        texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile("assets/images/setting_icon.png")) {
            std::cout << "설정 아이콘 로드 실패" << std::endl;
            return;
        }
        std::cout << "설정 아이콘 로드 성공" << std::endl;

        sprite = std::make_unique<sf::Sprite>(*texture);
        sprite->setScale({ 0.2f, 0.2f }); // 아이콘 크기 축소
    }

    // 아이콘 위치 설정 (창 크기를 기준으로 오른쪽 아래에 배치)
    void setPosition(float windowWidth, float windowHeight) {
        if (!sprite) return;
        float fixedWidth = 100.0f;   // 기준 너비
        float fixedHeight = 100.0f;  // 기준 높이
        sprite->setPosition({
            windowWidth - fixedWidth - 30.f,   // 오른쪽에서 30px 여백
            windowHeight - fixedHeight - 30.f  // 아래쪽에서 30px 여백
            });
    }

    // 아이콘 렌더링 (화면에 그리기)
    void render(sf::RenderWindow& window) {
        if (sprite) window.draw(*sprite);
    }

    // 특정 좌표가 아이콘 안에 포함되어 있는지 확인 (예: 마우스 클릭 위치)
    bool contains(const sf::Vector2f& point) const {
        return sprite && sprite->getGlobalBounds().contains(point);
    }
}; 
#endif // SETTINGS_ICON_HPP