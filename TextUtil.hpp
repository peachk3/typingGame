#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
class TextUtil
{
public:
    // ��ü �ؽ�Ʈ ������ �� ������ �ɰ�
    std::vector<std::wstring> splitStrtoVector(std::wstring contents);

    // ���� ����(lines)�� ��� wrapTextToPixel�� ó���Ͽ� 2���� ���ͷ� ��ȯ
    // �� ��¿� �ð��� �� ������ ����� �Է� ������ ���
    std::vector<std::vector<std::wstring>> wrapAllLinesToPixelWidth(
        const std::vector<std::wstring>& lines,
        const sf::Font& font,
        unsigned int fontSize,
        float maxWidth);

    std::wstring trim(const std::wstring& str);

private:
    // �ϳ��� �� ���ڿ��� ��Ʈ�� �ִ� �ʺ� �������� �ڵ� �ٹٲ��Ͽ� �� ������ ���� ���Ϳ� ����
    std::vector<std::wstring> wrapTextToPixel(
        const std::wstring& text,
        const sf::Font& font,
        unsigned int fontSize,
        float maxWidth);

};

// ��ü �ؽ�Ʈ -> ���ͷ� �ɰ� -> ���� ��Ʈ�� �ִ� ���̸� �������� �� ������ ���� ���Ϳ� ����(2, 3 �� �� �� �� ����) -> ª�� �� �� �� ������� �� �ϳ��� ���Ϳ� ��ħ (2���� ���� ����)
