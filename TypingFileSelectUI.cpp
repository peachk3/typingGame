#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include <iostream>

struct FileOption {
	std::wstring filePath;				// 실제 파일 경로
	std::function<void()> onClick;		// 파일 선택 시 이벤트 처리
	std::shared_ptr<sf::Text> label;	// 텍스트 객체 포인터
	bool isHovered = false;    // 마우스가 올려져 있는 상태
	bool isClicked = false;    // 클릭해서 선택된 상태

};

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

void initLangSelectUI(sf::RenderWindow& window,
	sf::Font& font,
	sf::View& listView,
	float& maxScrollOffset,
	sf::FloatRect& loadUserFileBtnBound,
	std::vector<std::wstring> typingFilePath,
	std::vector<FileOption>& filieOptions,
	std::wstring selectMod,						// 한글 / 영여 / 코딩
	std::vector<std::shared_ptr<sf::Drawable>>& drawables,
	std::vector<std::shared_ptr<sf::Drawable>>& fileNameDrawables,
	UIState& currentState)
{
	// 파일 선택 UI 창
	sf::RectangleShape fileSelectionPanel = makeRectangle(window, 0.45f, 0.8f);
	fileSelectionPanel.setFillColor(sf::Color::White);
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
	title.setPosition({ titlePos.x, mpos.y + 10.f});

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
	loadUserFileBtnBound = loadBtnBounds;

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
	for (int i = 0; i < fileCount; i++)
	{
		// 구조체 초기화
		FileOption option;
		//option.fileName = L"";
		option.filePath = typingFilePath[i];
		std::wstring thisPath = typingFilePath[i];

		// 파일 목록
		auto fileName = std::make_shared<sf::Text>(font, removeExtension(thisPath), 18);
		fileName->setFillColor(sf::Color::Black);
		sf::FloatRect textBounds = fileName->getGlobalBounds();
		fileName->setPosition({
			startX , startY + (i * spacing) + 15 
			});


		option.label = fileName;		

		option.onClick = [thisPath]()
		{
			std::wcout << thisPath << L" 경로 파일 선택" << std::endl;

				
			// 게임 시작 화면 이동
			//currentState = UIState::ProfileMain;  // 다시 메인 화면으로 전환
		};
		filieOptions.push_back(option);
		fileNameDrawables.push_back(fileName);    // 여전히 drawables에도 추가
	}

	


	drawables.push_back(std::make_shared<sf::RectangleShape>(fileSelectionPanel));
	drawables.push_back(std::make_shared<sf::Text>(title));
	drawables.push_back(std::make_shared<sf::RectangleShape>(listContainer));
	drawables.push_back(std::make_shared<sf::RectangleShape>(loadBtn));
	drawables.push_back(std::make_shared<sf::Text>(btnText));




}
UIState currentState = UIState::fileListShow;

int main()
{
	std::setlocale(LC_ALL, "");

	// 객체 초기화
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");
	std::vector<std::shared_ptr<sf::Drawable>> langSelectUIDrawables;
	std::vector<std::shared_ptr<sf::Drawable>> fileNameDrawables;
	std::vector<std::wstring> typingFilePath;
	std::vector<FileOption> fileOptions;
	std::wstring selectMod = L"한글";		// 한글 / 영어 / 코딩

	sf::View listView;

	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
		return 1;
	}
	typingFilePath = {
	L"assets/typing/한글1.txt",
	L"assets/typing/한글2.txt",
	L"assets/typing/english.txt",
	L"assets/typing/coding.cpp",
	L"assets/typing/한글1.txt",
	L"assets/typing/한글2.txt",
	L"assets/typing/english.txt",
	L"assets/typing/coding.cpp",
	L"assets/typing/coding.cpp",
	L"assets/typing/한글1.txt",
	
	};

	// 스크롤 조절 값
	float scrollOffset = 0.f;
	float maxScrollOffset = 0.f;
	float scrollStep = 30.f;

	// 파일 불러오기 버튼을 위치 저장
	sf::FloatRect loadUserFileBtnBound;

	// UI 생성
	initLangSelectUI(window, font, listView, maxScrollOffset, loadUserFileBtnBound,  typingFilePath, fileOptions, selectMod, langSelectUIDrawables, fileNameDrawables, currentState);

	while (window.isOpen())
	{
		// 이벤트 루프
		while (const std::optional event = window.pollEvent())
		{
			// "close requested" event: we close the window
			if (event->is<sf::Event::Closed>())
				window.close();

			// 나중에 하자..
			//// 마우스 휠 이벤트
			//if (event->is<sf::Event::MouseWheelScrolled>()) {
			//	const auto& scrollEvent = event->getIf<sf::Event::MouseWheelScrolled>();
			//	float delta = scrollEvent->delta;
			//	scrollOffset -= delta * scrollStep;

			//	// 범위 제한
			//	scrollOffset = std::clamp(scrollOffset, 0.f, maxScrollOffset);

			//	// 뷰 이동 적용
			//	listView.move({ 0.f, delta * scrollStep });
			//}
			// 마우스 클릭 이벤트
			if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouse->button == sf::Mouse::Button::Left) {
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

					if (loadUserFileBtnBound.contains(mousePos)) {
						std::wcout << L"[불러오기 버튼 클릭됨]" << std::endl;
						//currentState = UIState::TypingGame;
					}
					for (auto& option : fileOptions) {
						if (option.label->getGlobalBounds().contains(mousePos)) {
							option.onClick();
						}
					}
				}
			}
		}

		// 마우스 위치 얻기
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		// hover 처리
		for (auto& option : fileOptions) {
			if (option.label->getGlobalBounds().contains(mousePos)) {
				if (!option.isHovered) {
					option.isHovered = true;
					option.label->setFillColor(sf::Color::Red);
				}
			}
			else {
				if (option.isHovered) {
					option.isHovered = false;
					option.label->setFillColor(sf::Color::Black);
				}
			}
		}


		// 렌더링 루프
		window.clear();

		for (auto& d : langSelectUIDrawables)
			window.draw(*d);

		//window.setView(listView);
		for (auto& d : fileNameDrawables)
			window.draw(*d);

		//window.setView(window.getDefaultView());

		window.display();
	}
}
