#include "FileSelectList.hpp"
#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include "FileLoader.hpp"
#include "TypingGame.hpp"
#include "TextUtil.hpp"
#include <iostream>
#include "DrawUIR.hpp"

// ���� ���� ���� �Լ�
std::wstring removeExtension(const std::wstring& filePath) {
    // 1. ������ ��� ������ (\\ �Ǵ� /) ��ġ ã��
    size_t slashPos = filePath.find_last_of(L"\\/");

    // 2. ���ϸ� �߶󳻱�
    std::wstring fileName = (slashPos != std::wstring::npos)
        ? filePath.substr(slashPos + 1)
        : filePath;

    // 3. ������ ��(.) ��ġ ã��
    size_t dotPos = fileName.find_last_of(L'.');

    // 4. Ȯ���� ����
    if (dotPos != std::wstring::npos) {
        return fileName.substr(0, dotPos);
    }

    return fileName;
}

// typingFilePath: DB ���� ���, filieOptions: ���� ��Ͽ� �ִ� �ɼ� ���� ����ü, selectMod - �ѱ�/ ����/ �ڵ�
void renderFileList(sf::RenderWindow& window, GameState& game,
    sf::Font& font, int fontSize,
    std::vector<std::wstring> typingFilePath,
    std::vector<FileOption>& filieOptions,
    std::wstring selectMod)						// �ѱ� / ���� / �ڵ�)
{
    // ���� ���� UI â
    sf::RectangleShape fileSelectionPanel = makeRectangleR(window, 0.45f, 0.8f, sf::Color::White, sf::Color::Black, 2.f);
    //fileSelectionPanel.setFillColor(sf::Color::White);
    sf::FloatRect typingFileSelectorBounds = fileSelectionPanel.getGlobalBounds();
    sf::Vector2f mpos = getWindowCenterPosition(window, typingFileSelectorBounds);	// ������ ��� ����
    fileSelectionPanel.setPosition(mpos);


    // ������ ������ �� �г� ���� ����
    typingFileSelectorBounds = fileSelectionPanel.getGlobalBounds();


    // ����
    std::wstring listTitle = selectMod + L" ����";
    sf::Text title(font, listTitle, 45);
    title.setFillColor(sf::Color::Black);
    sf::FloatRect titleBounds = title.getGlobalBounds();
    sf::Vector2f titlePos = getWindowCenterPosition(window, titleBounds);
    title.setPosition({ titlePos.x, mpos.y + 10.f });

    // ����� ���� �����̳�
    sf::RectangleShape listContainer = makeRectangle(typingFileSelectorBounds, 0.7f, 0.7f, sf::Color::Red);
    sf::FloatRect listContainerBounds = listContainer.getGlobalBounds();

    sf::Vector2f listContainerPos = getWindowCenterPosition(window, listContainerBounds);
    listContainer.setPosition(listContainerPos);
    // �����̳� ��ġ ���� - ���� ��� UI ��ġ ���� �� ���
    listContainerBounds = listContainer.getGlobalBounds();

    //// ��
    //sf::FloatRect listBounds = listContainer.getGlobalBounds();

    //listView.setSize(listBounds.size);  // �� ������ ���� ũ�� ����

    //listView.setCenter(
    //	listBounds.position + listBounds.size / 2.f  // �� �߽� ��ǥ�� position + size/2
    //);



    // ����� ���� �ҷ����� ��ư
    sf::RectangleShape loadBtn = makeRectangle(typingFileSelectorBounds, 0.2f, 0.05f, sf::Color::Red);


    // ������ �Ʒ� ����
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

    // ��ư �ؽ�Ʈ
    sf::Text btnText(font, L"�ҷ�����", 18);
    btnText.setFillColor(sf::Color::Black);
    AlignTextCenter(btnText, loadBtnBounds);	// ��ư �ؽ�Ʈ�� ����

    // ��� ���� ��µ��� ����
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
            // ����ü �ʱ�ȭ
            FileOption option;
            //option.fileName = L"";
            option.filePath = typingFilePath[i];
            std::wstring thisPath = typingFilePath[i];

            // ���� ���
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
                    std::wcout << thisPath << L" ��� ���� ����" << std::endl;
                    game.selectPath = thisPath;
                    std::wstring content = loadText(game.selectPath);
                    std::vector<std::wstring> lines = splitStrtoVector(content);
                    std::cout << "[DEBUG] typingAreaWith: " << game.typingAreaWidth << '\n';
                    game.sentences = wrapAllLinesToPixelWidth(lines, font, game.user.fontSize, game.typingAreaWidth);
                    resetGameResult(game);

                    //initUserInputsAndCorrectMap(game); // �Է°� �ʱ�ȭ

                    game.currentScene = Scene::TYPING_GAME;



                };
            filieOptions.push_back(option);
            //fileNameDrawables.push_back(fileName);    // ������ drawables���� �߰�
        }
    }



    window.draw(fileSelectionPanel);
    for (auto& option : filieOptions) window.draw(*option.label);
    window.draw(title);
    window.draw(listContainer);


    window.draw(loadBtn);
    window.draw(btnText);


}

// ���� Ŭ�� �̺�Ʈ ó��
void handleFileClick(GameState& game, const sf::Event& event, sf::Vector2f& mousePos, std::vector<FileOption>& fileOptions, sf::Font& font)
{


    // �Ϲ� �ؽ�Ʈ �Է�
    if (event.is<sf::Event::MouseButtonPressed>())
    {

        const auto& mouse = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouse->button == sf::Mouse::Button::Left) {
            //sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (game.btn.loadBtnBounds.contains(mousePos)) {
                std::wcout << L"[DEBUG] �ҷ����� ��ư Ŭ����" << std::endl;
                // ���� Ž���� ����
                // ��� �޾ƿ��� 
                std::optional<std::wstring> path = openFileDialog();
                if (!path) {
                    std::wcout << L"[ERROR] ������ �������� �ʾҽ��ϴ�." << std::endl;
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