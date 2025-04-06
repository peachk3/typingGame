#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <functional>

// UI 상태를 나타내기 위한 구조체
enum class UIState {
	ProfileMain,
	ImageSelect,
	fileListShow,
	TypingGame
};

// 사진 이미지 저장을 위한 구조체
struct ImageOption {
	sf::Image image;
	sf::Texture texture;
	std::function<void()> onClick;
};


// 윈도우 비율로 사각형 생성
sf::RectangleShape makeRectangle(sf::RenderWindow& window, float widthRatio, float heightRatio);

// 부모 컨테이너 비율로 사각형 생성
sf::RectangleShape makeRectangle(sf::FloatRect& frameBounds, float widthRatio, float heightRatio, sf::Color color);


// 이미지 로드 및 리사이즈
sf::Image loadImg(std::wstring path);
sf::Image resizeImageKeepAspect(const sf::Image& src, sf::Vector2u targetSize = { 200, 200 }, sf::Color paddingColor = sf::Color::Transparent);


