#include "UserProfileUI.hpp"
#include "UserProfileSelectUI.hpp"
#include <iostream>


UIState currentState = UIState::ProfileMain;

int main()
{
	std::setlocale(LC_ALL, "");
	// 사용자 정보 받아오는 구조체 생성 및 초기화
	UserInfo info;
	info.id = L"gimddanggu";
	info.nickname = L"김땡구";
	info.rank = L"1";
	info.maxTpm = L"450";
	info.maxWpm = L"30.4";
	info.playTime = L"35";
	info.point = L"230";

	// 메인 윈도우 생성
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");

	// 폰트 설정
	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
		return 1;
	}

	// 기본 프로필 생성
	std::wstring imgPath = L"assets/profile_img/default_avatar.png";

	sf::Image profileImage = loadImg(imgPath);
	sf::Image resizeProfileImage = resizeImageKeepAspect(profileImage);
	sf::Texture profileTexture(resizeProfileImage);
	sf::Sprite profileSprite(profileTexture);

	
	// 프로필 hover 효과용 테두리
	sf::RectangleShape thumbnailHoverOutline;
	thumbnailHoverOutline.setSize({ 200.f, 200.f }); // 썸네일 크기에 맞게 조절
	thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
	thumbnailHoverOutline.setOutlineThickness(4.f);
	thumbnailHoverOutline.setOutlineColor(sf::Color::Red);

	bool isHovering = false;

	// 메인 프로필 UI 생성
	std::vector<std::shared_ptr<sf::Drawable>> profileDrawables;
	std::vector<sf::Text> profileTexts;
	sf::FloatRect selectButtonBound;
	initProfileUI(window, font, profileSprite, info, profileDrawables, profileTexts, selectButtonBound);

	// 프로필 이미지 선택 UI 생성
	std::vector<std::shared_ptr<sf::Drawable>> selectDrawables;
	std::vector<sf::Sprite> sprites;
	std::vector<ImageOption> imageOptions;
	sf::FloatRect loadButtonBound;
	initSelectUI(window, font, selectDrawables, sprites, imageOptions, loadButtonBound, profileTexture, profileSprite, currentState);


	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			// "close requested" event: we close the window
			if (event->is<sf::Event::Closed>())
				window.close();

			if (currentState == UIState::ProfileMain) {
				// 프로필 메인 UI의 클릭, 키보드 입력 등 처리
				if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
					if (mouse->button == sf::Mouse::Button::Left) {
						sf::Vector2f mousePos = window.mapPixelToCoords(mouse->position);
						// 여기서 클릭 처리

						// 불러오기 버튼 클릭 확인
						if (selectButtonBound.contains(mousePos)) {
							std::wcout << L"[불러오기 버튼 클릭됨]" << std::endl;
							currentState = UIState::ImageSelect;
						}
					}
				}
			}
			else if (currentState == UIState::ImageSelect) {
				// 프로필 이미지 선택 화면의 이벤트 처리
				if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
					if (mouse->button == sf::Mouse::Button::Left) {
						sf::Vector2f mousePos = window.mapPixelToCoords(mouse->position);
						// 여기서 클릭 처리
						for (int i = 0; i < sprites.size(); ++i) {
							if (sprites[i].getGlobalBounds().contains(mousePos)) {
								imageOptions[i].onClick();
							}
						}

						// 불러오기 버튼 클릭 확인
						if (loadButtonBound.contains(mousePos)) {
							std::wcout << L"[불러오기 버튼 클릭됨]" << std::endl;
							auto originalPath = std::filesystem::current_path();	// 현재 폴더 경로 저장
							std::wstring selectedImagePath = openImageFileDialog();
							std::filesystem::current_path(originalPath);  // 다시 원래 경로로 되돌림 - 선택된 폴더 경로로 바뀌는 거 방지
							sf::Image image;
							if (image.loadFromFile(selectedImagePath)) {
								// 성공 처리
								std::wcout << L"[이미지를 성공적으로 불러왔습니다]" << std::endl;
								std::wcout << selectedImagePath << std::endl;

								updateProfileImage(selectedImagePath, profileTexture, profileSprite);
								currentState = UIState::ProfileMain;


							}
							else {
								std::wcout << L"[이미지를 불러오지 못했습니다]" << std::endl;
								return -1;
							}

						}
					}
				}


				else if (const auto* move = event->getIf<sf::Event::MouseMoved>()) {
					sf::Vector2f mousePos = window.mapPixelToCoords(move->position);
					isHovering = false;

					for (int i = 0; i < sprites.size(); ++i) {
						if (sprites[i].getGlobalBounds().contains(mousePos)) {
							thumbnailHoverOutline.setPosition(sprites[i].getPosition());
							thumbnailHoverOutline.setScale(sprites[i].getScale());
							isHovering = true;
							break;
						}
					}
				}
			}

		}


		// 메인 프로필 화면
		if (currentState == UIState::ProfileMain) {
			// draw profile UI
			window.clear();

			// drawables 루프 
			for (auto& d : profileDrawables)
				window.draw(*d);

			for (auto& text : profileTexts)
				window.draw(text);

			window.display();

		} // 프로필 이미지 선택화면
		else if (currentState == UIState::ImageSelect) {
			window.clear();
			for (auto& d : selectDrawables) window.draw(*d);

			for (auto& sprite : sprites) {
				window.draw(sprite);
			}

			// hover 중일 때만 테두리 그리기
			if (isHovering)
				window.draw(thumbnailHoverOutline);
			window.display();
		}
	}
	return 0;
}

