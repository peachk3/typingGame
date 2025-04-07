#include "TypingUI.hpp"

TypingUI::TypingUI(sf::Font& font, Settings& settings)
    : font(font), settings(settings),
    practiceText(font, "", 15),
    userInputText(font, ""),
    currentInput(""),
    practiceContent(L"Ÿ�� ������ �����մϴ�. �� �ؽ�Ʈ�� ũ��� �������� ������ �� �ֽ��ϴ�.") {

    init();
}

void TypingUI::init() {
    // ���� �ؽ�Ʈ ����
    practiceText.setFont(font);
    practiceText.setString(practiceContent);
    practiceText.setCharacterSize(settings.getActualFontSize());
    practiceText.setFillColor(sf::Color::White);
    practiceText.setPosition({ 100.f, 200.f });

    // ����� �Է� �ؽ�Ʈ ����
    userInputText.setFont(font);
    userInputText.setString(currentInput);
    userInputText.setCharacterSize(settings.getActualFontSize());
    userInputText.setFillColor(sf::Color::Green);
    userInputText.setPosition({ 100.f, 250.f });

    // �Է� �ڽ� ����
    inputBox.setSize(sf::Vector2f(600.f, 50.f));
    inputBox.setPosition({ 90.f, 245.f });
    inputBox.setFillColor(sf::Color(30, 30, 30));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(sf::Color(100, 100, 100));
}

void TypingUI::update() {
    // ��Ʈ ũ�� ������Ʈ
    practiceText.setCharacterSize(settings.getActualFontSize());
    userInputText.setCharacterSize(settings.getActualFontSize());

    // ��ġ ������
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