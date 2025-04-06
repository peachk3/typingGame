#ifndef SETTINGS_H
#define SETTINGS_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>  // ���� ������� ���� ��� �߰�
#include <map>
#include <string>
#include <memory>

using std::unique_ptr;
using std::make_unique;
using std::move;
using std::ifstream;
using std::ofstream;
using namespace std;

// 1. ���� Ŭ���� ����
class Settings {
private:
    //���� ��������
    int bgmVol;                // ����� 0 ~ 100
    int sfVol;                 // ȿ���� 0 ~ 100
    int fontSize;              // ����ũ�� (0 ~ 8������ Ȯ��) �ּ� ũ�� --> �ִ� ũ��
    bool voiceConnect;         // ���� ��ý���Ʈ ON/OFF       --> ���� �����


    //�ӽ� ��������
    int tempBgmVol;
    int tempSfVol;
    int tempFontSize;


    // ȿ���� ���� ���
    map<string, sf::SoundBuffer> soundBuffers;
    map<string, sf::Sound> sounds;

public:
    // ������
    Settings();

    // ����� ����
    int getBgmVol() const;
    void setBgmVol(int vol);

    // ȿ���� ����
    int getSfVol() const;
    void setSfVol(int svol);

    // ���� ũ�� ����
    int getFontSize() const;
    void setFontSize(int fnt);
    int getActualFontSize() const;

    // ���� ��ý���Ʈ ����
    bool getVoiceConnect() const;
    void setVoiceConnect(bool connect);



    // ���� ���� �� �ε�
    bool saveSettings(const string& filename);
    bool loadSettings(const string& filename);

    // ȿ���� ���� �Լ�
    bool loadSound(const string& name, const string& filename);
    void playSound(const string& name);


    //�ӽ� ���� �ʱ�ȭ(���� â �� �� ȣ��)
    void initTempSettings() {
        tempBgmVol = bgmVol;
        tempSfVol = sfVol;
        tempFontSize = fontSize;
    }

    //�ӽü��� getter.setter
    int getTempBgmvol() const { return tempBgmVol; }
    void setTempBgmVol(int vol) {
        tempBgmVol = max(0, min(100, vol));
    }

    int getTempSfVol() const { return tempSfVol; }
    void setTempSfVol(int sfVol) {  // �޼��� �̸� ����
        tempSfVol = max(0, min(100, sfVol));
    }

    int getTempFontSize() const { return tempFontSize; }
    void setTempFontSize(int size) {
        tempFontSize = max(0, min(8, size));
    }

    // ����Ȯ��(��� �ӽ� ������ ���� �������� ����)
    void confirmSettings() {

        // ������ �α� (���� ��)
        cout << "���� ��: �����=" << bgmVol << ", ȿ����=" << sfVol << ", ����ũ��=" << fontSize << endl;
        cout << "�ӽð�: �����=" << tempBgmVol << ", ȿ����=" << tempSfVol << ", ����ũ��=" << tempFontSize << endl;


        // �ӽ� �������� ���� ���������� ����
        bgmVol = tempBgmVol;
        sfVol = tempSfVol;
        fontSize = tempFontSize;

        // �߰� ���� ���� (��: ���� ��� �ݿ�)
        // ȿ���� ���� ����
        for (auto& pair : sounds) {
            pair.second.setVolume(static_cast<float>(sfVol));
        }

        // ������ �α� (���� ��)
        cout << "���� ��: �����=" << bgmVol << ", ȿ����=" << sfVol << ", ����ũ��=" << fontSize << endl;
    }



    //���� ��� (�ӽ� ������ ���� �������� �ǵ���)
    void cancelSettings() {
        // ������ �α� (��� ��)
        cout << "��� ��: �����=" << bgmVol << ", ȿ����=" << sfVol << ", ����ũ��=" << fontSize << endl;
        cout << "�ӽð�: �����=" << tempBgmVol << ", ȿ����=" << tempSfVol << ", ����ũ��=" << tempFontSize << endl;

        //�ӽ� �������� ���� ���������� ����
        tempBgmVol = bgmVol;
        tempSfVol = sfVol;
        tempFontSize = fontSize;


        // ������ �α� (��� ��)
        cout << "��� ��: �����=" << bgmVol << ", ȿ����=" << sfVol << ", ����ũ��=" << fontSize << endl;
        cout << "�ӽð� ������: �����=" << tempBgmVol << ", ȿ����=" << tempSfVol << ", ����ũ��=" << tempFontSize << endl;
    }
};

// ------------------------ ������ ------------------------

Settings::Settings()
    : bgmVol(30), sfVol(70), fontSize(4),
    voiceConnect(true),
    tempBgmVol(30), tempSfVol(70), tempFontSize(5) {

}

// ����� ����
int Settings::getBgmVol() const { return bgmVol; }
void Settings::setBgmVol(int vol) {
    bgmVol = std::max(0, std::min(100, vol));
}

// ȿ���� ����
int Settings::getSfVol() const { return sfVol; }
void Settings::setSfVol(int svol) {
    sfVol = std::max(0, std::min(100, svol));
    for (auto& pair : sounds) {
        pair.second.setVolume(static_cast<float>(sfVol));
    }
}

// ���� ũ��
int Settings::getFontSize() const { return fontSize; }
void Settings::setFontSize(int fnt) {
    fontSize = std::max(0, std::min(8, fnt)); // 0 ~ 8 ������ Ȯ�� 
}
int Settings::getActualFontSize() const {
    // 0-8 ������ 12-36 �ȼ� ũ��� ����
    return 18 + (fontSize * 3);
}

// ���� ��ý���Ʈ
bool Settings::getVoiceConnect() const { return voiceConnect; }
void Settings::setVoiceConnect(bool connect) { voiceConnect = connect; }



// ���� ����/�ε� --> ���� 
bool Settings::saveSettings(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "���� ������ ������ �� �����ϴ�: " << filename << endl;
        return false;
    }

    //���� ������ ���Ͽ� ���� 
    file << "bgmVol=" << bgmVol << endl;
    file << "sfVol=" << sfVol << endl;
    file << "fontSize=" << fontSize << endl;
    file << "voiceConnect=" << voiceConnect << endl;


    file.close();
    cout << "������ ����Ǿ����ϴ�" << filename << endl;
    return true;

}


// ���� ����/�ε� --> �ε�
bool Settings::loadSettings(const string& filename) {
    //���� �����ϴ��� Ȯ��
    ifstream checkFile(filename);
    if (!checkFile.good()) {
        cout << "���� ������ �������� �ʽ��ϴ�!!\t �⺻ ������ �ҷ����ڽ��ϴ�" << endl;
        return false;
    }
    checkFile.close();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "���� ������ �ҷ��� �� �����ϴ�: " << filename << endl;
        return false;
    }  

    //���Ͽ��� ���پ� �о��
    string line;
    while (getline(file, line)) {
        // '=' ��ȣ�� ��ġ�� ã�Ƽ� Ű�� �� ���� �и�
        size_t pos = line.find('=');
        if (pos != string::npos) {  // '=' ã������
            //'=' �պκ��� ������ Ű(�̸�)
            string key = line.substr(0, pos);

            //'=' �޺κ��� ������ ��
            string value = line.substr(pos + 1);

            try {
                if (key == "bgmVol") {
                    bgmVol = stoi(value);
                    bgmVol = std::max(0, std::min(100, bgmVol)); // ���� Ȯ��
                }
                else if (key == "sfVol") {
                    sfVol = stoi(value);
                    sfVol = std::max(0, std::min(100, sfVol)); // ���� Ȯ��
                }
                else if (key == "fontSize") {
                    fontSize = stoi(value);
                    fontSize = std::max(0, std::min(8, fontSize)); // ���� Ȯ�� (0-8)
                }
                else if (key == "voiceConnect") {
                    voiceConnect = (value == "true");
                }

            }
            catch (const std::exception& e) {
                cout << "���� ���� �� ��ȯ �� ����: " << e.what() << endl;
                // ������ �߻��ص� ��� ����
            }
        }
    }
    file.close();
    cout << "������ �ҷ��Խ��ϴ�: " << filename << endl;

    // �ӽ� ������ �ʱ�ȭ
    tempBgmVol = bgmVol;
    tempSfVol = sfVol;
    tempFontSize = fontSize;

    cout << "�ε�� ���� ��: ����� =" << bgmVol << ", ȿ���� =" << sfVol
        << ", ���� ũ�� =" << fontSize << ", ���� ��ý���Ʈ =" << (voiceConnect ? "ON" : "OFF") << endl;

    return true;

}


// ȿ���� �ε�

bool Settings::loadSound(const string& name, const string& filename) {
    cout << "�ε� ����: " << name << ", ����: " << filename << endl;

    // ���� ���� Ȯ�� (fopen_s ���)
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename.c_str(), "r");
    if (err != 0 || !file) {
        cout << "������ �������� ����: " << filename << endl;
        return false;
    }
    fclose(file);

    try {
        // ���ۿ� ���� �ε�
        if (!soundBuffers[name].loadFromFile(filename)) {
            cout << "���� �ε� ����: " << filename << endl;
            return false;
        }
        cout << "���� �ε� ����" << endl;

        try {
            cout << "���� ��ü ���� ����" << endl;

            // sounds �ʿ��� Ű ����
            sounds.erase(name);
            cout << "���� ���� ���ŵ�" << endl;

            // ���۷� ���带 �����ϰ� �ʿ� ����
            // insert�� emplace�� ����Ͽ� �ʿ� ���� �߰�
            auto result = sounds.emplace(std::piecewise_construct,
                std::forward_as_tuple(name),
                std::forward_as_tuple(soundBuffers[name]));

            // ���� ����
            if (result.second) {
                result.first->second.setVolume(static_cast<float>(sfVol));
                cout << "���� ���� �Ϸ�: " << sfVol << endl;
            }

            cout << "���� ��ü ���� �Ϸ�" << endl;
        }
        catch (const std::exception& e) {
            cout << "���� ���� �� ���� �߻�: " << e.what() << endl;
            return false;
        }
        catch (...) {
            cout << "���� ���� �� �� �� ���� ���� �߻�" << endl;
            return false;
        }
        cout << name << " ȿ���� �ε� �Ϸ�" << endl;
        return true;
    }
    catch (const std::exception& e) {
        cout << "ȿ���� �ε� �� ���� �߻�: " << e.what() << endl;
        return false;
    }
    catch (...) {
        cout << "ȿ���� �ε� �� �� �� ���� ���� �߻�" << endl;
        return false;
    }
}
// ȿ���� ���

void Settings::playSound(const string& name) {
    cout << "ȿ���� ��� �õ�: " << name << endl;
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        try {
            cout << "ȿ���� ��� ����: " << name << endl;
            it->second.play();
            cout << "ȿ���� ��� ��� �Ϸ�: " << name << endl;
        }
        catch (const std::exception& e) {
            cout << "ȿ���� ��� �� ���� �߻�: " << e.what() << endl;
        }
        catch (...) {
            cout << "ȿ���� ��� �� �� �� ���� ���� �߻�" << endl;
        }
    }
    else {
        cout << "�ش� �̸��� ȿ������ ã�� �� ����: " << name << endl;
    }
}

//���� UI ������ �Լ� ����
void renderSettingUI(sf::RenderWindow* window, Settings& settings, sf::Font& font, float windowWidth);

#endif // SETTINGS_H
