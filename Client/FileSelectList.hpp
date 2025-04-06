#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include <functional>

struct FileOption {
    std::wstring filePath;				// ���� ���� ���
    std::function<void()> onClick;		// ���� ���� �� �̺�Ʈ ó��
    std::shared_ptr<sf::Text> label;	// �ؽ�Ʈ ��ü ������
    bool isHovered = false;    // ���콺�� �÷��� �ִ� ����
    bool isClicked = false;    // Ŭ���ؼ� ���õ� ����

};

// ���� ���� ���� �Լ�
std::wstring removeExtension(const std::wstring& filePath);


void renderFileList(sf::RenderWindow& window, GameState& game,
    sf::Font& font, int fontSize,
    std::vector<std::wstring> typingFilePath,
    std::vector<FileOption>& filieOptions,
    std::wstring selectMod);

// ���� Ŭ�� �̺�Ʈ ó��
void handleFileClick(GameState& game, const sf::Event& event, sf::Vector2f& mousePos, std::vector<FileOption>& fileOptions, sf::Font& font);


void hoverText(GameState& game, std::vector<FileOption>& fileOptions, sf::Vector2f& mousePos);