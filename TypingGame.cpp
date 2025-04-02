// TypingGame
// �⺻ ������ �����ų ��� ���Ǵ� �Լ�
// startNewGame() - ���� ���� 
// showResultWindow() - ���� ���� �� ��� ȭ��
// drawUserInputText() - ����� �Է� ��� UI
// drawOriginalText() - �ҷ��� ���� ��� UI
// calculateAccuracyWithBackspace() - �齺���̽��� ����� ��Ȯ�� ����
// updateStats() - �ӵ�, ��Ȯ��, �ð� ��� �Լ�

/* typing.cpp ���� �����丵(2025.04.01) 
 * Ʋ�� ���ڸ� ���� �������� ����ǵ��� ���� */

/* ���� �ؾ� �� �� (2025.04.02)
 * ��Ȯ�� ��� �߰� - �Ϸ�(�齺���̽� ������� ������ ���߿� ����غ���)
 * Ÿ�� ��� �߰� - �Ϸ�
 * ��� ���ڱ��� �� �Է¹��� �� �ֵ��� �����ϱ� - �Ϸ�
 * ���� ��� �� ���ھ� �ҷ����� �ɷ� �ٲٱ� - �Ϸ�
 * ��Ȯ�� ������ ���� ����غ��� ����� backspace ���� Ƚ���� �ϼ� ���ڿ��� ���µ� �װ� ��Ȯ���� ���� �� ����*/
  

#include <iostream>
#include "TypingGame.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <sstream>

// ���� ���� �Լ�
void TypingGame::startNewGame(sf::RenderWindow& window, sf::Font& font, int fontSize, std::vector<std::vector<std::wstring>>& sentences)
{


    // ������ ������ �� ���� ��
    size_t numSentences = sentences.size();                   // ��ü ���� ����
    size_t numLinesInLastSentence = sentences.back().size();  // ������ ������ �� ��
    size_t totalLines = 0;
    size_t correctLineCount = 0;


    // ��� ������ �� �� ����
    for (const auto& sentence : sentences) {
        totalLines += sentence.size();
    }
    std::wcout << L"currentSentenceIdx" << numSentences << std::endl;
    std::wcout << L"currentSentenceIdx" << numLinesInLastSentence << std::endl;


    // �α� ��� �� ���߿� ����
    std::wstring target = sentences[numSentences-1].back(); // ������ ���� ��
    for (size_t i = 0; i < target.size(); ++i) {
        std::wcout << L"[" << i << L"]: '" << target[i] << L"' (int: " << (int)target[i] << L")\n";
    }
    int currentSentenceIdx = 0;
    int currentLineIdx = 0;
    // �� �ٿ� �������� �������� �̷��� �� ���� (������ �� ���)
    //�Ʒ��� ���� ����
    //sentences = {
    //{ L"Ÿ�ڿ��� ������", L"���� ���鱸��" },   // �� ���� 0 (�� 2��)
    //{ L"��´�, �������" },              // �� ���� 1 (�� 1��)
    //};
    
    // ���� ���� �ð� ���
    auto startTime = std::chrono::high_resolution_clock::now();  // ���� ���� �ð� ���

    // ��踦 ���� ���� ���� 
    double totalWpm = 0.0;          // �� WPM (�ܾ�� ��)
    double totalAccuracy = 0.0;     // �� ��Ȯ��
    double totalTime = 0.0;         // �� �ð�
    double totalTpm = 0;
    int totalKeyPress = 0;          // Ű �Է� Ƚ��
    int backspacePressCount = 0;

    // userInput �ʱ�ȭ
    std::vector<std::vector<std::wstring>> userInputs;
    for (const auto& sentenceLine : sentences) {
        std::vector<std::wstring> inputLine(sentenceLine.size(), L"");
        userInputs.push_back(inputLine);
    }


    while (window.isOpen()) {
        // �̺�Ʈ ó��
        while (const std::optional<sf::Event> Event = window.pollEvent()) {
            if (Event->is<sf::Event::Closed>()) window.close();

            // Ű���� �Է� ó��
            // Ű �Է�
            if (const auto* textEvent = Event->getIf<sf::Event::TextEntered>()) {
                wchar_t c = static_cast<wchar_t>(textEvent->unicode);   // �Էµ� ���ڸ� ������
                totalKeyPress++;
                std::wstring& currentSentence = userInputs[currentSentenceIdx][currentLineIdx];
                std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                if (c == 8) {  // �齺���̽� ó��
                    backspacePressCount++;
                    if (!currentInput.empty()) {
                        currentInput.pop_back();  // �Էµ� ���ڿ��� ������ ���� ����
                    }
                }
                // �� ���� ������ ġ�� �Է� �ȵǵ��� ����
                if (currentInput.length() != sentences[currentSentenceIdx][currentLineIdx].length())
                {
                    if (iswprint(c)) {  // ��� ������ ������ ���
                        currentInput += c;  // �Է� �ؽ�Ʈ�� ���� �߰�
                    }
                }

            }
            
            // ���� �Է�
            // ���ٸ��� ��Ȯ��, �ӵ� ����
            // ���� ��Ȯ���� ������ �Ѿ�� �ʵ���
            // ������ �� �϶� ���� ������ ��� â �������� 
            // Ʋ���� ���� �۽ô� �̰� �� �ϰ� ����, �����̴� �Ŷ�
            if (const auto* keyEvent = Event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {

                    // ���⿡ ���� ó�� ����
                    std::wstring& currentInput = userInputs[currentSentenceIdx][currentLineIdx];
                    // ����� ����� �ùٸ� ���� ���� ���� �Է��� �� �ֵ��� ����
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
                            // ª�� ������ ��� ���� �������� �̵�
                            currentLineIdx = 0;
                            currentSentenceIdx++;

                            // ��ü ������ �� ���� �Է����� ���
                            if (currentSentenceIdx >= sentences.size())
                            {
                                std::wcout << L"������ �Է���: ���â ���" << currentSentenceIdx << std::endl;
                                auto endTime = std::chrono::high_resolution_clock::now();  // ���� �ð� ���
                                
                                // ��ü ���� ���� ���â ���
                                showResultWindow(window, font, fontSize, totalWpm /totalLines, 
                                    totalAccuracy / totalLines, totalTime, totalTpm / totalLines, sentences);
                            }
                        }
                    }
                    else
                    {   

                        // �α� ��¿� ���߿� ����
                        std::wcout << L"����" << std::endl;
                        std::wcout << L"currentInput:    " << currentInput << std::endl;
                        std::wcout << L"sentences:    " << sentences[currentSentenceIdx][currentLineIdx] << std::endl;
                        std::wcout << L"���� ���ڼ�    " << currentInput.size() << std::endl;
                        std::wcout << L"���� ���ڼ�    " << sentences[currentSentenceIdx][currentLineIdx].size() << std::endl;

                    }
                }

            }
        }
    // ������
    // ui �ʱ�ȭ

        window.clear(sf::Color::White);

        drawOriginalText(window, font, fontSize, sentences);
        drawUserInputText(window, font, fontSize, userInputs, sentences);

        window.display();
    }

}

// ��� ��� ������ �Լ�
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
    // ��� â ��� ������ ����
    sf::RenderWindow resultWindow(sf::VideoMode({ 600, 400 }), "Final Result");

    // ����� �������� ���� wstringstream ��ȯ �� ���
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(2);               // �Ǽ� �ڸ� �� ����
    ss << L"- Words Per Minute(WPM): " << wpm << L" WPM\n";
    ss << L"- Accuracy(%): " << accuracy << L"%\n";
    ss << L"- Time(s): " << time << L"s\n";

    ss.unsetf(std::ios::fixed); // ������ fixed ����
    ss << L"- Ÿ��: " << static_cast<int>(tpm) << L"Ÿ";


    // ��� �ؽ�Ʈ
    sf::Text resultDisplay(font, ss.str(), 24);
    resultDisplay.setPosition({ 50, 50 });
    resultDisplay.setFillColor(sf::Color::White);
    // ����� ��ư 
    sf::RectangleShape retryButton(sf::Vector2f(200, 50));
    retryButton.setPosition({ 200, 250 });
    retryButton.setFillColor(sf::Color::Green);
    // ����� �ؽ�Ʈ
    sf::Text retryText(font, "Retry", 24);
    retryText.setPosition({ 250, 260 });
    retryText.setFillColor(sf::Color::White);
    // ������ ��ư
    sf::RectangleShape exitButton(sf::Vector2f(200, 50));
    exitButton.setPosition({ 200, 320 });
    exitButton.setFillColor(sf::Color::Red);
    // ������ �ؽ�Ʈ
    sf::Text exitText(font, "Exit", 24);
    exitText.setPosition({ 250, 330 });
    exitText.setFillColor(sf::Color::White);

    while (resultWindow.isOpen()) {
        // �̺�Ʈ ó��
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
        // ������
        resultWindow.clear(sf::Color::Black);
        resultWindow.draw(resultDisplay);
        resultWindow.draw(retryButton);
        resultWindow.draw(retryText);
        resultWindow.draw(exitButton);
        resultWindow.draw(exitText);
        resultWindow.display();
    }
}

// ui ���� �Լ� - �Է� ����
void TypingGame::drawUserInputText(
    sf::RenderWindow& window,
    sf::Font& font,
    int fontSize,
    std::vector<std::vector<std::wstring>>& userInputs,
    std::vector<std::vector<std::wstring>>& sentences)
{
    float x = 30.f;
    float y = 50.f + fontSize; // y�� �����ؼ� �� ���� �ֱ�

    // �� ���ھ� �Է�
    // �ѱ� / �����϶� ��������� �� drawOriginalText�� ����
    float letterSpacing = fontSize * 0.50f; // ����
    //float letterSpacing = fontSize * 0.90f; // �ѱ�


    for (int i = 0; i < userInputs.size(); ++i)
    {
        for (size_t j = 0; j < userInputs[i].size(); ++j)
        {
            std::wstring& inputLine = userInputs[i][j];
            std::wstring& targetLine = sentences[i][j];  // target ����
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

            y += fontSize * 3; // �� ���� ����
        }
    }
}
// ui ���� �Լ� - ���� ����
void TypingGame::drawOriginalText(
    sf::RenderWindow& window,
    sf::Font& font, 
    int fontSize, 
    std::vector<std::vector<std::wstring>>& fileAllLines)
{
    float x = 30.f;
    float y = 50.f; // y�� �����ؼ� �� ���� �ֱ�

    // �� ���ھ� �Է�
    // �ѱ� / �����϶� ��������� �� drawUserInputText�� ����
    float letterSpacing = fontSize * 0.50f; // ����
    //float letterSpacing = fontSize * 0.90f; // �ѱ�


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
            //inputText.setFillColor(correct ? sf::Color::Green : sf::Color::Red);  // ��Ȯ�ϸ� Green, �ƴϸ� Red
            ////inputText.setFillColor(sf::Color::Green);
            //inputText.setPosition({ x, y + fontSize + 10.f });

            //window.draw(inputText);


            y += fontSize * 3; // �� ���� ����

        }
    }

}

// �������̽��� ����� ��Ȯ�� ��� �Լ�
// �� �Էµ� ���ڿ��� �齺���̽��� �� ������ ��Ȯ���� ���
double TypingGame::calculateAccuracyWithBackspace(int totalTyped, int backspaceCount) {
    return 100.0 * (totalTyped - backspaceCount) / totalTyped; 
}

// �� ���徿 ��� ���� - ���� �̺�Ʈ�� ���� - Ű���� �Է¸��� ����ǵ��� ����
TypingStats TypingGame::updateStats(
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime,
    std::wstring& currentSentence,
    std::wstring& userInput,
    int backspacePressCount,
    int totalKeyPress)
{
    auto endTime = std::chrono::high_resolution_clock::now();       // ���� �ð� ���
    std::chrono::duration<double> elapsed = endTime - startTime;    // Ÿ���� �ð� ���

    TypingStats stats;

    // WPM ���: (���� �� / 5) / (�ҿ� �ð�(��))
    stats.wpm = (userInput.length() / 5.0f / (elapsed.count() / 60.0f));
    // ��Ȯ�� ���: �齺���̽��� ������ �Է� ����
    // �齺���̽��� ������ ����� ��Ȯ�� ���
   
    stats.accuracy = TypingGame::calculateAccuracyWithBackspace(currentSentence.length(), backspacePressCount);
    // ��� ������Ʈ - �����ؼ� ���
    stats.elapsedTime = elapsed.count();

    // TMP ���: (��ü �Է��� ���� �� / ��� �ð�(��)) * 60
    stats.tmp = (userInput.length() / stats.elapsedTime) * 60;

    // �ǽð� ��� �α� ����
    std::wcout << L"WPM: " << stats.wpm << L", ��Ȯ��: " << stats.accuracy << L"%, �ð�: " << elapsed.count() << L"Ÿ��: " << stats.tmp << L"s\n";

    return stats;
    
}