#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
class TextUtil
{
public:
    // 전체 텍스트 파일을 줄 단위로 쪼갬
    std::vector<std::wstring> splitStrtoVector(std::wstring contents);

    // 여러 문장(lines)을 모두 wrapTextToPixel로 처리하여 2차원 벡터로 반환
    // → 출력용 시각적 줄 구조와 사용자 입력 구조에 사용
    std::vector<std::vector<std::wstring>> wrapAllLinesToPixelWidth(
        const std::vector<std::wstring>& lines,
        const sf::Font& font,
        unsigned int fontSize,
        float maxWidth);

    std::wstring trim(const std::wstring& str);

private:
    // 하나의 긴 문자열을 폰트와 최대 너비 기준으로 자동 줄바꿈하여 줄 단위로 나눠 벡터에 저장
    std::vector<std::wstring> wrapTextToPixel(
        const std::wstring& text,
        const sf::Font& font,
        unsigned int fontSize,
        float maxWidth);

};

// 전체 텍스트 -> 벡터로 쪼갬 -> 현재 폰트와 최대 넓이를 기준으로 줄 단위로 나눠 벡터에 저장(2, 3 줄 로 뉠 수 있음) -> 짧은 줄 긴 줄 상관없이 다 하나의 벡터에 합침 (2차원 벡터 생성)
