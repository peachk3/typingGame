#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.hpp"

// --- ·»´õ¸µ ---
void renderGame(sf::RenderWindow& window, GameState& game, const sf::Font& font, int fontSize);
void drawOriginalText(sf::RenderWindow& window, const GameState& game, const sf::Font& font, int fontSize,
    sf::Vector2f standardPos, std::vector<std::vector<std::wstring>>& displaySentences);
void drawUserInputText(sf::RenderWindow& window, const GameState& game, const sf::Font& font, int fontSize,
    sf::Vector2f standardPos, std::vector<std::vector<std::wstring>>& userInputs,
    std::vector<std::vector<std::wstring>>& sentences);

// --- ·ÎÁ÷ ---
void updateTypingStats(GameState& game, float elapsedSeconds);
void moveToNextLineOrParagraph(GameState& game);
void initUserInputsAndCorrectMap(GameState& game);
void resetGameResult(GameState& game);

// --- ÀÔ·Â ---
void handleInputGame(GameState& game, const sf::Event& event);

// --- µð¹ö±ë ---
void printWStringInfo(const std::wstring& name, const std::wstring& str);
void printCorrectLineDebug(const std::vector<bool>& correctLine);