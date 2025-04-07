#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
//
// UI ���� ��ƿ - SFML 3.0 ����
// - FloatRect: position, size ���
// - �ؽ�Ʈ, ��ư, ��������Ʈ �� ��ġ ���Ŀ� ���
//

// ���� ���� ������
enum class AlignX { Left, Center, Right };
enum class AlignY { Top, Center, Bottom };

// �⺻ ��ġ ���� ���
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

// �Ϲ� Drawable ��ü ����
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

// �ؽ�Ʈ ���� (��� ���� + ����)
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

// ��� ���� - ������ ����
inline sf::Vector2f getWindowCenterPosition(const sf::RenderWindow& window, const sf::FloatRect& targetBounds)
{
    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;

    float x = centerX - targetBounds.size.x / 2.f;
    float y = centerY - targetBounds.size.y / 2.f;
    

    return { x, y };
}

//// ��� ���� - ������ ���� - �������� ����
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


// XY �߾����� (��ư�� �ؽ�Ʈ)
inline sf::Vector2f getCenterPosition(const sf::Vector2f& targetSize, const sf::FloatRect& container)
{
    float x = container.position.x + (container.size.x - targetSize.x) / 2.f;
    float y = container.position.y + (container.size.y - targetSize.y) / 2.f;
    return { x, y };
}

// � ��ü ���� x �� ��� ����
inline sf::Vector2f getCenterXPosition(sf::Vector2f targetSize,
    const sf::FloatRect& refBounds,
    float y)
{
    float x = refBounds.position.x + (refBounds.size.x - targetSize.x) / 2.f;
    return { x, y };
}

// ���� ����
inline sf::Vector2f LeftInnerAlign(sf::FloatRect& refBounds, float x_margin)
{
    return { refBounds.position.x + x_margin, refBounds.position.y };
}

// ������ ����
inline sf::Vector2f RightInnerAlign(sf::Vector2f targetSize, sf::FloatRect& refBounds, float x_margin)
{
    return { refBounds.position.x + refBounds.size.x - targetSize.x - x_margin, refBounds.position.y };
}
inline sf::Vector2f RightInnerAlign(sf::FloatRect& targetBounds, sf::FloatRect& refBounds, float x_margin)
{

    return { refBounds.position.x + refBounds.size.x - targetBounds.size.x - x_margin, refBounds.position.y };

}


// �Ʒ��� ����
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
