#include "DrawUI.hpp"
#include <iostream>
// 결과창을 윈도우 비율에 맞게 만들기 위한 함수
sf::RectangleShape makeRectangle(sf::RenderWindow& window, float widthRatio, float heightRatio)
{
	// 현재 창 크기
	sf::Vector2u winSize = window.getSize();

	// 사각형 사이즈 조절
	sf::Vector2f rectSize(winSize.x * widthRatio, winSize.y * heightRatio);
	sf::RectangleShape rect(rectSize);
	rect.setFillColor(sf::Color::Transparent);
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



//sf::Vector2f BottomRightInnerAlign(sf::Vector2f targetSize, const sf::FloatRect& refBounds, float x_margin = 0.f, float y_margin = 0.f)
//{
//	return {
//		refBounds.left + refBounds.width - targetSize.x - x_margin,
//		refBounds.top + refBounds.height - targetSize.y - y_margin
//	};
//}



// 이미지 로드
sf::Image loadImg(std::wstring path)
{
	sf::Image image;
	std::filesystem::path absPath = std::filesystem::absolute(path);

	if (!image.loadFromFile(absPath.wstring())) {
		std::wcerr << L"이미지 로드 실패!" << std::endl;
		std::wcerr << L"입력된 경로: " << path << std::endl;
		std::wcerr << L"절대 경로: " << absPath.wstring() << std::endl;
		std::exit(EXIT_FAILURE);
	}

	return image;
}

// 이미지 리사이즈
sf::Image resizeImageKeepAspect(const sf::Image& src, sf::Vector2u targetSize, sf::Color paddingColor)
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


