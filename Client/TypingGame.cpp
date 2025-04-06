#include "TypingGame.hpp"
#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

// --- ������ ---
void renderGame(sf::RenderWindow& window, GameState& game, const sf::Font& font, int fontSize)
{
    // UI ��ġ�� ���� ������ ����
    // ������ ũ���� �簢�� ����
    sf::Vector2f winSizeF(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
    sf::RectangleShape winRact(winSizeF);
    sf::FloatRect winRactBounds = winRact.getGlobalBounds();
    sf::RectangleShape typingArea = makeRectangle(window, 0.66f, 0.9f);
    sf::RectangleShape gState = makeRectangle(window, 0.25f, 0.52f);
    //sf::RectangleShape pfImg = makeRectangle(window, 0.15f, 0.25f);
    sf::RectangleShape pfImg({ 200,200 });


    gState.setOutlineColor(sf::Color::Red);
    gState.setOutlineThickness(2.f);
    pfImg.setOutlineColor(sf::Color::Green);
    pfImg.setOutlineThickness(2.f);
    typingArea.setOutlineColor(sf::Color::Yellow);
    typingArea.setOutlineThickness(2.f);

    // ����
    sf::Vector2f pfImgSize = pfImg.getSize();
    sf::Vector2f gStateSize = gState.getSize();
    sf::Vector2f typingAreaSize = typingArea.getSize();

    sf::FloatRect typingAreaBounds = typingArea.getGlobalBounds();
    game.typingAreaWidth = typingAreaBounds.size.x;

    // ������ �Ʒ� ����
    //sf::Vector2f btnSize = loadBtn.getGlobalBounds().size;
    sf::Vector2f typingAreaPos = AlignPosition(
        typingAreaSize,
        winRactBounds,
        AlignX::Right,
        AlignY::Bottom,
        60.f, 30.f
    );

    // ���� �Ʒ� ����
    sf::Vector2f gStatePos = AlignPosition(
        gStateSize,
        winRactBounds,
        AlignX::Left,
        AlignY::Bottom,
        30.f, 30.f
    );
    typingArea.setPosition(typingAreaPos);
    gState.setPosition(gStatePos);

    // gstate ��� ����, typingAreaPos �� y ��ǥ ����
    sf::FloatRect s = gState.getGlobalBounds();
    sf::Vector2f pfImgPos = getCenterXPosition(pfImgSize, gState.getGlobalBounds(), typingAreaPos.y);
    pfImg.setPosition(pfImgPos);


    // ���� ��� ��ġ ����
    sf::Vector2f sentenceStartPos = typingAreaPos;

    // �Է� ���� ���� �ʱ�ȭ
    std::vector<std::vector<std::wstring>> displaySentences = game.sentences;
    std::vector<std::vector<std::wstring>> inputSentences = game.userInputs;


    // UI ��� �ֱ� 
    // ���� ���
    drawOriginalText(window, game, font, fontSize, sentenceStartPos, displaySentences);
    sentenceStartPos = { sentenceStartPos.x, sentenceStartPos.y + fontSize };
    drawUserInputText(window, game, font, fontSize, sentenceStartPos, inputSentences, displaySentences);



    // �̹���
    sf::Sprite imgSprite(game.user.profileTexture);
    imgSprite.setPosition(pfImgPos);



    // ���� ����
    sf::Text playTimeL(font, L"����ð�", 20);
    playTimeL.setFillColor(sf::Color::Black);

    sf::Text tpmL(font, L"Ÿ��(Ÿ/��)", 20);
    tpmL.setFillColor(sf::Color::Black);

    sf::Text accuracyL(font, L"��Ȯ��(%)", 20);
    accuracyL.setFillColor(sf::Color::Black);

    sf::Text progressL(font, L"���൵(%)", 20);
    progressL.setFillColor(sf::Color::Black);

    sf::Text playTimeV(font, L"", 20);
    playTimeV.setFillColor(sf::Color::Black);

    sf::Text tpmV(font, L"", 20);
    tpmV.setFillColor(sf::Color::Black);
    //tpmV.setString(game.tpm + L"Ÿ");

    sf::Text accuracyV(font, L"", 20);
    accuracyV.setFillColor(sf::Color::Black);

    sf::Text progressV(font, L"", 20);
    progressV.setFillColor(sf::Color::Black);


    // ����ð�
    int totalSeconds = static_cast<int>(game.elapsedSeconds);

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    std::wstringstream ss;
    ss << minutes << L":" << std::setw(2) << std::setfill(L'0') << seconds;

    playTimeV.setString(ss.str());  // ��: 01:23
    
    // Ÿ�� (����)
    tpmV.setString(std::to_wstring(static_cast<int>(game.tpm)) + L"Ÿ");

    // WPM - ������ �� �̰ɷ� �ٲܱ�? 
    //{
    //    std::wstringstream ss;
    //    ss << std::fixed << std::setprecision(1) << game.wpm;
    //    wpmV.setString(ss.str());
    //}

// ��Ȯ��
    {
        std::wstringstream ss;
        ss << std::fixed << std::setprecision(1) << game.accuracy;
        accuracyV.setString(ss.str() + L" %");
    }

    // ���൵
    {
        std::wstringstream ss;
        ss << std::fixed << std::setprecision(1) << game.progress;
        progressV.setString(ss.str() + L" %");
    }

    // �ؽ�Ʈ ��ġ
    std::vector<sf::Text*> labels = { &playTimeL, &tpmL, &accuracyL, &progressL };
    std::vector<sf::Text*> values = { &playTimeV, &tpmV, &accuracyV, &progressV };

    float lineHeight = 100.f;        // �� ����

    // �� ��ġ ������
    sf::Vector2 standardLPos = playTimeL.getGlobalBounds().size;
    sf::Vector2f textLPos = AlignPosition(standardLPos, gState.getGlobalBounds(), AlignX::Left, AlignY::Top, 20, 20); // ���� �� 
    playTimeL.setPosition(textLPos);


    // �� ��ġ ������
    sf::Vector2 standardVPos = playTimeV.getGlobalBounds().size;
    sf::Vector2f textVPos = AlignPosition(standardLPos, gState.getGlobalBounds(), AlignX::Right, AlignY::Top, 15, 20);


    for (size_t i = 0; i < labels.size(); ++i) {
        labels[i]->setPosition({ textLPos.x, textLPos.y + i * lineHeight });
        values[i]->setPosition({ textVPos.x, textVPos.y + i * lineHeight });
    }




    // �����̳� draw
    //window.draw(pfImg);
    window.draw(gState);
    window.draw(typingArea);

    // �̹��� draw
    window.draw(imgSprite);

    // �ؽ�Ʈ �� draw
    for (std::size_t i = 0; i < labels.size(); ++i) {
        window.draw(*labels[i]);
        window.draw(*values[i]);
    };


}

void drawUserInputText(sf::RenderWindow& window, const GameState& game,
    const sf::Font& font, int fontSize, sf::Vector2f standardPos,
    std::vector<std::vector<std::wstring>>& userInputs,
    std::vector<std::vector<std::wstring>>& sentences)
{
    float startX = standardPos.x + 20.f;
    float startY = standardPos.y;

    std::vector<std::vector<std::vector<bool>>> isCorrect = game.correctMap;

    float letterSpacing = 0.f;
    if (game.bHangle)
    {
        letterSpacing = fontSize * 0.90f; // �ѱ�
    }
    else {
        letterSpacing = fontSize * 0.50f;
    }


    for (int i = 0; i < userInputs.size(); ++i)
    {
        for (size_t j = 0; j < userInputs[i].size(); ++j)
        {
            std::wstring& inputLine = userInputs[i][j];
            std::wstring& targetLine = sentences[i][j];  // target ����
            for (size_t k = 0; k < inputLine.size(); ++k)
            {
                sf::Text inputText(font, inputLine.substr(k, 1), fontSize);

                inputText.setFillColor(isCorrect[i][j][k] ? sf::Color::Green : sf::Color::Red);
                inputText.setPosition({ startX + k * letterSpacing, startY + (fontSize * (i + 1)) + 10.f });


                window.draw(inputText);
            }

            // Ŀ�� ��ġ ǥ��
            if (i == game.curPara && j == game.curLine) {
                sf::Text cursorText(font, L"_", fontSize); // �Ǵ� L"_"
                cursorText.setFillColor(sf::Color::Blue);
                cursorText.setPosition({ startX + game.curChar * letterSpacing, startY + (fontSize * (i + 1)) + 10.f });

                window.draw(cursorText);
            }



            startY += fontSize * 4; // �� ���� ����
        }
    }
}

void drawOriginalText(sf::RenderWindow& window,
    const GameState& game, const sf::Font& font, int fontSize, sf::Vector2f standardPos,
    std::vector<std::vector<std::wstring>>& displaySentences)
{
    float startX = standardPos.x + 20.f;        // ��� ���� �е� 20
    float startY = standardPos.y;               // ��� ���� �е� 20

    float x = 30.f;
    float y = 50.f; // y�� �����ؼ� �� ���� �ֱ�
    float letterSpacing = fontSize * 0.90f;  // �ѱ�, ������ �� ���� ���� ����
    //std::vector<std::vector<std::wstring>> displaySentences = game.sentences;

    if (game.bHangle)
    {
        letterSpacing = fontSize * 0.90f; // �ѱ�
    }
    else {
        letterSpacing = fontSize * 0.50f;
    }

    // �� ���ھ� �Է�
    for (int i = 0; i < displaySentences.size(); ++i)
    {
        for (size_t j = 0; j < displaySentences[i].size(); ++j)
        {
            std::wstring& Line = displaySentences[i][j];

            for (size_t k = 0; k < Line.size(); ++k)
            {
                sf::Text oneChar(font, Line.substr(k, 1), fontSize);

                oneChar.setFillColor(sf::Color::Black);

                oneChar.setPosition({ startX + k * letterSpacing, startY + (fontSize * i) + 10.f });


                window.draw(oneChar);
            }

            startY += fontSize * 4; // �� ���� ����

        }
    }
}



// --- ���� ---
void updateTypingStats(GameState& game, float elapsedSeconds)
{
    int correctCount = 0;
    int totalInputChars = 0;
    int totalTargetChars = 0;

    for (size_t p = 0; p < game.sentences.size(); ++p) {
        for (size_t l = 0; l < game.sentences[p].size(); ++l) {
            const std::wstring& inputLine = game.userInputs[p][l];
            const std::wstring& targetLine = game.sentences[p][l];
            const std::vector<bool>& correctLine = game.correctMap[p][l];

            totalInputChars += inputLine.size();
            totalTargetChars += targetLine.size();

            for (bool b : correctLine)
                if (b) correctCount++;
        }
    }

    

    // ��Ȯ�� ���
    game.accuracy = (totalInputChars > 0)
        ? static_cast<float>(correctCount) / totalInputChars * 100.f
        : 100.f;
    // Tpm ���
    float elapsedMinutes = elapsedSeconds / 60.f;
    game.tpm = (elapsedMinutes > 0)
        ? static_cast<float>(totalInputChars) / elapsedMinutes
        : 0.f;
    // Wpm ���
    game.wpm = (elapsedMinutes > 0)
        ? (static_cast<float>(totalInputChars) / 5.f) / elapsedMinutes
        : 0.f;
    // ���൵ (%)
    game.progress = (totalTargetChars > 0)
        ? static_cast<float>(totalInputChars) / totalTargetChars * 100.f
        : 0.f;
}

void moveToNextLineOrParagraph(GameState& game) {
    int p = game.curPara;
    int l = game.curLine;

    if (l + 1 < game.userInputs[p].size()) {
        game.curLine++;
        game.curChar = 0;
    }
    else if (p + 1 < game.userInputs.size()) {
        game.curPara++;
        game.curLine = 0;
        game.curChar = 0;
    }
    else {
        std::wcout << L"[DEBUG] ������ ���� �Է� �Ϸ�! ��� ȭ������ �Ѿ �غ�." << std::endl;
        updateTypingStats(game, game.elapsedSeconds);
        game.result.gamePlayTime = game.elapsedSeconds;
        game.result.avgTpm = game.tpm;
        game.result.avgWpm = game.wpm;

        game.readyToShowResult = true;
        
    }
}

void initUserInputsAndCorrectMap(GameState& game) {
    game.userInputs.clear();
    game.correctMap.clear();

    for (size_t i = 0; i < game.sentences.size(); ++i) {
        std::vector<std::wstring> inputLine(game.sentences[i].size(), L"");
        std::vector<std::vector<bool>> correctness;

        for (size_t j = 0; j < game.sentences[i].size(); ++j) {
            correctness.push_back(std::vector<bool>(game.sentences[i][j].size(), false));
        }

        game.userInputs.push_back(inputLine);
        game.correctMap.push_back(correctness);
    }
}

void resetGameResult(GameState& game) {
    game.result = GameResult();  // �⺻������ �ʱ�ȭ

    game.curPara = 0;
    game.curLine = 0;
    game.curChar = 0;

    game.accuracy = 0.f;
    game.wpm = 0.f;
    game.tpm = 0.f;
    game.progress = 0.f;
    game.totalKeyPress = 0;

    game.elapsedSeconds = 0.f;


    initUserInputsAndCorrectMap(game);
    game.started = false;
}

// --- �Է� ó�� ---
void handleInputGame(GameState& game, const sf::Event& event)
{
    if (game.currentScene == Scene::TYPING_GAME)
    {

        // �Ϲ� �ؽ�Ʈ �Է�
        if (event.is<sf::Event::TextEntered>())
        {
            int p = game.curPara;
            int l = game.curLine;

            std::wstring& inputLine = game.userInputs[p][l];
            std::wstring& targetLine = game.sentences[p][l];
            std::vector<bool>& correctLine = game.correctMap[p][l];

            const auto& inputEvent = event.getIf<sf::Event::TextEntered>();
            if (!inputEvent) return;

            wchar_t inputChar = static_cast<wchar_t>(inputEvent->unicode);

            // �齺���̽� ó��
            if (inputChar == 8)
            {
                printCorrectLineDebug(correctLine);
                if (game.curChar > 0) {
                    //game.userInputs[game.curPara][game.curLine].erase(game.curChar - 1, 1);
                    inputLine.erase(game.curChar - 1, 1);
                    correctLine.erase(correctLine.begin() + game.curChar - 1);
                    game.curChar--;
                }
            }

            // �Ϲ� �Է� ó��
            else if (inputChar >= 32 && inputChar != 127)
            {

                // �����̽� ó�� - ������ ���� ���� �� �����̽� ó�� �ؼ� ���� �����ϵ���
                if (inputChar == 32 && inputLine == targetLine) {
                    moveToNextLineOrParagraph(game);
                    return;
                }

                if (inputLine.length() >= targetLine.length()) {
                    std::wcout << L"[DEBUG] �� �̻� �Է��� �� �����ϴ�." << std::endl;
                    return;
                }

                //game.userInputs[game.curPara][game.curLine].insert(game.curChar, 1, inputChar);
                inputLine.insert(game.curChar, 1, inputChar);

                // ��Ȯ�� üũ�ؼ� correctMap ����
                bool isCorrect = (game.curChar < targetLine.size() && inputChar == targetLine[game.curChar]);
                correctLine.insert(correctLine.begin() + game.curChar, isCorrect);

                game.curChar++;
                game.totalKeyPress++;
            }
        }

        // Ű���� Ű �Է� (����, Esc.?<- �� ���߿� ��)
        if (event.is<sf::Event::KeyPressed>())
        {

            if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Enter)
            {

                int p = game.curPara;
                int l = game.curLine;

                std::wstring& inputLine = game.userInputs[p][l];
                std::wstring& targetLine = game.sentences[p][l];


                // ����� �α�
                printWStringInfo(L"�Է��� ���ڿ�", inputLine);
                printWStringInfo(L"���� ���ڿ�", targetLine);

                if (inputLine == targetLine) {
                    std::wcout << L"[DEBUG] ��ġ�մϴ� " << std::endl;
                }
                else {
                    std::wcout << L"[DEBUG] �ٸ��ϴ� " << std::endl;
                }

                if (inputLine == targetLine) {
                    // �Է��� ����� ���� ���� �� �̵�
                    moveToNextLineOrParagraph(game);
                }
                else {
                    std::wcout << L"[DEBUG] Ʋ���� �ȳѾ ����" << std::endl;
                }
            }

        }

    }
}

// --- ����� �Լ� ---
void printWStringInfo(const std::wstring& name, const std::wstring& str) {
    std::wcout << L"[" << name << L"] length = " << str.size() << L"\n";

    for (size_t i = 0; i < str.size(); ++i) {
        wchar_t ch = str[i];
        std::wcout << L"  [" << i << L"] : '" << ch << L"' (U+"
            << std::hex << std::uppercase << std::setw(4) << std::setfill(L'0')
            << static_cast<int>(ch) << L")\n";
    }
}
// correctMap �����
void printCorrectLineDebug(const std::vector<bool>& correctLine) {
    std::wcout << L"[DEBUG] correctLine ����: ";
    for (bool b : correctLine) {
        std::wcout << (b ? L"t " : L"f ");
    }
    std::wcout << std::endl;
}