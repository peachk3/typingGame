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

// �̹��� ���� �ҷ����� Ž���� ����
std::wstring openImageFileDialog();

// �̹��� ���� ������Ʈ
void updateProfileImage(std::wstring newPath, GameState& game, sf::Sprite& sprite);

// �̹��� ����ȭ�� ������
void renderSelectImage(
	sf::RenderWindow& window,
	GameState& game,
	sf::Font& font,
	std::vector<ImageOption>& imageOptions,
	std::vector<sf::Sprite>& sprites,
	sf::Sprite& profileSprite,							// �߰�
	sf::RectangleShape& thumbnailHoverOutline);			// �߰�

