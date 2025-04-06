#pragma once

#include <SFML/Graphics.hpp>

// ���� ����� �޾� �簢�� ���� (���� �Լ�)
sf::RectangleShape makeRectangleR(
    const sf::Vector2f& baseSize,
    float widthRatio,
    float heightRatio,
    sf::Color fill = sf::Color::Transparent,
    sf::Color outline = sf::Color::Transparent,
    float outlineThickness = 0.f
);

// ��ü â ���� �簢�� ����
sf::RectangleShape makeRectangleR(
    sf::RenderWindow& window,
    float widthRatio,
    float heightRatio,
    sf::Color fill = sf::Color::Transparent,
    sf::Color outline = sf::Color::Transparent,
    float outlineThickness = 0.f
);

// Ư�� ������ ���� �簢�� ����
sf::RectangleShape makeRectangleR(
    const sf::FloatRect& frameBounds,
    float widthRatio,
    float heightRatio,
    sf::Color fill = sf::Color::Transparent,
    sf::Color outline = sf::Color::Transparent,
    float outlineThickness = 0.f
);