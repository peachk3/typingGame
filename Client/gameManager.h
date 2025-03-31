#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <mutex>
#include <iostream>
#include <locale>
#include <thread>

class GameManager {
private:
	sf::TcpSocket socket;
	std::mutex mtx;

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
	void receiveUpdates();
	void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font);

};

#endif
#pragma once
