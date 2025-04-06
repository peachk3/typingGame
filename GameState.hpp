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

    // 기록용
    float bestWPM = 0.f;            
    float bestAccuracy = 0.f;
    int totalPlayCount = 0;
    float totalPlayTime = 0.f;

    UserInfo() = default;

  
};

struct ButtonUI {
    sf::FloatRect loadBtnBounds;    // 파일 불러오기 버튼
    sf::FloatRect backBtnBounds;    // 이전 화면 버튼

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

    /*sf::Vector2u windowSize = { 1280, 720 };         */                               // 윈도우 사이즈

    std::wstring selectPath;                                        // 사용자가 선택한 파일 경로
    std::vector<std::vector<std::wstring>> sentences;               // 선택한 파일 문장 벡터
    std::vector<std::vector<std::wstring>> userInputs;              // 사용자 입력 벡터
    std::vector<std::vector<std::vector<bool>>> correctMap;         // 맞았는지 틀렸는지 확인

    // 문장 확인을 위한 인덱스
    int curPara = 0;
    int curLine = 0;
    int curChar = 0;

    bool started = false;           // 게임 시작하면 true

    float accuracy = 0.f;           // 게임 시작 후 정확도
    float wpm = 0.f;                // 게임 시작 후 WPM
    float tpm = 0.f;
    float progress = 0.f;
    int totalKeyPress = 0;
    //float startTime = 0.f;          // 게임 시작 시간
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;      // 게임 시작 시간 - 정밀하게 하기 위해서
    float elapsedSeconds = 0.f;       // 경과 시간

    float typingAreaWidth = 1280 * 0.66f;

    bool isPaused = false;          // 혹시 모를 일시정지 기능을 위해 
    bool bHangle = true;            // 한글 파일이면 true, 영어 파일이면 false
    bool readyToShowResult = false; // 게임 끝나면 true

    Scene currentScene = Scene::MAIN_MENU;  // 전역 enum 사용


};

#endif // GAMESTATE_HPP