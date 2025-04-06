#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <functional>

// UI ���¸� ��Ÿ���� ���� ����ü
enum class UIState {
	ProfileMain,
	ImageSelect,
	fileListShow,
	TypingGame
};

// ���� �̹��� ������ ���� ����ü
struct ImageOption {
	sf::Image image;
	sf::Texture texture;
	std::function<void()> onClick;
};


// ������ ������ �簢�� ����
sf::RectangleShape makeRectangle(sf::RenderWindow& window, float widthRatio, float heightRatio);

// �θ� �����̳� ������ �簢�� ����
sf::RectangleShape makeRectangle(sf::FloatRect& frameBounds, float widthRatio, float heightRatio, sf::Color color);


// �̹��� �ε� �� ��������
sf::Image loadImg(std::wstring path);
sf::Image resizeImageKeepAspect(const sf::Image& src, sf::Vector2u targetSize = { 200, 200 }, sf::Color paddingColor = sf::Color::Transparent);


