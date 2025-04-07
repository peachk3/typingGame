#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <map>
#include <memory>



class Settings {
private:
    // 실제 설정 값
    int bgmVol;        // 배경음악 볼륨
    int sfVol;         // 효과음 볼륨
    int fontSize;      // 글꼴 크기

    // 임시 설정 값 (설정 화면에서 실시간 변경을 위한 값)
    int tempBgmVol;    // 임시 배경음악 볼륨
    int tempSfVol;     // 임시 효과음 볼륨
    int tempFontSize;  // 임시 글꼴 크기

    // 효과음 버퍼와 사운드 객체 저장용 맵
    //std::map<std::string, sf::SoundBuffer> soundBuffers; // 효과음 파일 데이터를 담는 버퍼
    //std::map<std::string, sf::Sound> sounds;             // 실제 재생할 사운드 객체


public:
    // 기본값으로 생성자
    Settings();

    // 실제 설정 값 가져오기
    int getBgmVol() const;             // 배경음악 볼륨 가져오기
    int getSfVol() const;              // 효과음 볼륨 가져오기
    int getFontSize() const;           // 글꼴 크기 가져오기
    int getActualFontSize() const;     // 픽셀 단위 글꼴 크기 반환

    // 임시 설정 값 가져오기
    int getTempBgmvol() const;         // 임시 배경음악 볼륨 가져오기
    int getTempSfVol() const;          // 임시 효과음 볼륨 가져오기
    int getTempFontSize() const;       // 임시 글꼴 크기 가져오기

    // 임시 설정 값 변경하기
    void setTempBgmVol(int volume);    // 임시 배경음악 볼륨 설정
    void setTempSfVol(int volume);     // 임시 효과음 볼륨 설정
    void setTempFontSize(int size);    // 임시 글꼴 크기 설정

    // 설정 저장 및 불러오기
    bool saveSettings(const std::string& filename); // 설정을 파일에 저장
    bool loadSettings(const std::string& filename); // 설정을 파일에서 불러오기

    // 효과음 관리
    //bool loadSound(const std::string& name, const std::string& filepath); // 효과음 로드
    //void playSound(const std::string& name);                              // 효과음 재생

    // 임시 설정 값을 현재 설정 값으로 초기화
    void initTempSettings();    // 임시값을 현재값으로 초기화

    // 임시 설정 값을 실제 설정 값으로 적용
    void confirmSettings();     // 임시값을 실제값으로 확정

    // 임시 설정을 현재 설정 값으로 되돌리기
    void cancelSettings();      // 임시값을 초기값으로 되돌림
};
#endif // SETTINGS_HPP