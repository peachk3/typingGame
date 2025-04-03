/* (2025.04.03) 
 * ������ -> ������ �̹��� ���� ��ư ������ ������ ����â ���� 
 * �⺻ �̹��� 4�� ���� �����ϵ���
 * �ҷ����� ��ư Ŭ���� ���� Ž���� �����鼭 �̹��� ���� ������ �� ���� (png, jpg,				bmp, png, tga, jpg, gif, psd, hdr, pic 
 * �⺻ �̹��� �������� �� hoverȿ�� */


#include "UserProfileUI.hpp"
#include <windows.h>
#include <iostream>
#include <functional>

struct ImageOption {
	sf::Image image;
	sf::Texture texture;
	std::function<void()> onClick;
};

//sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");

// ���� ���� ��ȭ���� ���� �� ���õ� ��θ� ��ȯ�ϴ� �Լ�
std::wstring openImageFileDialog()
{
	wchar_t filePath[MAX_PATH] = L"";

	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = L"�̹��� ���� (PNG, JPG, JPEG)\0*.png;*.jpg;*.jpeg\0GIF ����\0*.gif\0\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	ofn.lpstrTitle = L"�̹��� ���� ����";

	if (GetOpenFileNameW(&ofn))
	{
		return std::wstring(filePath);
	}
	else
	{
		return L"";  // ����ڰ� ����߰ų� ���� �߻�
	}
}

void initSelectUI(sf::RenderWindow& window,
	sf::Font& font,
	std::vector<std::shared_ptr<sf::Drawable>>& drawables,
	std::vector<sf::Sprite>& sprites,
	std::vector<ImageOption>& imageOptions,
	sf::FloatRect& loadButtonBounds)
{
	sf::RectangleShape selectFrame = makeRectangle(window, 0.5f, 0.5f);
	// ���� â ��ġ ��� ����
	sf::FloatRect frameBound = selectFrame.getGlobalBounds();
	selectFrame.setPosition(getWindowCenterPosition(window, frameBound));
	// ���� �� ��ġ ����
	frameBound = selectFrame.getGlobalBounds();
	

	// ���� �����̳�
	sf::RectangleShape selectBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Red);

	

	// �̹��� ���� Ÿ��Ʋ �ؽ�Ʈ
	sf::Text selectTitle(font, L"������ �̹��� ����");
	sf::Vector2f TitleSize = selectBox_Title.getSize();
	//sf::FloatRect profileBoxTitleBound = selectBox_Title.getGlobalBounds();
	sf::Vector2f titlePos = getCenterXPosition(TitleSize, frameBound, frameBound.position.y);	// ������ ������ �������� �߾� ����
	selectTitle.setPosition({ titlePos.x, titlePos.y + 25.f });
	selectTitle.setFillColor(sf::Color::Black);

	// �̹��� ���� ��ư
	sf::RectangleShape selectFileLoadBtn({ 100, 30 });
	sf::FloatRect FileLoadBtnBound = selectFileLoadBtn.getGlobalBounds();
	selectFileLoadBtn.setFillColor(sf::Color::Red);
	sf::Vector2f btnPos = frameBound.getCenter();
	btnPos = { btnPos.x - (FileLoadBtnBound.size.x / 2), btnPos.y }; // frame �߾ӿ� ����
	selectFileLoadBtn.setPosition({ btnPos.x, btnPos.y + 100 });

	// ��ư �ؽ�Ʈ
	// ��ġ �ٽ� ������
	sf::Text fileLoadButtonText(font, L"�ҷ�����", 18);
	FileLoadBtnBound = selectFileLoadBtn.getGlobalBounds();
	sf::FloatRect FileLoadTextBound = fileLoadButtonText.getGlobalBounds();
	fileLoadButtonText.setPosition(getCenterPosition(FileLoadTextBound.size, FileLoadBtnBound));
	sf::Vector2f btnText = fileLoadButtonText.getPosition();
	fileLoadButtonText.setPosition({ btnText.x, btnText.y - 3.f });
	loadButtonBounds = FileLoadBtnBound;	// �̺�Ʈ ó���� ���� �Ѱ���


	//sf::Vector2f buttonoTextPos = RightInnerAlign(loadButtonText, frameBound, 30.f);


	std::vector<std::wstring> imgPaths = {
		L"assets/profile_img/avatar_profile1.png",
		L"assets/profile_img/avatar_profile2.png",
		L"assets/profile_img/avatar_profile3.png",
		L"assets/profile_img/avatar_profile4.png"

	};

	

	for (int i = 0; i < imgPaths.size(); ++i)
	{
		ImageOption option;

		/*if (!option.texture.loadFromFile(imgPaths[i])) {
			std::wcerr << L"�̹��� �ε� ����: " << imgPaths[i] << std::endl;
			continue;
		}*/
		option.image = resizeImageKeepAspect(loadImg(imgPaths[i]));
		option.texture = sf::Texture(option.image);


		option.onClick = [i]() {
			std::cout << i << "�� ����� ���õ�!" << std::endl;
			// ������ �̹��� ���� ����
		};

		imageOptions.push_back(std::move(option));
	}

	float spacing = 20.f;
	float startX = frameBound.position.x + 100.f;
	//float startX = 100.f;

	float startY = frameBound.position.y + 130.f;
	sf::Vector2f scale = { 0.5f, 0.5f };

	for (int i = 0; i < imageOptions.size(); ++i) {
		sf::Sprite sprite(imageOptions[i].texture);
		sprite.setScale(scale);
		sprite.setPosition({ startX + i * (100 + spacing), startY });

		sprites.push_back(sprite);  // sprite�� ���Ϳ� ����
	}

	// drawables ���Ϳ� push
	drawables.push_back(std::make_shared<sf::RectangleShape>(selectFrame));
	drawables.push_back(std::make_shared<sf::Text>(selectTitle));
	drawables.push_back(std::make_shared<sf::RectangleShape>(selectFileLoadBtn));
	drawables.push_back(std::make_shared<sf::Text>(fileLoadButtonText));

}

int main()
{

	//������
	std::setlocale(LC_ALL, "");  // �ֿܼ� �����ڵ� ��� �����ϰ� ����

	// ��Ʈ ����
	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] ��Ʈ �ε� ����!" << std::endl;
		return 1;
	}

	// ������ ����
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");
	std::vector<std::shared_ptr<sf::Drawable>> drawables;
	std::vector<sf::Sprite> sprites;
	std::vector<ImageOption> imageOptions;
	sf::FloatRect loadButtonBounds;
	initSelectUI(window, font, drawables, sprites, imageOptions, loadButtonBounds);

	// ������
	std::cout << "imageOptions.size(): " << imageOptions.size() << std::endl;


	// ������ hover ȿ���� �׵θ�
	sf::RectangleShape thumbnailHoverOutline;
	thumbnailHoverOutline.setSize({ 200.f, 200.f }); // ����� ũ�⿡ �°� ����
	thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
	thumbnailHoverOutline.setOutlineThickness(4.f);
	thumbnailHoverOutline.setOutlineColor(sf::Color::Red);

	bool isHovering = false;
	//// �⺻ ������ ����
	//std::wstring imgPath = L"assets/profile_img/default_avatar.png";
	//sf::Image profile = loadImg(imgPath);
	//sf::Image resizeProfileImage = resizeImageKeepAspect(profile, { 200, 200 });
	//sf::Texture texture(resizeProfileImage);
	//sf::Sprite imgSprite(texture);


	


	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			// "close requested" event: we close the window
			if (event->is<sf::Event::Closed>())
				window.close();

			// ���콺 �̺�Ʈ
			else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouse->button == sf::Mouse::Button::Left) {
					sf::Vector2f mousePos = window.mapPixelToCoords(mouse->position);
					// ���⼭ Ŭ�� ó��
					for (int i = 0; i < sprites.size(); ++i) {
						if (sprites[i].getGlobalBounds().contains(mousePos)) {
							imageOptions[i].onClick();
						}
					}

					// �ҷ����� ��ư Ŭ�� Ȯ��
					if (loadButtonBounds.contains(mousePos)) {
						std::wcout << L"[�ҷ����� ��ư Ŭ����]" << std::endl;
						std::wstring selectedImage = openImageFileDialog();
						sf::Image image;
						if (image.loadFromFile(selectedImage)) {
							// ���� ó��
							std::wcout << L"[�̹����� ���������� �ҷ��Խ��ϴ�]" << std::endl;
							std::wcout << selectedImage << std::endl;

						}
						else
						{ 
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

		window.clear();
		for (auto& d : drawables) window.draw(*d);

		for (auto& sprite : sprites) {
			window.draw(sprite);
		}

		// hover ���� ���� �׵θ� �׸���
		if (isHovering)
			window.draw(thumbnailHoverOutline);
		window.display();
	}

	return 0;
}