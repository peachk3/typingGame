#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace sf;
using namespace chrono;

// 함수 선언
// 문장을 파일에서 읽어오는 함수
vector<string> loadSentences(const string& filename) {
    vector<string> sentences;
    ifstream file(filename);  // 주어진 파일을 열기 위한 파일 스트림 객체 생성
    if (!file) {  // 파일을 열 수 없으면
        cerr << "파일을 열 수 없습니다: " << filename << endl;
        exit(1); // 파일 열기 실패 시 프로그램 종료
    }
    string line;
    while (getline(file, line)) {  // 파일에서 한 줄씩 읽기
        if (!line.empty()) sentences.push_back(line);  // 빈 줄이 아니면 문장을 추가
    }
    return sentences;  // 문장이 저장된 벡터 반환
}

// 백스페이스를 고려한 정확도 계산 함수
double calculateAccuracyWithBackspace(int totalTyped, int backspaceCount) {
    return 100.0 * (totalTyped - backspaceCount) / totalTyped;  // 총 입력된 문자에서 백스페이스를 뺀 비율을 정확도로 계산
}

// 게임을 시작하는 함수
void startNewGame(RenderWindow& window, const vector<string>& sentences) {
    size_t sentenceIndex = 0;  // 현재 문장의 인덱스
    string currentSentence = sentences[sentenceIndex];  // 현재 문장
    string userInput = "";  // 사용자가 입력한 텍스트
    bool isTyping = false;  // 타이핑 시작 여부
    auto startTime = high_resolution_clock::now();  // 게임 시작 시간 기록

    Font font;  // 폰트를 저장할 변수
    if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {  // 폰트 로드
        cerr << "폰트 로드 실패!" << endl;
        exit(1);  // 폰트 로드 실패 시 프로그램 종료
    }

    // 화면에 출력할 문장 텍스트 설정
    Text sentenceText(font, currentSentence, 24);
    sentenceText.setPosition({ 50, 100 });
    sentenceText.setFillColor(Color::White);

    // 사용자가 입력한 텍스트 설정
    Text userText(font, "", 24);
    userText.setPosition({ 50, 150 });
    userText.setFillColor(Color::Green);

    // 커서 깜빡이를 위한 시계 객체 생성
    Clock cursorClock;
    bool showCursor = true;  // 커서를 보여 줄지 말지 여부

    // 전체 게임 통계 계산을 위한 변수들
    double totalWpm = 0.0;  // 총 WPM (단어당 분)
    double totalAccuracy = 0.0;  // 총 정확도
    double totalTime = 0.0;  // 총 시간
    size_t totalSentences = 0;  // 총 문장 수

    // 메인 게임 루프 시작
    while (window.isOpen()) {
        // 창 이벤트 처리
        while (const optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();  // 창을 닫으면 종료
            }

            // 키보드 입력 처리
            if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                wchar_t c = static_cast<wchar_t>(textEvent->unicode);   // 입력된 문자를 가져옴

                if (c == 8) {  // 백스페이스 처리
                    if (!userInput.empty()) {
                        userInput.pop_back();  // 입력된 문자에서 마지막 문자 삭제
                    }
                }
                else if (iswprint(c)) {  // 출력 가능한 문자일 경우
                    userInput += c;  // 입력 텍스트에 문자 추가
                }
            }

            // Enter 키가 눌리면 문장이 완성된 것으로 처리
            if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
                if (userInput == currentSentence) {  // 문장이 정확히 일치하면
                    auto endTime = high_resolution_clock::now();  // 종료 시간 기록
                    duration<double> elapsed = endTime - startTime;  // 타이핑 시간 계산

                    // WPM 계산: (문자 수 / 5) / (소요 시간(분))
                    double wpm = (userInput.length() / 5.0) / (elapsed.count() / 60.0);
                    // 정확도 계산: 백스페이스를 제외한 입력 비율
                    double accuracy = calculateAccuracyWithBackspace(currentSentence.length(), count(userInput.begin(), userInput.end(), 8));

                    // 통계 업데이트 - 누적해서 계산
                    double totalTimeInSeconds = elapsed.count();
                    totalWpm += wpm;
                    totalAccuracy += accuracy;
                    totalTime += elapsed.count();
                    totalSentences++;

                    // 모든 문장을 입력한 경우 결과 표시
                    if (sentenceIndex + 1 >= sentences.size()) {
                        // 결과 창을 새로 생성
                        RenderWindow resultWindow(VideoMode({ 600, 400 }), "Final Result");

                        // 결과 텍스트 생성
                        string resultText = "- Words Per Minute(WPM): " + to_string(totalWpm / totalSentences) + " WPM\n";
                        resultText += "- Accuracy(%): " + to_string(totalAccuracy / totalSentences) + "%\n";
                        resultText += "- Time(s): " + to_string(totalTime) + "s";

                        Text resultDisplay(font, resultText, 24);       // 결과를 화면에 표시할 텍스트 객체 생성
                        resultDisplay.setPosition({ 50, 50 });
                        resultDisplay.setFillColor(Color::White);

                        // Retry 버튼 생성
                        RectangleShape retryButton(Vector2f(200, 50));
                        retryButton.setPosition({ 200, 250 });
                        retryButton.setFillColor(Color::Green);

                        Text retryText(font, "Retry", 24);
                        retryText.setPosition({ 250, 260 });
                        retryText.setFillColor(Color::White);

                        // Exit 버튼 생성
                        RectangleShape exitButton(Vector2f(200, 50));
                        exitButton.setPosition({ 200, 320 });
                        exitButton.setFillColor(Color::Red);

                        Text exitText(font, "Exit", 24);
                        exitText.setPosition({ 250, 330 });
                        exitText.setFillColor(Color::White);

                        // 결과 창에서 Retry 또는 Exit 버튼 클릭 이벤트 처리
                        while (resultWindow.isOpen()) {
                            while (const optional<Event> resultEvent = resultWindow.pollEvent()) {
                                if (resultEvent->is<Event::Closed>()) {
                                    resultWindow.close();  // 결과 창 닫기
                                }

                                if (resultEvent->is<Event::MouseButtonPressed>()) {     // 마우스 버튼 클릭 이벤트 처리
                                    Vector2i mousePos = Mouse::getPosition(resultWindow);

                                    // Retry 버튼 클릭 시 게임 재시작
                                    if (retryButton.getGlobalBounds().contains(Vector2f(mousePos))) {
                                        resultWindow.close();
                                        startNewGame(window, sentences);  // 새 게임 시작
                                    }
                                    // Exit 버튼 클릭 시 게임 종료
                                    else if (exitButton.getGlobalBounds().contains(Vector2f(mousePos))) {
                                        resultWindow.close();
                                        window.close();  // 게임 종료
                                    }
                                }
                            }

                            // 결과 창 화면 업데이트
                            resultWindow.clear(Color::Black);
                            resultWindow.draw(resultDisplay);
                            resultWindow.draw(retryButton);
                            resultWindow.draw(retryText);
                            resultWindow.draw(exitButton);
                            resultWindow.draw(exitText);
                            resultWindow.display();
                        }

                        window.close();  // 결과 창이 닫히면 게임 종료
                    }
                    else {  // 아직 문장이 남아 있으면 다음 문장으로 이동
                        sentenceIndex++;
                        currentSentence = sentences[sentenceIndex];   // 새로운 문장 설정
                        sentenceText.setString(currentSentence);     // 화면에 표시할 문장 변경
                        userInput = "";                             // 사용자 입력 텍스트 초기화
                        startTime = high_resolution_clock::now();  // 타이머 초기화
                    }
                }
            }
        }

        // 커서 깜빡임 처리
        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            showCursor = !showCursor;  // 커서의 상태를 반전
            cursorClock.restart();  // 타이머 초기화
        }

        // 사용자가 입력한 문장이 정확하면 텍스트 색을 초록색으로, 아니면 빨간색으로 설정
        if (currentSentence.rfind(userInput, 0) == 0) {
            userText.setFillColor(Color::Green);
        }
        else {
            userText.setFillColor(Color::Red);
        }

        // 사용자 입력 텍스트와 커서 표시
        userText.setString(userInput + (showCursor ? "|" : ""));

        // 화면 업데이트
        window.clear(Color::Black);
        window.draw(sentenceText);  // 현재 문장 그리기
        window.draw(userText);      // 사용자가 입력한 텍스트 그리기
        window.display();
    }
}

int main() {
    try {
        cerr << "Program started!" << endl;         // 프로그램 시작 메시지

        Font font;
        if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {  // 폰트 불러오기
            cerr << "폰트 로드 실패!" << endl;
            return 1;                               // 프로그램 종료
        }

        RenderWindow window(VideoMode({ 800, 400 }), "Typing Practice Game");  // 게임 창 생성

        vector<string> sentences = loadSentences("aegukga.txt");  // 문장 파일 로드

        startNewGame(window, sentences);  // 첫 번째 게임 시작

        cerr << "게임 종료!" << endl;
    }
    catch (const exception& e) {    // 예외 처리
        cerr << "예외 발생: " << e.what() << endl;  // 예외 메시지 출력
        return 1;
    }
    catch (...) {
        cerr << "알 수 없는 오류가 발생했습니다." << endl;  // 오류 메시지 출력
        return 1;
    }
    return 0;   // 정상 종료
}
