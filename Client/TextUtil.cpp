#include "TextUtil.hpp"
#include <fstream>
#include <sstream>

std::vector<std::wstring> splitStrtoVector(std::wstring contents)
{
    std::vector<std::wstring> lines;
    std::wstringstream ss(contents);
    std::wstring line;


    while (std::getline(ss, line)) {

        // √ﬂ∞°: ∞≥«‡ ¡¶∞≈
        line.erase(std::remove(line.begin(), line.end(), L'\n'), line.end());
        line.erase(std::remove(line.begin(), line.end(), L'\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), L'\t'), line.end());

        line = trim(line);

        // ∆Øºˆ πÆ¿⁄ ¡¶∞≈
        line = removeUninputtableCharacters(line);

        if (!line.empty()) {
            lines.push_back(line);
        }
           
    }
    return lines;

}

std::vector<std::wstring> wrapTextToPixel(
    const std::wstring& text,
    const sf::Font& font,
    unsigned int fontSize,
    float maxWidth)
{
    std::vector<std::wstring> result;       // √≥∏Æ ∞·∞˙ ¥„¥¬ ∫§≈Õ

    sf::Text measure(font);                 //font, fontSize √‚∑¬ πÆ¿⁄ø≠¿« ∞°∑Œ ≥–¿Ã √¯¡§«œ±‚ ¿ß«ÿ
    measure.setCharacterSize(fontSize);

    std::wstring current;
    for (wchar_t ch : text) {
        std::wstring next = current + ch;
        measure.setString(next);

        float width = measure.getLocalBounds().size.x;

        if (width > maxWidth) {
            result.push_back(current);
            current = ch; // ¥Ÿ¿Ω ¡Ÿ Ω√¿€
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
// ∞¯πÈ¡¶∞≈ «‘ºˆ
std::wstring trim(const std::wstring& str) {
    const std::wstring whitespace = L" \n\r\t";

    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);

    if (start == std::wstring::npos)
        return L""; // ¿¸∫Œ ∞¯πÈ¿Ã∏È ∫Û πÆ¿⁄ø≠ π›»Ø

    return str.substr(start, end - start + 1);
}

bool isKeyboardInputChar(wchar_t ch) {
    // ASCII ¿‘∑¬ ∞°¥… πÆ¿⁄
    if (ch >= 0x20 && ch <= 0x7E)
        return true;

    // øœº∫«¸ «—±€ (∞°~∆R)
    if (ch >= 0xAC00 && ch <= 0xD7A3)
        return true;

    return false;
}

std::wstring removeUninputtableCharacters(const std::wstring& line) {
    std::wstring result;
    for (wchar_t ch : line) {
        if (isKeyboardInputChar(ch)) {
            result += ch;
        }
    }
    return result;
}