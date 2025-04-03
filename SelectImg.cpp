/* (2025.04.03) 
 * 프로필 -> 프로필 이미지 선택 버튼 누르면 나오는 선택창 구현 
 * 기본 이미지 4개 선택 가능하도록
 * 불러오기 버튼 클릭시 파일 탐색기 열리면서 이미지 파일 가져올 수 있음 (png, jpg,				bmp, png, tga, jpg, gif, psd, hdr, pic 
 * 기본 이미지 선택했을 때 hover효과 */


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

// 파일 열기 대화상자 실행 후 선택된 경로를 반환하는 함수
std::wstring openImageFileDialog()
{
	wchar_t filePath[MAX_PATH] = L"";

	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = L"이미지 파일 (PNG, JPG, JPEG)\0*.png;*.jpg;*.jpeg\0GIF 파일\0*.gif\0\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	ofn.lpstrTitle = L"이미지 파일 열기";

	if (GetOpenFileNameW(&ofn))
	{
		return std::wstring(filePath);
	}
	else
	{
		return L"";  // 사용자가 취소했거나 오류 발생
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
	// 선택 창 위치 가운데 정렬
	sf::FloatRect frameBound = selectFrame.getGlobalBounds();
	selectFrame.setPosition(getWindowCenterPosition(window, frameBound));
	// 정렬 후 위치 저장
	frameBound = selectFrame.getGlobalBounds();
	

	// 제목 컨테이너
	sf::RectangleShape selectBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Red);

	

	// 이미지 선택 타이틀 텍스트
	sf::Text selectTitle(font, L"프로필 이미지 선택");
	sf::Vector2f TitleSize = selectBox_Title.getSize();
	//sf::FloatRect profileBoxTitleBound = selectBox_Title.getGlobalBounds();
	sf::Vector2f titlePos = getCenterXPosition(TitleSize, frameBound, frameBound.position.y);	// 프로필 프래임 기준으로 중앙 정렬
	selectTitle.setPosition({ titlePos.x, titlePos.y + 25.f });
	selectTitle.setFillColor(sf::Color::Black);

	// 이미지 선택 버튼
	sf::RectangleShape selectFileLoadBtn({ 100, 30 });
	sf::FloatRect FileLoadBtnBound = selectFileLoadBtn.getGlobalBounds();
	selectFileLoadBtn.setFillColor(sf::Color::Red);
	sf::Vector2f btnPos = frameBound.getCenter();
	btnPos = { btnPos.x - (FileLoadBtnBound.size.x / 2), btnPos.y }; // frame 중앙에 정렬
	selectFileLoadBtn.setPosition({ btnPos.x, btnPos.y + 100 });

	// 버튼 텍스트
	// 위치 다시 가져옴
	sf::Text fileLoadButtonText(font, L"불러오기", 18);
	FileLoadBtnBound = selectFileLoadBtn.getGlobalBounds();
	sf::FloatRect FileLoadTextBound = fileLoadButtonText.getGlobalBounds();
	fileLoadButtonText.setPosition(getCenterPosition(FileLoadTextBound.size, FileLoadBtnBound));
	sf::Vector2f btnText = fileLoadButtonText.getPosition();
	fileLoadButtonText.setPosition({ btnText.x, btnText.y - 3.f });
	loadButtonBounds = FileLoadBtnBound;	// 이벤트 처리를 위해 넘겨줌


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
			std::wcerr << L"이미지 로드 실패: " << imgPaths[i] << std::endl;
			continue;
		}*/
		option.image = resizeImageKeepAspect(loadImg(imgPaths[i]));
		option.texture = sf::Texture(option.image);


		option.onClick = [i]() {
			std::cout << i << "번 썸네일 선택됨!" << std::endl;
			// 프로필 이미지 변경 로직
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

		sprites.push_back(sprite);  // sprite를 벡터에 저장
	}

	// drawables 벡터에 push
	drawables.push_back(std::make_shared<sf::RectangleShape>(selectFrame));
	drawables.push_back(std::make_shared<sf::Text>(selectTitle));
	drawables.push_back(std::make_shared<sf::RectangleShape>(selectFileLoadBtn));
	drawables.push_back(std::make_shared<sf::Text>(fileLoadButtonText));

}

int main()
{

	//디버깅용
	std::setlocale(LC_ALL, "");  // 콘솔에 유니코드 출력 가능하게 설정

	// 폰트 설정
	sf::Font font;
	if (!font.openFromFile("assets/fonts/D2Coding.ttf")) {
		std::wcerr << L"[ERROR] 폰트 로드 실패!" << std::endl;
		return 1;
	}

	// 윈도우 생성
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "My window");
	std::vector<std::shared_ptr<sf::Drawable>> drawables;
	std::vector<sf::Sprite> sprites;
	std::vector<ImageOption> imageOptions;
	sf::FloatRect loadButtonBounds;
	initSelectUI(window, font, drawables, sprites, imageOptions, loadButtonBounds);

	// 디버깅용
	std::cout << "imageOptions.size(): " << imageOptions.size() << std::endl;


	// 프로필 hover 효과용 테두리
	sf::RectangleShape thumbnailHoverOutline;
	thumbnailHoverOutline.setSize({ 200.f, 200.f }); // 썸네일 크기에 맞게 조절
	thumbnailHoverOutline.setFillColor(sf::Color::Transparent);
	thumbnailHoverOutline.setOutlineThickness(4.f);
	thumbnailHoverOutline.setOutlineColor(sf::Color::Red);

	bool isHovering = false;
	//// 기본 프로필 설정
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

			// 마우스 이벤트
			else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouse->button == sf::Mouse::Button::Left) {
					sf::Vector2f mousePos = window.mapPixelToCoords(mouse->position);
					// 여기서 클릭 처리
					for (int i = 0; i < sprites.size(); ++i) {
						if (sprites[i].getGlobalBounds().contains(mousePos)) {
							imageOptions[i].onClick();
						}
					}

					// 불러오기 버튼 클릭 확인
					if (loadButtonBounds.contains(mousePos)) {
						std::wcout << L"[불러오기 버튼 클릭됨]" << std::endl;
						std::wstring selectedImage = openImageFileDialog();
						sf::Image image;
						if (image.loadFromFile(selectedImage)) {
							// 성공 처리
							std::wcout << L"[이미지를 성공적으로 불러왔습니다]" << std::endl;
							std::wcout << selectedImage << std::endl;

						}
						else
						{ 
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

		window.clear();
		for (auto& d : drawables) window.draw(*d);

		for (auto& sprite : sprites) {
			window.draw(sprite);
		}

		// hover 중일 때만 테두리 그리기
		if (isHovering)
			window.draw(thumbnailHoverOutline);
		window.display();
	}

	return 0;
}