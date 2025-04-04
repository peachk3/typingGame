// -- 2025.04.02 추가
#include "ServerManager.h"
#include <thread>

int main() {
    ServerManager server;

    // 클라이언트 대기용 스레드 실행
    std::thread clientThread(&ServerManager::waitForClients, &server);
    clientThread.detach();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 서버 유지
    }

    return 0;
}
// --


// 원본
//#include "ServerManager.h"
//#include <thread>
//
//
//int main() {
//    // 서버 실행
//    ServerManager server;
//    
//    server.waitForClients();
//
//    return 0;
//}
// 