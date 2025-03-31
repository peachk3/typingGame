#include <SFML/Graphics.hpp>
#include "networkManager.h"
#include "gameManager.h"
#include <thread>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "���̵� �Է�");

    sf::Font font;
    if (!font.openFromFile("D2Coding.ttf")) {  // �ѱ� ��Ʈ �ε�
        std::cerr << "��Ʈ �ε� ����!" << std::endl;
        return -1;
    }

    std::string clientID = "";
    showIDInputScreen(window, font, clientID);
    std::cout << "�Էµ� ���̵�: " << clientID << std::endl;

    NetworkManager network;
    if (network.connectToServer(clientID)==true) {
        // �����κ��� ��Ī �޽����� ������ ������ ���
        std::cout << "waitForGameStart: " << clientID << std::endl;
        waitForGameStart(network.getSocket(), window, font);
        
    }

    return 0;
}
