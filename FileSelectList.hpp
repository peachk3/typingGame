#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include <functional>

struct FileOption {
    std::wstring filePath;				// 실제 파일 경로
    std::function<void()> onClick;		// 파일 선택 시 이벤트 처리
    std::shared_ptr<sf::Text> label;	// 텍스트 객체 포인터
    bool isHovered = false;    // 마우스가 올려져 있는 상태
    bool isClicked = false;    // 클릭해서 선택된 상태

};

// 파일 제목 추출 함수
std::wstring removeExtension(const std::wstring& filePath);


void renderFileList(sf::RenderWindow& window, GameState& game,
    sf::Font& font, int fontSize,
    std::vector<std::wstring> typingFilePath,
    std::vector<FileOption>& filieOptions,
    std::wstring selectMod);

// 파일 클릭 이벤트 처리
void handleFileClick(GameState& game, const sf::Event& event, sf::Vector2f& mousePos, std::vector<FileOption>& fileOptions, sf::Font& font);


void hoverText(GameState& game, std::vector<FileOption>& fileOptions, sf::Vector2f& mousePos);