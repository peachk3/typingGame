#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "networkManager.h"

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <mutex>
#include <iostream>
#include <locale>
#include <thread>

class GameManager {
private:
	std::mutex mtx;
	sf::RenderWindow window;

	int clientID;
	int myID = -1;
	int myProgress = 0;
	int opponentProgress = 0;
	bool gameOver = false;
	bool isWindowClosed = false;
	std::string targetSentence = "타자 대결을 시작합니다!";
	std::string userInput = "";
public:
	void showIDInputScreen(sf::RenderWindow& window, sf::Font& font, std::string& clientID);
	void showLoadingScreen(sf::RenderWindow& window, sf::Font& font, bool& isMatching);
	void showCountdown(sf::RenderWindow& window, sf::Font& font);
	//bool showCountdown(sf::RenderWindow& window, sf::Font& font);
	void runGame(sf::RenderWindow& window, sf::Font& font);
	void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font);
	void updatePlayerPosition(int clientID, float x, float y);
	void playerMove(float x, float y);
	int getClientID() const { return clientID; }
};

extern GameManager game;  // 전역 변수 선언 (정의 X)

#endif

