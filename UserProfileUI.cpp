#include <SFML/Graphics.hpp>
#include <iostream>

struct UserInfo
{
	std::wstring id;
	std::wstring nickname;
	std::wstring rank;
	std::wstring maxTpm;
	std::wstring maxWpm;
	std::wstring playTime;
	std::wstring point;

};

// 결과창을 윈도우 비율에 맞게 만들기 위한 함수
sf::RectangleShape makeRectangle(sf::RenderWindow& window, float widthRatio, float heightRatio)
{
	// 현재 창 크기
	sf::Vector2u winSize = window.getSize();

	// 사각형 사이즈 조절
	sf::Vector2f rectSize(winSize.x * widthRatio, winSize.y * heightRatio);
	sf::RectangleShape rect(rectSize);
	rect.setFillColor(sf::Color::White);
	return rect;

}

// 컨테이너 생성용 함수 
sf::RectangleShape makeRectangle(sf::FloatRect& frameBounds, float widthRatio, float heightRatio, sf::Color color)
{
	// 현재 창 크기
	sf::Vector2f frameSize = frameBounds.size;

	// 사각형 사이즈 조절
	sf::Vector2f rectSize(frameSize.x * widthRatio, frameSize.y * heightRatio);
	sf::RectangleShape rect(rectSize);
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineThickness(1.f);
	rect.setOutlineColor(color);

	return rect;

}

// 가운데 정렬 - 윈도우 기준
sf::Vector2f getWindowCenterPosition(const sf::RenderWindow& window, const sf::FloatRect& targetBounds)
{
	float centerX = window.getSize().x / 2.f;
	float centerY = window.getSize().y / 2.f;

	float x = centerX - targetBounds.size.x / 2.f;
	float y = centerY - targetBounds.size.y / 2.f;

	return { x, y };
}

// XY 중앙정렬 (버튼에 텍스트)
sf::Vector2f getCenterPosition(const sf::Vector2f& targetSize, const sf::FloatRect& container)
{
	float x = container.position.x + (container.size.x - targetSize.x) / 2.f;
	float y = container.position.y + (container.size.y - targetSize.y) / 2.f;
	return { x, y };
}

// 어떤 객체 기준 x 축 가운데 정렬
sf::Vector2f getCenterXPosition(sf::Vector2f targetSize,
	const sf::FloatRect& refBounds,
	float y)
{
	float x = refBounds.position.x + (refBounds.size.x - targetSize.x) / 2.f;
	return { x, y };
}

// 왼쪽 정렬
sf::Vector2f LeftInnerAlign(sf::FloatRect& refBounds, float x_margin = 0.f)
{
	return { refBounds.position.x + x_margin, refBounds.position.y };
}

// 오른쪽 정렬
sf::Vector2f RightInnerAlign(sf::Vector2f targetSize, sf::FloatRect& refBounds, float x_margin = 0.f)
{
	return { refBounds.position.x + refBounds.size.x - targetSize.x - x_margin, refBounds.position.y };
}

// 이미지 로드
sf::Image loadImg(std::wstring path)
{
	sf::Image image;
	if (!image.loadFromFile(path)) {
		std::wcerr << L"이미지 로드 실패!" << std::endl;

		std::exit(EXIT_FAILURE);
	}
	return image;
}

// 이미지 리사이즈
sf::Image resizeImageKeepAspect(const sf::Image& src, sf::Vector2u targetSize, sf::Color paddingColor = sf::Color::Transparent)
{
	sf::Vector2u srcSize = src.getSize();
	float srcAspect = static_cast<float>(srcSize.x) / srcSize.y;
	float targetAspect = static_cast<float>(targetSize.x) / targetSize.y;

	// 새로운 실제 이미지 크기 계산
	unsigned int newWidth, newHeight;
	if (srcAspect > targetAspect) {
		newWidth = targetSize.x;
		newHeight = static_cast<unsigned int>(targetSize.x / srcAspect);
	}
	else {
		newHeight = targetSize.y;
		newWidth = static_cast<unsigned int>(targetSize.y * srcAspect);
	}

	// 리사이즈된 픽셀 배열 만들기 (nearest neighbor)
	std::vector<std::uint8_t> pixels(targetSize.x * targetSize.y * 4, 0);

	for (unsigned int y = 0; y < targetSize.y; ++y) {
		for (unsigned int x = 0; x < targetSize.x; ++x) {
			// 패딩 영역이면 배경색
			if (x < (targetSize.x - newWidth) / 2 ||
				x >= (targetSize.x + newWidth) / 2 ||
				y < (targetSize.y - newHeight) / 2 ||
				y >= (targetSize.y + newHeight) / 2) {
				std::size_t i = (y * targetSize.x + x) * 4;
				pixels[i + 0] = paddingColor.r;
				pixels[i + 1] = paddingColor.g;
				pixels[i + 2] = paddingColor.b;
				pixels[i + 3] = paddingColor.a;
			}
			else {
				// 원본 이미지 좌표 계산
				unsigned int srcX = (x - (targetSize.x - newWidth) / 2) * srcSize.x / newWidth;
				unsigned int srcY = (y - (targetSize.y - newHeight) / 2) * srcSize.y / newHeight;
				sf::Color color = src.getPixel({ srcX, srcY });
				std::size_t i = (y * targetSize.x + x) * 4;
				pixels[i + 0] = color.r;
				pixels[i + 1] = color.g;
				pixels[i + 2] = color.b;
				pixels[i + 3] = color.a;
			}
		}
	}

	sf::Image result;
	result.resize(targetSize, pixels.data());
	return result;
}



void initUI(sf::RenderWindow& window, sf::Font& font, sf::Sprite& profileImg, UserInfo info,
	std::vector<std::shared_ptr<sf::Drawable>>& drawables, std::vector<sf::Text>& profileTexts)
{

	// 윈도우 생성
	sf::RectangleShape profileBox_Frame = makeRectangle(window, 0.5f, 0.5f);

	// 기준 객체 (프로필 배경창)
	sf::FloatRect frameBound = profileBox_Frame.getGlobalBounds();
	profileBox_Frame.setPosition(getWindowCenterPosition(window, frameBound));			// 윈도우를 중심으로 중앙정렬
	frameBound = profileBox_Frame.getGlobalBounds();


	// 정렬을 위한 컨테이너 생성
	sf::RectangleShape profileBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Red);
	sf::RectangleShape profileBox_Image = makeRectangle(frameBound, 0.3f, 0.5f, sf::Color::Green);
	sf::RectangleShape profileBox_info = makeRectangle(frameBound, 0.5f, 0.6f, sf::Color::Yellow);



	// 제목 frame
	sf::Vector2f TitleSize = profileBox_Title.getSize();
	sf::FloatRect profileBox_Title_Bound = profileBox_Title.getGlobalBounds();
	sf::Vector2f titlePos = getCenterXPosition(TitleSize, frameBound, frameBound.position.y);	// 프로필 프래임 기준으로 중앙 정렬

	// 이미지 frame
	sf::Vector2f imgSize = profileBox_Image.getSize();
	sf::FloatRect profileBox_Image_Bound = profileBox_Image.getGlobalBounds();
	sf::Vector2f imgPos = LeftInnerAlign(frameBound, 30.f);										// 프로필 프래임 기준으로 왼쪽 정렬

	// 정보 입력 frame
	sf::Vector2f infoSize = profileBox_info.getSize();
	sf::FloatRect profileBox_info_Bound = profileBox_info.getGlobalBounds();
	sf::Vector2f infoPos = RightInnerAlign(infoSize, frameBound, 30.f);							// 프로필 프래임 기준으로 오른쪽 정렬

	// 컨테이너 위치 조정
	profileBox_Image.setPosition({ imgPos.x, imgPos.y + 90.0f });	// 왼쪽 정렬 됨
	profileBox_info.setPosition({ infoPos.x, infoPos.y + 90.0f });	// 오른쪽 정렬 됨
	profileBox_Title.setPosition(titlePos);

	// 요소 객체 생성
	sf::Text titleText(font, L"프로필", 40);
	sf::FloatRect box = profileBox_Title.getGlobalBounds();
	sf::FloatRect bounds = titleText.getGlobalBounds();
	sf::Vector2f center = box.getCenter();
	float x = center.x - bounds.size.x / 2.f;
	float y = center.y - bounds.size.y / 2.f;
	titleText.setPosition({ x, y });
	titleText.setFillColor(sf::Color::Black);

	


	// 버튼
	sf::RectangleShape loadButton({ 200, 40 });
	loadButton.setFillColor(sf::Color::Green);


	// 버튼 텍스트
	sf::Text loadButtonText(font, L"프로필 이미지 선택", 18);

	// 프로필 위치 설정
	center = profileBox_Image.getPosition();
	profileImg.setPosition(center);

	// 프로필 이미지 수정 버튼 위치 설정
	loadButton.setPosition({ center.x, center.y + 210.f });

	//loadButton.setFillColor(sf::Color::Magenta);
	
	// 프로필 이미지 수정 버튼 텍스트 위치 설정
	box = loadButton.getGlobalBounds();
	bounds = loadButtonText.getGlobalBounds();
	loadButtonText.setPosition(getCenterPosition(bounds.size, box));

	// 사용자 정보 텍스트
	std::vector<std::pair<std::wstring, std::wstring>> profileStats = {
	{L"ID", info.id},
	{L"닉네임", info.nickname},
	{L"랭킹", info.rank},
	{L"최고 타수", info.maxTpm},
	{L"최고 WPM", info.maxTpm},
	{L"플레이 타임", info.playTime},
	{L"포인트", info.point}
	};


	// 이건 나중에 매개변수로 빼던지 안빼던지 결정해야될 듯
	float lineHeight = 30.f;
	float startY = profileBox_info.getGlobalBounds().position.y + 10.f;
	float labelX = profileBox_info.getGlobalBounds().position.x + 20.f;
	float gap = 10.f;

	// 라벨의 최대 너비 측정
	float maxLabelWidth = 0.f;
	for (const auto& [label, _] : profileStats) {
		sf::Text temp(font, label + L" :", 20);
		float width = temp.getGlobalBounds().size.x;
		if (width > maxLabelWidth)
			maxLabelWidth = width;
	}

	// 텍스트 실제 배치
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


	// drawables 벡터에 요소 추가
	drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Frame));
	// 아래 두 개는 컨테이너 
	//drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Title));
	//drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Image));
	drawables.push_back(std::make_shared<sf::Text>(titleText));
	// drawables에 스프라이트 추가
	drawables.push_back(std::make_shared<sf::Sprite>(profileImg));
	drawables.push_back(std::make_shared<sf::RectangleShape>(loadButton));

	drawables.push_back(std::make_shared<sf::Text>(loadButtonText));
	//appendProfileTexts(info, profileBox_info, font, drawables);
}





int main()
{
	// 윈도우 생성
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");

	// 폰트 설정
	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
		return 1;
	}

	// 사용자 정보 받아옴 처음 한 번만
	UserInfo info;
	info.id = L"gimddanggu";
	info.nickname = L"김땡구";
	info.rank = L"1";
	info.maxTpm = L"450";
	info.maxWpm = L"30.4";
	info.playTime = L"35";
	info.point = L"230";

	// 이것도 init에 넣고 싶었는데 잘 안됨..
	std::vector<sf::Text> labelTexts;
	std::vector<sf::Text> colonTexts;
	std::vector<sf::Text> valueTexts;

	// 기본 프로필 설정
	std::wstring imgPath = L"assets/profile_img/default_avatar.png";
	sf::Image profile = loadImg(imgPath);
	sf::Image resizeProfileImage = resizeImageKeepAspect(profile, {200, 200});
	sf::Texture texture(resizeProfileImage);
	sf::Sprite imgSprite(texture);

	std::vector<std::shared_ptr<sf::Drawable>> drawables;
	std::vector<sf::Text> profileTexts;

	initContainer(window, font, imgSprite, info, drawables, profileTexts);

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			// "close requested" event: we close the window
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		window.clear();

		// drawables 루프 돌면서 draw
		for (auto& d : drawables)
			window.draw(*d);

		for (auto& text : profileTexts)
			window.draw(text);

		window.display();
	}

	return 0;
}
