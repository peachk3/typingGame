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

enum class UIState {
	ProfileMain,
	ImageSelect
};

std::wstring openImageFileDialog();

void updateProfileImage(std::wstring newPath, sf::Texture& texture, sf::Sprite& sprite);

void initSelectUI(
	sf::RenderWindow& window,
	sf::Font& font,
	std::vector<std::shared_ptr<sf::Drawable>>& drawables,
	std::vector<sf::Sprite>& sprites,
	std::vector<ImageOption>& imageOptions,
	sf::FloatRect& ButtonBounds,
	sf::Texture& profileTexture,			// �߰�
	sf::Sprite& profileSprite,				// �߰�
	UIState& currentState					// �߰� (ȭ�� ��ȯ ����)
);