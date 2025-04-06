#include "TypingGame.hpp"
#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

// --- 렌더링 ---
void renderGame(sf::RenderWindow& window, GameState& game, const sf::Font& font, int fontSize)
{
    // UI 배치를 위한 프레임 생성
    // 윈도우 크기의 사각형 생성
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

    // 정렬
    sf::Vector2f pfImgSize = pfImg.getSize();
    sf::Vector2f gStateSize = gState.getSize();
    sf::Vector2f typingAreaSize = typingArea.getSize();

    sf::FloatRect typingAreaBounds = typingArea.getGlobalBounds();
    game.typingAreaWidth = typingAreaBounds.size.x;

    // 오른쪽 아래 정렬
    //sf::Vector2f btnSize = loadBtn.getGlobalBounds().size;
    sf::Vector2f typingAreaPos = AlignPosition(
        typingAreaSize,
        winRactBounds,
        AlignX::Right,
        AlignY::Bottom,
        60.f, 30.f
    );

    // 왼쪽 아래 정렬
    sf::Vector2f gStatePos = AlignPosition(
        gStateSize,
        winRactBounds,
        AlignX::Left,
        AlignY::Bottom,
        30.f, 30.f
    );
    typingArea.setPosition(typingAreaPos);
    gState.setPosition(gStatePos);

    // gstate 가운데 정렬, typingAreaPos 와 y 좌표 맞춤
    sf::FloatRect s = gState.getGlobalBounds();
    sf::Vector2f pfImgPos = getCenterXPosition(pfImgSize, gState.getGlobalBounds(), typingAreaPos.y);
    pfImg.setPosition(pfImgPos);


    // 문장 출력 위치 기준
    sf::Vector2f sentenceStartPos = typingAreaPos;

    // 입력 받은 파일 초기화
    std::vector<std::vector<std::wstring>> displaySentences = game.sentences;
    std::vector<std::vector<std::wstring>> inputSentences = game.userInputs;


    // UI 요소 넣기 
    // 문장 출력
    drawOriginalText(window, game, font, fontSize, sentenceStartPos, displaySentences);
    sentenceStartPos = { sentenceStartPos.x, sentenceStartPos.y + fontSize };
    drawUserInputText(window, game, font, fontSize, sentenceStartPos, inputSentences, displaySentences);



    // 이미지
    sf::Sprite imgSprite(game.user.profileTexture);
    imgSprite.setPosition(pfImgPos);



    // 게임 상태
    sf::Text playTimeL(font, L"진행시간", 20);
    playTimeL.setFillColor(sf::Color::Black);

    sf::Text tpmL(font, L"타수(타/분)", 20);
    tpmL.setFillColor(sf::Color::Black);

    sf::Text accuracyL(font, L"정확도(%)", 20);
    accuracyL.setFillColor(sf::Color::Black);

    sf::Text progressL(font, L"진행도(%)", 20);
    progressL.setFillColor(sf::Color::Black);

    sf::Text playTimeV(font, L"", 20);
    playTimeV.setFillColor(sf::Color::Black);

    sf::Text tpmV(font, L"", 20);
    tpmV.setFillColor(sf::Color::Black);
    //tpmV.setString(game.tpm + L"타");

    sf::Text accuracyV(font, L"", 20);
    accuracyV.setFillColor(sf::Color::Black);

    sf::Text progressV(font, L"", 20);
    progressV.setFillColor(sf::Color::Black);


    // 진행시간
    int totalSeconds = static_cast<int>(game.elapsedSeconds);

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    std::wstringstream ss;
    ss << minutes << L":" << std::setw(2) << std::setfill(L'0') << seconds;

    playTimeV.setString(ss.str());  // 예: 01:23
    
    // 타수 (정수)
    tpmV.setString(std::to_wstring(static_cast<int>(game.tpm)) + L"타");

    // WPM - 영어일 때 이걸로 바꿀까? 
    //{
    //    std::wstringstream ss;
    //    ss << std::fixed << std::setprecision(1) << game.wpm;
    //    wpmV.setString(ss.str());
    //}

// 정확도
    {
        std::wstringstream ss;
        ss << std::fixed << std::setprecision(1) << game.accuracy;
        accuracyV.setString(ss.str() + L" %");
    }

    // 진행도
    {
        std::wstringstream ss;
        ss << std::fixed << std::setprecision(1) << game.progress;
        progressV.setString(ss.str() + L" %");
    }

    // 텍스트 배치
    std::vector<sf::Text*> labels = { &playTimeL, &tpmL, &accuracyL, &progressL };
    std::vector<sf::Text*> values = { &playTimeV, &tpmV, &accuracyV, &progressV };

    float lineHeight = 100.f;        // 줄 간격

    // 라벨 배치 기준점
    sf::Vector2 standardLPos = playTimeL.getGlobalBounds().size;
    sf::Vector2f textLPos = AlignPosition(standardLPos, gState.getGlobalBounds(), AlignX::Left, AlignY::Top, 20, 20); // 기준 점 
    playTimeL.setPosition(textLPos);


    // 값 배치 기준점
    sf::Vector2 standardVPos = playTimeV.getGlobalBounds().size;
    sf::Vector2f textVPos = AlignPosition(standardLPos, gState.getGlobalBounds(), AlignX::Right, AlignY::Top, 15, 20);


    for (size_t i = 0; i < labels.size(); ++i) {
        labels[i]->setPosition({ textLPos.x, textLPos.y + i * lineHeight });
        values[i]->setPosition({ textVPos.x, textVPos.y + i * lineHeight });
    }




    // 컨테이너 draw
    //window.draw(pfImg);
    window.draw(gState);
    window.draw(typingArea);

    // 이미지 draw
    window.draw(imgSprite);

    // 텍스트 라벨 draw
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
        letterSpacing = fontSize * 0.90f; // 한글
    }
    else {
        letterSpacing = fontSize * 0.50f;
    }


    for (int i = 0; i < userInputs.size(); ++i)
    {
        for (size_t j = 0; j < userInputs[i].size(); ++j)
        {
            std::wstring& inputLine = userInputs[i][j];
            std::wstring& targetLine = sentences[i][j];  // target 문장
            for (size_t k = 0; k < inputLine.size(); ++k)
            {
                sf::Text inputText(font, inputLine.substr(k, 1), fontSize);

                inputText.setFillColor(isCorrect[i][j][k] ? sf::Color::Green : sf::Color::Red);
                inputText.setPosition({ startX + k * letterSpacing, startY + (fontSize * (i + 1)) + 10.f });


                window.draw(inputText);
            }

            // 커서 위치 표시
            if (i == game.curPara && j == game.curLine) {
                sf::Text cursorText(font, L"_", fontSize); // 또는 L"_"
                cursorText.setFillColor(sf::Color::Blue);
                cursorText.setPosition({ startX + game.curChar * letterSpacing, startY + (fontSize * (i + 1)) + 10.f });

                window.draw(cursorText);
            }



            startY += fontSize * 4; // 줄 간격 설정
        }
    }
}

void drawOriginalText(sf::RenderWindow& window,
    const GameState& game, const sf::Font& font, int fontSize, sf::Vector2f standardPos,
    std::vector<std::vector<std::wstring>>& displaySentences)
{
    float startX = standardPos.x + 20.f;        // 노란 영역 패딩 20
    float startY = standardPos.y;               // 노란 영역 패딩 20

    float x = 30.f;
    float y = 50.f; // y를 누적해서 줄 간격 주기
    float letterSpacing = fontSize * 0.90f;  // 한글, 영어일 때 문자 간격 설정
    //std::vector<std::vector<std::wstring>> displaySentences = game.sentences;

    if (game.bHangle)
    {
        letterSpacing = fontSize * 0.90f; // 한글
    }
    else {
        letterSpacing = fontSize * 0.50f;
    }

    // 한 글자씩 입력
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

            startY += fontSize * 4; // 줄 간격 설정

        }
    }
}



// --- 로직 ---
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

    

    // 정확도 계산
    game.accuracy = (totalInputChars > 0)
        ? static_cast<float>(correctCount) / totalInputChars * 100.f
        : 100.f;
    // Tpm 계산
    float elapsedMinutes = elapsedSeconds / 60.f;
    game.tpm = (elapsedMinutes > 0)
        ? static_cast<float>(totalInputChars) / elapsedMinutes
        : 0.f;
    // Wpm 계산
    game.wpm = (elapsedMinutes > 0)
        ? (static_cast<float>(totalInputChars) / 5.f) / elapsedMinutes
        : 0.f;
    // 진행도 (%)
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
        std::wcout << L"[DEBUG] 마지막 문장 입력 완료! 결과 화면으로 넘어갈 준비." << std::endl;
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
    game.result = GameResult();  // 기본값으로 초기화

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

// --- 입력 처리 ---
void handleInputGame(GameState& game, const sf::Event& event)
{
    if (game.currentScene == Scene::TYPING_GAME)
    {

        // 일반 텍스트 입력
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

            // 백스페이스 처리
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

            // 일반 입력 처리
            else if (inputChar >= 32 && inputChar != 127)
            {

                // 스페이스 처리 - 문장의 끝에 왔을 때 스페이스 처리 해서 개행 가능하도록
                if (inputChar == 32 && inputLine == targetLine) {
                    moveToNextLineOrParagraph(game);
                    return;
                }

                if (inputLine.length() >= targetLine.length()) {
                    std::wcout << L"[DEBUG] 더 이상 입력할 수 없습니다." << std::endl;
                    return;
                }

                //game.userInputs[game.curPara][game.curLine].insert(game.curChar, 1, inputChar);
                inputLine.insert(game.curChar, 1, inputChar);

                // 정확도 체크해서 correctMap 갱신
                bool isCorrect = (game.curChar < targetLine.size() && inputChar == targetLine[game.curChar]);
                correctLine.insert(correctLine.begin() + game.curChar, isCorrect);

                game.curChar++;
                game.totalKeyPress++;
            }
        }

        // 키보드 키 입력 (엔터, Esc.?<- 는 나중에 ㅎ)
        if (event.is<sf::Event::KeyPressed>())
        {

            if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Enter)
            {

                int p = game.curPara;
                int l = game.curLine;

                std::wstring& inputLine = game.userInputs[p][l];
                std::wstring& targetLine = game.sentences[p][l];


                // 디버그 로그
                printWStringInfo(L"입력한 문자열", inputLine);
                printWStringInfo(L"정답 문자열", targetLine);

                if (inputLine == targetLine) {
                    std::wcout << L"[DEBUG] 일치합니다 " << std::endl;
                }
                else {
                    std::wcout << L"[DEBUG] 다릅니다 " << std::endl;
                }

                if (inputLine == targetLine) {
                    // 입력이 정답과 같을 때만 줄 이동
                    moveToNextLineOrParagraph(game);
                }
                else {
                    std::wcout << L"[DEBUG] 틀려서 안넘어감 ㅅㄱ" << std::endl;
                }
            }

        }

    }
}

// --- 디버깅 함수 ---
void printWStringInfo(const std::wstring& name, const std::wstring& str) {
    std::wcout << L"[" << name << L"] length = " << str.size() << L"\n";

    for (size_t i = 0; i < str.size(); ++i) {
        wchar_t ch = str[i];
        std::wcout << L"  [" << i << L"] : '" << ch << L"' (U+"
            << std::hex << std::uppercase << std::setw(4) << std::setfill(L'0')
            << static_cast<int>(ch) << L")\n";
    }
}
// correctMap 디버깅
void printCorrectLineDebug(const std::vector<bool>& correctLine) {
    std::wcout << L"[DEBUG] correctLine 상태: ";
    for (bool b : correctLine) {
        std::wcout << (b ? L"t " : L"f ");
    }
    std::wcout << std::endl;
}