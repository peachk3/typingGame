#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

void showIDInputScreen(sf::RenderWindow& window, sf::Font& font, std::string& clientID);
void showLoadingScreen(sf::RenderWindow& window, sf::Font& font);
void showCountdown(sf::RenderWindow& window, sf::Font& font);
void runGame(sf::RenderWindow& window, sf::Font& font);
void waitForGameStart(sf::TcpSocket& socket, sf::RenderWindow& window, sf::Font& font);

#endif // GAMEMANAGER_H
#pragma once
