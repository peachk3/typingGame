#include "networkManager.h"
#include <thread>
#include "gameManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <locale>
#include <codecvt>

std::thread receiveThread;  // 수신용 스레드 선언
int playerID;
sf::Vector2f playerPosition(100.f, 100.f);
sf::Vector2f opponentPosition(200.f, 100.f);



using namespace std;
using namespace sf;

//GameManager game;

// -- 2025.04.03 추가
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
//    std::cout << "NetworkManager 생성!" << std::endl;
//    // 생성자에서 필요한 초기화 수행 가능
//}
// 소멸자
//NetworkManager::~NetworkManager() {
//    stopReceiveThread();
//    std::cout << "NetworkManager 소멸!, stopReceiveThread()실행" << std::endl;
//}

bool NetworkManager::connectToServer() {
    sf::IpAddress serverIP = {210,119,12,65};
    // 서버에 연결 시도 (IP: 포트: 53000) // 210,119,12,60
    //sf::IpAddress serverIP = sf::/Localhost
    //if (socket.connect({ 210,119,12,69 }, 53000) == sf::Socket::Status::Done) {

    sf::Packet packet;

    if (socket.connect(serverIP, 53000) == sf::Socket::Status::Done) {
        std::cout << serverIP << "서버 연결됨!" << std::endl;
        return true;
    }

    if (socket.receive(packet) == sf::Socket::Status::Done) {
        std::string tag;
        packet >> tag >> clientID;  // 서버에서 ID 수신
        if (tag == "ID") {
            std::cout << "내 클라이언트 ID: " << clientID << std::endl;
            this->clientID = clientID;  // 클라이언트 ID 저장
        }
        //int clientID;
        else {
            std::cerr << "잘못된 패킷 수신! tag: " << tag << std::endl;
        }
    }
    else {
        std::cerr << "서버 연결 실패!" << std::endl;
        return false;
    }
}

bool NetworkManager::waitForMatch(sf::RenderWindow& window, sf::Font& font) {
    //GameManager game;
    //showLoadingScreen(window, font);
    // 서버로부터 메시지 수신 (매칭 시작 메시지)
    char buffer[128];  // 서버에서 받는 메시지 저장할 버퍼
    std::size_t received;  // 실제로 받은 바이트 수

    sf::TcpSocket player1, player2;
    sf::Time countdownStartTime = sf::seconds(5.0f);  // 카운트다운 5초 설정

    std::cout << "1:1 매칭을 기다리는 중..." << std::endl;
    while (true) {
        if (socket.receive(buffer, sizeof(buffer), received) == sf::Socket::Status::Done) {
            buffer[received] = '\0';  // 문자열 종료 처리
            std::string message = buffer;
            std::cout << "서버 메시지: " << buffer << std::endl;  // 서버 메시지 출력
            // 두 클라이언트에게 같은 카운트다운 시간 전송
            //player1.send(&countdownStartTime, sizeof(countdownStartTime));
            //player2.send(&countdownStartTime, sizeof(countdownStartTime));

            if (std::string(buffer) == "매칭 완료! 게임 시작!") {
                std::cout << "게임이 곧 시작됩니다!" << std::endl;
                return true;
            } else {
                std::cout << "현재 대기 중..." << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 대기
    }
}


// 2025.04.03 추가 --
// 클라이언트에서 이동정보를 서버로 보내는 코드
void NetworkManager::sendMove(sf::TcpSocket& socket, int clientID, float x, float y) {
    std::optional<sf::IpAddress> remoteAddress = socket.getRemoteAddress();
    sf::Packet packet;
    std::string tag = "MOVE";// tag값 설정
    clientID;
    //std::cout << "서버 주소: " << remoteAddress->toString() << ", 포트: " << socket.getRemotePort() << std::endl;
    
    std::cout << "[DEBUG] 서버 주소: "
        << (remoteAddress.has_value() ? remoteAddress->toString() : "없음")
        << ", 포트: " << socket.getRemotePort() << std::endl;

    if (!remoteAddress.has_value() || remoteAddress.value() == sf::IpAddress::LocalHost) {
        std::cerr << "[ERROR] 서버에 연결되지 않음. 재연결 시도..." << std::endl;
        if (!connectToServer()) {
            std::cerr << "[ERROR] 서버 재연결 실패!" << std::endl;
            return;
        }
    }
    //if (remoteAddress.has_value() && remoteAddress.value() == sf::IpAddress::LocalHost/* || socket.getRemotePort() == 0*/) {  // 서버 연결 확인
    //    std::cerr << "서버에 연결되지 않았습니다. 재연결 시도 중..." << std::endl;

    //    if (!connectToServer()) {
    //        return;
    //    }
    //}

    packet << tag << clientID << x << y;    // 패킷 생성
    
    // 패킷 전송
    sf::Socket::Status status = socket.send(packet);
    if (status == sf::Socket::Status::NotReady) {
        std::cerr << "[ERROR] 소켓이 준비되지 않음 (NotReady)" << std::endl;
    }
    else if (status == sf::Socket::Status::Disconnected) {
        std::cerr << "[ERROR] 소켓이 연결 끊김 (Disconnected)" << std::endl;
    }
    else if (status == sf::Socket::Status::Partial) {
        std::cerr << "Partial" << std::endl;
    }
    else if (status == sf::Socket::Status::Error) {
        std::cerr << "[ERROR] 소켓이 연결 끊김 (Error)" << std::endl;
    }
    else if (status != sf::Socket::Status::Done) {
        std::cerr << "[ERROR] 패킷 전송 실패. 상태 코드: " << static_cast<int>(status) << std::endl;
    }
    else {
        std::cout << "[SUCCESS] 이동 정보 전송 성공: (" << x << ", " << y << ")" << std::endl;
    }

    //do {
    //    status = socket.send(packet);
    //} while (status == sf::Socket::Status::Partial);

    //if (status != sf::Socket::Status::Done) {
    //    std::cerr << "이동 실패 ㅜㅜ 상태 코드: " << static_cast<int>(status) << std::endl;
    //}

    //sf::Socket::Status status = socket.send(packet);
    ////connectToServer();
    //if (status != sf::Socket::Status::Done) {
    //    std::cerr << "이동 실패 ㅜㅜ" << static_cast<int>(status) << std::endl;
    //}
    //else {
    //    std::cout << "이동 정보 전송 성공: (" << x << ", " << y << ")" << std::endl;
    //}
}
//서버에서 이동 정보를 받아 클라이언트가 적용하는 코드(서버 메시지를 지속적으로 듣는 함수)
// -- 3번
void NetworkManager::receiveMoves() {
    isReceiving = true;  // 스레드 실행 플래그 설정
    std::cout << "[DEBUG] receiveMoves 실행, isReceiving: " << isReceiving << std::endl;

    while (isReceiving) {  // 스레드 종료 조건
        sf::Packet packet;
        sf::Socket::Status status = socket.receive(packet);

        if (status == sf::Socket::Status::Disconnected) {
            std::cerr << "서버 연결이 끊어졌습니다. 스레드 종료..." << std::endl;
            break;
        }
        else if (status == sf::Socket::Status::NotReady) {
            std::cerr << "패킷을 받을 준비가 안됨. 대기 중..." << std::endl;
            sf::sleep(sf::milliseconds(10)); // CPU 점유율 방지
            continue;
        }
        else if (status != sf::Socket::Status::Done) {
            std::cerr << "패킷 수신 오류 발생! 스레드 종료..." << std::endl;
            break;
        }

        // 패킷이 비어 있는지 확인
        if (packet.getDataSize() == 0) {
            std::cerr << "수신한 패킷이 비어 있습니다!" << std::endl;
            continue;
        }

        // 디버깅을 위한 패킷 크기 출력
        std::cout << "수신한 패킷 크기: " << packet.getDataSize() << " 바이트" << std::endl;

        // 패킷에서 데이터 추출
        std::string tag;
        int clientID;
        float x, y;

        if (!(packet >> tag >> clientID >> x >> y)) {
            std::cerr << "패킷 데이터 언패킹 실패! 올바른 형식이 아닙니다." << std::endl;
            continue;
        }

        std::cout << "tag: " << tag << ", clientID: " << clientID << std::endl;

        if (tag == "MOVE") {
            std::cout << "클라이언트 " << clientID << "가 (" << x << ", " << y << ")로 이동함" << std::endl;

            // 해당 플레이어 찾기
            auto it = players.find(clientID);
            if (it != players.end()) {
                it->second->setPosition(sf::Vector2f(x, y));  // 위치 업데이트
            }
            else {
                std::cerr << "Error: ID " << clientID << "인 플레이어가 존재하지 않습니다." << std::endl;
            }
        }
    }
}

// --2번
//void NetworkManager::receiveMoves() {
//    std::string tag;
//    int clientID;
//    float x, y;
//    isReceiving = true;  // 스레드 실행 플래그 설정
//    std::cout << "receiveMove 호출, " << isReceiving << std::endl;
//    while (isReceiving) {  // 스레드 종료 조건
//        sf::Packet packet;
//        sf::Socket::Status status = socket.receive(packet);
//
//        if (status == sf::Socket::Status::Disconnected) {
//            std::cerr << "서버 연결이 끊어졌습니다. 스레드 종료..." << std::endl;
//            //stopReceiveThread();   // 스레드 종료 함수 호출
//            //isReceiving = false;
//            break;
//        }
//        else if (status == sf::Socket::Status::NotReady) {
//            std::cerr << "패킷을 받을 준비가 안됨. 대기 중..." << std::endl;
//            sf::sleep(sf::milliseconds(10)); // CPU 점유율 방지
//            continue;
//        }
//        else if (status != sf::Socket::Status::Done) {
//            std::cerr << "패킷 수신 오류 발생! 스레드 종료..." << std::endl;
//            //stopReceiveThread();   // 스레드 종료 함수 호출
//            //isReceiving = false;
//            break;
//        }
//
//        // 정상적인 패킷 처리
//   
//        packet >> tag >> clientID >> x >> y;
//        std::cout << "tag: " << tag << "clientID" << clientID << std::endl;
//        if (tag == "MOVE") {
//            std::cout << "클라이언트 " << clientID << "가 (" << x << ", " << y << ")로 이동함" << std::endl;
//
//            // 해당 플레이어 찾기
//            auto it = players.find(clientID);
//            if (it != players.end()) {
//                it->second->setPosition(sf::Vector2f(x, y));  // 위치 업데이트
//            }
//            else {
//                std::cerr << "Error: ID " << clientID << "인 플레이어가 존재하지 않습니다." << std::endl;
//            }
//            break;
//
//            // TODO: GameManager에서 이동 반영
//            // game.updatePlayerPosition(clientID, x, y);
//        }
//    }
//}

// --1번
//void NetworkManager::receiveMoves() {
//    while (isReceiving) {   // 스레드 종료 조건
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
//                std::cout << "클라이언트 " << clientID << "가 (" << x << ", " << y << ")로 이동함" << std::endl;
//
//                // 해당 플레이어 찾기
//                auto it = players.find(clientID);
//                if (it != players.end()) {
//                    // 해당 플레이어 위치 업데이트
//                    it->second->setPosition(sf::Vector2f(x, y));
//                }
//                else {
//                    std::cerr << "Error: ID " << clientID << "인 플레이어가 존재하지 않습니다." << std::endl;
//                }
//                // TODO: GameManager에서 클라이언트 이동 반영 (ex. 플레이어 캐릭터 이동)
//               // game.updatePlayerPosition(clientID, x, y);
//            }
//        }
//    }
//}
// 서버 메시지를 수신하는 스레드 실행
void NetworkManager::startReceiveThread() {
    receiveThread = std::thread([this]() {
        this->receiveMoves();
        });
    receiveThread.detach();
    //receiveThread = std::thread(&NetworkManager::receiveMoves, this);
    //receiveThread.detach();  // 메인 스레드와 분리하여 실행
}
// --
// 
// 스레드 종료 함수 추가
void NetworkManager::stopReceiveThread() {
    isReceiving = false;  // 루프 종료 신호
}
