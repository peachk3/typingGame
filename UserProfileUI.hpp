#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include <vector>
#include <memory>
#include <string>


// UI �ʱ�ȭ �Լ�
void renderProfile(sf::RenderWindow& window, GameState& game, 
	sf::Font& font, int fontSize,
	sf::Sprite& profileImg,
	std::vector<sf::Text>& profileTexts);		// game.button

std::wstring to_wstring_fixed(double value, int precision = 2);