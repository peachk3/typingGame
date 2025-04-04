#pragma once

#include "serverManager.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <optional>  // std::optional�� ����ϱ� ���� ��� �߰�
#include <thread>
#include <mutex>
#include <map>

using namespace std;
using namespace sf;



ServerManager::ServerManager() {
    // ������ 53000 ��Ʈ���� ������ ��ٸ����� ����
    if (listener.listen(53000) != sf::Socket::Status::Done) {
        std::cerr << "���� ���� ����!" << std::endl;
    }
    else {
        std::cout << "���� ���� ��... (��Ʈ 53000)" << std::endl;
    }
}

void ServerManager::waitForClients() {
    // Ŭ���̾�Ʈ�� ����Ǹ� ��⿭�� �߰�
    while (true) {
        sf::TcpSocket* client = new sf::TcpSocket();  // ���ο� Ŭ���̾�Ʈ ���� ����
        // Ŭ���̾�Ʈ�� ����Ǹ� ������ �޾ƿ�
        if (listener.accept(*client) == sf::Socket::Status::Done) {
            // Ŭ���̾�Ʈ�� IP�� Ȯ���Ͽ� �̹� ������ IP���� Ȯ��
            std::optional<sf::IpAddress> clientIPOpt = client->getRemoteAddress(); // std::optional�� IP �ּ� �ޱ�

            if (clientIPOpt) {  // IP ���� �����ϴ� ��쿡�� ó��
                sf::IpAddress clientIP = *clientIPOpt;  // ���� ������ ���
                std::string ipStr = clientIP.toString();

                //// �̹� �ش� IP�� ����� ���, ���� �ź�
                //if (connectedIPs.find(ipStr) != connectedIPs.end()) {
                //    std::cout << "IP " << ipStr << "���� �̹� Ŭ���̾�Ʈ�� ����Ǿ� �ֽ��ϴ�. ������ �ź��մϴ�." << std::endl;
                //    delete client;  // Ŭ���̾�Ʈ ���� �޸� ����
                //    continue;  // ���� Ŭ���̾�Ʈ ���� �õ�
                //}

                // �ش� IP�� ������ IP ��Ͽ� �߰�
                connectedIPs.insert(ipStr);
                waitingClients.push_back(client);  // ��⿭�� Ŭ���̾�Ʈ �߰�
                std::cout << "Ŭ���̾�Ʈ �����! ���� ��� �ο�: " << waitingClients.size() << std::endl;

                // ��� ���� Ŭ���̾�Ʈ�� 1�� �̻��� ��� '��� ��ٸ��� ��' �޽��� ������
                if (waitingClients.size() == 1) {
                    sf::TcpSocket* waitingClient = waitingClients.front();  // ��� ���� Ŭ���̾�Ʈ
                    std::string waitingMsg = "��� ��ٸ��� ��...";
                    waitingClient->send(waitingMsg.c_str(), waitingMsg.size() + 1);  // �޽��� ����
                    std::cout << "��⿭�� ��� �޽��� ����!" << std::endl;
                }

                // ��⿭�� 2���� �Ǹ� ��Ī ����
                if (waitingClients.size() >= 2) {
                    matchClients();  // 2�� ��Ī
                }

            }
            else {
                std::cerr << "Ŭ���̾�Ʈ�� IP �ּҸ� �������� �� �����߽��ϴ�." << std::endl;
                delete client;  // IP �ּҸ� ������ �� ������ ������ ����
            }
        }
    }
}


void ServerManager::matchClients() {
    std::lock_guard<std::mutex> lock(clientMutex);

    if (waitingClients.size() < 2) return;

    // ��Ī�� Ŭ���̾�Ʈ �� ���� ��⿭���� �����ϰ�, ���� ���� �޽��� ����
    sf::TcpSocket* client1 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());  // ù ��° Ŭ���̾�Ʈ ����
    cout << "client1 ����" << endl;
    sf::TcpSocket* client2 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());  // �� ��° Ŭ���̾�Ʈ ����
    cout << "client2 ����" << endl;

    //// ��Ī�� Ŭ���̾�Ʈ ��Ͽ� �߰�
    //matchedClients.push_back(client1);
    //matchedClients.push_back(client2);

    std::string startMsg = "��Ī �Ϸ�! ���� ����!";
    client1->send(startMsg.c_str(), startMsg.size() + 1);  // ù ��° Ŭ���̾�Ʈ���� �޽��� ����
    client2->send(startMsg.c_str(), startMsg.size() + 1);  // �� ��° Ŭ���̾�Ʈ���� �޽��� ����

    std::cout << "���� ���� �޽��� ���� �Ϸ�!" << std::endl;

}
void ServerManager::handleClient(sf::TcpSocket* client) {
    while (true) {
        sf::Packet packet;
        if (client->receive(packet) == sf::Socket::Status::Done) {
            std::string tag;
            float x, y;
            packet >> tag >> x >> y;

            if (tag == "MOVE") {
                std::cout << "Ŭ���̾�Ʈ �̵� ����: (" << x << ", " << y << ")" << std::endl;

                // ��� Ŭ���̾�Ʈ���� �̵� ������ ����
                std::lock_guard<std::mutex> lock(clientMutex);
                for (auto& pair : sockets) {
                    sf::Packet sendPacket;
                    sendPacket << "MOVE" << x << y;
                    pair.second->send(sendPacket);
                }
            }
        }
    }
}

// Ŭ�� 1 , Ŭ�� 2 ������ ����
// ���Ͼ��̵� �ο� int Ŭ��1 (1) Ŭ��2 (2)
// map �ڷᱸ�� <int , socket> sockets
// �±�, ������ ex) move, x, y��ǥ
// if �±װ� move�� �̵��Լ�
// sockets[id]  for
// ���� ��Ģ, Ŭ���̾�Ʈ���� ������
// Ŭ��1 -> ������ ��ġ�� ����
// ������ ��ġ���� 2������ �����ֱ�
// ��Ŷ�ް� Ŭ�󿡼� ������ġ ������� ������

// x, y ��ǥ ���� ����� 
// ���� ������ ������ Direction �޼��� ������
// �������� Direction �޾Ƽ� ó��
// Ŭ��1 �Լ�, Ŭ��2 �Լ� �ۼ�