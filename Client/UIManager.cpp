//#include "UIManager.hpp"
//#include "gameManager.h"
//#include "networkManager.h"
//#include <iostream>
//
//// Button Ŭ���� ����
//Button::Button(const sf::Texture& texture, const sf::Font& font, const std::wstring& buttonText,
//    float x, float y, float scale, unsigned int fontSize, const std::function<void()>& callback)
//{
//    // ��������Ʈ ����
//    sprite.setTexture(texture);
//    sf::Vector2u textureSize = texture.getSize();
//    float scaleX = scale / textureSize.x;
//    float scaleY = scale / textureSize.y;
//    sprite.setScale(sf::Vector2f(scaleX, scaleY));
//    sprite.setPosition(sf::Vector2f(x, y));
//
//    // �ؽ�Ʈ ����
//    sf::Text text;
//    text.setFont(font);
//    text.setString(buttonText);  // SFML 3.0 ���
//    text.setCharacterSize(fontSize);  // SFML 3.0 ���
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
//// UIManager Ŭ���� ����
//UIManager::UIManager(sf::RenderWindow& win, sf::Font& fnt) : window(win), font(fnt) {}
//
//bool UIManager::loadTextures() {
//    // �ؽ�ó �ε�
//    if (!loadTexture("background", "./assets/image/bgimg.png") ||
//        !loadTexture("profile", "./assets/image/user.png") ||
//        !loadTexture("button", "./assets/image/button.png")) {
//        return false;
//    }
//
//    // ��� ����
//    backgroundSprite.setTexture(textures["background"]);
//    backgroundSprite.setPosition(sf::Vector2f(0.f, 0.f));
//
//    // ���� ��� ��� ����
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
//        std::wcerr << L"�ؽ�ó �ε� ����: " << path.c_str() << std::endl;
//        return false;
//    }
//    textures[name] = texture;
//    return true;
//}
//
//void UIManager::setupMainMenu(GameManager& game, NetworkManager& client) {
//    // ������ ��ư
//    uiElements.push_back(std::make_unique<Button>(
//        textures["profile"], font, L"My Profile",
//        20.f, 20.f, 50.f, 20,
//        [&]() { /* ������ Ŭ�� �� ���� */ }
//    ));
//
//    // �ѱ� Ÿ�� ��ư
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"�ѱ� Ÿ�� ����",
//        320.f, 190.f, 50.f, 30,
//        [&]() {
//            // �ѱ� ��� ���� ����
//            // ��: game.runKoreanMode(window, font);
//        }
//    ));
//
//    // ���� Ÿ�� ��ư
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"���� Ÿ�� ����",
//        320.f, 290.f, 50.f, 30,
//        [&]() {
//            // ���� ��� ���� ����
//            // ��: game.runEnglishMode(window, font);
//        }
//    ));
//
//    // �ڵ� Ÿ�� ��ư
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"�ڵ� ��� ����",
//        320.f, 390.f, 50.f, 30,
//        [&]() {
//            // �ڵ� ��� ���� ����
//            // ��: game.runCodingMode(window, font);
//        }
//    ));
//
//    // ��� ��� ��ư
//    uiElements.push_back(std::make_unique<Button>(
//        textures["button"], font, L"�ڵ� ���",
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