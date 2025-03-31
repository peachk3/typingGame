#include "serverManager.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <optional>  // std::optional을 사용하기 위한 헤더 추가
#include <thread>
#include <mutex>
#include <map>

ServerManager::ServerManager() {
    // 서버가 53000 포트에서 연결을 기다리도록 설정
    if (listener.listen(53000) != sf::Socket::Status::Done) {
        std::cerr << "서버 실행 실패!" << std::endl;
    }
    else {
        std::cout << "서버 실행 중... (포트 53000)" << std::endl;
    }
}

void ServerManager::waitForClients() {
    // 클라이언트가 연결되면 대기열에 추가
    while (true) {
        sf::TcpSocket* client = new sf::TcpSocket();  // 새로운 클라이언트 소켓 생성
        // 클라이언트가 연결되면 소켓을 받아옴
        if (listener.accept(*client) == sf::Socket::Status::Done) {
            // 클라이언트의 IP를 확인하여 이미 접속한 IP인지 확인
            std::optional<sf::IpAddress> clientIPOpt = client->getRemoteAddress(); // std::optional로 IP 주소 받기

            if (clientIPOpt) {  // IP 값이 존재하는 경우에만 처리
                sf::IpAddress clientIP = *clientIPOpt;  // 값을 꺼내서 사용
                std::string ipStr = clientIP.toString();

                //// 이미 해당 IP가 연결된 경우, 접속 거부
                //if (connectedIPs.find(ipStr) != connectedIPs.end()) {
                //    std::cout << "IP " << ipStr << "에서 이미 클라이언트가 연결되어 있습니다. 연결을 거부합니다." << std::endl;
                //    delete client;  // 클라이언트 소켓 메모리 해제
                //    continue;  // 다음 클라이언트 연결 시도
                //}

                // 해당 IP를 접속한 IP 목록에 추가
                connectedIPs.insert(ipStr);
                waitingClients.push_back(client);  // 대기열에 클라이언트 추가
                std::cout << "클라이언트 연결됨! 현재 대기 인원: " << waitingClients.size() << std::endl;

                // 대기 중인 클라이언트가 1명 이상일 경우 '상대 기다리는 중' 메시지 보내기
                if (waitingClients.size() == 1) {
                    sf::TcpSocket* waitingClient = waitingClients.front();  // 대기 중인 클라이언트
                    std::string waitingMsg = "상대 기다리는 중...";
                    waitingClient->send(waitingMsg.c_str(), waitingMsg.size() + 1);  // 메시지 전송
                    std::cout << "대기열에 대기 메시지 전송!" << std::endl;
                }

                // 대기열에 2명이 되면 매칭 시작
                if (waitingClients.size() >= 2) {
                    matchClients();  // 2명 매칭
                }

            }
            else {
                std::cerr << "클라이언트의 IP 주소를 가져오는 데 실패했습니다." << std::endl;
                delete client;  // IP 주소를 가져올 수 없으면 연결을 끊음
            }
        }
    }
}


void ServerManager::matchClients() {
    std::lock_guard<std::mutex> lock(clientMutex);

    if (waitingClients.size() < 2) return;

    // 매칭된 클라이언트 두 명을 대기열에서 제거하고, 게임 시작 메시지 전송
    sf::TcpSocket* client1 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());  // 첫 번째 클라이언트 제거
    sf::TcpSocket* client2 = waitingClients.front();
    waitingClients.erase(waitingClients.begin());  // 두 번째 클라이언트 제거

    //// 매칭된 클라이언트 목록에 추가
    //matchedClients.push_back(client1);
    //matchedClients.push_back(client2);

    std::string startMsg = "매칭 완료! 게임 시작!";
    client1->send(startMsg.c_str(), startMsg.size() + 1);  // 첫 번째 클라이언트에게 메시지 전송
    client2->send(startMsg.c_str(), startMsg.size() + 1);  // 두 번째 클라이언트에게 메시지 전송

    std::cout << "게임 시작 메시지 전송 완료!" << std::endl;

}
