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

//ServerManager server;

std::mutex clientMutex;
std::vector<sf::TcpSocket*> waitingClients;
std::set<std::string> connectedIPs;
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
        sf::TcpSocket* client = new sf::TcpSocket();                        // 클라이언트 연결 대기
        if (listener.accept(*client) == sf::Socket::Status::Done) {         // 새로운 클라이언트 접속 대기, 연결 요청 수락
            std::optional<sf::IpAddress> clientIPOpt = client->getRemoteAddress();  // 클라이언트 IP 주소를 가져옴

            if (clientIPOpt) {                          
                std::lock_guard<std::mutex> lock(clientMutex);
                sf::IpAddress clientIP = *clientIPOpt;
                std::string ipStr = clientIP.toString();

                connectedIPs.insert(ipStr);

                // **🆕 클라이언트 ID 할당 후 map에 저장**
                int clientID = clientIDCounter++;                   // 클라이언트에게 고유한 ID 부여
                sockets[clientID] = client;

                waitingClients.push_back(client);
                std::cout << "클라이언트 " << clientID << " 연결됨! 현재 대기 인원: " << waitingClients.size() << std::endl;

                // 클라이언트에게 본인의 ID 전송
                sf::Packet idPacket;                                // 클라이언트에게 ID 정보를 전송
                //int ID;
                idPacket << "ID" << clientID;
                client->send(idPacket);

                if (waitingClients.size() == 1) {
                    sf::TcpSocket* waitingClient = waitingClients.front();
                    std::string waitingMsg = "상대 기다리는 중...";
                    //waitingClient->send(waitingMsg.c_str(), waitingMsg.size() + 1);
                }

                if (waitingClients.size() >= 2) {       // 대기중 클라이언트가 2명 이상이면 자동 매칭 실행
                    std::thread matchThread(&ServerManager::matchClients, this);
                    matchThread.detach();
                }

            }
            else {
                std::cerr << "클라이언트 IP 주소 가져오기 실패!" << std::endl;
                delete client;
            }
        }
    }
}
// 매개변수 float x, float y 추가
void ServerManager::handleClient(int clientID, float x, float y) {
    TcpSocket* client = sockets[clientID];

    while (true) {
        sf::Packet packet;
        if (client->receive(packet) == sf::Socket::Status::Done) {
            std::string tag;
            float x, y;
            tag = "GAME";
            packet >> tag >> clientID >> x >> y;

            if (tag == "GAME") {
                std::cout << "클라이언트 " << clientID << " 이동 감지: (" << x << ", " << y << ")" << std::endl;

                // **자기 자신을 포함하여 모든 클라이언트에게 이동 정보 전송**
                for (auto& [id, socket] : sockets) {
                    sf::Packet sendPacket;
                    sendPacket << "GAME" << clientID << x << y;
                    socket->send(sendPacket);
                }
            }
        }
    }
}
// 자기 자신 제외
//void ServerManager::handleClient(int clientID) {
//    TcpSocket* client = sockets[clientID];
//
//    while (true) {
//        sf::Packet packet;
//        if (client->receive(packet) == sf::Socket::Status::Done) {
//            std::string tag;
//            float x, y;
//            packet >> tag >> x >> y;
//
//            if (tag == "MOVE") {
//                std::cout << "클라이언트 " << clientID << " 이동 감지: (" << x << ", " << y << ")" << std::endl;
//
//                // ** 다른 모든 클라이언트에게 이동 데이터 전송**
//                for (auto& [id, socket] : sockets) {
//                    if (id != clientID) {  // 자기 자신 제외
//                        sf::Packet sendPacket;
//                        sendPacket << "MOVE" << clientID << x << y;
//                        socket->send(sendPacket);
//                    }
//                }
//            }
//        }
//    }
//}

// 클라이언트 매칭 (별도 스레드 실행)
void ServerManager::matchClients() {
    std::lock_guard<std::mutex> lock(clientMutex);

    if (waitingClients.size() < 2) return;

    sf::TcpSocket* client1 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());
    sf::TcpSocket* client2 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());

    int id1 = -1, id2 = -1;
    float x = 0.0;
    float y = 0.0;

    // ** 클라이언트 ID 찾기**
    for (auto& [id, socket] : sockets) {
        if (socket == client1) id1 = id;
        if (socket == client2) id2 = id;
    }

    std::string startMsg = "매칭 완료! 게임 시작!";
    client1->send(startMsg.c_str(), startMsg.size() + 1);
    client2->send(startMsg.c_str(), startMsg.size() + 1);

    std::cout << "게임 시작 메시지 전송 완료! (" << id1 << " vs " << id2 << ")" << std::endl;

    // ** 클라이언트 ID를 넘겨서 관리**
    std::thread client1Thread(&ServerManager::handleClient, this, id1, x, y); // (id, x, y)
    std::thread client2Thread(&ServerManager::handleClient, this, id2, x, y);

    client1Thread.detach();
    client2Thread.detach();
}
