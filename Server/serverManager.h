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
    bool running; // 대기 메시지 전송 스레드 종료를 위한 플래그

public:
    ServerManager();
    //~ServerManager();
    void waitForClients();
    void matchClients();
    std::string receiveClientID(sf::TcpSocket* client);  // 클라이언트로부터 ID 받기
    void sendWaitingMessage(sf::TcpSocket* client);        // 대기 메시지 전송
    void handleClient(int clientID, float x, float y);              // 클라이언트 처리
    //void handleClients();              // 클라이언트 처리

    // 추가: 1명일 때 주기적으로 대기 메시지를 보내는 함수
    void updateWaitingClients();
};

extern ServerManager server;  // 전역 변수 선언 (정의 X)

#endif // SERVERMANAGER_H
