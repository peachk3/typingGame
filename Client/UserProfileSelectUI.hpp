#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

struct ImageOption {
	sf::Image image;
	sf::Texture texture;
	std::function<void()> onClick;
};

// 이미지 파일 불러오는 탐색기 열기
std::wstring openImageFileDialog();

// 이미지 파일 업데이트
void updateProfileImage(std::wstring newPath, GameState& game, sf::Sprite& sprite);

// 이미지 선택화면 렌더링
void renderSelectImage(
	sf::RenderWindow& window,
	GameState& game,
	sf::Font& font,
	std::vector<ImageOption>& imageOptions,
	std::vector<sf::Sprite>& sprites,
	sf::Sprite& profileSprite,							// 추가
	sf::RectangleShape& thumbnailHoverOutline);			// 추가

