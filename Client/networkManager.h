#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>  // sf::RenderWindow�� sf::Font�� ����ϱ� ���� �߰�
#include <string>
#include "networkManager.h"
#include "gameManager.h"

#include <SFML/Network.hpp>
#include <string>

class NetworkManager {
private:
    sf::TcpSocket socket;

public:
    NetworkManager(); // ������
    bool connectToServer(); // ���� ���� �Լ�
    bool waitForMatch(sf::RenderWindow& window, sf::Font& font);    // 1:1 ��Ī ��� �Լ�
};

#endif // NETWORKMANAGER_H
