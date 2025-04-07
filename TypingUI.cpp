#include "TypingUI.hpp"

TypingUI::TypingUI(sf::Font& font, Settings& settings)
    : font(font), settings(settings),
    practiceText(font, "", 15),
    userInputText(font, ""),
    currentInput(""),
    practiceContent(L"타자 연습을 시작합니다. 이 텍스트의 크기는 설정에서 변경할 수 있습니다.") {

    init();
}

void TypingUI::init() {
    // 연습 텍스트 설정
    practiceText.setFont(font);
    practiceText.setString(practiceContent);
    practiceText.setCharacterSize(settings.getActualFontSize());
    practiceText.setFillColor(sf::Color::White);
    practiceText.setPosition({ 100.f, 200.f });

    // 사용자 입력 텍스트 설정
    userInputText.setFont(font);
    userInputText.setString(currentInput);
    userInputText.setCharacterSize(settings.getActualFontSize());
    userInputText.setFillColor(sf::Color::Green);
    userInputText.setPosition({ 100.f, 250.f });

    // 입력 박스 설정
    inputBox.setSize(sf::Vector2f(600.f, 50.f));
    inputBox.setPosition({ 90.f, 245.f });
    inputBox.setFillColor(sf::Color(30, 30, 30));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(sf::Color(100, 100, 100));
}

void TypingUI::update() {
    // 폰트 크기 업데이트
    practiceText.setCharacterSize(settings.getActualFontSize());
    userInputText.setCharacterSize(settings.getActualFontSize());

    // 위치 재조정
    practiceText.setPosition({ 100.f, 200.f });
    userInputText.setPosition({ 100.f, 250.f + settings.getActualFontSize() });
    inputBox.setPosition({ 90.f, 245.f + settings.getActualFontSize() });
}

void TypingUI::render(sf::RenderWindow& window) {
    window.draw(inputBox);
    window.draw(practiceText);
    window.draw(userInputText);
}

void TypingUI::handleTextInput(const sf::Event& event) {
    // Handle text entered event
    if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
        // Regular character input
        if (textEvent->unicode >= 32 && textEvent->unicode < 128) {
            currentInput += static_cast<char>(textEvent->unicode);
        }
        // Backspace
        else if (textEvent->unicode == 8 && !currentInput.empty()) {
            currentInput.pop_back();
        }
        // Update the displayed text
        userInputText.setString(currentInput);
    }
}