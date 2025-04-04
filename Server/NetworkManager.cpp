#include "networkManager.h"
#include <thread>
#include "gameManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <locale>
#include <codecvt>

std::thread receiveThread;  // ���ſ� ������ ����
int playerID;
sf::Vector2f playerPosition(100.f, 100.f);
sf::Vector2f opponentPosition(200.f, 100.f);



using namespace std;
using namespace sf;

//GameManager game;

// -- 2025.04.03 �߰�
void NetworkManager::addPlayer(int clientID, sf::Sprite* sprite) {
    players[clientID] = sprite;
}
// --
void NetworkManager::sendPosition(float x, float y) {
    sf::Packet packet;
    packet << playerPosition.x << playerPosition.y;
    //std::wcout << playerPosition.x << std::endl;
    socket.send(packet);
}

void NetworkManager::receivePosition() {
    sf::Packet packet;
    if (socket.receive(packet) == sf::Socket::Status::Done) {
        int receivedID;
        float x, y;
        packet >> receivedID >> x >> y;
        if (receivedID != playerID) {
            opponentPosition = sf::Vector2f(x, y);
        }
    }
}

//NetworkManager::NetworkManager() {
//    std::cout << "NetworkManager ����!" << std::endl;
//    // �����ڿ��� �ʿ��� �ʱ�ȭ ���� ����
//}
// �Ҹ���
//NetworkManager::~NetworkManager() {
//    stopReceiveThread();
//    std::cout << "NetworkManager �Ҹ�!, stopReceiveThread()����" << std::endl;
//}

bool NetworkManager::connectToServer() {
    sf::IpAddress serverIP = {210,119,12,65};
    // ������ ���� �õ� (IP: ��Ʈ: 53000) // 210,119,12,60
    //sf::IpAddress serverIP = sf::/Localhost
    //if (socket.connect({ 210,119,12,69 }, 53000) == sf::Socket::Status::Done) {

    sf::Packet packet;

    if (socket.connect(serverIP, 53000) == sf::Socket::Status::Done) {
        std::cout << serverIP << "���� �����!" << std::endl;
        return true;
    }

    if (socket.receive(packet) == sf::Socket::Status::Done) {
        std::string tag;
        packet >> tag >> clientID;  // �������� ID ����
        if (tag == "ID") {
            std::cout << "�� Ŭ���̾�Ʈ ID: " << clientID << std::endl;
            this->clientID = clientID;  // Ŭ���̾�Ʈ ID ����
        }
        //int clientID;
        else {
            std::cerr << "�߸��� ��Ŷ ����! tag: " << tag << std::endl;
        }
    }
    else {
        std::cerr << "���� ���� ����!" << std::endl;
        return false;
    }
}

bool NetworkManager::waitForMatch(sf::RenderWindow& window, sf::Font& font) {
    //GameManager game;
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
            //player1.send(&countdownStartTime, sizeof(countdownStartTime));
            //player2.send(&countdownStartTime, sizeof(countdownStartTime));

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


// 2025.04.03 �߰� --
// Ŭ���̾�Ʈ���� �̵������� ������ ������ �ڵ�
void NetworkManager::sendMove(sf::TcpSocket& socket, int clientID, float x, float y) {
    std::optional<sf::IpAddress> remoteAddress = socket.getRemoteAddress();
    sf::Packet packet;
    std::string tag = "MOVE";// tag�� ����
    clientID;
    //std::cout << "���� �ּ�: " << remoteAddress->toString() << ", ��Ʈ: " << socket.getRemotePort() << std::endl;
    
    std::cout << "[DEBUG] ���� �ּ�: "
        << (remoteAddress.has_value() ? remoteAddress->toString() : "����")
        << ", ��Ʈ: " << socket.getRemotePort() << std::endl;

    if (!remoteAddress.has_value() || remoteAddress.value() == sf::IpAddress::LocalHost) {
        std::cerr << "[ERROR] ������ ������� ����. �翬�� �õ�..." << std::endl;
        if (!connectToServer()) {
            std::cerr << "[ERROR] ���� �翬�� ����!" << std::endl;
            return;
        }
    }
    //if (remoteAddress.has_value() && remoteAddress.value() == sf::IpAddress::LocalHost/* || socket.getRemotePort() == 0*/) {  // ���� ���� Ȯ��
    //    std::cerr << "������ ������� �ʾҽ��ϴ�. �翬�� �õ� ��..." << std::endl;

    //    if (!connectToServer()) {
    //        return;
    //    }
    //}

    packet << tag << clientID << x << y;    // ��Ŷ ����
    
    // ��Ŷ ����
    sf::Socket::Status status = socket.send(packet);
    if (status == sf::Socket::Status::NotReady) {
        std::cerr << "[ERROR] ������ �غ���� ���� (NotReady)" << std::endl;
    }
    else if (status == sf::Socket::Status::Disconnected) {
        std::cerr << "[ERROR] ������ ���� ���� (Disconnected)" << std::endl;
    }
    else if (status == sf::Socket::Status::Partial) {
        std::cerr << "Partial" << std::endl;
    }
    else if (status == sf::Socket::Status::Error) {
        std::cerr << "[ERROR] ������ ���� ���� (Error)" << std::endl;
    }
    else if (status != sf::Socket::Status::Done) {
        std::cerr << "[ERROR] ��Ŷ ���� ����. ���� �ڵ�: " << static_cast<int>(status) << std::endl;
    }
    else {
        std::cout << "[SUCCESS] �̵� ���� ���� ����: (" << x << ", " << y << ")" << std::endl;
    }

    //do {
    //    status = socket.send(packet);
    //} while (status == sf::Socket::Status::Partial);

    //if (status != sf::Socket::Status::Done) {
    //    std::cerr << "�̵� ���� �̤� ���� �ڵ�: " << static_cast<int>(status) << std::endl;
    //}

    //sf::Socket::Status status = socket.send(packet);
    ////connectToServer();
    //if (status != sf::Socket::Status::Done) {
    //    std::cerr << "�̵� ���� �̤�" << static_cast<int>(status) << std::endl;
    //}
    //else {
    //    std::cout << "�̵� ���� ���� ����: (" << x << ", " << y << ")" << std::endl;
    //}
}
//�������� �̵� ������ �޾� Ŭ���̾�Ʈ�� �����ϴ� �ڵ�(���� �޽����� ���������� ��� �Լ�)
// -- 3��
void NetworkManager::receiveMoves() {
    isReceiving = true;  // ������ ���� �÷��� ����
    std::cout << "[DEBUG] receiveMoves ����, isReceiving: " << isReceiving << std::endl;

    while (isReceiving) {  // ������ ���� ����
        sf::Packet packet;
        sf::Socket::Status status = socket.receive(packet);

        if (status == sf::Socket::Status::Disconnected) {
            std::cerr << "���� ������ ���������ϴ�. ������ ����..." << std::endl;
            break;
        }
        else if (status == sf::Socket::Status::NotReady) {
            std::cerr << "��Ŷ�� ���� �غ� �ȵ�. ��� ��..." << std::endl;
            sf::sleep(sf::milliseconds(10)); // CPU ������ ����
            continue;
        }
        else if (status != sf::Socket::Status::Done) {
            std::cerr << "��Ŷ ���� ���� �߻�! ������ ����..." << std::endl;
            break;
        }

        // ��Ŷ�� ��� �ִ��� Ȯ��
        if (packet.getDataSize() == 0) {
            std::cerr << "������ ��Ŷ�� ��� �ֽ��ϴ�!" << std::endl;
            continue;
        }

        // ������� ���� ��Ŷ ũ�� ���
        std::cout << "������ ��Ŷ ũ��: " << packet.getDataSize() << " ����Ʈ" << std::endl;

        // ��Ŷ���� ������ ����
        std::string tag;
        int clientID;
        float x, y;

        if (!(packet >> tag >> clientID >> x >> y)) {
            std::cerr << "��Ŷ ������ ����ŷ ����! �ùٸ� ������ �ƴմϴ�." << std::endl;
            continue;
        }

        std::cout << "tag: " << tag << ", clientID: " << clientID << std::endl;

        if (tag == "MOVE") {
            std::cout << "Ŭ���̾�Ʈ " << clientID << "�� (" << x << ", " << y << ")�� �̵���" << std::endl;

            // �ش� �÷��̾� ã��
            auto it = players.find(clientID);
            if (it != players.end()) {
                it->second->setPosition(sf::Vector2f(x, y));  // ��ġ ������Ʈ
            }
            else {
                std::cerr << "Error: ID " << clientID << "�� �÷��̾ �������� �ʽ��ϴ�." << std::endl;
            }
        }
    }
}

// --2��
//void NetworkManager::receiveMoves() {
//    std::string tag;
//    int clientID;
//    float x, y;
//    isReceiving = true;  // ������ ���� �÷��� ����
//    std::cout << "receiveMove ȣ��, " << isReceiving << std::endl;
//    while (isReceiving) {  // ������ ���� ����
//        sf::Packet packet;
//        sf::Socket::Status status = socket.receive(packet);
//
//        if (status == sf::Socket::Status::Disconnected) {
//            std::cerr << "���� ������ ���������ϴ�. ������ ����..." << std::endl;
//            //stopReceiveThread();   // ������ ���� �Լ� ȣ��
//            //isReceiving = false;
//            break;
//        }
//        else if (status == sf::Socket::Status::NotReady) {
//            std::cerr << "��Ŷ�� ���� �غ� �ȵ�. ��� ��..." << std::endl;
//            sf::sleep(sf::milliseconds(10)); // CPU ������ ����
//            continue;
//        }
//        else if (status != sf::Socket::Status::Done) {
//            std::cerr << "��Ŷ ���� ���� �߻�! ������ ����..." << std::endl;
//            //stopReceiveThread();   // ������ ���� �Լ� ȣ��
//            //isReceiving = false;
//            break;
//        }
//
//        // �������� ��Ŷ ó��
//   
//        packet >> tag >> clientID >> x >> y;
//        std::cout << "tag: " << tag << "clientID" << clientID << std::endl;
//        if (tag == "MOVE") {
//            std::cout << "Ŭ���̾�Ʈ " << clientID << "�� (" << x << ", " << y << ")�� �̵���" << std::endl;
//
//            // �ش� �÷��̾� ã��
//            auto it = players.find(clientID);
//            if (it != players.end()) {
//                it->second->setPosition(sf::Vector2f(x, y));  // ��ġ ������Ʈ
//            }
//            else {
//                std::cerr << "Error: ID " << clientID << "�� �÷��̾ �������� �ʽ��ϴ�." << std::endl;
//            }
//            break;
//
//            // TODO: GameManager���� �̵� �ݿ�
//            // game.updatePlayerPosition(clientID, x, y);
//        }
//    }
//}

// --1��
//void NetworkManager::receiveMoves() {
//    while (isReceiving) {   // ������ ���� ����
//        sf::Packet packet;
//        sf::Socket::Status status = socket.receive(packet);
//        if (socket.receive(packet) == sf::Socket::Status::Done) {
//            std::string tag;
//            int clientID;
//            clientID = 0;
//            float x, y;
//         
//            packet >> tag >> clientID >> x >> y;
//
//            if (tag == "MOVE") {
//                std::cout << "Ŭ���̾�Ʈ " << clientID << "�� (" << x << ", " << y << ")�� �̵���" << std::endl;
//
//                // �ش� �÷��̾� ã��
//                auto it = players.find(clientID);
//                if (it != players.end()) {
//                    // �ش� �÷��̾� ��ġ ������Ʈ
//                    it->second->setPosition(sf::Vector2f(x, y));
//                }
//                else {
//                    std::cerr << "Error: ID " << clientID << "�� �÷��̾ �������� �ʽ��ϴ�." << std::endl;
//                }
//                // TODO: GameManager���� Ŭ���̾�Ʈ �̵� �ݿ� (ex. �÷��̾� ĳ���� �̵�)
//               // game.updatePlayerPosition(clientID, x, y);
//            }
//        }
//    }
//}
// ���� �޽����� �����ϴ� ������ ����
void NetworkManager::startReceiveThread() {
    receiveThread = std::thread([this]() {
        this->receiveMoves();
        });
    receiveThread.detach();
    //receiveThread = std::thread(&NetworkManager::receiveMoves, this);
    //receiveThread.detach();  // ���� ������� �и��Ͽ� ����
}
// --
// 
// ������ ���� �Լ� �߰�
void NetworkManager::stopReceiveThread() {
    isReceiving = false;  // ���� ���� ��ȣ
}
