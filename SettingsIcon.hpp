#ifndef SETTINGS_ICON_HPP
#define SETTINGS_ICON_HPP

#include <SFML/Graphics.hpp>
#include <iostream>

class SettingsIcon {
private:
    // ���� ������ �ؽ�ó�� ��������Ʈ�� ����Ʈ �����ͷ� ����
    std::unique_ptr<sf::Texture> texture; // ������ �̹��� �ؽ�ó
    std::unique_ptr<sf::Sprite> sprite;   // �ؽ�ó�� ȭ�鿡 �׸� ��������Ʈ

public:
    // ������: ������ �̹��� �ε� �� ��������Ʈ ����
    SettingsIcon() {
        texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile("assets/images/setting_icon.png")) {
            std::cout << "���� ������ �ε� ����" << std::endl;
            return;
        }
        std::cout << "���� ������ �ε� ����" << std::endl;

        sprite = std::make_unique<sf::Sprite>(*texture);
        sprite->setScale({ 0.2f, 0.2f }); // ������ ũ�� ���
    }

    // ������ ��ġ ���� (â ũ�⸦ �������� ������ �Ʒ��� ��ġ)
    void setPosition(float windowWidth, float windowHeight) {
        if (!sprite) return;
        float fixedWidth = 100.0f;   // ���� �ʺ�
        float fixedHeight = 100.0f;  // ���� ����
        sprite->setPosition({
            windowWidth - fixedWidth - 30.f,   // �����ʿ��� 30px ����
            windowHeight - fixedHeight - 30.f  // �Ʒ��ʿ��� 30px ����
            });
    }

    // ������ ������ (ȭ�鿡 �׸���)
    void render(sf::RenderWindow& window) {
        if (sprite) window.draw(*sprite);
    }

    // Ư�� ��ǥ�� ������ �ȿ� ���ԵǾ� �ִ��� Ȯ�� (��: ���콺 Ŭ�� ��ġ)
    bool contains(const sf::Vector2f& point) const {
        return sprite && sprite->getGlobalBounds().contains(point);
    }
}; 
#endif // SETTINGS_ICON_HPP