#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>

// ����� ���� ����ü
struct UserInfo
{
	std::wstring id;
	std::wstring nickname;
	std::wstring rank;
	std::wstring maxTpm;
	std::wstring maxWpm;
	std::wstring playTime;
	std::wstring point;
};

// ������ ������ �簢�� ����
sf::RectangleShape makeRectangle(sf::RenderWindow& window, float widthRatio, float heightRatio);

// �θ� �����̳� ������ �簢�� ����
sf::RectangleShape makeRectangle(sf::FloatRect& frameBounds, float widthRatio, float heightRatio, sf::Color color);

// ���� ��ƿ
sf::Vector2f getWindowCenterPosition(const sf::RenderWindow& window, const sf::FloatRect& targetBounds);
sf::Vector2f getCenterPosition(const sf::Vector2f& targetSize, const sf::FloatRect& container);
sf::Vector2f getCenterXPosition(sf::Vector2f targetSize, const sf::FloatRect& refBounds, float y);
sf::Vector2f LeftInnerAlign(sf::FloatRect& refBounds, float x_margin = 0.f);
sf::Vector2f RightInnerAlign(sf::Vector2f targetSize, sf::FloatRect& refBounds, float x_margin = 0.f);

// �̹��� �ε� �� ��������
sf::Image loadImg(std::wstring path);
sf::Image resizeImageKeepAspect(const sf::Image& src, sf::Vector2u targetSize = {200, 200}, sf::Color paddingColor = sf::Color::Transparent);

// UI �ʱ�ȭ �Լ�
void initProfileUI(sf::RenderWindow& window, sf::Font& font,
	sf::Sprite& profileImg, UserInfo info,
	std::vector<std::shared_ptr<sf::Drawable>>& drawables,
	std::vector<sf::Text>& profileTexts,sf::FloatRect& loadButtonBound);