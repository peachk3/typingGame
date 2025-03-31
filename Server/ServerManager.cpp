#include "ServerManager.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

ServerManager::ServerManager() : running(true) {
    // 포트 53000에서 클라이언트 연결 대기
    if (listener.listen(53000) != sf::Socket::Status::Done) {
        std::cerr << "서버 실행 실패!" << std::endl;
    }
    else {
        std::cout << "서버 실행 중... (포트 53000)" << std::endl;
    }
    // 별도 스레드에서 대기 중인 클라이언트에게 주기적으로 대기 메시지 전송
    std::thread(&ServerManager::updateWaitingClients, this).detach();
}

ServerManager::~ServerManager() {
    running = false;
    // 필요한 경우 추가 정리 작업
}

void ServerManager::updateWaitingClients() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            // waitingClients에 있는 모든 클라이언트에게 메시지 전송 시도
            for (auto it = waitingClients.begin(); it != waitingClients.end(); ) {
                sf::TcpSocket* client = *it;
                std::string message = "상대 기다리는 중...";
                if (client->send(message.c_str(), message.size() + 1) != sf::Socket::Status::Done) {
                    std::cerr << "대기 메시지 전송 실패!" << std::endl;
                    // 전송 실패하면 해당 클라이언트를 대기열에서 제거
                    it = waitingClients.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ServerManager::waitForClients() {
    while (true) {
        sf::TcpSocket* client = new sf::TcpSocket();
        // 클라이언트 연결 대기
        if (listener.accept(*client) == sf::Socket::Status::Done) {
            // 새로운 클라이언트 처리 스레드 실행
            std::thread(&ServerManager::handleClient, this, client).detach();
        }
    }
}

std::string ServerManager::receiveClientID(sf::TcpSocket* client) {
    char buffer[256];
    std::size_t received;
    // 클라이언트로부터 ID 받기
    if (client->receive(buffer, sizeof(buffer), received) != sf::Socket::Status::Done) {
        std::cerr << "ID 수신 실패!" << std::endl;
        return "";
    }
    return std::string(buffer, received);
}

void ServerManager::sendWaitingMessage(sf::TcpSocket* client) {
    std::string message = "상대 기다리는 중...";
    // 대기 중인 클라이언트에게 대기 메시지 전송
    if (client->send(message.c_str(), message.size() + 1) != sf::Socket::Status::Done) {
        std::cerr << "대기 메시지 전송 실패!" << std::endl;
    }
}

void ServerManager::matchClients() {
    if (waitingClients.size() < 2) return;

    // 매칭된 클라이언트 두 명을 대기열에서 제거
    sf::TcpSocket* client1 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());
    sf::TcpSocket* client2 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());

    std::string startMsg = "매칭 완료! 게임 시작!";
    // 매칭된 클라이언트들에게 게임 시작 메시지 전송
    if (client1->send(startMsg.c_str(), startMsg.size() + 1) != sf::Socket::Status::Done) {
        std::cerr << "메시지 전송 실패 (client1)!" << std::endl;
    }
    if (client2->send(startMsg.c_str(), startMsg.size() + 1) != sf::Socket::Status::Done) {
        std::cerr << "메시지 전송 실패 (client2)!" << std::endl;
    }
    std::cout << "게임 시작 메시지 전송 완료!" << std::endl;

    //// 소켓 종료 및 메모리 해제 (필요에 따라 추가 처리)
    //client1->disconnect();
    //client2->disconnect();
    //delete client1;
    //delete client2;
}

void ServerManager::handleClient(sf::TcpSocket* client) {
    std::string clientID = receiveClientID(client);
    if (clientID.empty()) {
        delete client;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(clientMutex);
        if (connectedClients.find(clientID) != connectedClients.end()) {
            std::cerr << "아이디 중복: " << clientID << " 연결 거부" << std::endl;
            delete client;
            return;
        }

        // 연결된 클라이언트 등록 및 대기열 등록
        connectedClients.insert(clientID);
        waitingClients.push_back(client);
        std::cout << "클라이언트 연결됨: " << clientID << std::endl;

        // 두 명 이상이 모이면 매칭 진행
        if (waitingClients.size() >= 2) {
            matchClients();
        }
    }
}
