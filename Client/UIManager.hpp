#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class UIManager {
public:

    UIManager();

    // 폰트 로드
    bool loadFont(const std::string& filename);

    // 로드된 폰트 참조 반환 (비const 버전)
    sf::Font& getFont();
    
    // 로드된 폰트 참조 반환
    // @return 로드된 sf::Font 객체 참조
    const sf::Font& getFont() const;

    // 텍스처 로드
    bool loadTexture(const std::string& name, const std::string& filename);

    // 스프라이트 생성
    sf::Sprite createSprite(const std::string& textureKey, sf::Vector2f position, sf::Vector2f size);

    // 텍스트 생성
    sf::Text createText(const std::wstring& content, unsigned int fontSize, sf::Vector2f position, sf::Color color = sf::Color::Black);

    void centerTextOnSprite(const sf::Sprite& sprite, sf::Text& text);

    sf::Sprite createIcon(const std::string& path, sf::Vector2f position, sf::Vector2f scale);

    sf::RectangleShape createButton(sf::Vector2f size, sf::Vector2f position, sf::Color color);

private:
    sf::Font font;
    std::unordered_map<std::string, sf::Texture> textures;
};
