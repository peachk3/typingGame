#include "networkManager.h"
#include "gameManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <locale>
#include <codecvt>
#include <thread>

NetworkManager::NetworkManager() {
    // 생성자에서 필요한 초기화 수행 가능
}

bool NetworkManager::connectToServer() {
    // 서버에 연결 시도 (IP: 포트: 53000) // 210,119,12,60
    sf::IpAddress serverIP = {210,119,12,60};
    //sf::IpAddress serverIP = sf::/Localhost
    //if (socket.connect({ 210,119,12,69 }, 53000) == sf::Socket::Status::Done) {

    if (socket.connect(serverIP, 53000) == sf::Socket::Status::Done) {
        std::cout << "서버 연결됨!" << std::endl;
        return true;
    }
    else {
        std::cerr << "서버 연결 실패!" << std::endl;
        return false;
    }
}

bool NetworkManager::waitForMatch(sf::RenderWindow& window, sf::Font& font) {
    GameManager game;
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
            player1.send(&countdownStartTime, sizeof(countdownStartTime));
            player2.send(&countdownStartTime, sizeof(countdownStartTime));

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