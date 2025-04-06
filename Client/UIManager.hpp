#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class UIManager {
public:

    UIManager();

    // ��Ʈ �ε�
    bool loadFont(const std::string& filename);

    // �ε�� ��Ʈ ���� ��ȯ (��const ����)
    sf::Font& getFont();
    
    // �ε�� ��Ʈ ���� ��ȯ
    // @return �ε�� sf::Font ��ü ����
    const sf::Font& getFont() const;

    // �ؽ�ó �ε�
    bool loadTexture(const std::string& name, const std::string& filename);

    // ��������Ʈ ����
    sf::Sprite createSprite(const std::string& textureKey, sf::Vector2f position, sf::Vector2f size);

    // �ؽ�Ʈ ����
    sf::Text createText(const std::wstring& content, unsigned int fontSize, sf::Vector2f position, sf::Color color = sf::Color::Black);

    void centerTextOnSprite(const sf::Sprite& sprite, sf::Text& text);

    sf::Sprite createIcon(const std::string& path, sf::Vector2f position, sf::Vector2f scale);

    sf::RectangleShape createButton(sf::Vector2f size, sf::Vector2f position, sf::Color color);

private:
    sf::Font font;
    std::unordered_map<std::string, sf::Texture> textures;
};
