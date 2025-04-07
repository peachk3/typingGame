#include "Settings.hpp"
#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// ������ - �⺻������ �ʱ�ȭ
Settings::Settings()
    : bgmVol(50), sfVol(50), fontSize(4),
    tempBgmVol(50), tempSfVol(50), tempFontSize(4) {
}

// ���� ������� ���� ��ȯ
int Settings::getBgmVol() const {
    return bgmVol;
}

// ���� ȿ���� ���� ��ȯ
int Settings::getSfVol() const {
    return sfVol;
}

// ���� �۲� ũ�� �����̴� �� ��ȯ (0~8)
int Settings::getFontSize() const {
    return fontSize;
}

// ���� �ȼ� ���� �۲� ũ�� ��ȯ (18 ~ 42)
int Settings::getActualFontSize() const {
    return 18 + (fontSize * 3);
}

// �ӽ� ������� ���� ��ȯ
int Settings::getTempBgmvol() const {
    return tempBgmVol;
}

// �ӽ� ȿ���� ���� ��ȯ
int Settings::getTempSfVol() const {
    return tempSfVol;
}

// �ӽ� �۲� ũ�� ��ȯ
int Settings::getTempFontSize() const {
    return tempFontSize;
}

// �ӽ� ������� ���� ����
void Settings::setTempBgmVol(int volume) {
    tempBgmVol = volume;
}

// �ӽ� ȿ���� ���� ����
void Settings::setTempSfVol(int volume) {
    tempSfVol = volume;
}

// �ӽ� �۲� ũ�� ����
void Settings::setTempFontSize(int size) {
    tempFontSize = size;
}

// ���� ���� ���Ϸ� ����
bool Settings::saveSettings(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "���� ������ �� �� �����ϴ�: " << filename << std::endl;
        return false;
    }

    file << bgmVol << std::endl;
    file << sfVol << std::endl;
    file << fontSize << std::endl;

    file.close();
    std::cout << "������ ����Ǿ����ϴ�: " << filename << std::endl;
    return true;
}

// ���Ϸκ��� ���� ���� �ҷ�����
bool Settings::loadSettings(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "���� ������ �� �� �����ϴ�: " << filename << std::endl;
        return false;
    }

    file >> bgmVol >> sfVol >> fontSize;

    // �ҷ��� �������� �ӽð����� �ݿ�
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;

    file.close();
    std::cout << "������ �ε�Ǿ����ϴ�: " << filename << std::endl;
    return true;
}

// ȿ���� �ε�
//bool Settings::loadSound(const std::string& name, const std::string& filepath) {
//    // ���� ���ۿ� ���� ����
//    soundBuffers.erase(name);
//    sounds.erase(name);
//
//
//    // �� ���� ���� �� �ε�
//    sf::SoundBuffer buffer;
//    if (!buffer.loadFromFile("assets/audio/music3.ogg")) {
//        std::cout << "���� ���� �ε� ����: " << filepath << std::endl;
//        return false;
//    }
//
//    // ���� �߰�
//    soundBuffers[name] = buffer;
//
//    // ���� ��ü ���� �� �߰�
//    sounds[name] = sf::Sound(soundBuffers[name]);
//    sounds[name].setVolume(static_cast<float>(sfVol));
//
//    return true;
//}


// ȿ���� ���
//void Settings::playSound(const std::string& name) {
//    auto it = sounds.find(name);
//    if (it != sounds.end()) {
//        it->second.setVolume(static_cast<float>(sfVol));
//        it->second.play();
//    }
//    else {
//        std::cerr << "�ش� �̸��� ȿ������ ã�� �� ����: " << name << std::endl;
//    }
//}


// �ӽ� ������ ���� ���������� �ʱ�ȭ
void Settings::initTempSettings() {
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;
}

// �ӽ� ������ ���� �������� ����
void Settings::confirmSettings() {
    bgmVol = tempBgmVol;
    sfVol = tempSfVol;
    fontSize = tempFontSize;

    // ��� ȿ���� ������ �ֽ� ������ ����
    /*for (auto& sound : sounds) {
        sound.second.setVolume(static_cast<float>(sfVol));
    }*/
}

// �ӽ� ������ ����ϰ� ���� ���������� �ǵ�����
void Settings::cancelSettings() {
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;
}