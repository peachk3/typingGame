#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include <iostream>

struct FileOption {
	std::wstring filePath;				// ���� ���� ���
	std::function<void()> onClick;		// ���� ���� �� �̺�Ʈ ó��
	std::shared_ptr<sf::Text> label;	// �ؽ�Ʈ ��ü ������
	bool isHovered = false;    // ���콺�� �÷��� �ִ� ����
	bool isClicked = false;    // Ŭ���ؼ� ���õ� ����

};

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

void initLangSelectUI(sf::RenderWindow& window,
	sf::Font& font,
	sf::View& listView,
	float& maxScrollOffset,
	sf::FloatRect& loadUserFileBtnBound,
	std::vector<std::wstring> typingFilePath,
	std::vector<FileOption>& filieOptions,
	std::wstring selectMod,						// �ѱ� / ���� / �ڵ�
	std::vector<std::shared_ptr<sf::Drawable>>& drawables,
	std::vector<std::shared_ptr<sf::Drawable>>& fileNameDrawables,
	UIState& currentState)
{
	// ���� ���� UI â
	sf::RectangleShape fileSelectionPanel = makeRectangle(window, 0.45f, 0.8f);
	fileSelectionPanel.setFillColor(sf::Color::White);
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
	title.setPosition({ titlePos.x, mpos.y + 10.f});

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
	loadUserFileBtnBound = loadBtnBounds;

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
	for (int i = 0; i < fileCount; i++)
	{
		// ����ü �ʱ�ȭ
		FileOption option;
		//option.fileName = L"";
		option.filePath = typingFilePath[i];
		std::wstring thisPath = typingFilePath[i];

		// ���� ���
		auto fileName = std::make_shared<sf::Text>(font, removeExtension(thisPath), 18);
		fileName->setFillColor(sf::Color::Black);
		sf::FloatRect textBounds = fileName->getGlobalBounds();
		fileName->setPosition({
			startX , startY + (i * spacing) + 15 
			});


		option.label = fileName;		

		option.onClick = [thisPath]()
		{
			std::wcout << thisPath << L" ��� ���� ����" << std::endl;

				
			// ���� ���� ȭ�� �̵�
			//currentState = UIState::ProfileMain;  // �ٽ� ���� ȭ������ ��ȯ
		};
		filieOptions.push_back(option);
		fileNameDrawables.push_back(fileName);    // ������ drawables���� �߰�
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

	// ��ü �ʱ�ȭ
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");
	std::vector<std::shared_ptr<sf::Drawable>> langSelectUIDrawables;
	std::vector<std::shared_ptr<sf::Drawable>> fileNameDrawables;
	std::vector<std::wstring> typingFilePath;
	std::vector<FileOption> fileOptions;
	std::wstring selectMod = L"�ѱ�";		// �ѱ� / ���� / �ڵ�

	sf::View listView;

	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
		return 1;
	}
	typingFilePath = {
	L"assets/typing/�ѱ�1.txt",
	L"assets/typing/�ѱ�2.txt",
	L"assets/typing/english.txt",
	L"assets/typing/coding.cpp",
	L"assets/typing/�ѱ�1.txt",
	L"assets/typing/�ѱ�2.txt",
	L"assets/typing/english.txt",
	L"assets/typing/coding.cpp",
	L"assets/typing/coding.cpp",
	L"assets/typing/�ѱ�1.txt",
	
	};

	// ��ũ�� ���� ��
	float scrollOffset = 0.f;
	float maxScrollOffset = 0.f;
	float scrollStep = 30.f;

	// ���� �ҷ����� ��ư�� ��ġ ����
	sf::FloatRect loadUserFileBtnBound;

	// UI ����
	initLangSelectUI(window, font, listView, maxScrollOffset, loadUserFileBtnBound,  typingFilePath, fileOptions, selectMod, langSelectUIDrawables, fileNameDrawables, currentState);

	while (window.isOpen())
	{
		// �̺�Ʈ ����
		while (const std::optional event = window.pollEvent())
		{
			// "close requested" event: we close the window
			if (event->is<sf::Event::Closed>())
				window.close();

			// ���߿� ����..
			//// ���콺 �� �̺�Ʈ
			//if (event->is<sf::Event::MouseWheelScrolled>()) {
			//	const auto& scrollEvent = event->getIf<sf::Event::MouseWheelScrolled>();
			//	float delta = scrollEvent->delta;
			//	scrollOffset -= delta * scrollStep;

			//	// ���� ����
			//	scrollOffset = std::clamp(scrollOffset, 0.f, maxScrollOffset);

			//	// �� �̵� ����
			//	listView.move({ 0.f, delta * scrollStep });
			//}
			// ���콺 Ŭ�� �̺�Ʈ
			if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouse->button == sf::Mouse::Button::Left) {
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

					if (loadUserFileBtnBound.contains(mousePos)) {
						std::wcout << L"[�ҷ����� ��ư Ŭ����]" << std::endl;
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

		// ���콺 ��ġ ���
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		// hover ó��
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


		// ������ ����
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
