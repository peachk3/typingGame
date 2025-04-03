#include "UserProfileUI.hpp"
#include "UserProfileSelectUI.hpp"
#include <iostream>


UIState currentState = UIState::ProfileMain;

int main()
{
	std::setlocale(LC_ALL, "");
	// ����� ���� �޾ƿ��� ����ü ���� �� �ʱ�ȭ
	UserInfo info;
	info.id = L"gimddanggu";
	info.nickname = L"�趯��";
	info.rank = L"1";
	info.maxTpm = L"450";
	info.maxWpm = L"30.4";
	info.playTime = L"35";
	info.point = L"230";

	// ���� ������ ����
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");

	// ��Ʈ ����
	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
		return 1;
	}

	// �⺻ ������ ����
	std::wstring imgPath = L"assets/profile_img/default_avatar.png";

	sf::Image profileImage = loadImg(imgPath);
	sf::Image resizeProfileImage = resizeImageKeepAspect(profileImage);
	sf::Texture profileTexture(resizeProfileImage);
	sf::Sprite profileSprite(profileTexture);

	
	// ������ hover ȿ���� �׵θ�
	sf::RectangleShape thumbnailHoverOutline;
	thumbnailHoverOutline.setSize({ 200.f, 200.f }); // ����� ũ�⿡ �°� ����
	thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
	thumbnailHoverOutline.setOutlineThickness(4.f);
	thumbnailHoverOutline.setOutlineColor(sf::Color::Red);

	bool isHovering = false;

	// ���� ������ UI ����
	std::vector<std::shared_ptr<sf::Drawable>> profileDrawables;
	std::vector<sf::Text> profileTexts;
	sf::FloatRect selectButtonBound;
	initProfileUI(window, font, profileSprite, info, profileDrawables, profileTexts, selectButtonBound);

	// ������ �̹��� ���� UI ����
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
				// ������ ���� UI�� Ŭ��, Ű���� �Է� �� ó��
				if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
					if (mouse->button == sf::Mouse::Button::Left) {
						sf::Vector2f mousePos = window.mapPixelToCoords(mouse->position);
						// ���⼭ Ŭ�� ó��

						// �ҷ����� ��ư Ŭ�� Ȯ��
						if (selectButtonBound.contains(mousePos)) {
							std::wcout << L"[�ҷ����� ��ư Ŭ����]" << std::endl;
							currentState = UIState::ImageSelect;
						}
					}
				}
			}
			else if (currentState == UIState::ImageSelect) {
				// ������ �̹��� ���� ȭ���� �̺�Ʈ ó��
				if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
					if (mouse->button == sf::Mouse::Button::Left) {
						sf::Vector2f mousePos = window.mapPixelToCoords(mouse->position);
						// ���⼭ Ŭ�� ó��
						for (int i = 0; i < sprites.size(); ++i) {
							if (sprites[i].getGlobalBounds().contains(mousePos)) {
								imageOptions[i].onClick();
							}
						}

						// �ҷ����� ��ư Ŭ�� Ȯ��
						if (loadButtonBound.contains(mousePos)) {
							std::wcout << L"[�ҷ����� ��ư Ŭ����]" << std::endl;
							auto originalPath = std::filesystem::current_path();	// ���� ���� ��� ����
							std::wstring selectedImagePath = openImageFileDialog();
							std::filesystem::current_path(originalPath);  // �ٽ� ���� ��η� �ǵ��� - ���õ� ���� ��η� �ٲ�� �� ����
							sf::Image image;
							if (image.loadFromFile(selectedImagePath)) {
								// ���� ó��
								std::wcout << L"[�̹����� ���������� �ҷ��Խ��ϴ�]" << std::endl;
								std::wcout << selectedImagePath << std::endl;

								updateProfileImage(selectedImagePath, profileTexture, profileSprite);
								currentState = UIState::ProfileMain;


							}
							else {
								std::wcout << L"[�̹����� �ҷ����� ���߽��ϴ�]" << std::endl;
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


		// ���� ������ ȭ��
		if (currentState == UIState::ProfileMain) {
			// draw profile UI
			window.clear();

			// drawables ���� 
			for (auto& d : profileDrawables)
				window.draw(*d);

			for (auto& text : profileTexts)
				window.draw(text);

			window.display();

		} // ������ �̹��� ����ȭ��
		else if (currentState == UIState::ImageSelect) {
			window.clear();
			for (auto& d : selectDrawables) window.draw(*d);

			for (auto& sprite : sprites) {
				window.draw(sprite);
			}

			// hover ���� ���� �׵θ� �׸���
			if (isHovering)
				window.draw(thumbnailHoverOutline);
			window.display();
		}
	}
	return 0;
}

