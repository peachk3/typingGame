#include "ResultScreen.hpp"
#include <sstream>

bool showResultWindow(
    GameState& game,
    const sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& sentences)
{
    // ��� â ��� ������ ����
    sf::RenderWindow resultWindow(sf::VideoMode({ 600, 400 }), "Final Result");

    //���� �÷��� �ð� ���
    int totalSeconds = static_cast<int>(game.result.gamePlayTime);

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;



    // ����� �������� ���� wstringstream ��ȯ �� ���
    std::wstringstream ss;
    ss << L"- Time(s): " << minutes << L":" << std::setw(2) << std::setfill(L'0') << seconds << L"\n";
    ss.unsetf(std::ios::fixed); // ������ fixed ����
    ss << L"- Ÿ��: " << static_cast<int>(game.result.avgTpm) << L"Ÿ\n";
    ss << std::fixed << std::setprecision(2);               // �Ǽ� �ڸ� �� ����
    ss << L"- Words Per Minute(WPM): " << game.result.avgWpm << L" WPM\n";
    ss << L"- Accuracy(%): " << game.accuracy << L"%";



    // ��� �ؽ�Ʈ
    sf::Text resultDisplay(font, ss.str(), 24);
    resultDisplay.setPosition({ 50, 50 });
    resultDisplay.setFillColor(sf::Color::White);
    // ����� ��ư 
    sf::RectangleShape retryButton(sf::Vector2f(200, 50));
    retryButton.setPosition({ 200, 250 });
    retryButton.setFillColor(sf::Color::Green);
    // ����� �ؽ�Ʈ
    sf::Text retryText(font, "Retry", 24);
    retryText.setPosition({ 250, 260 });
    retryText.setFillColor(sf::Color::White);
    // ������ ��ư
    sf::RectangleShape exitButton(sf::Vector2f(200, 50));
    exitButton.setPosition({ 200, 320 });
    exitButton.setFillColor(sf::Color::Red);
    // ������ �ؽ�Ʈ
    sf::Text exitText(font, "Exit", 24);
    exitText.setPosition({ 250, 330 });
    exitText.setFillColor(sf::Color::White);

    while (resultWindow.isOpen()) {
        // �̺�Ʈ ó��
        while (const std::optional<sf::Event> resultEvent = resultWindow.pollEvent()) {
            if (resultEvent->is<sf::Event::Closed>()) resultWindow.close();

            if (resultEvent->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(resultWindow);
                if (retryButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    resultWindow.close();
                    game.readyToShowResult = false;
                    return true;    // ������ ����� �� ���
                }
                else if (exitButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    resultWindow.close();
                    game.readyToShowResult = false;
                    return false;   // ���� �޴��� ���ư� ���
                    //window.close();
                }
            }
        }
        // ������
        resultWindow.clear(sf::Color::Black);
        resultWindow.draw(resultDisplay);
        resultWindow.draw(retryButton);
        resultWindow.draw(retryText);
        resultWindow.draw(exitButton);
        resultWindow.draw(exitText);
        resultWindow.display();
    }
}