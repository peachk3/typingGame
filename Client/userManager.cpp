/* user manager */
#include "userManager.hpp"
#include <iostream>

using namespace std;

#define SERVER_IP   "tcp://210.119.12.60:3306"
#define USERNAME    "root"
#define PASSWORD    "1234"
#define DATABASE    "teamDB"

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
            "SELECT password FROM users WHERE user_id = ?"));
        pstmt->setString(1, id);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return res->getString("password") == pw;
        }
        else {
            std::cerr << "아이디가 존재하지 않습니다." << std::endl;  // 아이디가 없으면 메시지 출력
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "로그인 오류: " << e.what() << std::endl;
    }
    return false;
}

bool MySQLConnector::registerUser(const std::string& id, const std::string& pw, const std::string& nickname) {
    try {
        // 아이디 중복 체크
        std::unique_ptr<sql::PreparedStatement> checkIdStmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE user_id = ?"));
        checkIdStmt->setString(1, id);
        std::unique_ptr<sql::ResultSet> resId(checkIdStmt->executeQuery());
        if (resId->next() && resId->getInt(1) > 0) {
            std::cerr << "아이디가 이미 존재합니다." << std::endl;  // 아이디 중복 메시지
            return false;  // 중복된 아이디 처리
        }

        // 닉네임 중복 체크
        std::unique_ptr<sql::PreparedStatement> checkNicknameStmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE nickname = ?"));
        checkNicknameStmt->setString(1, nickname);
        std::unique_ptr<sql::ResultSet> resNickname(checkNicknameStmt->executeQuery());
        if (resNickname->next() && resNickname->getInt(1) > 0) {
            std::cerr << "닉네임이 이미 존재합니다." << std::endl;  // 닉네임 중복 메시지
            return false;  // 중복된 닉네임 처리
        }

        // 아이디와 닉네임이 중복되지 않으면, 회원가입 처리
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO users(user_id, password, nickname, profile_img_id) VALUES (?, ?, ?, ?)"));
        pstmt->setString(1, id);
        pstmt->setString(2, pw);
        pstmt->setString(3, nickname);
        pstmt->setString(4, "test_profile");  // not null이라 테스트용 임시 설정
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "회원가입 오류: " << e.what() << std::endl;
    }
    return false;
}