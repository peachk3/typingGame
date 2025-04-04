//#include "UIManager.hpp"
//#include "gameManager.h"
//#include "networkManager.h"
//#include <iostream>
//
//// Button 클래스 구현
//Button::Button(const sf::Texture& texture, const sf::Font& font, const std::wstring& buttonText,
//    float x, float y, float scale, unsigned int fontSize, const std::function<void()>& callback)
//{
//    // 스프라이트 설정
//    sprite.setTexture(texture);
//    sf::Vector2u textureSize = texture.getSize();
//    float scaleX = scale / textureSize.x;
//    float scaleY = scale / textureSize.y;
//    sprite.setScale(sf::Vector2f(scaleX, scaleY));
//    sprite.setPosition(sf::Vector2f(x, y));
//
//    // 텍스트 설정
//    sf::Text text;
//    text.setFont(font);
//    text.setString(buttonText);  // SFML 3.0 방식
//    text.setCharacterSize(fontSize);  // SFML 3.0 방식
//    text.setPosition(sf::Vector2f(x + scale + 30.f, y + scale / 2 - fontSize / 2));
//    text.setFillColor(sf::Color::Black);
//
//    onClick = callback;
//}
//
//void Button::draw(sf::RenderWindow& window) {
//    window.draw(sprite);
//    window.draw(text);
//}
//
//bool Button::isMouseOver(const sf::Vector2f& mousePos) {
//    return sprite.getGlobalBounds().contains(mousePos);
//}
//
//void Button::executeCallback() {
//    if (onClick) {
//        onClick();
//    }
//}
//
//// UIManager 클래스 구현
//UIManager::UIManager(sf::RenderWindow& win, sf::Font& fnt) : window(win), font(fnt) {}
//
//bool UIManager::loadTextures() {
//    // 텍스처 로드
//    if (!loadTexture("background", "./assets/image/bgimg.png") ||
//        !loadTexture("profile", "./assets/image/user.png") ||
//        !loadTexture("button", "./assets/image/button.png")) {
//        return false;
//    }
//
//    // 배경 설정
//    backgroundSprite.setTexture(textures["background"]);
//    backgroundSprite.setPosition(sf::Vector2f(0.f, 0.f));
//
//    // 투명 흰색 배경 설정
//    whiteBackground.setSize(sf::Vector2f(800.f, 400.f));
//    whiteBackground.setPosition(sf::Vector2f(200.f, 150.f));
//    whiteBackground.setFillColor(sf::Color(255, 255, 255, 200));
//    whiteBackground.setOutlineThickness(1.f);
//    whiteBackground.setOutlineColor(sf::Color(100, 100, 100));
//
//    return true;
//}
//
//bool UIManager::loadTexture(const std::string& name, const std::string& path) {
//    sf::Texture texture;
//    if (!texture.loadFromFile(path)) {
//        std::wcerr << L"텍스처 로드 실패: " << path.c_str() << std::endl;
//        return false;
//    }
//    textures[name] = texture;
//    return true;
//}
//
//void UIManager::setupMainMenu(GameManager& game, NetworkManager& client) {
//    // 프로필 버튼
//    uiElements.push_back(std::make_unique<Button>(
//        textures["profile"], font, L"My Profile",
//        20.f, 20.f, 50.f, 20,
//        [&]() { /* 프로필 클릭 시 동작 */ }
//    ));
//
//    // 한글 타자 버튼
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"한글 타자 연습",
//        320.f, 190.f, 50.f, 30,
//        [&]() {
//            // 한글 모드 실행 구현
//            // 예: game.runKoreanMode(window, font);
//        }
//    ));
//
//    // 영문 타자 버튼
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"영문 타자 연습",
//        320.f, 290.f, 50.f, 30,
//        [&]() {
//            // 영문 모드 실행 구현
//            // 예: game.runEnglishMode(window, font);
//        }
//    ));
//
//    // 코딩 타자 버튼
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"코딩 모드 연습",
//        320.f, 390.f, 50.f, 30,
//        [&]() {
//            // 코딩 모드 실행 구현
//            // 예: game.runCodingMode(window, font);
//        }
//    ));
//
//    // 대결 모드 버튼
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"코딩 대결",
//        320.f, 490.f, 50.f, 30,
//        [&]() {
//            client.waitForMatch(window, font);
//        }
//    ));
//}
//
//void UIManager::drawAll() {
//    window.draw(backgroundSprite);
//    window.draw(whiteBackground);
//
//    for (auto& element : uiElements) {
//        element->draw(window);
//    }
//}
//
//void UIManager::handleEvents(const sf::Event& event) {
//   if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
//       sf::Vector2i mousePos(mousePressed->position.x, mousePressed->position.y);
//
//       for (auto& element : uiElements) {
//           if (element->isMouseOver(static_cast<sf::Vector2f>(mousePos))) {
//               if (auto* button = dynamic_cast<Button*>(element.get())) {
//                   button->executeCallback();
//               }
//           }
//       }
//   }
//}