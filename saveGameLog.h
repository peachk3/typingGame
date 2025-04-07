#pragma once
#include <memory>
#include <string>
#include <mysql/jdbc.h>
#include "GameState.hpp"  // GameState 구조체 선언 필요

class MySQLConnector {
private:
    std::unique_ptr<sql::Connection> conn;

public:
    MySQLConnector();
    ~MySQLConnector();

    bool checkLogin(const std::string& id, const std::string& pw);
    bool registerUser(const std::string& id, const std::string& pw, const std::string& nickname);

    // 게임 로그 저장
    bool saveGameLog(const GameState& game, const std::string& sessionId);
};
