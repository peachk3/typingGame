/* login ui.hpp */
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "userManager.hpp"

// 로그인 창을 띄우고 로그인 성공 시 true 반환, userID를 참조로 받아옴
bool showLoginWindow(sf::RenderWindow& window, MySQLConnector& db, std::string& userID);

// 회원가입 창 내부에서 사용됨
void showSignupWindow(sf::RenderWindow& parentWindow, MySQLConnector& db);