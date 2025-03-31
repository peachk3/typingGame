#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>  // RenderWindow�� Font ���
#include <string>
#include "gameManager.h"

class NetworkManager {
private:
    sf::TcpSocket socket;

public:
    NetworkManager(); // ������
    bool connectToServer(const std::string& clientID); // ���� ���� (ID ���� ����)
    // �߰�: �ܺο��� ���Ͽ� ������ �� �ֵ��� getter
    sf::TcpSocket& getSocket();
};

#endif // NETWORKMANAGER_H
