#include "FileSelectList.hpp"
#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include "FileLoader.hpp"
#include "TypingGame.hpp"
#include "TextUtil.hpp"
#include <iostream>
#include "DrawUIR.hpp"

// 파일 제목 추출 함수
std::wstring removeExtension(const std::wstring& filePath) {
    // 1. 마지막 경로 구분자 (\\ 또는 /) 위치 찾기
    size_t slashPos = filePath.find_last_of(L"\\/");

    // 2. 파일명만 잘라내기
    std::wstring fileName = (slashPos != std::wstring::npos)
        ? filePath.substr(slashPos + 1)
        : filePath;

    // 3. 마지막 점(.) 위치 찾기
    size_t dotPos = fileName.find_last_of(L'.');

    // 4. 확장자 제거
    if (dotPos != std::wstring::npos) {
        return fileName.substr(0, dotPos);
    }

    return fileName;
}

// typingFilePath: DB 파일 목록, filieOptions: 파일 목록에 있는 옵션 담은 구조체, selectMod - 한글/ 영어/ 코딩
void renderFileList(sf::RenderWindow& window, GameState& game,
    sf::Font& font, int fontSize,
    std::vector<std::wstring> typingFilePath,
    std::vector<FileOption>& filieOptions,
    std::wstring selectMod)						// 한글 / 영여 / 코딩)
{
    // 파일 선택 UI 창
    sf::RectangleShape fileSelectionPanel = makeRectangleR(window, 0.45f, 0.8f, sf::Color::White, sf::Color::Black, 2.f);
    //fileSelectionPanel.setFillColor(sf::Color::White);
    sf::FloatRect typingFileSelectorBounds = fileSelectionPanel.getGlobalBounds();
    sf::Vector2f mpos = getWindowCenterPosition(window, typingFileSelectorBounds);	// 윈도우 가운데 정렬
    fileSelectionPanel.setPosition(mpos);


    // 정렬의 기준이 될 패널 정보 저장
    typingFileSelectorBounds = fileSelectionPanel.getGlobalBounds();


    // 제목
    std::wstring listTitle = selectMod + L" 연습";
    sf::Text title(font, listTitle, 45);
    title.setFillColor(sf::Color::Black);
    sf::FloatRect titleBounds = title.getGlobalBounds();
    sf::Vector2f titlePos = getWindowCenterPosition(window, titleBounds);
    title.setPosition({ titlePos.x, mpos.y + 10.f });

    // 목록을 담을 컨테이너
    sf::RectangleShape listContainer = makeRectangle(typingFileSelectorBounds, 0.7f, 0.7f, sf::Color::Red);
    sf::FloatRect listContainerBounds = listContainer.getGlobalBounds();

    sf::Vector2f listContainerPos = getWindowCenterPosition(window, listContainerBounds);
    listContainer.setPosition(listContainerPos);
    // 컨테이너 위치 저장 - 파일 목록 UI 위치 조정 시 사용
    listContainerBounds = listContainer.getGlobalBounds();

    //// 뷰
    //sf::FloatRect listBounds = listContainer.getGlobalBounds();

    //listView.setSize(listBounds.size);  // ← 보여줄 영역 크기 설정

    //listView.setCenter(
    //	listBounds.position + listBounds.size / 2.f  // ← 중심 좌표는 position + size/2
    //);



    // 사용자 파일 불러오기 버튼
    sf::RectangleShape loadBtn = makeRectangle(typingFileSelectorBounds, 0.2f, 0.05f, sf::Color::Red);


    // 오른쪽 아래 정렬
    sf::Vector2f btnSize = loadBtn.getGlobalBounds().size;
    sf::Vector2f loadBtnPos = AlignPosition(
        btnSize,
        typingFileSelectorBounds,
        AlignX::Right,
        AlignY::Bottom,
        10.f, 10.f
    );
    loadBtn.setPosition(loadBtnPos);
    sf::FloatRect loadBtnBounds = loadBtn.getGlobalBounds();
    game.btn.loadBtnBounds = loadBtnBounds;

    // 버튼 텍스트
    sf::Text btnText(font, L"불러오기", 18);
    btnText.setFillColor(sf::Color::Black);
    AlignTextCenter(btnText, loadBtnBounds);	// 버튼 텍스트에 맞춤

    // 어디서 부터 출력될지 설정
    float spacing = 40.f;
    float startX = listContainerBounds.position.x + 15.f;
    float startY = listContainerBounds.position.y;

    /*float firstLineOffset = 15.f;
    float lineSpacing = spacing;
    float totalHeight = firstLineOffset + (typingFilePath.size() - 1) * lineSpacing;
    float visibleHeight = listContainerBounds.size.y;

    maxScrollOffset = std::max(0.f, totalHeight - visibleHeight);*/

    int fileCount = typingFilePath.size();
    //filieOptions.clear();
    if (filieOptions.empty()) {
        for (int i = 0; i < fileCount; i++)
        {
            // 구조체 초기화
            FileOption option;
            //option.fileName = L"";
            option.filePath = typingFilePath[i];
            std::wstring thisPath = typingFilePath[i];

            // 파일 목록
            auto fileName = std::make_shared<sf::Text>(font, removeExtension(thisPath), fontSize + 4);
            fileName->setFillColor(sf::Color::Black);
            fileName->setPosition({
                startX , startY + (i * spacing) + 15 });
            //fileName->setCharacterSize(18);
            fileName->setFont(font);

            sf::FloatRect textBounds = fileName->getGlobalBounds();


            option.label = fileName;

            option.onClick = [thisPath, &game, &font]()
                {
                    std::wcout << thisPath << L" 경로 파일 선택" << std::endl;
                    game.selectPath = thisPath;
                    std::wstring content = loadText(game.selectPath);
                    std::vector<std::wstring> lines = splitStrtoVector(content);
                    std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                    game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                    resetGameResult(game);

                    //initUserInputsAndCorrectMap(game); // 입력값 초기화

                    game.currentScene = Scene::TYPING_GAME;



                };
            filieOptions.push_back(option);
            //fileNameDrawables.push_back(fileName);    // 여전히 drawables에도 추가
        }
    }



    window.draw(fileSelectionPanel);
    for (auto& option : filieOptions) window.draw(*option.label);
    window.draw(title);
    window.draw(listContainer);


    window.draw(loadBtn);
    window.draw(btnText);


}

// 파일 클릭 이벤트 처리
void handleFileClick(GameState& game, const sf::Event& event, sf::Vector2f& mousePos, std::vector<FileOption>& fileOptions, sf::Font& font)
{


    // 일반 텍스트 입력
    if (event.is<sf::Event::MouseButtonPressed>())
    {

        const auto& mouse = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouse->button == sf::Mouse::Button::Left) {
            //sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (game.btn.loadBtnBounds.contains(mousePos)) {
                std::wcout << L"[DEBUG] 불러오기 버튼 클릭됨" << std::endl;
                // 파일 탐색기 열림
                // 경로 받아오고 
                std::optional<std::wstring> path = openFileDialog();
                if (!path) {
                    std::wcout << L"[ERROR] 파일을 선택하지 않았습니다." << std::endl;
                    return;
                }
                game.selectPath = loadText(path.value());
                std::vector<std::wstring> lines = splitStrtoVector(game.selectPath);
                std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                resetGameResult(game);
                game.currentScene = Scene::TYPING_GAME;
            }
            for (auto& option : fileOptions) {
                if (option.label->getGlobalBounds().contains(mousePos)) {
                    option.onClick();
                }
            }
        }
    }


}


void hoverText(GameState& game, std::vector<FileOption>& fileOptions, sf::Vector2f& mousePos) {
    for (auto& option : fileOptions) {
        if (option.label->getGlobalBounds().contains(mousePos)) {
            option.isHovered = true;
            option.label->setFillColor(sf::Color::Blue);
        }
        else {
            option.isHovered = false;
            option.label->setFillColor(sf::Color::Black);
        }
    }

}