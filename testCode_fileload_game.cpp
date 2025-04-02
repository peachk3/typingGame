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
	_setmode(_fileno(stdout), _O_U16TEXT);  // UTF-16 출력 활성화

	FileLoader loader;
	TextUtil util;
	TypingGame game;

	// 1. 파일 열기 테스트
	std::optional<std::wstring> path = loader.openFileDialog();
	if (!path) {
		std::wcout << L"[ERROR] 파일을 선택하지 않았습니다." << std::endl;
		system("pause");
		return 1;
	}
	
	std::wcout << L"[INFO] 파일 선택됨: " << path.value() << std::endl;

	// 2. 텍스트 로딩 테스트
	std::wstring content = loader.loadText(path.value());
	std::wcout << L"[INFO] 파일에서 읽은 내용:\n" << content << L"...\n\n";
	//std::wcout << L"[INFO] 파일에서 읽은 내용:\n" << content.substr(0, 200) << L"...\n\n";

	// 3. 줄 나누기 테스트
	std::vector<std::wstring> lines = util.splitStrtoVector(content);
	std::wcout << L"[INFO] 줄 개수: " << lines.size() << std::endl;

	for (int i = 0; i < lines.size(); ++i) {
		std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
	}
	/*for (int i = 0; i < std::min((size_t)5, lines.size()); ++i) {
		std::wcout << L"[LINE " << i << L"] " << lines[i] << std::endl;
	}*/

	// 4. 줄바꿈 테스트
	sf::Font font;
	std::filesystem::current_path("C:/Source/typingGame");
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
		return 1;
	}

	auto wrapped = util.wrapAllLinesToPixelWidth(lines, font, 30, 1280.f);
	std::wcout << L"\n[INFO] 줄바꿈 결과 (최대 3줄):" << std::endl;
	/*for (int i = 0; i < std::min((size_t)3, wrapped.size()); ++i) {
		for (const auto& sub : wrapped[i]) {
			std::wcout << L"  → " << sub << std::endl;
		}
	}*/

	for (int i = 0; i < wrapped.size(); ++i) {
		for (const auto& sub : wrapped[i]) {
			std::wcout << L"  → " << sub << std::endl;
		}
	}

	// 게임이랑 연결해보자
	sf::RenderWindow Window(sf::VideoMode({ 1280, 750 }), "Typing Test");
	game.startNewGame(Window, font, 18, wrapped);
	system("pause");
	return 0;
}