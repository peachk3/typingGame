#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <SFML/Network.hpp>
#include <vector>
#include <set>
#include <mutex>
#include <thread>

class ServerManager {
private:
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
    void handleClient(sf::TcpSocket* client);              // Ŭ���̾�Ʈ ó��

    // �߰�: 1���� �� �ֱ������� ��� �޽����� ������ �Լ�
    void updateWaitingClients();
};

#endif // SERVERMANAGER_H
