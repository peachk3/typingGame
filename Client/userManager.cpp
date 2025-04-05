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
        std::cout << "MySQL ���� ����!" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "MySQL ���� ����: " << e.what() << std::endl;
    }
}

MySQLConnector::~MySQLConnector() {
    std::cout << "MySQL ���� ����!" << std::endl;
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
            std::cerr << "���̵� �������� �ʽ��ϴ�." << std::endl;  // ���̵� ������ �޽��� ���
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "�α��� ����: " << e.what() << std::endl;
    }
    return false;
}

bool MySQLConnector::registerUser(const std::string& id, const std::string& pw, const std::string& nickname) {
    try {
        // ���̵� �ߺ� üũ
        std::unique_ptr<sql::PreparedStatement> checkIdStmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE user_id = ?"));
        checkIdStmt->setString(1, id);
        std::unique_ptr<sql::ResultSet> resId(checkIdStmt->executeQuery());
        if (resId->next() && resId->getInt(1) > 0) {
            std::cerr << "���̵� �̹� �����մϴ�." << std::endl;  // ���̵� �ߺ� �޽���
            return false;  // �ߺ��� ���̵� ó��
        }

        // �г��� �ߺ� üũ
        std::unique_ptr<sql::PreparedStatement> checkNicknameStmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE nickname = ?"));
        checkNicknameStmt->setString(1, nickname);
        std::unique_ptr<sql::ResultSet> resNickname(checkNicknameStmt->executeQuery());
        if (resNickname->next() && resNickname->getInt(1) > 0) {
            std::cerr << "�г����� �̹� �����մϴ�." << std::endl;  // �г��� �ߺ� �޽���
            return false;  // �ߺ��� �г��� ó��
        }

        // ���̵�� �г����� �ߺ����� ������, ȸ������ ó��
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO users(user_id, password, nickname, profile_img_id) VALUES (?, ?, ?, ?)"));
        pstmt->setString(1, id);
        pstmt->setString(2, pw);
        pstmt->setString(3, nickname);
        pstmt->setString(4, "test_profile");  // not null�̶� �׽�Ʈ�� �ӽ� ����
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "ȸ������ ����: " << e.what() << std::endl;
    }
    return false;
}