#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <SFML/Network.hpp>
#include <vector>
#include <set>
#include <mutex>
#include <thread>

class ServerManager {
private:
    std::map<int, sf::TcpSocket*> sockets;
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> waitingClients;
    std::set<std::string> connectedIPs;
    std::mutex clientMutex;
    bool running; // ��� �޽��� ���� ������ ���Ḧ ���� �÷���

public:
    ServerManager();
    //~ServerManager();
    void waitForClients();
    void matchClients();
    std::string receiveClientID(sf::TcpSocket* client);  // Ŭ���̾�Ʈ�κ��� ID �ޱ�
    void sendWaitingMessage(sf::TcpSocket* client);        // ��� �޽��� ����
    void handleClient(int clientID, float x, float y);              // Ŭ���̾�Ʈ ó��
    //void handleClients();              // Ŭ���̾�Ʈ ó��

    // �߰�: 1���� �� �ֱ������� ��� �޽����� ������ �Լ�
    void updateWaitingClients();
};

extern ServerManager server;  // ���� ���� ���� (���� X)

#endif // SERVERMANAGER_H
