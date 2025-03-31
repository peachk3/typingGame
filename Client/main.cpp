#include <SFML/Graphics.hpp>
#include "networkManager.h"
#include "gameManager.h"
#include <thread>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "아이디 입력");

    sf::Font font;
    if (!font.openFromFile("D2Coding.ttf")) {  // 한글 폰트 로드
        std::cerr << "폰트 로드 실패!" << std::endl;
        return -1;
    }

    std::string clientID = "";
    showIDInputScreen(window, font, clientID);
    std::cout << "입력된 아이디: " << clientID << std::endl;

    NetworkManager network;
    if (network.connectToServer(clientID)==true) {
        // 서버로부터 매칭 메시지를 수신할 때까지 대기
        std::cout << "waitForGameStart: " << clientID << std::endl;
        waitForGameStart(network.getSocket(), window, font);
        
    }

    return 0;
}
