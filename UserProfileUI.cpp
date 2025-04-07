#include "UserProfileUI.hpp"
#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>



void renderProfile(sf::RenderWindow& window, GameState& game, 
	sf::Font& font, int fontSize,
	sf::Sprite& profileImg,
	std::vector<sf::Text>& profileTexts)
{
	// 기존 텍스트 제거 
	profileTexts.clear();

	 //윈도우 생성
	sf::RectangleShape profileBox_Frame = makeRectangle(window, 0.5f, 0.5f);
	profileBox_Frame.setOutlineColor(sf::Color::Black);
	profileBox_Frame.setOutlineThickness(2.f);

	 //기준 객체 (프로필 배경창)
	sf::FloatRect frameBound = profileBox_Frame.getGlobalBounds();
	profileBox_Frame.setPosition(getWindowCenterPosition(window, frameBound));			// 윈도우를 중심으로 중앙정렬
	frameBound = profileBox_Frame.getGlobalBounds();


	 //정렬을 위한 컨테이너 생성
	sf::RectangleShape profileBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Red);
	sf::RectangleShape profileBox_Image = makeRectangle(frameBound, 0.3f, 0.5f, sf::Color::Green);
	sf::RectangleShape profileBox_info = makeRectangle(frameBound, 0.5f, 0.6f, sf::Color::Yellow);



	 //제목 frame
	sf::Vector2f TitleSize = profileBox_Title.getSize();
	sf::FloatRect profileBox_Title_Bound = profileBox_Title.getGlobalBounds();
	sf::Vector2f titlePos = getCenterXPosition(TitleSize, frameBound, frameBound.position.y);	// 프로필 프래임 기준으로 중앙 정렬

	 //이미지 frame
	sf::Vector2f imgSize = profileBox_Image.getSize();
	sf::FloatRect profileBox_Image_Bound = profileBox_Image.getGlobalBounds();
	sf::Vector2f imgPos = LeftInnerAlign(frameBound, 30.f);										// 프로필 프래임 기준으로 왼쪽 정렬

	 //정보 입력 frame
	sf::Vector2f infoSize = profileBox_info.getSize();
	sf::FloatRect profileBox_info_Bound = profileBox_info.getGlobalBounds();
	sf::Vector2f infoPos = RightInnerAlign(infoSize, frameBound, 30.f);							// 프로필 프래임 기준으로 오른쪽 정렬

	 //컨테이너 위치 조정
	profileBox_Image.setPosition({ imgPos.x, imgPos.y + 90.0f });	// 왼쪽 정렬 됨
	profileBox_info.setPosition({ infoPos.x, infoPos.y + 90.0f });	// 오른쪽 정렬 됨
	profileBox_Title.setPosition(titlePos);

	 //요소 객체 생성
	sf::Text titleText(font, L"프로필", 40);
	sf::FloatRect box = profileBox_Title.getGlobalBounds();
	sf::FloatRect bounds = titleText.getGlobalBounds();
	sf::Vector2f center = box.getCenter();
	float x = center.x - bounds.size.x / 2.f;
	float y = center.y - bounds.size.y / 2.f;
	titleText.setPosition({ x, y + 10.f });
	titleText.setFillColor(sf::Color::Black);

	


	 //버튼
	sf::RectangleShape loadButton({ 200, 40 });
	loadButton.setFillColor(sf::Color::Green);


	 //버튼 텍스트
	sf::Text loadButtonText(font, L"프로필 이미지 선택", 20);

	 //프로필 위치 설정
	center = profileBox_Image.getPosition();
	profileImg.setPosition(center);

	 //프로필 이미지 수정 버튼 위치 설정
	loadButton.setPosition({ center.x, center.y + 210.f });

	loadButton.setFillColor(sf::Color::Magenta);
	
	 //프로필 이미지 수정 버튼 텍스트 위치 설정
	box = loadButton.getGlobalBounds();
	bounds = loadButtonText.getGlobalBounds();
	loadButtonText.setPosition(getCenterPosition(bounds.size, box));
	 //텍스트 위치 조정 (한글)
	bounds = loadButtonText.getGlobalBounds();
	loadButtonText.setPosition({ bounds.position.x, bounds.position.y - 6.f });
	
	 //버튼 이벤트를 위해 위치 받아옴
	bounds = loadButton.getGlobalBounds();
	game.btn.selectImgBtnBounds = bounds;

	 //사용자 정보 텍스트
	std::vector<std::pair<std::wstring, std::wstring>> profileStats = {
		{L"ID", game.user.id},
		{L"닉네임", game.user.nickname},
		{L"랭킹", std::to_wstring(game.user.ranking)},
		{L"최고 타수", to_wstring_fixed(game.user.bestTPM)},
		{L"최고 WPM", to_wstring_fixed(game.user.bestWPM)},
		{L"플레이 타임", to_wstring_fixed(game.user.totalPlayTime)},
		{L"포인트", std::to_wstring(game.user.point)}
	};



	 //이건 나중에 매개변수로 빼던지 안빼던지 결정해야될 듯
	float lineHeight = 30.f;
	float startY = profileBox_info.getGlobalBounds().position.y + 10.f;
	float labelX = profileBox_info.getGlobalBounds().position.x + 20.f;
	float gap = 10.f;

	 //라벨의 최대 너비 측정
	float maxLabelWidth = 0.f;
	for (const auto& [label, _] : profileStats) {
		sf::Text temp(font, label + L" :", 20);
		float width = temp.getGlobalBounds().size.x;
		if (width > maxLabelWidth)
			maxLabelWidth = width;
	}

	 //텍스트 실제 배치
	for (std::size_t i = 0; i < profileStats.size(); ++i) {
		const auto& [labelStr, valueStr] = profileStats[i];
		float y = startY + i * lineHeight;

		sf::Text labelText(font, labelStr, 20);
		labelText.setFillColor(sf::Color::Black);
		labelText.setPosition({ labelX, y });

		sf::Text colonText(font, L": ", 20);
		colonText.setFillColor(sf::Color::Black);
		colonText.setPosition({ labelX + maxLabelWidth + gap, y });

		sf::Text valueText(font, valueStr, 20);
		valueText.setFillColor(sf::Color::Black);
		valueText.setPosition({ labelX + maxLabelWidth + gap * 2 + colonText.getGlobalBounds().size.x, y });

		profileTexts.push_back(labelText);
		profileTexts.push_back(colonText);
		profileTexts.push_back(valueText);

	}

	for (auto& text : profileTexts)
		window.draw(text);

	window.draw(profileBox_Frame);
	window.draw(titleText);
	window.draw(profileImg);
	window.draw(loadButton);
	window.draw(loadButtonText);

	//drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Frame));
	//// 아래 두 개는 컨테이너 
	////drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Title));
	////drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Image));
	//drawables.push_back(std::make_shared<sf::Text>(titleText));
	//// drawables에 스프라이트 추가
	//drawables.push_back(std::make_shared<sf::Sprite>(profileImg));
	//drawables.push_back(std::make_shared<sf::RectangleShape>(loadButton));

	//drawables.push_back(std::make_shared<sf::Text>(loadButtonText));
	////appendProfileTexts(info, profileBox_info, font, drawables);
}

// 숫자 → wstring 변환 함수
std::wstring to_wstring_fixed(double value, int precision) {
	std::wstringstream wss;
	wss << std::fixed << std::setprecision(precision) << value;
	return wss.str();
}



//int main()
//{
//	// 윈도우 생성
//	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");
//
//	// 폰트 설정
//	sf::Font font;
//	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
//		std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
//		return 1;
//	}
//
//	// 사용자 정보 받아옴 처음 한 번만
//	UserInfo info;
//	info.id = L"gimddanggu";
//	info.nickname = L"김땡구";
//	info.rank = L"1";
//	info.maxTpm = L"450";
//	info.maxWpm = L"30.4";
//	info.playTime = L"35";
//	info.point = L"230";
//
//
//	// 기본 프로필 설정
//	std::wstring imgPath = L"assets/profile_img/default_avatar.png";
//	sf::Image profile = loadImg(imgPath);
//	sf::Image resizeProfileImage = resizeImageKeepAspect(profile, {200, 200});
//	sf::Texture texture(resizeProfileImage);
//	sf::Sprite imgSprite(texture);
//
//	std::vector<std::shared_ptr<sf::Drawable>> drawables;
//	std::vector<sf::Text> profileTexts;
//	sf::FloatRect loadButtonBound;
//	initProfileUI(window, font, imgSprite, info, drawables, profileTexts, loadButtonBound);
//
//	while (window.isOpen())
//	{
//		while (const std::optional event = window.pollEvent())
//		{
//			// "close requested" event: we close the window
//			if (event->is<sf::Event::Closed>())
//				window.close();
//		}
//
//		window.clear();
//
//		// drawables 루프 돌면서 draw
//		for (auto& d : drawables)
//			window.draw(*d);
//
//		for (auto& text : profileTexts)
//			window.draw(text);
//
//		window.display();
//	}
//
//	return 0;
//}
