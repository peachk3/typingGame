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

// ���â�� ������ ������ �°� ����� ���� �Լ�
sf::RectangleShape makeRectangle(sf::RenderWindow& window, float widthRatio, float heightRatio)
{
	// ���� â ũ��
	sf::Vector2u winSize = window.getSize();

	// �簢�� ������ ����
	sf::Vector2f rectSize(winSize.x * widthRatio, winSize.y * heightRatio);
	sf::RectangleShape rect(rectSize);
	rect.setFillColor(sf::Color::White);
	return rect;

}

// �����̳� ������ �Լ� 
sf::RectangleShape makeRectangle(sf::FloatRect& frameBounds, float widthRatio, float heightRatio, sf::Color color)
{
	// ���� â ũ��
	sf::Vector2f frameSize = frameBounds.size;

	// �簢�� ������ ����
	sf::Vector2f rectSize(frameSize.x * widthRatio, frameSize.y * heightRatio);
	sf::RectangleShape rect(rectSize);
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineThickness(1.f);
	rect.setOutlineColor(color);

	return rect;

}

// ��� ���� - ������ ����
sf::Vector2f getWindowCenterPosition(const sf::RenderWindow& window, const sf::FloatRect& targetBounds)
{
	float centerX = window.getSize().x / 2.f;
	float centerY = window.getSize().y / 2.f;

	float x = centerX - targetBounds.size.x / 2.f;
	float y = centerY - targetBounds.size.y / 2.f;

	return { x, y };
}

// XY �߾����� (��ư�� �ؽ�Ʈ)
sf::Vector2f getCenterPosition(const sf::Vector2f& targetSize, const sf::FloatRect& container)
{
	float x = container.position.x + (container.size.x - targetSize.x) / 2.f;
	float y = container.position.y + (container.size.y - targetSize.y) / 2.f;
	return { x, y };
}

// � ��ü ���� x �� ��� ����
sf::Vector2f getCenterXPosition(sf::Vector2f targetSize,
	const sf::FloatRect& refBounds,
	float y)
{
	float x = refBounds.position.x + (refBounds.size.x - targetSize.x) / 2.f;
	return { x, y };
}

// ���� ����
sf::Vector2f LeftInnerAlign(sf::FloatRect& refBounds, float x_margin = 0.f)
{
	return { refBounds.position.x + x_margin, refBounds.position.y };
}

// ������ ����
sf::Vector2f RightInnerAlign(sf::Vector2f targetSize, sf::FloatRect& refBounds, float x_margin = 0.f)
{
	return { refBounds.position.x + refBounds.size.x - targetSize.x - x_margin, refBounds.position.y };
}

// �̹��� �ε�
sf::Image loadImg(std::wstring path)
{
	sf::Image image;
	if (!image.loadFromFile(path)) {
		std::wcerr << L"�̹��� �ε� ����!" << std::endl;

		std::exit(EXIT_FAILURE);
	}
	return image;
}

// �̹��� ��������
sf::Image resizeImageKeepAspect(const sf::Image& src, sf::Vector2u targetSize, sf::Color paddingColor = sf::Color::Transparent)
{
	sf::Vector2u srcSize = src.getSize();
	float srcAspect = static_cast<float>(srcSize.x) / srcSize.y;
	float targetAspect = static_cast<float>(targetSize.x) / targetSize.y;

	// ���ο� ���� �̹��� ũ�� ���
	unsigned int newWidth, newHeight;
	if (srcAspect > targetAspect) {
		newWidth = targetSize.x;
		newHeight = static_cast<unsigned int>(targetSize.x / srcAspect);
	}
	else {
		newHeight = targetSize.y;
		newWidth = static_cast<unsigned int>(targetSize.y * srcAspect);
	}

	// ��������� �ȼ� �迭 ����� (nearest neighbor)
	std::vector<std::uint8_t> pixels(targetSize.x * targetSize.y * 4, 0);

	for (unsigned int y = 0; y < targetSize.y; ++y) {
		for (unsigned int x = 0; x < targetSize.x; ++x) {
			// �е� �����̸� ����
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
				// ���� �̹��� ��ǥ ���
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

	// ������ ����
	sf::RectangleShape profileBox_Frame = makeRectangle(window, 0.5f, 0.5f);

	// ���� ��ü (������ ���â)
	sf::FloatRect frameBound = profileBox_Frame.getGlobalBounds();
	profileBox_Frame.setPosition(getWindowCenterPosition(window, frameBound));			// �����츦 �߽����� �߾�����
	frameBound = profileBox_Frame.getGlobalBounds();


	// ������ ���� �����̳� ����
	sf::RectangleShape profileBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Red);
	sf::RectangleShape profileBox_Image = makeRectangle(frameBound, 0.3f, 0.5f, sf::Color::Green);
	sf::RectangleShape profileBox_info = makeRectangle(frameBound, 0.5f, 0.6f, sf::Color::Yellow);



	// ���� frame
	sf::Vector2f TitleSize = profileBox_Title.getSize();
	sf::FloatRect profileBox_Title_Bound = profileBox_Title.getGlobalBounds();
	sf::Vector2f titlePos = getCenterXPosition(TitleSize, frameBound, frameBound.position.y);	// ������ ������ �������� �߾� ����

	// �̹��� frame
	sf::Vector2f imgSize = profileBox_Image.getSize();
	sf::FloatRect profileBox_Image_Bound = profileBox_Image.getGlobalBounds();
	sf::Vector2f imgPos = LeftInnerAlign(frameBound, 30.f);										// ������ ������ �������� ���� ����

	// ���� �Է� frame
	sf::Vector2f infoSize = profileBox_info.getSize();
	sf::FloatRect profileBox_info_Bound = profileBox_info.getGlobalBounds();
	sf::Vector2f infoPos = RightInnerAlign(infoSize, frameBound, 30.f);							// ������ ������ �������� ������ ����

	// �����̳� ��ġ ����
	profileBox_Image.setPosition({ imgPos.x, imgPos.y + 90.0f });	// ���� ���� ��
	profileBox_info.setPosition({ infoPos.x, infoPos.y + 90.0f });	// ������ ���� ��
	profileBox_Title.setPosition(titlePos);

	// ��� ��ü ����
	sf::Text titleText(font, L"������", 40);
	sf::FloatRect box = profileBox_Title.getGlobalBounds();
	sf::FloatRect bounds = titleText.getGlobalBounds();
	sf::Vector2f center = box.getCenter();
	float x = center.x - bounds.size.x / 2.f;
	float y = center.y - bounds.size.y / 2.f;
	titleText.setPosition({ x, y });
	titleText.setFillColor(sf::Color::Black);

	


	// ��ư
	sf::RectangleShape loadButton({ 200, 40 });
	loadButton.setFillColor(sf::Color::Green);


	// ��ư �ؽ�Ʈ
	sf::Text loadButtonText(font, L"������ �̹��� ����", 18);

	// ������ ��ġ ����
	center = profileBox_Image.getPosition();
	profileImg.setPosition(center);

	// ������ �̹��� ���� ��ư ��ġ ����
	loadButton.setPosition({ center.x, center.y + 210.f });

	//loadButton.setFillColor(sf::Color::Magenta);
	
	// ������ �̹��� ���� ��ư �ؽ�Ʈ ��ġ ����
	box = loadButton.getGlobalBounds();
	bounds = loadButtonText.getGlobalBounds();
	loadButtonText.setPosition(getCenterPosition(bounds.size, box));

	// ����� ���� �ؽ�Ʈ
	std::vector<std::pair<std::wstring, std::wstring>> profileStats = {
	{L"ID", info.id},
	{L"�г���", info.nickname},
	{L"��ŷ", info.rank},
	{L"�ְ� Ÿ��", info.maxTpm},
	{L"�ְ� WPM", info.maxTpm},
	{L"�÷��� Ÿ��", info.playTime},
	{L"����Ʈ", info.point}
	};


	// �̰� ���߿� �Ű������� ������ �Ȼ����� �����ؾߵ� ��
	float lineHeight = 30.f;
	float startY = profileBox_info.getGlobalBounds().position.y + 10.f;
	float labelX = profileBox_info.getGlobalBounds().position.x + 20.f;
	float gap = 10.f;

	// ���� �ִ� �ʺ� ����
	float maxLabelWidth = 0.f;
	for (const auto& [label, _] : profileStats) {
		sf::Text temp(font, label + L" :", 20);
		float width = temp.getGlobalBounds().size.x;
		if (width > maxLabelWidth)
			maxLabelWidth = width;
	}

	// �ؽ�Ʈ ���� ��ġ
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


	// drawables ���Ϳ� ��� �߰�
	drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Frame));
	// �Ʒ� �� ���� �����̳� 
	//drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Title));
	//drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Image));
	drawables.push_back(std::make_shared<sf::Text>(titleText));
	// drawables�� ��������Ʈ �߰�
	drawables.push_back(std::make_shared<sf::Sprite>(profileImg));
	drawables.push_back(std::make_shared<sf::RectangleShape>(loadButton));

	drawables.push_back(std::make_shared<sf::Text>(loadButtonText));
	//appendProfileTexts(info, profileBox_info, font, drawables);
}





int main()
{
	// ������ ����
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");

	// ��Ʈ ����
	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
		return 1;
	}

	// ����� ���� �޾ƿ� ó�� �� ����
	UserInfo info;
	info.id = L"gimddanggu";
	info.nickname = L"�趯��";
	info.rank = L"1";
	info.maxTpm = L"450";
	info.maxWpm = L"30.4";
	info.playTime = L"35";
	info.point = L"230";

	// �̰͵� init�� �ְ� �;��µ� �� �ȵ�..
	std::vector<sf::Text> labelTexts;
	std::vector<sf::Text> colonTexts;
	std::vector<sf::Text> valueTexts;

	// �⺻ ������ ����
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

		// drawables ���� ���鼭 draw
		for (auto& d : drawables)
			window.draw(*d);

		for (auto& text : profileTexts)
			window.draw(text);

		window.display();
	}

	return 0;
}
