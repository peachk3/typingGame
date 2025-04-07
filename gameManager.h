#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <mutex>
#include <iostream>
#include <Windows.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <random>
#include <filesystem>
#include <io.h>

#include <locale>
#include <thread>
class FileLoader;
class TextUtil;
class TypingGame;

class GameManager {
private:
	sf::TcpSocket socket;
	std::mutex mtx;
	sf::RenderWindow window;
	FileLoader* loader;
	TextUtil* util;
	TypingGame* typing;
	int currentLineIndex = 0; // 현재 출력 중인 줄 인덱스

	int myID = -1;
	int myProgress = 0;
	int opponentProgress = 0;
	bool gameOver = false;
	bool isWindowClosed = false;
	std::string targetSentence = "타자 대결을 시작합니다!";
	std::string userInput = "";

public:
	void showLoadingScreen(sf::RenderWindow& window, sf::Font& font, bool& isMatching);
	void runGame(sf::RenderWindow& window, sf::Font& font);
	void runGame2(sf::RenderWindow& window, sf::Font& font, int fontSize,
		std::vector<std::vector<std::wstring>>& sentences);


	void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font);
	void handleTypingGameMode(FileLoader& loader, TextUtil& util, TypingGame& typing, sf::RenderWindow& window, sf::Font& font, bool userGameManager);
	//void handleTypingMode(FileLoader& loader, TextUtil& util, TypingGame& typing, sf::RenderWindow& window, sf::Font& font);
	void drawOriginTotalText(sf::RenderWindow& window,
		sf::Font& font,
		int fontSize,
		std::vector<std::vector<std::wstring>>& fileAllLines);
	void drawOriginText(
		sf::RenderWindow& window,
		sf::Font& font,
		int fontSize,
		std::vector<std::vector<std::wstring>>& fileAllLines);
	void drawUserInputText(
		sf::RenderWindow& window,
		sf::Font& font,
		int fontSize,
		std::vector<std::vector<std::wstring>>& userInputs,
		std::vector<std::vector<std::wstring>>& sentences);
	void nextLine();
	void userInputCompleted();
	//string getRandomFile(const string& folderPath, const string& extension);
	//void openFile(const std::string& filePath);

};

#endif
#pragma once