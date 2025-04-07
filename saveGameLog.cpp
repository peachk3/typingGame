#include "saveGameLog.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <ctime>
#include <sstream>
#include <Windows.h>
#include <string>

#define SERVER_IP   "tcp://210.119.12.60:3306"
#define USERNAME    "root"
#define PASSWORD    "1234"
#define DATABASE    "teamDB"

using namespace std;

MySQLConnector::MySQLConnector() {
    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        conn = std::unique_ptr<sql::Connection>(driver->connect(SERVER_IP, USERNAME, PASSWORD));
        conn->setSchema(DATABASE);
        std::cout << "MySQL 연결 성공!" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "MySQL 연결 실패: " << e.what() << std::endl;
    }
}

MySQLConnector::~MySQLConnector() {
    std::cout << "MySQL 연결 종료!" << std::endl;
}

bool MySQLConnector::checkLogin(const string& id, const string& pw) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE BINARY user_id = ? AND BINARY password = ?"));
        pstmt->setString(1, id);
        pstmt->setString(2, pw);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next() && res->getInt(1) > 0) {
            return true;
        }
        else {
            std::cerr << "아이디 또는 비밀번호가 올바르지 않습니다." << std::endl;
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "로그인 오류: " << e.what() << std::endl;
    }
    return false;
}

bool MySQLConnector::registerUser(const std::string& id, const std::string& pw, const std::string& nickname) {
    try {
        std::unique_ptr<sql::PreparedStatement> checkIdStmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE BINARY user_id = ?"));
        checkIdStmt->setString(1, id);
        std::unique_ptr<sql::ResultSet> resId(checkIdStmt->executeQuery());
        if (resId->next() && resId->getInt(1) > 0) {
            std::cerr << "아이디가 이미 존재합니다." << std::endl;
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> checkNicknameStmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE nickname = ?"));
        checkNicknameStmt->setString(1, nickname);
        std::unique_ptr<sql::ResultSet> resNickname(checkNicknameStmt->executeQuery());
        if (resNickname->next() && resNickname->getInt(1) > 0) {
            std::cerr << "닉네임이 이미 존재합니다." << std::endl;
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO users(user_id, password, nickname, profile_img_id) VALUES (?, ?, ?, ?)"));
        pstmt->setString(1, id);
        pstmt->setString(2, pw);
        pstmt->setString(3, nickname);
        pstmt->setString(4, "test_profile");  // 기본 이미지 설정
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "회원가입 오류: " << e.what() << std::endl;
    }
    return false;
}

// 게임 로그 저장 함수
bool MySQLConnector::saveGameLog(const GameState& game, const std::string& sessionId) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO game_logs (session_id, user_id, log_date, play_time, log_speed, log_accuracy, result) "
            "VALUES (?, ?, NOW(), ?, ?, ?, ?)"
        ));

        // wstring → UTF-8 string 변환
        std::wstring wuserId = game.user.id;
        std::string userId;
        if (!wuserId.empty()) {
            int size = WideCharToMultiByte(CP_UTF8, 0, wuserId.c_str(), -1, nullptr, 0, nullptr, nullptr);
            userId.resize(size);
            WideCharToMultiByte(CP_UTF8, 0, wuserId.c_str(), -1, &userId[0], size, nullptr, nullptr);
            userId.pop_back(); // null 제거
        }

        pstmt->setString(1, sessionId);
        pstmt->setString(2, userId);
        pstmt->setInt(3, static_cast<int>(game.elapsedSeconds));
        pstmt->setInt(4, static_cast<int>(game.tpm));
        pstmt->setInt(5, static_cast<int>(game.accuracy));

        // 성공 여부 계산
        int resultCode = game.progress >= 100.0f ? 1 : 0;
        pstmt->setInt(6, resultCode);

        pstmt->execute();
        std::wcout << L"[DB] 게임 결과가 저장되었습니다!" << std::endl;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[DB ERROR] 게임 로그 저장 실패: " << e.what() << std::endl;
    }
    return false;
}