#pragma once
//#ifndef NETWORKMANAGER_H
//#define NETWORKMANAGER_H

#include "gameManager.h"
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>  // sf::RenderWindow�� sf::Font�� ����ϱ� ���� �߰�
#include <string>
#include <thread>

//#include "networkManager.h"
//#include <SFML/Network.hpp>
//#include <string>
using namespace std;
using namespace sf;

class NetworkManager {
private:
    int clientID;
    std::thread receiveThread;  // ���� ������
    //std::thread startReceiveThread;
    std::unordered_map<int, sf::Sprite*> players; // �÷��̾� ID -> Sprite ������ ����
    bool isReceiving = true;  // ������ ���Ḧ ���� �÷���

    NetworkManager() = default;                         // ������ private
    ~NetworkManager() = default;                        // �Ҹ��� private
    NetworkManager(const NetworkManager&) = delete;     // ���� ������ ����
    NetworkManager& operator=(const NetworkManager&) = delete;

    // ���� �� �������� stop ���ָ��.
    //~NetworkManager() {
    //    stopReceiveThread();
    //    std::cout << "NetworkManager �Ҹ�!, stopReceiveThread()����" << std::endl;
    //};  // �Ҹ��� �߰�

public:
    sf::TcpSocket socket;
    // �̱��� �����̶�� �ϴµ�, 
    static NetworkManager& getInstance() {
        static NetworkManager instance;
        return instance;
    }
    sf::TcpSocket& getSocket() { return socket; }
    bool connectToServer(); // ���� ���� �Լ�
    bool waitForMatch(sf::RenderWindow& window, sf::Font& font);    // 1:1 ��Ī ��� �Լ�
    //-- �߰�
    void sendMove(sf::TcpSocket& socket, int clientID, float x, float y);
    void startReceiveThread();
    //void startReceivingMoves(); // ���� ������ ����
    void receiveMoves();  // �������� �̵� ������ ����
    void addPlayer(int clientID, sf::Sprite* sprite); // �÷��̾� �߰� �Լ�
    void sendPosition(float x, float y);
    void receivePosition();
    void stopReceiveThread(); // ������ ���� �Լ� �߰� > ���� �� ���� ����
    //--
};

//#endif // NETWORKMANAGER_H
