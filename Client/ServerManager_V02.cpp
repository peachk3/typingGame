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

std::mutex clientMutex;  // 클라이언트 리스트 보호
std::vector<sf::TcpSocket*> connectedClients;  // 연결된 클라이언트 목록
std::set<std::string> connectedIPs;  // 중복 연결 방지
TcpListener listener;
std::map<int, TcpSocket*> sockets;  // 클라이언트 ID - 소켓 매핑
int clientIDCounter = 0;  // 클라이언트 ID 증가용

ServerManager::ServerManager() {
    if (listener.listen(53000) != sf::Socket::Status::Done) {
        std::cerr << "서버 실행 실패!" << std::endl;
        return;
    }
    else {
        std::cout << "서버 실행 중... (포트 53000)" << std::endl;
    }
  
}

// 클라이언트 대기 함수 (스레드에서 실행)
void ServerManager::waitForClients() {
    while (true) {
        sf::TcpSocket* client = new sf::TcpSocket();
        if (listener.accept(*client) == sf::Socket::Status::Done) {
            std::optional<sf::IpAddress> clientIPOpt = client->getRemoteAddress();

            if (clientIPOpt) {
                std::lock_guard<std::mutex> lock(clientMutex);  // 뮤텍스 잠금
                sf::IpAddress clientIP = *clientIPOpt;
                std::string ipStr = clientIP.toString();

                connectedIPs.insert(ipStr);
                waitingClients.push_back(client);
                std::cout << "클라이언트 연결됨! 현재 대기 인원: " << waitingClients.size() << std::endl;

                if (waitingClients.size() == 1) {
                    sf::TcpSocket* waitingClient = waitingClients.front();
                    std::string waitingMsg = "상대 기다리는 중...";
                    waitingClient->send(waitingMsg.c_str(), waitingMsg.size() + 1);
                }

                if (waitingClients.size() >= 2) {
                    std::thread matchThread(&ServerManager::matchClients, this);
                    matchThread.detach();  // 매칭 스레드 분리 실행
                }

            }
            else {
                std::cerr << "클라이언트 IP 주소 가져오기 실패!" << std::endl;
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
                std::cout << "클라이언트 이동 감지: (" << x << ", " << y << ")" << std::endl;

                // 모든 클라이언트에게 이동 데이터 전송
                for (auto& c : connectedClients) {
                    sf::Packet sendPacket;
                    sendPacket << "MOVE" << x << y;
                    c->send(sendPacket);
                }
            }
        }
    }
}

// 클라이언트 매칭 (별도 스레드 실행)
void ServerManager::matchClients() {
    std::lock_guard<std::mutex> lock(clientMutex);

    if (waitingClients.size() < 2) return;

    sf::TcpSocket* client1 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());
    sf::TcpSocket* client2 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());

    std::string startMsg = "매칭 완료! 게임 시작!";
    client1->send(startMsg.c_str(), startMsg.size() + 1);
    client2->send(startMsg.c_str(), startMsg.size() + 1);

    std::cout << "게임 시작 메시지 전송 완료!" << std::endl;

    // 각 클라이언트에 대한 스레드 생성
    std::thread client1Thread(&ServerManager::handleClient, this, client1);
    std::thread client2Thread(&ServerManager::handleClient, this, client2);

    client1Thread.detach();
    client2Thread.detach();
}

// 클라이언트 데이터 처리 함수
//void ServerManager::handleClient(sf::TcpSocket* client) {
//    char buffer[1024];
//    std::size_t received;
//
//    while (true) {
//        if (client->receive(buffer, sizeof(buffer), received) == sf::Socket::Status::Done) {
//            std::cout << "클라이언트 메시지: " << buffer << std::endl;
//        }
//    }
//}
/*
void ServerManager::handleClients(sf::TcpSocket* client1, sf::TcpSocket* client2) {
    while (true) {
        sf::Packet packet;
        sf::Socket::Status status = client1->receive(packet);

        if (status == sf::Socket::Status::Done) {
            // 받은 메시지를 클라이언트 2에게 전달
            std::lock_guard<std::mutex> lock(clientMutex);
            client2->send(packet);
        }
        else if (status == sf::Socket::Status::Disconnected) {
            // 클라이언트1이 연결 종료 -> 클라이언트2에게 알림
            std::lock_guard<std::mutex> lock(clientMutex);
            std::string disconnectMsg = "상대방이 연결을 종료했습니다.";
            sf::Packet disconnectPacket;
            disconnectPacket << disconnectMsg;
            client2->send(disconnectPacket);
            break;  // 루프 종료
        }

        // 반대 방향 메시지 전달
        status = client2->receive(packet);
        if (status == sf::Socket::Status::Done) {
            std::lock_guard<std::mutex> lock(clientMutex);
            client1->send(packet);
        }
        else if (status == sf::Socket::Status::Disconnected) {
            std::lock_guard<std::mutex> lock(clientMutex);
            std::string disconnectMsg = "상대방이 연결을 종료했습니다.";
            sf::Packet disconnectPacket;
            disconnectPacket << disconnectMsg;
            client1->send(disconnectPacket);
            break;
        }
    }

    // 클라이언트 소켓 정리
    std::lock_guard<std::mutex> lock(clientMutex);
    client1->disconnect();
    client2->disconnect();
    delete client1;
    delete client2;

    std::cout << "클라이언트 연결 종료 및 정리 완료" << std::endl;
}
*/
