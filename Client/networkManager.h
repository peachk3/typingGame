#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>  // RenderWindow와 Font 사용
#include <string>
#include "gameManager.h"

class NetworkManager {
private:
    sf::TcpSocket socket;

public:
    NetworkManager(); // 생성자
    bool connectToServer(const std::string& clientID); // 서버 연결 (ID 전송 포함)
    // 추가: 외부에서 소켓에 접근할 수 있도록 getter
    sf::TcpSocket& getSocket();
};

#endif // NETWORKMANAGER_H
