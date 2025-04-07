#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "Settings.hpp"

class SettingsUI {
private:


    // 상수 값들
    const float settingWindowWidth = 500.f;      // 설정 창 너비
    const float sliderStartX = 120.f;            // 슬라이더 시작 위치 (X 좌표)
    const float sliderWidth = 200.f;             // 슬라이더 길이

    // 리소스 관련
    sf::Texture confirmButtonTexture;            // 확인 버튼 텍스처
    sf::Texture cancelButtonTexture;             // 취소 버튼 텍스처
    sf::Sprite confirmButtonSprite;              // 확인 버튼 스프라이트 
    sf::Sprite cancelButtonSprite;               // 취소 버튼 스프라이트 
    sf::Font& font;                               // 폰트는 참조로 받아옴
   
    bool resourcesLoaded = false;               // 리소스 로드 여부

    // 상태 플래그 (슬라이더 드래그 중인지 여부)
    bool isDraggingBgmSlider = false;            // 배경음악 슬라이더 드래그 중 여부
    bool isDraggingSfxSlider = false;            // 효과음 슬라이더 드래그 중 여부
    bool isDraggingFontSizeSlider = false;       // 글꼴 크기 슬라이더 드래그 중 여부

    // 설정 창 관련
    std::unique_ptr<sf::RenderWindow> settingWindow;  // 설정 창
    bool isWindowOpen = false;                         // 창 열림 여부

    // 설정 객체 및 배경 음악 참조
    Settings& settings;      // Settings 객체 참조
    sf::Music& bgMusic;      // 배경 음악 객체 참조

public:
    // 생성자: 설정, 폰트, 음악 참조 받아서 초기화
    SettingsUI(Settings& settings, sf::Font& font, sf::Music& bgMusic);
    ~SettingsUI();

    // 버튼 등 리소스 초기화
    bool loadResources();

    // 설정 창 열기
    void openWindow();

    // 설정 창이 열려있는지 확인
    bool isOpen() const;

    // 설정 창 이벤트 처리
    void processEvents();

    // 설정 창 그리기
    void render();

    // 설정 창 닫기
    void close();
};