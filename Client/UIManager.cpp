#include "UIManager.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace sf;

UIManager::UIManager() {}

bool UIManager::loadFont(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.good()) {
        std::cerr << "��Ʈ ������ �������� �ʽ��ϴ�: " << filename << std::endl;
        return false;
    }

    // 2. SFML ��Ʈ �ε� �õ�
    if (!font.openFromFile(filename)) {
        std::cerr << "SFML ��Ʈ �ε� ����: " << filename << std::endl;
        return false;
    }

    // 3. �ε� ���� �޽��� ���
    std::cout << "��Ʈ ���������� �ε��: " << filename << std::endl;
    return true;
}

// ��const ������ getFont
sf::Font& UIManager::getFont() {
    return font;
}

// ��Ʈ ���� ��ȯ �Լ� ����
const sf::Font& UIManager::getFont() const {
    return font;
}

bool UIManager::loadTexture(const std::string& name, const std::string& filename) {
    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }
    textures[name] = std::move(texture);
    return true;
}

sf::Sprite UIManager::createSprite(const std::string& textureKey, sf::Vector2f position, sf::Vector2f size) {
    auto it = textures.find(textureKey);
    if (it == textures.end()) {
        throw std::runtime_error("Texture not found: " + textureKey);
    }

    sf::Sprite sprite(it->second);

    sf::Vector2u texSize = it->second.getSize();
    float scaleX = size.x / texSize.x;
    float scaleY = size.y / texSize.y;

    sprite.setScale(Vector2(scaleX, scaleY));
    sprite.setPosition(position);

    return sprite;
}

sf::Text UIManager::createText(const std::wstring& content, unsigned int fontSize, sf::Vector2f position, sf::Color color) {
    sf::Text text(font, content, fontSize);
    text.setFillColor(color);
    text.setPosition(position);
    return text;
}

void UIManager::centerTextOnSprite(const sf::Sprite& sprite, sf::Text& text) {
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    sf::FloatRect textBounds = text.getGlobalBounds();

    text.setPosition(Vector2f(
        spriteBounds.position.x + (spriteBounds.size.x - textBounds.size.x) / 2,
        spriteBounds.position.y + (spriteBounds.size.y - textBounds.size.y) / 2
    ));
}

sf::Sprite UIManager::createIcon(const std::string& path, sf::Vector2f position, sf::Vector2f scale) {
    sf::Texture* texture = new sf::Texture(); // ����: �޸� ���� �ʿ�
    if (!texture->loadFromFile(path)) {
        std::cerr << "������ �̹��� �ε� ����: " << path << std::endl;
    }

    sf::Sprite sprite(*texture);
    sprite.setPosition(position);
    sprite.setScale(scale);
    return sprite;
}

sf::RectangleShape UIManager::createButton(sf::Vector2f size, sf::Vector2f position, sf::Color color) {
    sf::RectangleShape button(size);
    button.setFillColor(color);
    button.setPosition(position);
    return button;
}