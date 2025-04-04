// -- 2025.04.02 �߰�
#include "ServerManager.h"
#include <thread>

int main() {
    ServerManager server;

    // Ŭ���̾�Ʈ ���� ������ ����
    std::thread clientThread(&ServerManager::waitForClients, &server);
    clientThread.detach();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // ���� ����
    }

    return 0;
}
// --


// ����
//#include "ServerManager.h"
//#include <thread>
//
//
//int main() {
//    // ���� ����
//    ServerManager server;
//    
//    server.waitForClients();
//
//    return 0;
//}
// 