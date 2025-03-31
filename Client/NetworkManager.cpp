#include "networkManager.h"
#include <SFML/Network.hpp>
#include <iostream>

NetworkManager::NetworkManager() {}

bool NetworkManager::connectToServer(const std::string& clientID) {
    if (socket.connect({ 210,119,12,69 }, 53000) == sf::Socket::Status::Done) {  // ������ ����
        std::cout << "���� �����!" << std::endl;
        // ������ Ŭ���̾�Ʈ ID ����
        socket.send(clientID.c_str(), clientID.size() + 1);

        return true;
    }
    else {
        std::cerr << "���� ���� ����!" << std::endl;
        return false;
    }
}

sf::TcpSocket& NetworkManager::getSocket() {
    return socket;
}
