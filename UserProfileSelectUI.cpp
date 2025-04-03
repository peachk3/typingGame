#include "UserProfileUI.hpp"
#include "UserProfileSelectUI.hpp"
#include <windows.h>
#include <iostream>
#include <functional>


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
// 프로필 업데이트 함수
void updateProfileImage(std::wstring newPath, sf::Texture& texture, sf::Sprite& sprite) {
	sf::Image image = loadImg(newPath);
	sf::Image resized = resizeImageKeepAspect(image);
	texture.loadFromImage(resized);
	sprite.setTexture(texture);
}

// 프로필 이미지 선택 UI 생성 함수
void initSelectUI(
	sf::RenderWindow& window,
	sf::Font& font,
	std::vector<std::shared_ptr<sf::Drawable>>& drawables,
	std::vector<sf::Sprite>& sprites,
	std::vector<ImageOption>& imageOptions,
	sf::FloatRect& ButtonBounds,
	sf::Texture& profileTexture,			// 추가
	sf::Sprite& profileSprite,				// 추가
	UIState& currentState)					// 추가 (화면 전환 위해)

{
	sf::RectangleShape selectFrame = makeRectangle(window, 0.5f, 0.5f);
	// 선택 창 위치 가운데 정렬
	sf::FloatRect frameBound = selectFrame.getGlobalBounds();
	selectFrame.setPosition(getWindowCenterPosition(window, frameBound));
	// 정렬 후 위치 저장
	frameBound = selectFrame.getGlobalBounds();


	// 제목 컨테이너
	sf::RectangleShape selectBox_Title = makeRectangle(frameBound, 0.4f, 0.1f, sf::Color::Red);


	// 이미지 선택 제목 텍스트
	sf::Text selectTitle(font, L"프로필 이미지 선택");
	sf::Vector2f TitleSize = selectBox_Title.getSize();
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
	// 버튼 텍스트 위치 재조정 (한글)
	fileLoadButtonText.setPosition({ btnText.x, btnText.y - 3.f });
	ButtonBounds = FileLoadBtnBound;	// 이벤트 처리를 위해 넘겨줌



	// 기본 이미지 파일 저장 경로 저장
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

	//	// 이벤트 로그 출력 
	//	option.onClick = [i]() {
	//		std::cout << i << "번 썸네일 선택됨!" << std::endl;
	//		// 프로필 이미지 변경 로직
	//		
	//		};

	//	imageOptions.push_back(std::move(option));
	//}

	// 전역 또는 main 함수 상단에 있는 profileTexture, profileSprite를 캡처
	for (int i = 0; i < imgPaths.size(); ++i)
	{
		ImageOption option;
		std::wstring path = imgPaths[i];
		option.image = resizeImageKeepAspect(loadImg(imgPaths[i]));
		option.texture.loadFromImage(option.image);

		// 람다 안에서 외부 상태 캡처
		option.onClick = [&, path]() {
			std::wcout << path << L"번 썸네일 선택됨!" << std::endl;
			updateProfileImage(path, profileTexture, profileSprite);
			currentState = UIState::ProfileMain;  // 다시 메인 화면으로 전환
			};

		imageOptions.push_back(std::move(option));
	}


	float spacing = 20.f;
	float startX = frameBound.position.x + 100.f;
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