#include "DrawUIR.hpp"

// ������ makeRectangle �Լ� - �����ε� ���
sf::RectangleShape makeRectangleR(
    const sf::Vector2f& baseSize,     // ������ �Ǵ� ũ�� (window �Ǵ� frame�� size)
    float widthRatio,
    float heightRatio,
    sf::Color fill,
    sf::Color outline,
    float outlineThickness
) {
    sf::Vector2f rectSize(baseSize.x * widthRatio, baseSize.y * heightRatio);
    sf::RectangleShape rect(rectSize);
    rect.setFillColor(fill);
    rect.setOutlineColor(outline);
    rect.setOutlineThickness(outlineThickness);
    return rect;
}

// ������ ���� ȣ���
sf::RectangleShape makeRectangleR(
    sf::RenderWindow& window,
    float widthRatio,
    float heightRatio,
    sf::Color fill,
    sf::Color outline,
    float outlineThickness
) {
    return makeRectangleR(
        static_cast<sf::Vector2f>(window.getSize()),
        widthRatio,
        heightRatio,
        fill,
        outline,
        outlineThickness
    );
}

// Ư�� ������ ���� ȣ���
sf::RectangleShape makeRectangleR(
    const sf::FloatRect& frameBounds,
    float widthRatio,
    float heightRatio,
    sf::Color fill,
    sf::Color outline,
    float outlineThickness
) {
    return makeRectangleR(
        frameBounds.size,
        widthRatio,
        heightRatio,
        fill,
        outline,
        outlineThickness
    );
}
