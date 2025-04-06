//#ifndef UIMANAGER_H
//#define UIMANAGER_H
//
//#include <SFML/Graphics.hpp>
//#include <string>
//#include <vector>
//#include <unordered_map>
//#include <functional>
//#include <memory>
//
//// 전방 선언
//class GameManager;
//class NetworkManager;
//
//// UI 요소의 기본 클래스
//class UIElement {
//public:
//    virtual void draw(sf::RenderWindow& window) = 0;
//    virtual bool isMouseOver(const sf::Vector2f& mousePos) = 0;
//    virtual ~UIElement() {}
//};
//
//// 버튼 클래스
//class Button : public UIElement {
//private:
//    sf::Texture texture;
//    sf::Sprite sprite;
//    sf::Text text;
//    std::function<void()> onClick;
//
//public:
//    Button(const sf::Texture& texture, const sf::Font& font, const std::wstring& buttonText,
//        float x, float y, float scale, unsigned int fontSize, const std::function<void()>& callback);
//
//    void draw(sf::RenderWindow& window) override;
//    bool isMouseOver(const sf::Vector2f& mousePos) override;
//    void executeCallback();
//};
//
//// UI 관리 클래스
//class UIManager {
//private:
//    sf::RenderWindow& window;
//    sf::Font& font;
//
//    // 리소스 관리
//    std::unordered_map<std::string, sf::Texture> textures;
//
//    // UI 요소들
//    sf::Sprite backgroundSprite;
//    sf::RectangleShape whiteBackground;
//    std::vector<std::unique_ptr<UIElement>> uiElements;
//
//public:
//    UIManager(sf::RenderWindow& win, sf::Font& fnt);
//    //~UIManager();
//
//    bool loadTextures();
//    bool loadTexture(const std::string& name, const std::string& path);
//    void setupMainMenu(GameManager& game, NetworkManager& client);
//    void drawAll();
//    void handleEvents(const sf::Event& event);
//};
//
//
//#endif