#include "ResultScreen.hpp"
#include <sstream>

bool showResultWindow(
    GameState& game,
    const sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& sentences)
{
    // 결과 창 띄울 윈도우 생성
    sf::RenderWindow resultWindow(sf::VideoMode({ 600, 400 }), "Final Result");

    //게임 플레이 시간 계산
    int totalSeconds = static_cast<int>(game.result.gamePlayTime);

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;



    // 깔끔한 포멧팅을 위해 wstringstream 변환 후 출력
    std::wstringstream ss;
    ss << L"- Time(s): " << minutes << L":" << std::setw(2) << std::setfill(L'0') << seconds << L"\n";
    ss.unsetf(std::ios::fixed); // 정수는 fixed 없이
    ss << L"- 타수: " << static_cast<int>(game.result.avgTpm) << L"타\n";
    ss << std::fixed << std::setprecision(2);               // 실수 자리 수 제한
    ss << L"- Words Per Minute(WPM): " << game.result.avgWpm << L" WPM\n";
    ss << L"- Accuracy(%): " << game.accuracy << L"%";



    // 결과 텍스트
    sf::Text resultDisplay(font, ss.str(), 24);
    resultDisplay.setPosition({ 50, 50 });
    resultDisplay.setFillColor(sf::Color::White);
    // 재시작 버튼 
    sf::RectangleShape retryButton(sf::Vector2f(200, 50));
    retryButton.setPosition({ 200, 250 });
    retryButton.setFillColor(sf::Color::Green);
    // 재시작 텍스트
    sf::Text retryText(font, "Retry", 24);
    retryText.setPosition({ 250, 260 });
    retryText.setFillColor(sf::Color::White);
    // 나가기 버튼
    sf::RectangleShape exitButton(sf::Vector2f(200, 50));
    exitButton.setPosition({ 200, 320 });
    exitButton.setFillColor(sf::Color::Red);
    // 나가기 텍스트
    sf::Text exitText(font, "Exit", 24);
    exitText.setPosition({ 250, 330 });
    exitText.setFillColor(sf::Color::White);

    while (resultWindow.isOpen()) {
        // 이벤트 처리
        while (const std::optional<sf::Event> resultEvent = resultWindow.pollEvent()) {
            if (resultEvent->is<sf::Event::Closed>()) resultWindow.close();

            if (resultEvent->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(resultWindow);
                if (retryButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    resultWindow.close();
                    game.readyToShowResult = false;
                    return true;    // 게임을 재시작 할 경우
                }
                else if (exitButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    resultWindow.close();
                    game.readyToShowResult = false;
                    return false;   // 메인 메뉴로 돌아갈 경우
                    //window.close();
                }
            }
        }
        // 렌더링
        resultWindow.clear(sf::Color::Black);
        resultWindow.draw(resultDisplay);
        resultWindow.draw(retryButton);
        resultWindow.draw(retryText);
        resultWindow.draw(exitButton);
        resultWindow.draw(exitText);
        resultWindow.display();
    }
}