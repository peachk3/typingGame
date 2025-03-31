#include "ServerManager.h"
#include <thread>


int main() {
    // 서버 실행
    ServerManager server;
    
    server.waitForClients();


    return 0;
}
 