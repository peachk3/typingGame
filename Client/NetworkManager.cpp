#include "networkManager.h"
#include <SFML/Network.hpp>
#include <iostream>

NetworkManager::NetworkManager() {}

bool NetworkManager::connectToServer(const std::string& clientID) {
    if (socket.connect({ 210,119,12,69 }, 53000) == sf::Socket::Status::Done) {  // 서버와 연결
        std::cout << "서버 연결됨!" << std::endl;
        // 서버에 클라이언트 ID 전송
        socket.send(clientID.c_str(), clientID.size() + 1);

        return true;
    }
    else {
        std::cerr << "서버 연결 실패!" << std::endl;
        return false;
    }
}

sf::TcpSocket& NetworkManager::getSocket() {
    return socket;
}
