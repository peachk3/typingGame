#include "DrawUI.hpp"
#include <iostream>
// ���â�� ������ ������ �°� ����� ���� �Լ�
sf::RectangleShape makeRectangle(sf::RenderWindow& window, float widthRatio, float heightRatio)
{
	// ���� â ũ��
	sf::Vector2u winSize = window.getSize();

	// �簢�� ������ ����
	sf::Vector2f rectSize(winSize.x * widthRatio, winSize.y * heightRatio);
	sf::RectangleShape rect(rectSize);
	rect.setFillColor(sf::Color::Transparent);
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



//sf::Vector2f BottomRightInnerAlign(sf::Vector2f targetSize, const sf::FloatRect& refBounds, float x_margin = 0.f, float y_margin = 0.f)
//{
//	return {
//		refBounds.left + refBounds.width - targetSize.x - x_margin,
//		refBounds.top + refBounds.height - targetSize.y - y_margin
//	};
//}



// �̹��� �ε�
sf::Image loadImg(std::wstring path)
{
	sf::Image image;
	std::filesystem::path absPath = std::filesystem::absolute(path);

	if (!image.loadFromFile(absPath.wstring())) {
		std::wcerr << L"�̹��� �ε� ����!" << std::endl;
		std::wcerr << L"�Էµ� ���: " << path << std::endl;
		std::wcerr << L"���� ���: " << absPath.wstring() << std::endl;
		std::exit(EXIT_FAILURE);
	}

	return image;
}

// �̹��� ��������
sf::Image resizeImageKeepAspect(const sf::Image& src, sf::Vector2u targetSize, sf::Color paddingColor)
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


