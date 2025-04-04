#include "serverManager.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <optional>
#include <thread>
#include <mutex>
#include <map>

using namespace std;
using namespace sf;

std::mutex clientMutex;  // Ŭ���̾�Ʈ ����Ʈ ��ȣ
std::vector<sf::TcpSocket*> connectedClients;  // ����� Ŭ���̾�Ʈ ���
std::set<std::string> connectedIPs;  // �ߺ� ���� ����
TcpListener listener;
std::map<int, TcpSocket*> sockets;  // Ŭ���̾�Ʈ ID - ���� ����
int clientIDCounter = 0;  // Ŭ���̾�Ʈ ID ������

ServerManager::ServerManager() {
    if (listener.listen(53000) != sf::Socket::Status::Done) {
        std::cerr << "���� ���� ����!" << std::endl;
        return;
    }
    else {
        std::cout << "���� ���� ��... (��Ʈ 53000)" << std::endl;
    }
  
}

// Ŭ���̾�Ʈ ��� �Լ� (�����忡�� ����)
void ServerManager::waitForClients() {
    while (true) {
        sf::TcpSocket* client = new sf::TcpSocket();
        if (listener.accept(*client) == sf::Socket::Status::Done) {
            std::optional<sf::IpAddress> clientIPOpt = client->getRemoteAddress();

            if (clientIPOpt) {
                std::lock_guard<std::mutex> lock(clientMutex);  // ���ؽ� ���
                sf::IpAddress clientIP = *clientIPOpt;
                std::string ipStr = clientIP.toString();

                connectedIPs.insert(ipStr);
                waitingClients.push_back(client);
                std::cout << "Ŭ���̾�Ʈ �����! ���� ��� �ο�: " << waitingClients.size() << std::endl;

                if (waitingClients.size() == 1) {
                    sf::TcpSocket* waitingClient = waitingClients.front();
                    std::string waitingMsg = "��� ��ٸ��� ��...";
                    waitingClient->send(waitingMsg.c_str(), waitingMsg.size() + 1);
                }

                if (waitingClients.size() >= 2) {
                    std::thread matchThread(&ServerManager::matchClients, this);
                    matchThread.detach();  // ��Ī ������ �и� ����
                }

            }
            else {
                std::cerr << "Ŭ���̾�Ʈ IP �ּ� �������� ����!" << std::endl;
                delete client;
            }
        }
    }
}
void ServerManager::handleClient(sf::TcpSocket* client1) {
    while (true) {
        sf::Packet packet;
        if (client1->receive(packet) == sf::Socket::Status::Done) {
            std::string tag;
            float x, y;
            packet >> tag >> x >> y;

            if (tag == "MOVE") {
                std::cout << "Ŭ���̾�Ʈ �̵� ����: (" << x << ", " << y << ")" << std::endl;

                // ��� Ŭ���̾�Ʈ���� �̵� ������ ����
                for (auto& c : connectedClients) {
                    sf::Packet sendPacket;
                    sendPacket << "MOVE" << x << y;
                    c->send(sendPacket);
                }
            }
        }
    }
}

// Ŭ���̾�Ʈ ��Ī (���� ������ ����)
void ServerManager::matchClients() {
    std::lock_guard<std::mutex> lock(clientMutex);

    if (waitingClients.size() < 2) return;

    sf::TcpSocket* client1 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());
    sf::TcpSocket* client2 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());

    std::string startMsg = "��Ī �Ϸ�! ���� ����!";
    client1->send(startMsg.c_str(), startMsg.size() + 1);
    client2->send(startMsg.c_str(), startMsg.size() + 1);

    std::cout << "���� ���� �޽��� ���� �Ϸ�!" << std::endl;

    // �� Ŭ���̾�Ʈ�� ���� ������ ����
    std::thread client1Thread(&ServerManager::handleClient, this, client1);
    std::thread client2Thread(&ServerManager::handleClient, this, client2);

    client1Thread.detach();
    client2Thread.detach();
}

// Ŭ���̾�Ʈ ������ ó�� �Լ�
//void ServerManager::handleClient(sf::TcpSocket* client) {
//    char buffer[1024];
//    std::size_t received;
//
//    while (true) {
//        if (client->receive(buffer, sizeof(buffer), received) == sf::Socket::Status::Done) {
//            std::cout << "Ŭ���̾�Ʈ �޽���: " << buffer << std::endl;
//        }
//    }
//}
/*
void ServerManager::handleClients(sf::TcpSocket* client1, sf::TcpSocket* client2) {
    while (true) {
        sf::Packet packet;
        sf::Socket::Status status = client1->receive(packet);

        if (status == sf::Socket::Status::Done) {
            // ���� �޽����� Ŭ���̾�Ʈ 2���� ����
            std::lock_guard<std::mutex> lock(clientMutex);
            client2->send(packet);
        }
        else if (status == sf::Socket::Status::Disconnected) {
            // Ŭ���̾�Ʈ1�� ���� ���� -> Ŭ���̾�Ʈ2���� �˸�
            std::lock_guard<std::mutex> lock(clientMutex);
            std::string disconnectMsg = "������ ������ �����߽��ϴ�.";
            sf::Packet disconnectPacket;
            disconnectPacket << disconnectMsg;
            client2->send(disconnectPacket);
            break;  // ���� ����
        }

        // �ݴ� ���� �޽��� ����
        status = client2->receive(packet);
        if (status == sf::Socket::Status::Done) {
            std::lock_guard<std::mutex> lock(clientMutex);
            client1->send(packet);
        }
        else if (status == sf::Socket::Status::Disconnected) {
            std::lock_guard<std::mutex> lock(clientMutex);
            std::string disconnectMsg = "������ ������ �����߽��ϴ�.";
            sf::Packet disconnectPacket;
            disconnectPacket << disconnectMsg;
            client1->send(disconnectPacket);
            break;
        }
    }

    // Ŭ���̾�Ʈ ���� ����
    std::lock_guard<std::mutex> lock(clientMutex);
    client1->disconnect();
    client2->disconnect();
    delete client1;
    delete client2;

    std::cout << "Ŭ���̾�Ʈ ���� ���� �� ���� �Ϸ�" << std::endl;
}
*/
