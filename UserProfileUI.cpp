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
	// ���� �ؽ�Ʈ ���� 
	profileTexts.clear();

	 //������ ����
	sf::RectangleShape profileBox_Frame = makeRectangle(window, 0.5f, 0.5f);
	profileBox_Frame.setOutlineColor(sf::Color::Black);
	profileBox_Frame.setOutlineThickness(2.f);

	 //���� ��ü (������ ���â)
	sf::FloatRect frameBound = profileBox_Frame.getGlobalBounds();
	profileBox_Frame.setPosition(getWindowCenterPosition(window, frameBound));			// �����츦 �߽����� �߾�����
	frameBound = profileBox_Frame.getGlobalBounds();


	 //������ ���� �����̳� ����
	sf::RectangleShape profileBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Red);
	sf::RectangleShape profileBox_Image = makeRectangle(frameBound, 0.3f, 0.5f, sf::Color::Green);
	sf::RectangleShape profileBox_info = makeRectangle(frameBound, 0.5f, 0.6f, sf::Color::Yellow);



	 //���� frame
	sf::Vector2f TitleSize = profileBox_Title.getSize();
	sf::FloatRect profileBox_Title_Bound = profileBox_Title.getGlobalBounds();
	sf::Vector2f titlePos = getCenterXPosition(TitleSize, frameBound, frameBound.position.y);	// ������ ������ �������� �߾� ����

	 //�̹��� frame
	sf::Vector2f imgSize = profileBox_Image.getSize();
	sf::FloatRect profileBox_Image_Bound = profileBox_Image.getGlobalBounds();
	sf::Vector2f imgPos = LeftInnerAlign(frameBound, 30.f);										// ������ ������ �������� ���� ����

	 //���� �Է� frame
	sf::Vector2f infoSize = profileBox_info.getSize();
	sf::FloatRect profileBox_info_Bound = profileBox_info.getGlobalBounds();
	sf::Vector2f infoPos = RightInnerAlign(infoSize, frameBound, 30.f);							// ������ ������ �������� ������ ����

	 //�����̳� ��ġ ����
	profileBox_Image.setPosition({ imgPos.x, imgPos.y + 90.0f });	// ���� ���� ��
	profileBox_info.setPosition({ infoPos.x, infoPos.y + 90.0f });	// ������ ���� ��
	profileBox_Title.setPosition(titlePos);

	 //��� ��ü ����
	sf::Text titleText(font, L"������", 40);
	sf::FloatRect box = profileBox_Title.getGlobalBounds();
	sf::FloatRect bounds = titleText.getGlobalBounds();
	sf::Vector2f center = box.getCenter();
	float x = center.x - bounds.size.x / 2.f;
	float y = center.y - bounds.size.y / 2.f;
	titleText.setPosition({ x, y + 10.f });
	titleText.setFillColor(sf::Color::Black);

	


	 //��ư
	sf::RectangleShape loadButton({ 200, 40 });
	loadButton.setFillColor(sf::Color::Green);


	 //��ư �ؽ�Ʈ
	sf::Text loadButtonText(font, L"������ �̹��� ����", 20);

	 //������ ��ġ ����
	center = profileBox_Image.getPosition();
	profileImg.setPosition(center);

	 //������ �̹��� ���� ��ư ��ġ ����
	loadButton.setPosition({ center.x, center.y + 210.f });

	loadButton.setFillColor(sf::Color::Magenta);
	
	 //������ �̹��� ���� ��ư �ؽ�Ʈ ��ġ ����
	box = loadButton.getGlobalBounds();
	bounds = loadButtonText.getGlobalBounds();
	loadButtonText.setPosition(getCenterPosition(bounds.size, box));
	 //�ؽ�Ʈ ��ġ ���� (�ѱ�)
	bounds = loadButtonText.getGlobalBounds();
	loadButtonText.setPosition({ bounds.position.x, bounds.position.y - 6.f });
	
	 //��ư �̺�Ʈ�� ���� ��ġ �޾ƿ�
	bounds = loadButton.getGlobalBounds();
	game.btn.selectImgBtnBounds = bounds;

	 //����� ���� �ؽ�Ʈ
	std::vector<std::pair<std::wstring, std::wstring>> profileStats = {
		{L"ID", game.user.id},
		{L"�г���", game.user.nickname},
		{L"��ŷ", std::to_wstring(game.user.ranking)},
		{L"�ְ� Ÿ��", to_wstring_fixed(game.user.bestTPM)},
		{L"�ְ� WPM", to_wstring_fixed(game.user.bestWPM)},
		{L"�÷��� Ÿ��", to_wstring_fixed(game.user.totalPlayTime)},
		{L"����Ʈ", std::to_wstring(game.user.point)}
	};



	 //�̰� ���߿� �Ű������� ������ �Ȼ����� �����ؾߵ� ��
	float lineHeight = 30.f;
	float startY = profileBox_info.getGlobalBounds().position.y + 10.f;
	float labelX = profileBox_info.getGlobalBounds().position.x + 20.f;
	float gap = 10.f;

	 //���� �ִ� �ʺ� ����
	float maxLabelWidth = 0.f;
	for (const auto& [label, _] : profileStats) {
		sf::Text temp(font, label + L" :", 20);
		float width = temp.getGlobalBounds().size.x;
		if (width > maxLabelWidth)
			maxLabelWidth = width;
	}

	 //�ؽ�Ʈ ���� ��ġ
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
	//// �Ʒ� �� ���� �����̳� 
	////drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Title));
	////drawables.push_back(std::make_shared<sf::RectangleShape>(profileBox_Image));
	//drawables.push_back(std::make_shared<sf::Text>(titleText));
	//// drawables�� ��������Ʈ �߰�
	//drawables.push_back(std::make_shared<sf::Sprite>(profileImg));
	//drawables.push_back(std::make_shared<sf::RectangleShape>(loadButton));

	//drawables.push_back(std::make_shared<sf::Text>(loadButtonText));
	////appendProfileTexts(info, profileBox_info, font, drawables);
}

// ���� �� wstring ��ȯ �Լ�
std::wstring to_wstring_fixed(double value, int precision) {
	std::wstringstream wss;
	wss << std::fixed << std::setprecision(precision) << value;
	return wss.str();
}



//int main()
//{
//	// ������ ����
//	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");
//
//	// ��Ʈ ����
//	sf::Font font;
//	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
//		std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
//		return 1;
//	}
//
//	// ����� ���� �޾ƿ� ó�� �� ����
//	UserInfo info;
//	info.id = L"gimddanggu";
//	info.nickname = L"�趯��";
//	info.rank = L"1";
//	info.maxTpm = L"450";
//	info.maxWpm = L"30.4";
//	info.playTime = L"35";
//	info.point = L"230";
//
//
//	// �⺻ ������ ����
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
//		// drawables ���� ���鼭 draw
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
