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
//// ���� ����
//class GameManager;
//class NetworkManager;
//
//// UI ����� �⺻ Ŭ����
//class UIElement {
//public:
//    virtual void draw(sf::RenderWindow& window) = 0;
//    virtual bool isMouseOver(const sf::Vector2f& mousePos) = 0;
//    virtual ~UIElement() {}
//};
//
//// ��ư Ŭ����
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
//// UI ���� Ŭ����
//class UIManager {
//private:
//    sf::RenderWindow& window;
//    sf::Font& font;
//
//    // ���ҽ� ����
//    std::unordered_map<std::string, sf::Texture> textures;
//
//    // UI ��ҵ�
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