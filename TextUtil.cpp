#include "TextUtil.hpp"
#include <fstream>
#include <sstream>

std::vector<std::wstring> TextUtil::splitStrtoVector(std::wstring contents)
{
    std::vector<std::wstring> lines;
    std::wstringstream ss(contents);
    std::wstring line;


    while (std::getline(ss, line)) {

        // �߰�: ���� ����
        line.erase(std::remove(line.begin(), line.end(), L'\n'), line.end());
        line.erase(std::remove(line.begin(), line.end(), L'\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), L'\t'), line.end());

        line = trim(line);

        if (!line.empty()) {
            lines.push_back(line);
        }
           
    }
    return lines;

}

std::vector<std::wstring> TextUtil::wrapTextToPixel(
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

std::vector<std::vector<std::wstring>> TextUtil::wrapAllLinesToPixelWidth(
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
// �������� �Լ�
std::wstring TextUtil::trim(const std::wstring& str) {
    const std::wstring whitespace = L" \n\r\t";

    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);

    if (start == std::wstring::npos)
        return L""; // ���� �����̸� �� ���ڿ� ��ȯ

    return str.substr(start, end - start + 1);
}
