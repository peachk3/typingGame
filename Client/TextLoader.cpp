/* 파일 불러오기 버튼 누르면 파일 가져오는 창 만들기 일단 txt파일만
 * openFileDialog(), readFileContents() 함수 추가 (2025.03.28) */

 /* .java, .cpp, ,py, txt 파일 모두 불러올 수 있도록 수정
  * ansiToWstring(), utf8ToWstring() 함수 추가
  * readFileContents(0 -> readTextAutoEncoding() 변경 (2025.03.30) */

  /* 파일 불러오기 후에 새로운 화면에 글자 출력되도록 윈도우 수정
   * 한 줄 입력 받고 오탈자 확인해야 하기 때문에 관리하기 쉽도록 std::vector<std::wstring> 로 변환
   * 한 화면에 넘어가는 긴 문자열일 경우 화면 크기에 맞게 쪼개야 하기 때문에 2차원 백터로 변환 (20205.03.31) */



#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <windows.h>     // Windows API - 파일 선택기 열기 위해 필요
#include <commdlg.h>     // GetOpenFileName 함수
#include <optional>      // 선택적 반환
#include <fstream>       // 파일 읽어오기 위해
#include <sstream>
#include <string>
#pragma comment(lib, "Comdlg32.lib")
   //#include <locale>       // wcout 출력을 위해 잠깐 넣음 나중에 삭제

using namespace std;

// 함수 선언

// 사용자가 파일을 선택할 수 있는 탐색기 창을 띄우고, 선택한 파일 경로를 반환
std::optional<std::wstring> openFileDialog();

// ANSI 인코딩 문자열(std::string)을 UTF-16 문자열(std::wstring)으로 변환
std::wstring ansiToWstring(const std::string& str);

// UTF-8 인코딩 문자열(std::string)을 UTF-16 문자열(std::wstring)으로 변환
std::wstring utf8ToWstring(const std::string& str);

// 파일 경로를 받아 자동으로 인코딩을 판별한 뒤 내용을 std::wstring 형태로 반환
std::wstring readTextAutoEncoding(const std::wstring& filepath);

// 하나의 긴 문자열을 폰트와 최대 너비 기준으로 자동 줄바꿈하여 줄 단위로 나눠 벡터에 저장
std::vector<std::wstring> wrapTextToPixel(
    const std::wstring& text,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth);

// 여러 문장(lines)을 모두 wrapTextToPixel로 처리하여 2차원 벡터로 반환
// → 출력용 시각적 줄 구조와 사용자 입력 구조에 사용
std::vector<std::vector<std::wstring>> wrapAllLinesToPixelWidth(
    const std::vector<std::wstring>& lines,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth);


// 전역 변수 선언

std::vector<std::wstring> lines;
std::vector<std::vector<std::wstring>> userInputs;  // 사용자 입력 창
std::vector<std::vector<std::wstring>> visualLines; // 긴 줄 읽어오기 위해 2차원 벡터
int currentLineIndex = 0;                           // 현재 커서 위치 추적을 위한 변수 생성
int currentSubLineIndex = 0;
int fontSize = 18;                                  // fontsize 설정

// 화면 창 관리를 위한 CLASS
enum class AppState {
    FileSelect, // 파일 불러오기 window
    TypingGame  // 타자 기본연습 window
};

int wmain()
{
    // 파일 탐색 화면 
    AppState currentState = AppState::FileSelect;

    // 폰트 설정
    sf::Font font;
    if (!font.openFromFile("assets/fonts/D2Coding.ttf"))
    {
        wcerr << L"해당 폰트가 존재하지 않습니다. 다시 확인해주세요" << endl;
    }

    // 윈도우 창 생성
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");


    // 파일 불러오기 버튼 생성
    sf::RectangleShape fileLoadButton({ 140.f, 50.f });
    fileLoadButton.setPosition({ 1120.f, 650.f });
    fileLoadButton.setFillColor(sf::Color::Blue);

    // 파일 불러오기 버튼 텍스트 
    sf::Text buttonText(font);
    buttonText.setString(L"파일 불러오기");
    buttonText.setCharacterSize(18);
    buttonText.setFillColor(sf::Color::White);

    // 텍스트 중앙 정렬
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.getCenter()); // SFML 3.0의 getCenter()

    sf::FloatRect buttonBounds = fileLoadButton.getGlobalBounds();
    buttonText.setPosition(buttonBounds.getCenter()); // 버튼 중앙에 텍스트 배치



    sf::Text lineText(font);                    // 한 라인 읽어올 텍스트 객체 생성
    lineText.setFillColor(sf::Color::Black);    // 타자연습 게임에 출력될 글씨 색

    while (window.isOpen())
    {
        // 이벤트 처리
        while (const std::optional<sf::Event> event = window.pollEvent()) {

            // 윈도우 창 닫기 클릭시 이벤트 처리
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // 파일 선택 버튼 클릭 시 이벤트 처리
            if (currentState == AppState::FileSelect) {
                // 파일 선택 관련 이벤트 처리
                if (event->is<sf::Event::MouseButtonPressed>()) {
                    const auto& mouse = event->getIf<sf::Event::MouseButtonPressed>();
                    if (mouse->button == sf::Mouse::Button::Left)
                    {
                        sf::Vector2f mousePos = window.mapPixelToCoords({ mouse->position.x, mouse->position.y });

                        if (fileLoadButton.getGlobalBounds().contains(mousePos))
                        {
                            auto selected = openFileDialog();
                            if (selected) {
                                std::wstring contents = readTextAutoEncoding(selected.value());

                                // 타자 오탈자 확인을 위해 벡터로 변환
                                std::wstringstream ss(contents);
                                std::wstring line;

                                lines.clear();
                                userInputs.clear();

                                // 벡터로 쪼개기
                                while (std::getline(ss, line)) {
                                    if (!line.empty()) {
                                        // 빈 줄 or 공백만 존재하는 줄은 무시
                                        if (!line.empty() && line.find_first_not_of(L" \t\r\n") != std::wstring::npos) {
                                            lines.push_back(line);
                                        }
                                        //userInputs.push_back(L""); // 입력 칸 초기화
                                    }
                                }

                                visualLines = wrapAllLinesToPixelWidth(lines, font, fontSize, window.getSize().x - 100.f);

                                // 입력창 초기화
                                userInputs.clear();

                                for (const auto& wrappedLine : visualLines) {
                                    std::vector<std::wstring> inputLine(wrappedLine.size(), L"");
                                    userInputs.push_back(inputLine);
                                }


                                // 파일 선택이 끝나면 게임 창으로 이동
                                currentState = AppState::TypingGame;
                            }
                        }
                    }
                }
            }
            // 기본 타자 연습 게임 실행 시 이벤트 처리
            else if (currentState == AppState::TypingGame) {
                // 이후 타자 연습 로직이랑 합침
            }
        }

        // 윈도우 흰 화면으로 초기화
        window.clear(sf::Color::White);

        // 파일 선택 화면일 때 랜더링
        if (currentState == AppState::FileSelect) {
            // 파일 선택 화면 그리기
            window.draw(fileLoadButton);
            window.draw(buttonText);
        }
        // 기본 타자 연습 게임일 때 렌더링
        else if (currentState == AppState::TypingGame) {
            // 타자 게임 화면 그리기
            float x = 30.f;
            float y = 50.f; // y를 누적해서 줄 간격 주기

            for (int i = 0; i < visualLines.size(); ++i)
            {
                for (const auto& subLine : visualLines[i])
                {
                    sf::Text lineText(font);
                    lineText.setCharacterSize(fontSize);
                    lineText.setFillColor(sf::Color::Black);
                    lineText.setString(subLine);
                    lineText.setPosition({ x, y });

                    window.draw(lineText);
                    y += lineText.getLocalBounds().size.y + fontSize * 3; // 줄 간격 설정
                }
            }
        }

        window.display();
    }

    return 0;
}


// 함수

std::optional<std::wstring> openFileDialog()
{
    wchar_t filename[MAX_PATH] = L"";  // 파일 경로를 담을 변수

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"텍스트 파일\0*.txt\0모든 파일\0*.*\0파이썬 파일\0*.py\0자바 파일\0*.java\0C++ 파일\0*.cpp\0\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = L"파일 열기";

    if (GetOpenFileNameW(&ofn)) {
        return std::wstring(filename);
    }
    else {
        return std::nullopt;
    }
}



std::wstring ansiToWstring(const std::string& str) {
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(len, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &result[0], len);
    return result;
}


std::wstring utf8ToWstring(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], len);
    return result;
}


std::wstring readTextAutoEncoding(const std::wstring& filepath) {
    std::ifstream file;
    file.open(std::filesystem::path(filepath), std::ios::binary);
    if (!file) return L"파일을 열 수 없습니다.";

    // BOM 검사
    unsigned char bom[3] = { 0 };
    file.read(reinterpret_cast<char*>(bom), 3);
    file.seekg(0); // 다시 처음으로

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // UTF-8 with BOM
    if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
        return utf8ToWstring(content);
    }

    // UTF-8 without BOM 감지용 간단 검사
    bool isUtf8 = true;
    int expected = 0;
    for (unsigned char c : content) {
        if (expected == 0) {
            if ((c >> 5) == 0x6) expected = 1;           // 110xxxxx
            else if ((c >> 4) == 0xE) expected = 2;      // 1110xxxx
            else if ((c >> 3) == 0x1E) expected = 3;     // 11110xxx
            else if ((c >> 7)) { isUtf8 = false; break; } // 10xxxxxx or invalid
        }
        else {
            if ((c >> 6) != 0x2) { isUtf8 = false; break; } // 10xxxxxx
            expected--;
        }
    }

    if (isUtf8) {
        return utf8ToWstring(content);  // BOM 없이도 UTF-8로 처리
    }

    // 그 외는 ANSI (CP949 등)로 처리
    return ansiToWstring(content);
}


std::vector<std::wstring> wrapTextToPixel(
    const std::wstring& text,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth)
{

    std::vector<std::wstring> result;       // 처리 결과 담는 벡터


    sf::Text measure(font);                 //font, fontSize 출력 문자열의 가로 넓이 측정하기 위해
    measure.setCharacterSize(fontSize);

    std::wstring current;
    for (wchar_t ch : text) {
        std::wstring next = current + ch;
        measure.setString(next);

        float width = measure.getLocalBounds().size.x;

        if (width > maxWidth) {
            result.push_back(current);
            current = ch; // 다음 줄 시작
        }
        else {
            current += ch;
        }
    }

    if (!current.empty()) result.push_back(current);
    return result;
}

std::vector<std::vector<std::wstring>> wrapAllLinesToPixelWidth(
    const std::vector<std::wstring>& lines,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth)
{
    std::vector<std::vector<std::wstring>> result;
    for (const auto& line : lines) {
        result.push_back(wrapTextToPixel(line, font, fontSize, maxWidth));
    }
    return result;
}
