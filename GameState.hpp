// GameState.hpp
#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "Scene.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct UserInfo {
    std::wstring id;
    std::wstring nickname;
    std::wstring profileImagePath = L"assets/profile_img/default_avatar.png";
    sf::Texture profileTexture;
    int fontSize = 18;
    int point = 0;
    //sf::Sprite profileSprite;

    // ��Ͽ�
    float bestWPM = 0.f;            
    float bestAccuracy = 0.f;
    int totalPlayCount = 0;
    float totalPlayTime = 0.f;

    UserInfo() = default;

  
};

struct ButtonUI {
    sf::FloatRect loadBtnBounds;    // ���� �ҷ����� ��ư
    sf::FloatRect backBtnBounds;    // ���� ȭ�� ��ư

};


struct GameResult {
    float avgWpm = 0.f;
    float avgTpm = 0.f;
    float gamePlayTime = 0.f;
};

struct GameState {
    UserInfo user;
    GameResult result;
    ButtonUI btn;

    /*sf::Vector2u windowSize = { 1280, 720 };         */                               // ������ ������

    std::wstring selectPath;                                        // ����ڰ� ������ ���� ���
    std::vector<std::vector<std::wstring>> sentences;               // ������ ���� ���� ����
    std::vector<std::vector<std::wstring>> userInputs;              // ����� �Է� ����
    std::vector<std::vector<std::vector<bool>>> correctMap;         // �¾Ҵ��� Ʋ�ȴ��� Ȯ��

    // ���� Ȯ���� ���� �ε���
    int curPara = 0;
    int curLine = 0;
    int curChar = 0;

    bool started = false;           // ���� �����ϸ� true

    float accuracy = 0.f;           // ���� ���� �� ��Ȯ��
    float wpm = 0.f;                // ���� ���� �� WPM
    float tpm = 0.f;
    float progress = 0.f;
    int totalKeyPress = 0;
    //float startTime = 0.f;          // ���� ���� �ð�
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;      // ���� ���� �ð� - �����ϰ� �ϱ� ���ؼ�
    float elapsedSeconds = 0.f;       // ��� �ð�

    float typingAreaWidth = 1280 * 0.66f;

    bool isPaused = false;          // Ȥ�� �� �Ͻ����� ����� ���� 
    bool bHangle = true;            // �ѱ� �����̸� true, ���� �����̸� false
    bool readyToShowResult = false; // ���� ������ true

    Scene currentScene = Scene::MAIN_MENU;  // ���� enum ���


};

#endif // GAMESTATE_HPP