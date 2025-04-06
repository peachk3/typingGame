#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.hpp"

// ��� â�� �������ϰ� ���� ����� ��ȯ (true: �����, false: ����)
bool showResultWindow(
    GameState& game,
    const sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& sentences);