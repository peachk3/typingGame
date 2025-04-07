// 타자 게임 메인 
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
#include <codecvt>  // wide → utf8 변환 (선택)


void debugPrintSentences(const std::vector<std::vector<std::wstring>>& sentences) {
    for (size_t i = 0; i < sentences.size(); ++i) {
        std::wcout << L"[문장 그룹 " << i << L"]\n";
        for (size_t j = 0; j < sentences[i].size(); ++j) {
            const std::wstring& line = sentences[i][j];
            std::wcout << L"  줄 " << j << L": \"" << line << L"\"\n";
            for (size_t k = 0; k < line.size(); ++k) {
                wchar_t ch = line[k];
                std::wstring type;
                if (ch == L' ')
                    type = L"공백";
                else if (ch == L'\t')
                    type = L"탭";
                else
                    type = L"일반";

                std::wcout << L"    [" << k << L"] '" << (ch == L' ' ? L'공백' : (ch == L'\t' ? L'tap' : ch))
                    << L"' (" << type << L")\n";
            }
        }
    }
}


int main() {
    // 로그 출력용
    //SetConsoleOutputCP(CP_UTF8);               // 콘솔 UTF-8 설정
    std::wcout.imbue(std::locale(""));         // 유니코드 로케일 설정
    

    // 윈도우 생성 
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Typing Game");

    GameState game;


    

    // 테스트 용 임시 유저 정보
    game.user.id = L"test_user_001";
    game.user.nickname = L"타자마스터";
    game.user.profileImagePath = L"assets/profile_img/default_avatar.png";

    // 폰트 설정
    sf::Font font;
    if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
        std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
        return 1;
    }

    int fontSize = game.user.fontSize;

    // 기본 프로필 이미지 로드
    sf::Image img = loadImg(game.user.profileImagePath);
    game.user.profileTexture = sf::Texture(resizeImageKeepAspect(img));
    sf::Sprite userImage(game.user.profileTexture);

    std::vector<FileOption> fileOptions;
    std::vector<std::wstring> typingFilePath;
    std::wstring selectMod = L"한글";

    game.selectPath = L"assets/hangleFile/애국가.txt";    

    // 게임 기록 - 일단 임의로 초기화
    game.user.point = 1234;
    game.user.bestWPM = 87.5f;
    game.user.bestAccuracy = 96.2f;
    game.user.totalPlayCount = 12;
    game.user.totalPlayTime = 983.7f;

    
    game.currentScene = Scene::PROFILE; // 초기값 줬으므로 필요 x 나중에 복사용으로 남겨뒀음



    /*typingFilePath = {
            L"C:/Source/IoT-C-2025/Day02/c03.conditional Statements.c",
            L"C:/Source/IoT-C-2025/Day02/c04.loop Statements.c",
            L"C:/Source/IoT-python-2025/day04/py03_module.py",
            L"C:/Source/IoT-python-2025/day02/py02_datastruct.py",
            L"C://Users//Admin//Downloads//TypingTest_EnglishOnly.java",
            L"assets/typing/한글2.txt",
            L"assets/typing/english.txt",
            L"assets/typing/coding.cpp",
            L"assets/typing/coding.cpp",
            L"assets/typing/한글1.txt",

    };*/

    typingFilePath = {
        L"C:\\Users\\Admin\\Documents\\카카오톡 받은 파일"
    };

    // 추가 할 것들
    std::vector<sf::Sprite> sprites;            
    std::vector<ImageOption> imageOptions;      
    std::vector<sf::Text> profileTexts;

    // 프로필 선택 hover 효과
    sf::RectangleShape thumbnailHoverOutline;
    thumbnailHoverOutline.setSize({ 200, 200 }); // 썸네일 크기에 맞게
    thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
    thumbnailHoverOutline.setOutlineColor(sf::Color::Red);
    thumbnailHoverOutline.setOutlineThickness(4.f);


    // 디버그 출력용
    debugPrintSentences(game.sentences);

    // 메인 while 루프
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

                            // 불러오기 버튼 클릭 확인
                            if (game.btn.loadImgBtnBounds.contains(worldPos)) {
                                std::wcout << L"[불러오기 버튼 클릭됨]" << std::endl;
                                auto originalPath = std::filesystem::current_path();	// 현재 폴더 경로 저장
                                std::wstring selectedImagePath = openImageFileDialog();
                                std::filesystem::current_path(originalPath);  // 다시 원래 경로로 되돌림 - 선택된 폴더 경로로 바뀌는 거 방지
                                sf::Image image;
                                if (image.loadFromFile(selectedImagePath)) {
                                    // 성공 처리
                                    std::wcout << L"[이미지를 성공적으로 불러왔습니다]" << std::endl;
                                    std::wcout << selectedImagePath << std::endl;
                                    game.user.profileImagePath = selectedImagePath;
                                    updateProfileImage(selectedImagePath, game, userImage);
                                    game.showImageOverlay = false;
                                    game.currentScene = Scene::PROFILE;


                                }
                                else {
                                    std::wcout << L"[이미지를 불러오지 못했습니다]" << std::endl;
                                    return -1;
                                }

                            }   // HOVER 이벤트
                            
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
                // Scene별 입력 처리
                handleInputGame(game, *event);
                break;
            }
            }
        }

        // HOVER 효과
        if (game.currentScene == Scene::FILE_SELECT) {
            hoverText(game, fileOptions, worldPos);
        }
        

        window.clear(sf::Color::White);

        // 씬에 따라 그리기
        switch (game.currentScene) {
        case Scene::MAIN_MENU:
            //renderMenu(window, game);
            break;
        case Scene::PROFILE: {
            renderProfile(window, game, font, fontSize, userImage, profileTexts);       


            if (game.showImageOverlay) {
                renderSelectImage(window, game, font, imageOptions, sprites, userImage, thumbnailHoverOutline);  // 프로필 이미지 선택 화면 겹쳐서 그림
            }
            break;
        }

        case Scene::FILE_SELECT: {
            renderFileList(window, game, font, fontSize, typingFilePath, fileOptions, selectMod);
            break;
        }
        case Scene::TYPING_GAME: {
            if (game.currentScene == Scene::TYPING_GAME && !game.started) {
                game.startTime = std::chrono::high_resolution_clock::now();     // 게임 시작 시간 저장
                game.started = true;                    // 한 번만 초기화 하도록
            }
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float>(now - game.startTime).count();
            game.elapsedSeconds = elapsed;

            if (game.readyToShowResult)
                //game.progress = 100.f;
                game.currentScene = Scene::RESULT;

            renderGame(window, game, font, fontSize);         // UI 렌더링
            updateTypingStats(game, elapsed);           // 실시간 통계 업데이트
            updateGame(game);
            break;
        }
        case Scene::RESULT: {
            std::wcout << L"[DEBUG] 렌더링 RESULT 로 변경합니다." << std::endl;

            bool bRstart = showResultWindow(game, font, fontSize, game.sentences);
            if (bRstart) {
                resetGameResult(game);  // 값 초기화
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