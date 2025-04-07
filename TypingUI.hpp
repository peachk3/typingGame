#include <SFML/Graphics.hpp>
#include "Settings.hpp"
#include <string>

class TypingUI {
private:
    sf::Font& font;            // ��Ʈ ���� (���� �������� ���)
    Settings& settings;        // ���� �� ���� (�۲� ũ�� �� �ݿ�)

    sf::Text practiceText;     // ������ �ؽ�Ʈ (����ڰ� ����ĥ ����)
    sf::Text userInputText;    // ����ڰ� �Է��� �ؽ�Ʈ
    sf::RectangleShape inputBox; // �Է�â ��� �簢��
    std::string currentInput;  // ������� ����ڰ� �Է��� ���ڿ�
    std::wstring practiceContent; // ���� ���� (�����ڵ� ���� ���� wstring ���)

public:
    // ������ - ��Ʈ�� ���� ��ü�� ������ �޾� �ʱ�ȭ
    TypingUI(sf::Font& font, Settings& settings);

    // Ÿ�� ���� UI �ʱ�ȭ (�ؽ�Ʈ, ��ġ �� ����)
    void init();

    // ���� ���� ����Ǿ��� �� UI ������Ʈ (��: �۲� ũ�� �ݿ�)
    void update();

    // UI ������ (ȭ�鿡 �׸���)
    void render(sf::RenderWindow& window);

    // Ű���� �Է� ó��
    void handleTextInput(const sf::Event& event);
};