#ifndef SETTINGS_H
#define SETTINGS_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>  // 파일 입출력을 위한 헤더 추가
#include <map>
#include <string>
#include <memory>

using std::unique_ptr;
using std::make_unique;
using std::move;
using std::ifstream;
using std::ofstream;
using namespace std;

// 1. 설정 클래스 구조
class Settings {
private:
    //현재 설정값들
    int bgmVol;                // 배경음 0 ~ 100
    int sfVol;                 // 효과음 0 ~ 100
    int fontSize;              // 글자크기 (0 ~ 8범위로 확장) 최소 크기 --> 최대 크기
    bool voiceConnect;         // 음성 어시스턴트 ON/OFF       --> 구현 고민중


    //임시 설정값들
    int tempBgmVol;
    int tempSfVol;
    int tempFontSize;


    // 효과음 관련 멤버
    map<string, sf::SoundBuffer> soundBuffers;
    map<string, sf::Sound> sounds;

public:
    // 생성자
    Settings();

    // 배경음 관련
    int getBgmVol() const;
    void setBgmVol(int vol);

    // 효과음 관련
    int getSfVol() const;
    void setSfVol(int svol);

    // 글자 크기 관련
    int getFontSize() const;
    void setFontSize(int fnt);
    int getActualFontSize() const;

    // 음성 어시스턴트 관련
    bool getVoiceConnect() const;
    void setVoiceConnect(bool connect);



    // 설정 저장 및 로드
    bool saveSettings(const string& filename);
    bool loadSettings(const string& filename);

    // 효과음 관련 함수
    bool loadSound(const string& name, const string& filename);
    void playSound(const string& name);


    //임시 설정 초기화(설정 창 열 때 호출)
    void initTempSettings() {
        tempBgmVol = bgmVol;
        tempSfVol = sfVol;
        tempFontSize = fontSize;
    }

    //임시설정 getter.setter
    int getTempBgmvol() const { return tempBgmVol; }
    void setTempBgmVol(int vol) {
        tempBgmVol = max(0, min(100, vol));
    }

    int getTempSfVol() const { return tempSfVol; }
    void setTempSfVol(int sfVol) {  // 메서드 이름 수정
        tempSfVol = max(0, min(100, sfVol));
    }

    int getTempFontSize() const { return tempFontSize; }
    void setTempFontSize(int size) {
        tempFontSize = max(0, min(8, size));
    }

    // 설정확인(모든 임시 설정을 실제 설정으로 적용)
    void confirmSettings() {

        // 디버깅용 로그 (저장 전)
        cout << "저장 전: 배경음=" << bgmVol << ", 효과음=" << sfVol << ", 글자크기=" << fontSize << endl;
        cout << "임시값: 배경음=" << tempBgmVol << ", 효과음=" << tempSfVol << ", 글자크기=" << tempFontSize << endl;


        // 임시 설정값을 실제 설정값으로 적용
        bgmVol = tempBgmVol;
        sfVol = tempSfVol;
        fontSize = tempFontSize;

        // 추가 적용 로직 (예: 볼륨 즉시 반영)
        // 효과음 볼륨 적용
        for (auto& pair : sounds) {
            pair.second.setVolume(static_cast<float>(sfVol));
        }

        // 디버깅용 로그 (저장 후)
        cout << "저장 후: 배경음=" << bgmVol << ", 효과음=" << sfVol << ", 글자크기=" << fontSize << endl;
    }



    //설정 취소 (임시 설정을 원래 설정으로 되돌림)
    void cancelSettings() {
        // 디버깅용 로그 (취소 전)
        cout << "취소 전: 배경음=" << bgmVol << ", 효과음=" << sfVol << ", 글자크기=" << fontSize << endl;
        cout << "임시값: 배경음=" << tempBgmVol << ", 효과음=" << tempSfVol << ", 글자크기=" << tempFontSize << endl;

        //임시 설정값을 원래 설정값으로 복원
        tempBgmVol = bgmVol;
        tempSfVol = sfVol;
        tempFontSize = fontSize;


        // 디버깅용 로그 (취소 후)
        cout << "취소 후: 배경음=" << bgmVol << ", 효과음=" << sfVol << ", 글자크기=" << fontSize << endl;
        cout << "임시값 복원됨: 배경음=" << tempBgmVol << ", 효과음=" << tempSfVol << ", 글자크기=" << tempFontSize << endl;
    }
};

// ------------------------ 구현부 ------------------------

Settings::Settings()
    : bgmVol(30), sfVol(70), fontSize(4),
    voiceConnect(true),
    tempBgmVol(30), tempSfVol(70), tempFontSize(5) {

}

// 배경음 볼륨
int Settings::getBgmVol() const { return bgmVol; }
void Settings::setBgmVol(int vol) {
    bgmVol = std::max(0, std::min(100, vol));
}

// 효과음 볼륨
int Settings::getSfVol() const { return sfVol; }
void Settings::setSfVol(int svol) {
    sfVol = std::max(0, std::min(100, svol));
    for (auto& pair : sounds) {
        pair.second.setVolume(static_cast<float>(sfVol));
    }
}

// 글자 크기
int Settings::getFontSize() const { return fontSize; }
void Settings::setFontSize(int fnt) {
    fontSize = std::max(0, std::min(8, fnt)); // 0 ~ 8 범위로 확장 
}
int Settings::getActualFontSize() const {
    // 0-8 범위를 12-36 픽셀 크기로 매핑
    return 18 + (fontSize * 3);
}

// 음성 어시스턴트
bool Settings::getVoiceConnect() const { return voiceConnect; }
void Settings::setVoiceConnect(bool connect) { voiceConnect = connect; }



// 설정 저장/로드 --> 저장 
bool Settings::saveSettings(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "설정 파일을 저장할 수 없습니다: " << filename << endl;
        return false;
    }

    //설정 값들을 파일에 저장 
    file << "bgmVol=" << bgmVol << endl;
    file << "sfVol=" << sfVol << endl;
    file << "fontSize=" << fontSize << endl;
    file << "voiceConnect=" << voiceConnect << endl;


    file.close();
    cout << "설정이 저장되었습니다" << filename << endl;
    return true;

}


// 설정 저장/로드 --> 로드
bool Settings::loadSettings(const string& filename) {
    //파일 존재하는지 확인
    ifstream checkFile(filename);
    if (!checkFile.good()) {
        cout << "설정 파일이 존재하지 않습니다!!\t 기본 설정을 불러오겠습니다" << endl;
        return false;
    }
    checkFile.close();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "설정 파일을 불러올 수 없습니다: " << filename << endl;
        return false;
    }  

    //파일에서 한줄씩 읽어옴
    string line;
    while (getline(file, line)) {
        // '=' 기호의 위치를 찾아서 키와 값 먼저 분리
        size_t pos = line.find('=');
        if (pos != string::npos) {  // '=' 찾았으면
            //'=' 앞부분은 설정의 키(이름)
            string key = line.substr(0, pos);

            //'=' 뒷부분은 설정의 값
            string value = line.substr(pos + 1);

            try {
                if (key == "bgmVol") {
                    bgmVol = stoi(value);
                    bgmVol = std::max(0, std::min(100, bgmVol)); // 범위 확인
                }
                else if (key == "sfVol") {
                    sfVol = stoi(value);
                    sfVol = std::max(0, std::min(100, sfVol)); // 범위 확인
                }
                else if (key == "fontSize") {
                    fontSize = stoi(value);
                    fontSize = std::max(0, std::min(8, fontSize)); // 범위 확인 (0-8)
                }
                else if (key == "voiceConnect") {
                    voiceConnect = (value == "true");
                }

            }
            catch (const std::exception& e) {
                cout << "설정 파일 값 변환 중 오류: " << e.what() << endl;
                // 오류가 발생해도 계속 진행
            }
        }
    }
    file.close();
    cout << "설정을 불러왔습니다: " << filename << endl;

    // 임시 설정값 초기화
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;

    cout << "로드된 설정 값: 배경음 =" << bgmVol << ", 효과음 =" << sfVol
        << ", 글자 크기 =" << fontSize << ", 음성 어시스턴트 =" << (voiceConnect ? "ON" : "OFF") << endl;

    return true;

}


// 효과음 로드

bool Settings::loadSound(const string& name, const string& filename) {
    cout << "로드 시작: " << name << ", 파일: " << filename << endl;

    // 파일 존재 확인 (fopen_s 사용)
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename.c_str(), "r");
    if (err != 0 || !file) {
        cout << "파일이 존재하지 않음: " << filename << endl;
        return false;
    }
    fclose(file);

    try {
        // 버퍼에 사운드 로드
        if (!soundBuffers[name].loadFromFile(filename)) {
            cout << "버퍼 로드 실패: " << filename << endl;
            return false;
        }
        cout << "버퍼 로드 성공" << endl;

        try {
            cout << "사운드 객체 설정 시작" << endl;

            // sounds 맵에서 키 제거
            sounds.erase(name);
            cout << "기존 사운드 제거됨" << endl;

            // 버퍼로 사운드를 생성하고 맵에 삽입
            // insert와 emplace를 사용하여 맵에 직접 추가
            auto result = sounds.emplace(std::piecewise_construct,
                std::forward_as_tuple(name),
                std::forward_as_tuple(soundBuffers[name]));

            // 볼륨 설정
            if (result.second) {
                result.first->second.setVolume(static_cast<float>(sfVol));
                cout << "볼륨 설정 완료: " << sfVol << endl;
            }

            cout << "사운드 객체 설정 완료" << endl;
        }
        catch (const std::exception& e) {
            cout << "사운드 설정 중 예외 발생: " << e.what() << endl;
            return false;
        }
        catch (...) {
            cout << "사운드 설정 중 알 수 없는 예외 발생" << endl;
            return false;
        }
        cout << name << " 효과음 로드 완료" << endl;
        return true;
    }
    catch (const std::exception& e) {
        cout << "효과음 로드 중 예외 발생: " << e.what() << endl;
        return false;
    }
    catch (...) {
        cout << "효과음 로드 중 알 수 없는 예외 발생" << endl;
        return false;
    }
}
// 효과음 재생

void Settings::playSound(const string& name) {
    cout << "효과음 재생 시도: " << name << endl;
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        try {
            cout << "효과음 재생 시작: " << name << endl;
            it->second.play();
            cout << "효과음 재생 명령 완료: " << name << endl;
        }
        catch (const std::exception& e) {
            cout << "효과음 재생 중 예외 발생: " << e.what() << endl;
        }
        catch (...) {
            cout << "효과음 재생 중 알 수 없는 예외 발생" << endl;
        }
    }
    else {
        cout << "해당 이름의 효과음을 찾을 수 없음: " << name << endl;
    }
}

//설정 UI 렌더링 함수 선언
void renderSettingUI(sf::RenderWindow* window, Settings& settings, sf::Font& font, float windowWidth);

#endif // SETTINGS_H
