#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>  // sf::RenderWindow와 sf::Font를 사용하기 위해 추가
#include <string>
#include "networkManager.h"
#include "gameManager.h"

#include <SFML/Network.hpp>
#include <string>

class NetworkManager {
private:
    sf::TcpSocket socket;

public:
    NetworkManager(); // 생성자
    bool connectToServer(); // 서버 연결 함수
    bool waitForMatch(sf::RenderWindow& window, sf::Font& font);    // 1:1 매칭 대기 함수
};

#endif // NETWORKMANAGER_H
