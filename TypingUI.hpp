#include <SFML/Graphics.hpp>
#include "Settings.hpp"
#include <string>

class TypingUI {
private:
    sf::Font& font;            // 폰트 참조 (글자 렌더링에 사용)
    Settings& settings;        // 설정 값 참조 (글꼴 크기 등 반영)

    sf::Text practiceText;     // 연습용 텍스트 (사용자가 따라칠 문장)
    sf::Text userInputText;    // 사용자가 입력한 텍스트
    sf::RectangleShape inputBox; // 입력창 배경 사각형
    std::string currentInput;  // 현재까지 사용자가 입력한 문자열
    std::wstring practiceContent; // 연습 내용 (유니코드 지원 위해 wstring 사용)

public:
    // 생성자 - 폰트와 설정 객체를 참조로 받아 초기화
    TypingUI(sf::Font& font, Settings& settings);

    // 타자 연습 UI 초기화 (텍스트, 위치 등 설정)
    void init();

    // 설정 값이 변경되었을 때 UI 업데이트 (예: 글꼴 크기 반영)
    void update();

    // UI 렌더링 (화면에 그리기)
    void render(sf::RenderWindow& window);

    // 키보드 입력 처리
    void handleTextInput(const sf::Event& event);
};