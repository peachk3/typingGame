#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.hpp"

// 결과 창을 렌더링하고 선택 결과를 반환 (true: 재시작, false: 종료)
bool showResultWindow(
    GameState& game,
    const sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& sentences);