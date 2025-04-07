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
#include "UserProfileUI.hpp"
#include "UserProfileSelectUI.hpp"
#include <locale>
#include <windows.h>


#include <iomanip>  // std::hex, std::setw
#include <codecvt>  // wide �� utf8 ��ȯ (����)


void debugPrintSentences(const std::vector<std::vector<std::wstring>>& sentences) {
    for (size_t i = 0; i < sentences.size(); ++i) {
        std::wcout << L"[���� �׷� " << i << L"]\n";
        for (size_t j = 0; j < sentences[i].size(); ++j) {
            const std::wstring& line = sentences[i][j];
            std::wcout << L"  �� " << j << L": \"" << line << L"\"\n";
            for (size_t k = 0; k < line.size(); ++k) {
                wchar_t ch = line[k];
                std::wstring type;
                if (ch == L' ')
                    type = L"����";
                else if (ch == L'\t')
                    type = L"��";
                else
                    type = L"�Ϲ�";

                std::wcout << L"    [" << k << L"] '" << (ch == L' ' ? L'����' : (ch == L'\t' ? L'tap' : ch))
                    << L"' (" << type << L")\n";
            }
        }
    }
}


int main() {
    // �α� ��¿�
    //SetConsoleOutputCP(CP_UTF8);               // �ܼ� UTF-8 ����
    std::wcout.imbue(std::locale(""));         // �����ڵ� ������ ����
    

    // ������ ���� 
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Typing Game");

    GameState game;


    

    // �׽�Ʈ �� �ӽ� ���� ����
    game.user.id = L"test_user_001";
    game.user.nickname = L"Ÿ�ڸ�����";
    game.user.profileImagePath = L"assets/profile_img/default_avatar.png";

    // ��Ʈ ����
    sf::Font font;
    if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
        std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
        return 1;
    }

    int fontSize = game.user.fontSize;

    // �⺻ ������ �̹��� �ε�
    sf::Image img = loadImg(game.user.profileImagePath);
    game.user.profileTexture = sf::Texture(resizeImageKeepAspect(img));
    sf::Sprite userImage(game.user.profileTexture);

    std::vector<FileOption> fileOptions;
    std::vector<std::wstring> typingFilePath;
    std::wstring selectMod = L"�ѱ�";

    game.selectPath = L"assets/hangleFile/�ֱ���.txt";    

    // ���� ��� - �ϴ� ���Ƿ� �ʱ�ȭ
    game.user.point = 1234;
    game.user.bestWPM = 87.5f;
    game.user.bestAccuracy = 96.2f;
    game.user.totalPlayCount = 12;
    game.user.totalPlayTime = 983.7f;

    
    game.currentScene = Scene::PROFILE; // �ʱⰪ �����Ƿ� �ʿ� x ���߿� ��������� ���ܵ���



    /*typingFilePath = {
            L"C:/Source/IoT-C-2025/Day02/c03.conditional Statements.c",
            L"C:/Source/IoT-C-2025/Day02/c04.loop Statements.c",
            L"C:/Source/IoT-python-2025/day04/py03_module.py",
            L"C:/Source/IoT-python-2025/day02/py02_datastruct.py",
            L"C://Users//Admin//Downloads//TypingTest_EnglishOnly.java",
            L"assets/typing/�ѱ�2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/coding.cpp",
            L"assets/typing/�ѱ�1.txt",

    };*/

    typingFilePath = {
        L"C:\\Users\\Admin\\Documents\\īī���� ���� ����"
    };

    // �߰� �� �͵�
    std::vector<sf::Sprite> sprites;            
    std::vector<ImageOption> imageOptions;      
    std::vector<sf::Text> profileTexts;

    // ������ ���� hover ȿ��
    sf::RectangleShape thumbnailHoverOutline;
    thumbnailHoverOutline.setSize({ 200, 200 }); // ����� ũ�⿡ �°�
    thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
    thumbnailHoverOutline.setOutlineColor(sf::Color::Red);
    thumbnailHoverOutline.setOutlineThickness(4.f);


    // ����� ��¿�
    debugPrintSentences(game.sentences);

    // ���� while ����
    while (window.isOpen()) 
    {

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            switch (game.currentScene) {
            case Scene::PROFILE: {
                if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()){
                    if (mouse->button == sf::Mouse::Button::Left) {
                        if (game.showImageOverlay) {
                            for (int i = 0; i < sprites.size(); ++i) {
                                if (sprites[i].getGlobalBounds().contains(worldPos)) {
                                    imageOptions[i].onClick();
                                    
                                }
                            }

                            // �ҷ����� ��ư Ŭ�� Ȯ��
                            if (game.btn.loadImgBtnBounds.contains(worldPos)) {
                                std::wcout << L"[�ҷ����� ��ư Ŭ����]" << std::endl;
                                auto originalPath = std::filesystem::current_path();	// ���� ���� ��� ����
                                std::wstring selectedImagePath = openImageFileDialog();
                                std::filesystem::current_path(originalPath);  // �ٽ� ���� ��η� �ǵ��� - ���õ� ���� ��η� �ٲ�� �� ����
                                sf::Image image;
                                if (image.loadFromFile(selectedImagePath)) {
                                    // ���� ó��
                                    std::wcout << L"[�̹����� ���������� �ҷ��Խ��ϴ�]" << std::endl;
                                    std::wcout << selectedImagePath << std::endl;
                                    game.user.profileImagePath = selectedImagePath;
                                    updateProfileImage(selectedImagePath, game, userImage);
                                    game.showImageOverlay = false;
                                    game.currentScene = Scene::PROFILE;


                                }
                                else {
                                    std::wcout << L"[�̹����� �ҷ����� ���߽��ϴ�]" << std::endl;
                                    return -1;
                                }

                            }   // HOVER �̺�Ʈ
                            
                        }
                        if (game.btn.selectImgBtnBounds.contains(worldPos)) {
                            game.showImageOverlay = true;
                            std::cout << 1 << std::endl;
                        }
                    }
                }

                else if (const auto* move = event->getIf<sf::Event::MouseMoved>()) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(move->position);
                    game.bHoveringThumbnail = false;

                    for (int i = 0; i < sprites.size(); ++i) {
                        if (sprites[i].getGlobalBounds().contains(mousePos)) {
                            sf::FloatRect bounds = sprites[i].getGlobalBounds();
                            thumbnailHoverOutline.setPosition(bounds.position);
                            thumbnailHoverOutline.setSize(bounds.size);
                            game.bHoveringThumbnail = true;
                            break;
                        }
                    }
                }
                break;

                
            }
            case Scene::FILE_SELECT: {
                handleFileClick(game, *event, worldPos, fileOptions, font);
                break;
            }
            case Scene::TYPING_GAME: {
                // Scene�� �Է� ó��
                handleInputGame(game, *event);
                break;
            }
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
        case Scene::PROFILE: {
            renderProfile(window, game, font, fontSize, userImage, profileTexts);       


            if (game.showImageOverlay) {
                renderSelectImage(window, game, font, imageOptions, sprites, userImage, thumbnailHoverOutline);  // ������ �̹��� ���� ȭ�� ���ļ� �׸�
            }
            break;
        }

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
            updateGame(game);
            break;
        }
        case Scene::RESULT: {
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