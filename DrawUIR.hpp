#pragma once

#include <SFML/Graphics.hpp>

// 기준 사이즈를 받아 사각형 생성 (공통 함수)
sf::RectangleShape makeRectangleR(
    const sf::Vector2f& baseSize,
    float widthRatio,
    float heightRatio,
    sf::Color fill = sf::Color::Transparent,
    sf::Color outline = sf::Color::Transparent,
    float outlineThickness = 0.f
);

// 전체 창 기준 사각형 생성
sf::RectangleShape makeRectangleR(
    sf::RenderWindow& window,
    float widthRatio,
    float heightRatio,
    sf::Color fill = sf::Color::Transparent,
    sf::Color outline = sf::Color::Transparent,
    float outlineThickness = 0.f
);

// 특정 프레임 기준 사각형 생성
sf::RectangleShape makeRectangleR(
    const sf::FloatRect& frameBounds,
    float widthRatio,
    float heightRatio,
    sf::Color fill = sf::Color::Transparent,
    sf::Color outline = sf::Color::Transparent,
    float outlineThickness = 0.f
);