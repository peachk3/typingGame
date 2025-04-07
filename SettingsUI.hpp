#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "Settings.hpp"

class SettingsUI {
private:


    // ��� ����
    const float settingWindowWidth = 500.f;      // ���� â �ʺ�
    const float sliderStartX = 120.f;            // �����̴� ���� ��ġ (X ��ǥ)
    const float sliderWidth = 200.f;             // �����̴� ����

    // ���ҽ� ����
    sf::Texture confirmButtonTexture;            // Ȯ�� ��ư �ؽ�ó
    sf::Texture cancelButtonTexture;             // ��� ��ư �ؽ�ó
    sf::Sprite confirmButtonSprite;              // Ȯ�� ��ư ��������Ʈ 
    sf::Sprite cancelButtonSprite;               // ��� ��ư ��������Ʈ 
    sf::Font& font;                               // ��Ʈ�� ������ �޾ƿ�
   
    bool resourcesLoaded = false;               // ���ҽ� �ε� ����

    // ���� �÷��� (�����̴� �巡�� ������ ����)
    bool isDraggingBgmSlider = false;            // ������� �����̴� �巡�� �� ����
    bool isDraggingSfxSlider = false;            // ȿ���� �����̴� �巡�� �� ����
    bool isDraggingFontSizeSlider = false;       // �۲� ũ�� �����̴� �巡�� �� ����

    // ���� â ����
    std::unique_ptr<sf::RenderWindow> settingWindow;  // ���� â
    bool isWindowOpen = false;                         // â ���� ����

    // ���� ��ü �� ��� ���� ����
    Settings& settings;      // Settings ��ü ����
    sf::Music& bgMusic;      // ��� ���� ��ü ����

public:
    // ������: ����, ��Ʈ, ���� ���� �޾Ƽ� �ʱ�ȭ
    SettingsUI(Settings& settings, sf::Font& font, sf::Music& bgMusic);
    ~SettingsUI();

    // ��ư �� ���ҽ� �ʱ�ȭ
    bool loadResources();

    // ���� â ����
    void openWindow();

    // ���� â�� �����ִ��� Ȯ��
    bool isOpen() const;

    // ���� â �̺�Ʈ ó��
    void processEvents();

    // ���� â �׸���
    void render();

    // ���� â �ݱ�
    void close();
};