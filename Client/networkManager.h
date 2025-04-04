#pragma once
//#ifndef NETWORKMANAGER_H
//#define NETWORKMANAGER_H

#include "gameManager.h"
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>  // sf::RenderWindow와 sf::Font를 사용하기 위해 추가
#include <string>
#include <thread>

//#include "networkManager.h"
//#include <SFML/Network.hpp>
//#include <string>
using namespace std;
using namespace sf;

class NetworkManager {
private:
    int clientID;
    std::thread receiveThread;  // 수신 스레드
    //std::thread startReceiveThread;
    std::unordered_map<int, sf::Sprite*> players; // 플레이어 ID -> Sprite 포인터 매핑
    bool isReceiving = true;  // 스레드 종료를 위한 플래그

    NetworkManager() = default;                         // 생성자 private
    ~NetworkManager() = default;                        // 소멸자 private
    NetworkManager(const NetworkManager&) = delete;     // 복사 생성자 삭제
    NetworkManager& operator=(const NetworkManager&) = delete;

    // 에러 등 연결끊기면 stop 해주면됨.
    //~NetworkManager() {
    //    stopReceiveThread();
    //    std::cout << "NetworkManager 소멸!, stopReceiveThread()실행" << std::endl;
    //};  // 소멸자 추가

public:
    sf::TcpSocket socket;
    // 싱글톤 패턴이라고 하는데, 
    static NetworkManager& getInstance() {
        static NetworkManager instance;
        return instance;
    }
    sf::TcpSocket& getSocket() { return socket; }
    bool connectToServer(); // 서버 연결 함수
    bool waitForMatch(sf::RenderWindow& window, sf::Font& font);    // 1:1 매칭 대기 함수
    //-- 추가
    void sendMove(sf::TcpSocket& socket, int clientID, float x, float y);
    void startReceiveThread();
    //void startReceivingMoves(); // 수신 스레드 시작
    void receiveMoves();  // 서버에서 이동 데이터 수신
    void addPlayer(int clientID, sf::Sprite* sprite); // 플레이어 추가 함수
    void sendPosition(float x, float y);
    void receivePosition();
    void stopReceiveThread(); // 스레드 종료 함수 추가 > 없을 시 무한 루프
    //--
};

//#endif // NETWORKMANAGER_H
