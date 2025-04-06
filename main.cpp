// Ÿ�� ���� ���� 
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include "GameState.hpp"
#include "Scene.hpp"
#include "UIAlign.hpp"
#include "DrawUI.hpp"
#include "FileLoader.hpp"
#include "TextUtil.hpp"
#include "TypingGame.hpp";
#include "ResultScreen.hpp"
#include "DrawUIR.hpp"
#include "FileSelectList.hpp"

#include <locale>
#include <windows.h>


#include <iomanip>  // std::hex, std::setw
#include <codecvt>  // wide �� utf8 ��ȯ (����)



int main() {
    // �α� ��¿�
    //SetConsoleOutputCP(CP_UTF8);               // �ܼ� UTF-8 ����
    std::wcout.imbue(std::locale(""));         // �����ڵ� ������ ����
    


    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Typing Game");

    GameState game;
    // �α��� �����ϸ� ���� ���� �ʱ�ȭ
    //game.user.nickname = L"Player1";
    //game.user.profileImagePath = "images/profile.png";
    //game.user.loadProfileImage();

    // �׽�Ʈ �� �ӽ� ���� ����
    game.user.id = L"test_user_001";
    game.user.nickname = L"Ÿ�ڸ�����";
    game.user.profileImagePath = L"assets/profile_img/default_avatar.png";

    sf::Image img = loadImg(game.user.profileImagePath);
    game.user.profileTexture = sf::Texture(resizeImageKeepAspect(img));
    
    // ��Ʈ ����
    sf::Font font;
    if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
        std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
        return 1;
    }

    int fontSize = game.user.fontSize;
    
    
    //// ������ �̹��� �ε�
    //if (game.user.profileTexture.loadFromFile(game.user.profileImagePath)) {
    //    // �ε� ����
    //}
    //else {
    //    std::wcout << L"������ �̹��� �ε� ����!" << std::endl;
    //}
    game.selectPath = L"assets/hangleFile/�ֱ���.txt";

    // ���� ��θ� �޾ƿ��� 2���� ���ͷ� ��ȯ�ϴ� ����
    //std::wstring content = loadText(game.selectPath);
    //std::vector<std::wstring> lines = splitStrtoVector(content);
    //auto wrapped = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, 1280.f);       // �̰� ����� â�� ũ�⸦ �޾ƿ;� �ϴµ�
    //game.sentences = wrapped;


    //// input, correctMap �ʱ�ȭ
    //initUserInputsAndCorrectMap(game);
    

    // ���� ��� - �ϴ� ���Ƿ� �ʱ�ȭ
    game.user.point = 1234;
    game.user.bestWPM = 87.5f;
    game.user.bestAccuracy = 96.2f;
    game.user.totalPlayCount = 12;
    game.user.totalPlayTime = 983.7f;

    // ���� �÷��̾��� ���� ȭ�� ����
    //game.currentScene = Scene::MAIN_MENU; // �ʱⰪ �����Ƿ� �ʿ� x ���߿� ��������� ���ܵ���
    
    game.currentScene = Scene::FILE_SELECT; // �ʱⰪ �����Ƿ� �ʿ� x ���߿� ��������� ���ܵ���
    //TYPING_GAME

    // �÷��� Ÿ�� ��Ͽ�(?)
    sf::Clock clock;
    

    // ���� ��Ͽ� �׽�Ʈ ����
    std::vector<FileOption> fileOptions;
    std::wstring selectMod = L"�ѱ�";
    std::vector<std::wstring> typingFilePath;

    typingFilePath = {
            L"assets/typing/�ѱ�1.txt",
            L"assets/typing/�ѱ�2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/�ѱ�1.txt",
            L"assets/typing/�ѱ�2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/coding.cpp",
            L"assets/typing/�ѱ�1.txt",

    };




    // ���� while ����
    while (window.isOpen()) 
    {
        //std::wcout << L"[DEBUG] ���� Scene: " << static_cast<int>(game.currentScene) << std::endl;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        while (const std::optional event = window.pollEvent())
        {
            //// �������� �̺�Ʈ
            //if (event->is < sf::Event::Resized>()) {
            //    const auto* resizeEvent = event->getIf<sf::Event::Resized>();
            //    sf::Vector2u newSize(resizeEvent->size.x, resizeEvent->size.y);
            //    game.windowSize = newSize;
            //    std::wcout << L"[DEBUG] ������ ũ�� ���ŵ�: " << newSize.x << L"x" << newSize.y << std::endl;
            //}

            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
            switch (game.currentScene) {
            case Scene::FILE_SELECT: {
                handleFileClick(game, *event, worldPos, fileOptions, font);
                break;
            }         
            case Scene::TYPING_GAME: 
                // Scene�� �Է� ó��
                handleInputGame(game, *event);
                break;
            }

        }

        // HOVER ȿ��
        if (game.currentScene == Scene::FILE_SELECT) {
            hoverText(game, fileOptions, worldPos);
        }
        


        window.clear(sf::Color::White);

        // ���� ���� �׸���
        switch (game.currentScene) {
        case Scene::MAIN_MENU:
            //renderMenu(window, game);
            break;
        case Scene::FILE_SELECT: {
            renderFileList(window, game, font, fontSize, typingFilePath, fileOptions, selectMod);
            break;
        }
        case Scene::TYPING_GAME: {
            if (game.currentScene == Scene::TYPING_GAME && !game.started) {
                game.startTime = std::chrono::high_resolution_clock::now();     // ���� ���� �ð� ����
                game.started = true;                    // �� ���� �ʱ�ȭ �ϵ���
            }
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float>(now - game.startTime).count();
            game.elapsedSeconds = elapsed;

            if (game.readyToShowResult)
                //game.progress = 100.f;
                game.currentScene = Scene::RESULT;

            renderGame(window, game, font, fontSize);         // UI ������
            updateTypingStats(game, elapsed);           // �ǽð� ��� ������Ʈ
            break;
        }
        case Scene::RESULT: {
            //renderResult(window, game);
            std::wcout << L"[DEBUG] ������ RESULT �� �����մϴ�." << std::endl;

            bool bRstart = showResultWindow(game, font, fontSize, game.sentences);
            if (bRstart) {
                resetGameResult(game);  // �� �ʱ�ȭ
                game.currentScene = Scene::TYPING_GAME;
            }
            else {
                game.currentScene = Scene::FILE_SELECT;
            }

            break;
        }
        }

        window.display();
    }

    return 0;
}






