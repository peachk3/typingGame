#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
//
// UI 정렬 유틸 - SFML 3.0 기준
// - FloatRect: position, size 사용
// - 텍스트, 버튼, 스프라이트 등 위치 정렬에 사용
//

// 정렬 방향 열거형
enum class AlignX { Left, Center, Right };
enum class AlignY { Top, Center, Bottom };

// 기본 위치 정렬 계산
inline sf::Vector2f AlignPosition(
    const sf::Vector2f& targetSize,
    const sf::FloatRect& refBounds,
    AlignX alignX,
    AlignY alignY,
    float x_margin = 0.f,
    float y_margin = 0.f
) {
    float x = refBounds.position.x;
    float y = refBounds.position.y;

    switch (alignX) {
    case AlignX::Left:
        x = refBounds.position.x + x_margin;
        break;
    case AlignX::Center:
        x = refBounds.position.x + (refBounds.size.x - targetSize.x) / 2.f;
        break;
    case AlignX::Right:
        x = refBounds.position.x + refBounds.size.x - targetSize.x - x_margin;
        break;
    }

    switch (alignY) {
    case AlignY::Top:
        y = refBounds.position.y + y_margin;
        break;
    case AlignY::Center:
        y = refBounds.position.y + (refBounds.size.y - targetSize.y) / 2.f;
        break;
    case AlignY::Bottom:
        y = refBounds.position.y + refBounds.size.y - targetSize.y - y_margin;
        break;
    }

    return { x, y };
}

// 일반 Drawable 객체 정렬
template<typename Drawable>
void AlignTo(
    Drawable& drawable,
    const sf::FloatRect& refBounds,
    AlignX alignX,
    AlignY alignY,
    float x_margin = 0.f,
    float y_margin = 0.f
) {
    sf::Vector2f size = drawable.getGlobalBounds().size;
    sf::Vector2f pos = AlignPosition(size, refBounds, alignX, alignY, x_margin, y_margin);
    drawable.setPosition(pos);
}

// 텍스트 정렬 (가운데 정렬 + 마진)
inline void AlignTextCenter(
    sf::Text& text,
    const sf::FloatRect& box,
    const sf::Vector2f& margin = { 0.f, 0.f }
) {
    sf::FloatRect textBounds = text.getGlobalBounds();

    sf::Vector2f offset;
    offset.x = (box.size.x - textBounds.size.x) / 2.f - textBounds.position.x + margin.x;
    offset.y = (box.size.y - textBounds.size.y) / 2.f - textBounds.position.y + margin.y;

    text.setPosition(box.position + offset);
}

// 가운데 정렬 - 윈도우 기준
inline sf::Vector2f getWindowCenterPosition(const sf::RenderWindow& window, const sf::FloatRect& targetBounds)
{
    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;

    float x = centerX - targetBounds.size.x / 2.f;
    float y = centerY - targetBounds.size.y / 2.f;
    

    return { x, y };
}

//// 가운데 정렬 - 윈도우 기준 - 리사이즈 버전
//inline sf::Vector2f getWindowCenterPosition(const sf::RenderWindow& window, const sf::FloatRect& targetBounds)
//{
//    float centerX = window.getSize().x / 2.f;
//    float centerY = window.getSize().y / 2.f;
//
//    float x = centerX - targetBounds.size.x / 2.f;
//    float y = centerY - targetBounds.size.y / 2.f;
//    std::cout << "x: " << x << "y:" << y << std::endl;
//    std::cout << "x: " << window.getSize().x << "y:" << window.getSize().y << std::endl;
//
//    return { x, y };
//}


// XY 중앙정렬 (버튼에 텍스트)
inline sf::Vector2f getCenterPosition(const sf::Vector2f& targetSize, const sf::FloatRect& container)
{
    float x = container.position.x + (container.size.x - targetSize.x) / 2.f;
    float y = container.position.y + (container.size.y - targetSize.y) / 2.f;
    return { x, y };
}

// 어떤 객체 기준 x 축 가운데 정렬
inline sf::Vector2f getCenterXPosition(sf::Vector2f targetSize,
    const sf::FloatRect& refBounds,
    float y)
{
    float x = refBounds.position.x + (refBounds.size.x - targetSize.x) / 2.f;
    return { x, y };
}

// 왼쪽 정렬
inline sf::Vector2f LeftInnerAlign(sf::FloatRect& refBounds, float x_margin)
{
    return { refBounds.position.x + x_margin, refBounds.position.y };
}

// 오른쪽 정렬
inline sf::Vector2f RightInnerAlign(sf::Vector2f targetSize, sf::FloatRect& refBounds, float x_margin)
{
    return { refBounds.position.x + refBounds.size.x - targetSize.x - x_margin, refBounds.position.y };
}
inline sf::Vector2f RightInnerAlign(sf::FloatRect& targetBounds, sf::FloatRect& refBounds, float x_margin)
{

    return { refBounds.position.x + refBounds.size.x - targetBounds.size.x - x_margin, refBounds.position.y };

}


// 아래쪽 정렬
inline sf::Vector2f BottomInnerAlign(sf::Vector2f targetSize, const sf::FloatRect& refBounds, float y_margin)
{
    return {
        refBounds.position.x,
        refBounds.position.y + refBounds.size.y - targetSize.y - y_margin
    };
}
inline sf::Vector2f BottomInnerAlign(sf::FloatRect& targetBounds, const sf::FloatRect& refBounds, float y_margin)
{
    return {
        refBounds.position.x,
        refBounds.position.y + refBounds.size.y - targetBounds.size.y - y_margin
    };
}
inline sf::Vector2f BottomInnerAlign(sf::Vector2f targetSize, sf::Vector2f refSize, float y_margin)
{
    return {
        refSize.x,
        refSize.y + refSize.y - targetSize.y - y_margin
    };
}
