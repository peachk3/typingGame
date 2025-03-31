#include "ServerManager.h"

int main() {
    // 서버 실행
    ServerManager server;

    server.waitForClients(); // 연결 무한 대기
    return 0;
}
