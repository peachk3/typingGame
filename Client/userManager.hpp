/* userManager.hpp */
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <vector>
#include <string>
#include <memory>
#include <mysql/jdbc.h>
//#include <cppconn/driver.h>
//#include <cppconn/connection.h>
//#include <cppconn/prepared_statement.h>
//#include <cppconn/resultset.h>

struct PlayerRanking {
    std::string nickname;
    int totalScore;
    int totalPlayTime;
    float avgAccuracy;
};

class MySQLConnector {
private:
    std::unique_ptr<sql::Connection> conn;

public:
    MySQLConnector();
    ~MySQLConnector();

    bool checkLogin(const std::string& id, const std::string& pw);
    bool registerUser(const std::string& id, const std::string& pw, const std::string& nickname);

    std::vector<PlayerRanking> getRankingData();
};

#endif // USERMANAGER_H