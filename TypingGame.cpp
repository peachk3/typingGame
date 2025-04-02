// TypingGame
// 기본 게임을 실행시킬 경우 사용되는 함수
// startNewGame() - 게임 시작 
// showResultWindow() - 게임 종료 후 결과 화면
// drawUserInputText() - 사용자 입력 출력 UI
// drawOriginalText() - 불러온 파일 출력 UI
// calculateAccuracyWithBackspace() - 백스페이스를 고려한 정확도 측정
// updateStats() - 속도, 정확도, 시간 기록 함수

/* typing.cpp 파일 리팩토링(2025.04.01) 
 * 틀린 글자만 색이 빨강으로 변경되도록 수정 */

/* 오늘 해야 할 것 (2025.04.02)
 * 정확도 기능 추가 - 완료(백스페이스 고려할지 말지는 나중에 얘기해보기)
 * 타수 기능 추가 - 완료
 * 출력 글자까지 만 입력받을 수 있도록 제한하기 - 완료
 * 파일 출력 한 글자씩 불러오는 걸로 바꾸기 - 완료
 * 정확도 측정에 대해 얘기해보기 현재는 backspace 누른 횟수를 완성 글자에서 빼는데 그건 정확하지 않은 거 같음*/
  

#include <iostream>
#include "TypingGame.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <sstream>

// 게임 시작 함수
void TypingGame::startNewGame(sf::RenderWindow& window, sf::Font& font, int fontSize, std::vector<std::vector<std::wstring>>& sentences)
{


    // 선택한 파일의 총 문장 수
    size_t numSentences = sentences.size();                   // 전체 문장 개수
    size_t numLinesInLastSentence = sentences.back().size();  // 마지막 문장의 줄 수
    size_t totalLines = 0;
    size_t correctLineCount = 0;


    // 모든 문장의 줄 수 세기
    for (const auto& sentence : sentences) {
        totalLines += sentence.size();
    }
    std::wcout << L"currentSentenceIdx" << numSentences << std::endl;
    std::wcout << L"currentSentenceIdx" << numLinesInLastSentence << std::endl;


    // 로그 출력 용 나중에 삭제
    std::wstring target = sentences[numSentences-1].back(); // 마지막 문장 줄
    for (size_t i = 0; i < target.size(); ++i) {
        std::wcout << L"[" << i << L"]: '" << target[i] << L"' (int: " << (int)target[i] << L")\n";
    }
    int currentSentenceIdx = 0;
    int currentLineIdx = 0;
    // 한 줄에 여려개의 문장으로 이뤄질 수 있음 (문장이 긴 경우)
    //아래와 같은 형태
    //sentences = {
    //{ L"타자연습 제작중", L"삶이 힘들구만" },   // ← 문장 0 (줄 2개)
    //{ L"잠온다, 배고프다" },              // ← 문장 1 (줄 1개)
    //};
    
    // 게임 시작 시간 기록
    auto startTime = std::chrono::high_resolution_clock::now();  // 게임 시작 시간 기록

    // 통계를 내기 위한 변수 
    double totalWpm = 0.0;          // 총 WPM (단어당 분)
    double totalAccuracy = 0.0;     // 총 정확도
    double totalTime = 0.0;         // 총 시간
    double totalTpm = 0;
    int totalKeyPress = 0;          // 키 입력 횟수
    int backspacePressCount = 0;

    // userInput 초기화
    std::vector<std::vector<std::wstring>> userInputs;
    for (const auto& sentenceLine : sentences) {
        std::vector<std::wstring> inputLine(sentenceLine.size(), L"");
        userInputs.push_back(inputLine);
    }


    while (window.isOpen()) {
        // 이벤트 처리
        while (const std::optional<sf::Event> Event = window.pollEvent()) {
            if (Event->is<sf::Event::Closed>()) window.close();

            // 키보드 입력 처리
            // 키 입력
            if (const auto* textEvent = Event->getIf<sf::Event::TextEntered>()) {
                wchar_t c = static_cast<wchar_t>(textEvent->unicode);   // 입력된 문자를 가져옴
                totalKeyPress++;
                std::wstring& currentSentence = userInputs[currentSentenceIdx][currentLineIdx];
                std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                if (c == 8) {  // 백스페이스 처리
                    backspacePressCount++;
                    if (!currentInput.empty()) {
                        currentInput.pop_back();  // 입력된 문자에서 마지막 문자 삭제
                    }
                }
                // 한 문장 끝까지 치면 입력 안되도록 제한
                if (currentInput.length() != sentences[currentSentenceIdx][currentLineIdx].length())
                {
                    if (iswprint(c)) {  // 출력 가능한 문자일 경우
                        currentInput += c;  // 입력 텍스트에 문자 추가
                    }
                }

            }
            
            // 엔터 입력
            // 한줄마다 정확도, 속도 나옴
            // 한줄 정확하지 않으면 넘어가지 않도록
            // 마지막 줄 일때 엔터 누르면 결과 창 나오도록 
            // 틀리면 빨강 글시는 이거 다 하고 구현, 깜박이는 거랑
            if (const auto* keyEvent = Event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {

                    // 여기에 엔터 처리 로직
                    std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                    // 현재는 결과가 올바를 때만 다음 문장 입력할 수 있도록 구현
                    if (currentInput == sentences[currentSentenceIdx][currentLineIdx])
                    {
                        TypingStats stats = TypingGame::updateStats(startTime, sentences[currentSentenceIdx][currentLineIdx],
                            currentInput, backspacePressCount, totalKeyPress);


                        totalWpm += stats.wpm;
                        totalAccuracy += stats.accuracy;
                        totalTime += stats.elapsedTime;
                        totalTpm += stats.tmp;
                        std::wcout << L"totalTpm" << totalTpm << std::endl;
                        correctLineCount++;
                        currentLineIdx++;

                        if (currentLineIdx >= sentences[currentSentenceIdx].size())
                        {
                            // 짧은 문장일 경우 다음 문장으로 이동
                            currentLineIdx = 0;
                            currentSentenceIdx++;

                            // 전체 문장을 끝 까지 입력했을 경우
                            if (currentSentenceIdx >= sentences.size())
                            {
                                std::wcout << L"끝까지 입력함: 결과창 출력" << currentSentenceIdx << std::endl;
                                auto endTime = std::chrono::high_resolution_clock::now();  // 종료 시간 기록
                                
                                // 전체 문장 종료 결과창 출력
                                showResultWindow(window, font, fontSize, totalWpm /totalLines, 
                                    totalAccuracy / totalLines, totalTime, totalTpm / totalLines, sentences);
                            }
                        }
                    }
                    else
                    {   

                        // 로그 출력용 나중에 삭제
                        std::wcout << L"오답" << std::endl;
                        std::wcout << L"currentInput:    " << currentInput << std::endl;
                        std::wcout << L"sentences:    " << sentences[currentSentenceIdx][currentLineIdx] << std::endl;
                        std::wcout << L"오답 글자수    " << currentInput.size() << std::endl;
                        std::wcout << L"정답 글자수    " << sentences[currentSentenceIdx][currentLineIdx].size() << std::endl;

                    }
                }

            }
        }
    // 렌더링
    // ui 초기화

        window.clear(sf::Color::White);

        drawOriginalText(window, font, fontSize, sentences);
        drawUserInputText(window, font, fontSize, userInputs, sentences);

        window.display();
    }

}

// 결과 출력 윈도우 함수
void TypingGame::showResultWindow(
    sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    double wpm,
    double accuracy,
    double time,
    double tpm,
    std::vector<std::vector<std::wstring>>& sentences)
{
    // 결과 창 띄울 윈도우 생성
    sf::RenderWindow resultWindow(sf::VideoMode({ 600, 400 }), "Final Result");

    // 깔끔한 포멧팅을 위해 wstringstream 변환 후 출력
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(2);               // 실수 자리 수 제한
    ss << L"- Words Per Minute(WPM): " << wpm << L" WPM\n";
    ss << L"- Accuracy(%): " << accuracy << L"%\n";
    ss << L"- Time(s): " << time << L"s\n";

    ss.unsetf(std::ios::fixed); // 정수는 fixed 없이
    ss << L"- 타수: " << static_cast<int>(tpm) << L"타";


    // 결과 텍스트
    sf::Text resultDisplay(font, ss.str(), 24);
    resultDisplay.setPosition({ 50, 50 });
    resultDisplay.setFillColor(sf::Color::White);
    // 재시작 버튼 
    sf::RectangleShape retryButton(sf::Vector2f(200, 50));
    retryButton.setPosition({ 200, 250 });
    retryButton.setFillColor(sf::Color::Green);
    // 재시작 텍스트
    sf::Text retryText(font, "Retry", 24);
    retryText.setPosition({ 250, 260 });
    retryText.setFillColor(sf::Color::White);
    // 나가기 버튼
    sf::RectangleShape exitButton(sf::Vector2f(200, 50));
    exitButton.setPosition({ 200, 320 });
    exitButton.setFillColor(sf::Color::Red);
    // 나가기 텍스트
    sf::Text exitText(font, "Exit", 24);
    exitText.setPosition({ 250, 330 });
    exitText.setFillColor(sf::Color::White);

    while (resultWindow.isOpen()) {
        // 이벤트 처리
        while (const std::optional<sf::Event> resultEvent = resultWindow.pollEvent()) {
            if (resultEvent->is<sf::Event::Closed>()) resultWindow.close();

            if (resultEvent->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(resultWindow);
                if (retryButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    resultWindow.close();
                    startNewGame(window, font, fontSize, sentences);
                }
                else if (exitButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    resultWindow.close();
                    //window.close();
                }
            }
        }
        // 렌더링
        resultWindow.clear(sf::Color::Black);
        resultWindow.draw(resultDisplay);
        resultWindow.draw(retryButton);
        resultWindow.draw(retryText);
        resultWindow.draw(exitButton);
        resultWindow.draw(exitText);
        resultWindow.display();
    }
}

// ui 생성 함수 - 입력 문장
void TypingGame::drawUserInputText(
    sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& userInputs,
    std::vector<std::vector<std::wstring>>& sentences)
{
    float x = 30.f;
    float y = 50.f + fontSize; // y를 누적해서 줄 간격 주기

    // 한 글자씩 입력
    // 한글 / 영어일때 수정해줘야 됨 drawOriginalText도 같이
    float letterSpacing = fontSize * 0.50f; // 영어
    //float letterSpacing = fontSize * 0.90f; // 한글


    for (int i = 0; i < userInputs.size(); ++i)
    {
        for (size_t j = 0; j < userInputs[i].size(); ++j)
        {
            std::wstring& inputLine = userInputs[i][j];
            std::wstring& targetLine = sentences[i][j];  // target 문장
            for (size_t k = 0; k < inputLine.size(); ++k)
            {
                sf::Text inputText(font, inputLine.substr(k, 1), fontSize);
                
                if (k < targetLine.size() && inputLine[k] == targetLine[k]) {
                    inputText.setFillColor(sf::Color::Green);
                }
                else {
                    inputText.setFillColor(sf::Color::Red);
                }
                inputText.setPosition({ x + k * letterSpacing, y + fontSize + 10.f });

                
                window.draw(inputText);
            }

            y += fontSize * 3; // 줄 간격 설정
        }
    }
}
// ui 생성 함수 - 기준 문장
void TypingGame::drawOriginalText(
    sf::RenderWindow& window,
    sf::Font& font, 
    int fontSize, 
    std::vector<std::vector<std::wstring>>& fileAllLines)
{
    float x = 30.f;
    float y = 50.f; // y를 누적해서 줄 간격 주기

    // 한 글자씩 입력
    // 한글 / 영어일때 수정해줘야 됨 drawUserInputText도 같이
    float letterSpacing = fontSize * 0.50f; // 영어
    //float letterSpacing = fontSize * 0.90f; // 한글


    for (int i = 0; i < fileAllLines.size(); ++i)
    {
        for (size_t j = 0; j < fileAllLines[i].size(); ++j)
        {
            std::wstring& Line = fileAllLines[i][j];

            for (size_t k = 0; k < Line.size(); ++k)
            {
                sf::Text oneChar(font, Line.substr(k, 1), fontSize);

                oneChar.setFillColor(sf::Color::Black);
                
                oneChar.setPosition({ x + k * letterSpacing, y + fontSize + 10.f });


                window.draw(oneChar);
            }

            //sf::Text inputText(font, userInputs[i][j], fontSize);
            //inputText.setFillColor(correct ? sf::Color::Green : sf::Color::Red);  // 정확하면 Green, 아니면 Red
            ////inputText.setFillColor(sf::Color::Green);
            //inputText.setPosition({ x, y + fontSize + 10.f });

            //window.draw(inputText);


            y += fontSize * 3; // 줄 간격 설정

        }
    }

}

// 벡스페이스를 고려한 정확도 계산 함수
// 총 입력된 문자에서 백스페이스를 뺀 비율을 정확도로 계산
double TypingGame::calculateAccuracyWithBackspace(int totalTyped, int backspaceCount) {
    return 100.0 * (totalTyped - backspaceCount) / totalTyped; 
}

// 한 문장씩 기록 저장 - 엔터 이벤트에 실행 - 키보드 입력마다 실행되도록 수정
TypingStats TypingGame::updateStats(
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime,
    std::wstring& currentSentence,
    std::wstring& userInput,
    int backspacePressCount,
    int totalKeyPress)
{
    auto endTime = std::chrono::high_resolution_clock::now();       // 종료 시간 기록
    std::chrono::duration<double> elapsed = endTime - startTime;    // 타이핑 시간 계산

    TypingStats stats;

    // WPM 계산: (문자 수 / 5) / (소요 시간(분))
    stats.wpm = (userInput.length() / 5.0f / (elapsed.count() / 60.0f));
    // 정확도 계산: 백스페이스를 제외한 입력 비율
    // 백스페이스의 영향을 고려한 정확도 계산
   
    stats.accuracy = TypingGame::calculateAccuracyWithBackspace(currentSentence.length(), backspacePressCount);
    // 통계 업데이트 - 누적해서 계산
    stats.elapsedTime = elapsed.count();

    // TMP 계산: (전체 입력한 글자 수 / 경과 시간(초)) * 60
    stats.tmp = (userInput.length() / stats.elapsedTime) * 60;

    // 실시간 출력 로그 한줄
    std::wcout << L"WPM: " << stats.wpm << L", 정확도: " << stats.accuracy << L"%, 시간: " << elapsed.count() << L"타수: " << stats.tmp << L"s\n";

    return stats;
    
}