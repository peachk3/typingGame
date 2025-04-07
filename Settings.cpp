#include "Settings.hpp"
#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// 생성자 - 기본값으로 초기화
Settings::Settings()
    : bgmVol(50), sfVol(50), fontSize(4),
    tempBgmVol(50), tempSfVol(50), tempFontSize(4) {
}

// 실제 배경음악 볼륨 반환
int Settings::getBgmVol() const {
    return bgmVol;
}

// 실제 효과음 볼륨 반환
int Settings::getSfVol() const {
    return sfVol;
}

// 실제 글꼴 크기 슬라이더 값 반환 (0~8)
int Settings::getFontSize() const {
    return fontSize;
}

// 실제 픽셀 단위 글꼴 크기 반환 (18 ~ 42)
int Settings::getActualFontSize() const {
    return 18 + (fontSize * 3);
}

// 임시 배경음악 볼륨 반환
int Settings::getTempBgmvol() const {
    return tempBgmVol;
}

// 임시 효과음 볼륨 반환
int Settings::getTempSfVol() const {
    return tempSfVol;
}

// 임시 글꼴 크기 반환
int Settings::getTempFontSize() const {
    return tempFontSize;
}

// 임시 배경음악 볼륨 설정
void Settings::setTempBgmVol(int volume) {
    tempBgmVol = volume;
}

// 임시 효과음 볼륨 설정
void Settings::setTempSfVol(int volume) {
    tempSfVol = volume;
}

// 임시 글꼴 크기 설정
void Settings::setTempFontSize(int size) {
    tempFontSize = size;
}

// 설정 값을 파일로 저장
bool Settings::saveSettings(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "설정 파일을 열 수 없습니다: " << filename << std::endl;
        return false;
    }

    file << bgmVol << std::endl;
    file << sfVol << std::endl;
    file << fontSize << std::endl;

    file.close();
    std::cout << "설정이 저장되었습니다: " << filename << std::endl;
    return true;
}

// 파일로부터 설정 값을 불러오기
bool Settings::loadSettings(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "설정 파일을 열 수 없습니다: " << filename << std::endl;
        return false;
    }

    file >> bgmVol >> sfVol >> fontSize;

    // 불러온 설정값을 임시값에도 반영
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;

    file.close();
    std::cout << "설정이 로드되었습니다: " << filename << std::endl;
    return true;
}

// 효과음 로드
//bool Settings::loadSound(const std::string& name, const std::string& filepath) {
//    // 기존 버퍼와 사운드 제거
//    soundBuffers.erase(name);
//    sounds.erase(name);
//
//
//    // 새 버퍼 생성 및 로드
//    sf::SoundBuffer buffer;
//    if (!buffer.loadFromFile("assets/audio/music3.ogg")) {
//        std::cout << "사운드 파일 로드 실패: " << filepath << std::endl;
//        return false;
//    }
//
//    // 버퍼 추가
//    soundBuffers[name] = buffer;
//
//    // 사운드 객체 생성 및 추가
//    sounds[name] = sf::Sound(soundBuffers[name]);
//    sounds[name].setVolume(static_cast<float>(sfVol));
//
//    return true;
//}


// 효과음 재생
//void Settings::playSound(const std::string& name) {
//    auto it = sounds.find(name);
//    if (it != sounds.end()) {
//        it->second.setVolume(static_cast<float>(sfVol));
//        it->second.play();
//    }
//    else {
//        std::cerr << "해당 이름의 효과음을 찾을 수 없음: " << name << std::endl;
//    }
//}


// 임시 설정을 현재 설정값으로 초기화
void Settings::initTempSettings() {
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;
}

// 임시 설정을 실제 설정값에 적용
void Settings::confirmSettings() {
    bgmVol = tempBgmVol;
    sfVol = tempSfVol;
    fontSize = tempFontSize;

    // 모든 효과음 볼륨을 최신 값으로 갱신
    /*for (auto& sound : sounds) {
        sound.second.setVolume(static_cast<float>(sfVol));
    }*/
}

// 임시 설정을 취소하고 실제 설정값으로 되돌리기
void Settings::cancelSettings() {
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;
}