/* ���� �ҷ����� ��ư ������ ���� �������� â ����� �ϴ� txt���ϸ�
 * openFileDialog(), readFileContents() �Լ� �߰� (2025.03.28) */

 /* .java, .cpp, ,py, txt ���� ��� �ҷ��� �� �ֵ��� ����
  * ansiToWstring(), utf8ToWstring() �Լ� �߰�
  * readFileContents(0 -> readTextAutoEncoding() ���� (2025.03.30) */

  /* ���� �ҷ����� �Ŀ� ���ο� ȭ�鿡 ���� ��µǵ��� ������ ����
   * �� �� �Է� �ް� ��Ż�� Ȯ���ؾ� �ϱ� ������ �����ϱ� ������ std::vector<std::wstring> �� ��ȯ
   * �� ȭ�鿡 �Ѿ�� �� ���ڿ��� ��� ȭ�� ũ�⿡ �°� �ɰ��� �ϱ� ������ 2���� ���ͷ� ��ȯ (20205.03.31) */



#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <windows.h>     // Windows API - ���� ���ñ� ���� ���� �ʿ�
#include <commdlg.h>     // GetOpenFileName �Լ�
#include <optional>      // ������ ��ȯ
#include <fstream>       // ���� �о���� ����
#include <sstream>
#include <string>
#pragma comment(lib, "Comdlg32.lib")
   //#include <locale>       // wcout ����� ���� ��� ���� ���߿� ����

using namespace std;

// �Լ� ����

// ����ڰ� ������ ������ �� �ִ� Ž���� â�� ����, ������ ���� ��θ� ��ȯ
std::optional<std::wstring> openFileDialog();

// ANSI ���ڵ� ���ڿ�(std::string)�� UTF-16 ���ڿ�(std::wstring)���� ��ȯ
std::wstring ansiToWstring(const std::string& str);

// UTF-8 ���ڵ� ���ڿ�(std::string)�� UTF-16 ���ڿ�(std::wstring)���� ��ȯ
std::wstring utf8ToWstring(const std::string& str);

// ���� ��θ� �޾� �ڵ����� ���ڵ��� �Ǻ��� �� ������ std::wstring ���·� ��ȯ
std::wstring readTextAutoEncoding(const std::wstring& filepath);

// �ϳ��� �� ���ڿ��� ��Ʈ�� �ִ� �ʺ� �������� �ڵ� �ٹٲ��Ͽ� �� ������ ���� ���Ϳ� ����
std::vector<std::wstring> wrapTextToPixel(
    const std::wstring& text,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth);

// ���� ����(lines)�� ��� wrapTextToPixel�� ó���Ͽ� 2���� ���ͷ� ��ȯ
// �� ��¿� �ð��� �� ������ ����� �Է� ������ ���
std::vector<std::vector<std::wstring>> wrapAllLinesToPixelWidth(
    const std::vector<std::wstring>& lines,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth);


// ���� ���� ����

std::vector<std::wstring> lines;
std::vector<std::vector<std::wstring>> userInputs;  // ����� �Է� â
std::vector<std::vector<std::wstring>> visualLines; // �� �� �о���� ���� 2���� ����
int currentLineIndex = 0;                           // ���� Ŀ�� ��ġ ������ ���� ���� ����
int currentSubLineIndex = 0;
int fontSize = 18;                                  // fontsize ����

// ȭ�� â ������ ���� CLASS
enum class AppState {
    FileSelect, // ���� �ҷ����� window
    TypingGame  // Ÿ�� �⺻���� window
};

int wmain()
{
    // ���� Ž�� ȭ�� 
    AppState currentState = AppState::FileSelect;

    // ��Ʈ ����
    sf::Font font;
    if (!font.openFromFile("assets/fonts/D2Coding.ttf"))
    {
        wcerr << L"�ش� ��Ʈ�� �������� �ʽ��ϴ�. �ٽ� Ȯ�����ּ���" << endl;
    }

    // ������ â ����
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");


    // ���� �ҷ����� ��ư ����
    sf::RectangleShape fileLoadButton({ 140.f, 50.f });
    fileLoadButton.setPosition({ 1120.f, 650.f });
    fileLoadButton.setFillColor(sf::Color::Blue);

    // ���� �ҷ����� ��ư �ؽ�Ʈ 
    sf::Text buttonText(font);
    buttonText.setString(L"���� �ҷ�����");
    buttonText.setCharacterSize(18);
    buttonText.setFillColor(sf::Color::White);

    // �ؽ�Ʈ �߾� ����
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.getCenter()); // SFML 3.0�� getCenter()

    sf::FloatRect buttonBounds = fileLoadButton.getGlobalBounds();
    buttonText.setPosition(buttonBounds.getCenter()); // ��ư �߾ӿ� �ؽ�Ʈ ��ġ



    sf::Text lineText(font);                    // �� ���� �о�� �ؽ�Ʈ ��ü ����
    lineText.setFillColor(sf::Color::Black);    // Ÿ�ڿ��� ���ӿ� ��µ� �۾� ��

    while (window.isOpen())
    {
        // �̺�Ʈ ó��
        while (const std::optional<sf::Event> event = window.pollEvent()) {

            // ������ â �ݱ� Ŭ���� �̺�Ʈ ó��
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // ���� ���� ��ư Ŭ�� �� �̺�Ʈ ó��
            if (currentState == AppState::FileSelect) {
                // ���� ���� ���� �̺�Ʈ ó��
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

                                // Ÿ�� ��Ż�� Ȯ���� ���� ���ͷ� ��ȯ
                                std::wstringstream ss(contents);
                                std::wstring line;

                                lines.clear();
                                userInputs.clear();

                                // ���ͷ� �ɰ���
                                while (std::getline(ss, line)) {
                                    if (!line.empty()) {
                                        // �� �� or ���鸸 �����ϴ� ���� ����
                                        if (!line.empty() && line.find_first_not_of(L" \t\r\n") != std::wstring::npos) {
                                            lines.push_back(line);
                                        }
                                        //userInputs.push_back(L""); // �Է� ĭ �ʱ�ȭ
                                    }
                                }

                                visualLines = wrapAllLinesToPixelWidth(lines, font, fontSize, window.getSize().x - 100.f);

                                // �Է�â �ʱ�ȭ
                                userInputs.clear();

                                for (const auto& wrappedLine : visualLines) {
                                    std::vector<std::wstring> inputLine(wrappedLine.size(), L"");
                                    userInputs.push_back(inputLine);
                                }


                                // ���� ������ ������ ���� â���� �̵�
                                currentState = AppState::TypingGame;
                            }
                        }
                    }
                }
            }
            // �⺻ Ÿ�� ���� ���� ���� �� �̺�Ʈ ó��
            else if (currentState == AppState::TypingGame) {
                // ���� Ÿ�� ���� �����̶� ��ħ
            }
        }

        // ������ �� ȭ������ �ʱ�ȭ
        window.clear(sf::Color::White);

        // ���� ���� ȭ���� �� ������
        if (currentState == AppState::FileSelect) {
            // ���� ���� ȭ�� �׸���
            window.draw(fileLoadButton);
            window.draw(buttonText);
        }
        // �⺻ Ÿ�� ���� ������ �� ������
        else if (currentState == AppState::TypingGame) {
            // Ÿ�� ���� ȭ�� �׸���
            float x = 30.f;
            float y = 50.f; // y�� �����ؼ� �� ���� �ֱ�

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
                    y += lineText.getLocalBounds().size.y + fontSize * 3; // �� ���� ����
                }
            }
        }

        window.display();
    }

    return 0;
}


// �Լ�

std::optional<std::wstring> openFileDialog()
{
    wchar_t filename[MAX_PATH] = L"";  // ���� ��θ� ���� ����

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"�ؽ�Ʈ ����\0*.txt\0��� ����\0*.*\0���̽� ����\0*.py\0�ڹ� ����\0*.java\0C++ ����\0*.cpp\0\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = L"���� ����";

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
    if (!file) return L"������ �� �� �����ϴ�.";

    // BOM �˻�
    unsigned char bom[3] = { 0 };
    file.read(reinterpret_cast<char*>(bom), 3);
    file.seekg(0); // �ٽ� ó������

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // UTF-8 with BOM
    if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
        return utf8ToWstring(content);
    }

    // UTF-8 without BOM ������ ���� �˻�
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
        return utf8ToWstring(content);  // BOM ���̵� UTF-8�� ó��
    }

    // �� �ܴ� ANSI (CP949 ��)�� ó��
    return ansiToWstring(content);
}


std::vector<std::wstring> wrapTextToPixel(
    const std::wstring& text,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth)
{

    std::vector<std::wstring> result;       // ó�� ��� ��� ����


    sf::Text measure(font);                 //font, fontSize ��� ���ڿ��� ���� ���� �����ϱ� ����
    measure.setCharacterSize(fontSize);

    std::wstring current;
    for (wchar_t ch : text) {
        std::wstring next = current + ch;
        measure.setString(next);

        float width = measure.getLocalBounds().size.x;

        if (width > maxWidth) {
            result.push_back(current);
            current = ch; // ���� �� ����
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
