#include "DrawUIR.hpp"

// 개선된 makeRectangle 함수 - 오버로드 기반
sf::RectangleShape makeRectangleR(
    const sf::Vector2f& baseSize,     // 기준이 되는 크기 (window 또는 frame의 size)
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

// 윈도우 기준 호출용
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

// 특정 프레임 기준 호출용
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
