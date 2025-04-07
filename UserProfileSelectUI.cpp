#include "UserProfileUI.hpp"
#include "UserProfileSelectUI.hpp"
#include "DrawUI.hpp"
#include "UIAlign.hpp"
#include <windows.h>
#include <iostream>
#include <functional>


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

// ������ ������Ʈ �Լ�
void updateProfileImage(std::wstring newPath, GameState& game, sf::Sprite& sprite) {
	sf::Image image = loadImg(newPath);
	sf::Image resized = resizeImageKeepAspect(image);
	game.user.profileTexture.loadFromImage(resized);
	sprite.setTexture(game.user.profileTexture);
}




// ������ �̹��� ���� UI ���� �Լ�
void renderSelectImage(
	sf::RenderWindow& window,
	GameState& game,
	sf::Font& font,
	std::vector<ImageOption>& imageOptions,
	std::vector<sf::Sprite>& sprites,
	sf::Sprite& profileSprite,								// �߰�
	sf::RectangleShape& thumbnailHoverOutline)				// �߰�
{
	imageOptions.clear();
	

	sf::RectangleShape selectFrame = makeRectangle(window, 0.5f, 0.5f);
	selectFrame.setFillColor(sf::Color::White);
	selectFrame.setOutlineColor(sf::Color::Black);
	selectFrame.setOutlineThickness(3.f);
	// ���� â ��ġ ��� ����
	sf::FloatRect frameBound = selectFrame.getGlobalBounds();
	selectFrame.setPosition(getWindowCenterPosition(window, frameBound));
	// ���� �� ��ġ ����
	frameBound = selectFrame.getGlobalBounds();


	// ���� �����̳�
	sf::RectangleShape selectBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Black);


	// �̹��� ���� ���� �ؽ�Ʈ
	sf::Text selectTitle(font, L"������ �̹��� ����");
	sf::Vector2f TitleSize = selectBox_Title.getSize();
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
	// ��ư �ؽ�Ʈ ��ġ ������ (�ѱ�)
	fileLoadButtonText.setPosition({ btnText.x, btnText.y - 3.f });
	game.btn.loadImgBtnBounds = FileLoadBtnBound; // �̺�Ʈ ó���� ���� �Ѱ���
	


	// �⺻ �̹��� ���� ���� ��� ����
	std::vector<std::wstring> imgPaths = {
		L"assets/profile_img/avatar_profile1.png",
		L"assets/profile_img/avatar_profile2.png",
		L"assets/profile_img/avatar_profile3.png",
		L"assets/profile_img/avatar_profile4.png"
	};



	//for (int i = 0; i < imgPaths.size(); ++i)
	//{
	//	ImageOption option;

	//	option.image = resizeImageKeepAspect(loadImg(imgPaths[i]));
	//	option.texture = sf::Texture(option.image);

	//	// �̺�Ʈ �α� ��� 
	//	option.onClick = [i]() {
	//		std::cout << i << "�� ����� ���õ�!" << std::endl;
	//		// ������ �̹��� ���� ����
	//		
	//		};

	//	imageOptions.push_back(std::move(option));
	//}

	// ���� �Ǵ� main �Լ� ��ܿ� �ִ� profileTexture, profileSprite�� ĸó
	for (int i = 0; i < imgPaths.size(); ++i)
	{
		ImageOption option;
		std::wstring path = imgPaths[i];
		option.image = resizeImageKeepAspect(loadImg(imgPaths[i]));
		option.texture.loadFromImage(option.image);

		// ���� �ȿ��� �ܺ� ���� ĸó
		option.onClick = [&, path]() {
			std::cout << i << "Ŭ��" << std::endl;
			std::wcout << path << L"�� ����� ���õ�!" << std::endl;
			updateProfileImage(path, game, profileSprite);
			game.showImageOverlay = false;
			game.currentScene = Scene::PROFILE; // �ٽ� ���� ȭ������ ��ȯ
			};

		imageOptions.push_back(std::move(option));
	}


	float spacing = 20.f;
	float startX = frameBound.position.x + 100.f;
	float startY = frameBound.position.y + 130.f;
	sf::Vector2f scale = { 0.5f, 0.5f };
	
	window.draw(selectFrame);
	sprites.clear();
	//sprites.clear(); // ���� �������� sprite ���� ����
	for (int i = 0; i < imageOptions.size(); ++i) {
		sf::Sprite sprite(imageOptions[i].texture);
		sprite.setScale(scale);
		sprite.setPosition({ startX + i * (100 + spacing), startY });


		//// ������ sprites ���� �־�� ��
		sprites.push_back(sprite);  // sprite�� ���Ϳ� ���� Ŭ�� �̺�Ʈ�� ����
		window.draw(sprite);
	}
	if (game.showImageOverlay && game.bHoveringThumbnail) {
		window.draw(thumbnailHoverOutline);
	}

	window.draw(selectTitle);
	window.draw(selectFileLoadBtn);
	window.draw(fileLoadButtonText);


}