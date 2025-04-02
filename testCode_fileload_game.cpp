#include <iostream>
#include <SFML/Graphics.hpp>
#include "FileLoader.hpp"
#include "TextUtil.hpp"
#include "TypingGame.hpp"
#include <filesystem>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>

int main()
{
	_setmode(_fileno(stdout), _O_U16TEXT);  // UTF-16 ��� Ȱ��ȭ

	FileLoader loader;
	TextUtil util;
	TypingGame game;

	// 1. ���� ���� �׽�Ʈ
	std::optional<std::wstring> path = loader.openFileDialog();
	if (!path) {
		std::wcout << L"[ERROR] ������ �������� �ʾҽ��ϴ�." << std::endl;
		system("pause");
		return 1;
	}
	
	std::wcout << L"[INFO] ���� ���õ�: " << path.value() << std::endl;

	// 2. �ؽ�Ʈ �ε� �׽�Ʈ
	std::wstring content = loader.loadText(path.value());
	std::wcout << L"[INFO] ���Ͽ��� ���� ����:\n" << content << L"...\n\n";
	//std::wcout << L"[INFO] ���Ͽ��� ���� ����:\n" << content.substr(0, 200) << L"...\n\n";

	// 3. �� ������ �׽�Ʈ
	std::vector<std::wstring> lines = util.splitStrtoVector(content);
	std::wcout << L"[INFO] �� ����: " << lines.size() << std::endl;

	for (int i = 0; i < lines.size(); ++i) {
		std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
	}
	/*for (int i = 0; i < std::min((size_t)5, lines.size()); ++i) {
		std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
	}*/

	// 4. �ٹٲ� �׽�Ʈ
	sf::Font font;
	std::filesystem::current_path("C:/Source/typingGame");
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
		return 1;
	}

	auto wrapped = util.wrapAllLinesToPixelWidth(lines, font, 30, 1280.f);
	std::wcout << L"\n[INFO] �ٹٲ� ��� (�ִ� 3��):" << std::endl;
	/*for (int i = 0; i < std::min((size_t)3, wrapped.size()); ++i) {
		for (const auto& sub : wrapped[i]) {
			std::wcout << L"  �� " << sub << std::endl;
		}
	}*/

	for (int i = 0; i < wrapped.size(); ++i) {
		for (const auto& sub : wrapped[i]) {
			std::wcout << L"  �� " << sub << std::endl;
		}
	}

	// �����̶� �����غ���
	sf::RenderWindow Window(sf::VideoMode({ 1280, 750 }), "Typing Test");
	game.startNewGame(Window, font, 18, wrapped);
	system("pause");
	return 0;
}