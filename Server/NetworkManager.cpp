#include "networkManager.h"
#include "gameManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <locale>
#include <codecvt>
#include <thread>

NetworkManager::NetworkManager() {
    // �����ڿ��� �ʿ��� �ʱ�ȭ ���� ����
}

bool NetworkManager::connectToServer() {
    // ������ ���� �õ� (IP: ��Ʈ: 53000) // 210,119,12,60
    sf::IpAddress serverIP = {210,119,12,60};
    //sf::IpAddress serverIP = sf::/Localhost
    //if (socket.connect({ 210,119,12,69 }, 53000) == sf::Socket::Status::Done) {

    if (socket.connect(serverIP, 53000) == sf::Socket::Status::Done) {
        std::cout << "���� �����!" << std::endl;
        return true;
    }
    else {
        std::cerr << "���� ���� ����!" << std::endl;
        return false;
    }
}

bool NetworkManager::waitForMatch(sf::RenderWindow& window, sf::Font& font) {
    GameManager game;
    //showLoadingScreen(window, font);
    // �����κ��� �޽��� ���� (��Ī ���� �޽���)
    char buffer[128];  // �������� �޴� �޽��� ������ ����
    std::size_t received;  // ������ ���� ����Ʈ ��

    sf::TcpSocket player1, player2;
    sf::Time countdownStartTime = sf::seconds(5.0f);  // ī��Ʈ�ٿ� 5�� ����

    std::cout << "1:1 ��Ī�� ��ٸ��� ��..." << std::endl;
    while (true) {
        if (socket.receive(buffer, sizeof(buffer), received) == sf::Socket::Status::Done) {
            buffer[received] = '\0';  // ���ڿ� ���� ó��
            std::string message = buffer;
            std::cout << "���� �޽���: " << buffer << std::endl;  // ���� �޽��� ���
            // �� Ŭ���̾�Ʈ���� ���� ī��Ʈ�ٿ� �ð� ����
            player1.send(&countdownStartTime, sizeof(countdownStartTime));
            player2.send(&countdownStartTime, sizeof(countdownStartTime));

            if (std::string(buffer) == "��Ī �Ϸ�! ���� ����!") {
                std::cout << "������ �� ���۵˴ϴ�!" << std::endl;
                return true;
            } else {
                std::cout << "���� ��� ��..." << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // ���
    }
}