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
    // ���� ���� ��
    int bgmVol;        // ������� ����
    int sfVol;         // ȿ���� ����
    int fontSize;      // �۲� ũ��

    // �ӽ� ���� �� (���� ȭ�鿡�� �ǽð� ������ ���� ��)
    int tempBgmVol;    // �ӽ� ������� ����
    int tempSfVol;     // �ӽ� ȿ���� ����
    int tempFontSize;  // �ӽ� �۲� ũ��

    // ȿ���� ���ۿ� ���� ��ü ����� ��
    //std::map<std::string, sf::SoundBuffer> soundBuffers; // ȿ���� ���� �����͸� ��� ����
    //std::map<std::string, sf::Sound> sounds;             // ���� ����� ���� ��ü


public:
    // �⺻������ ������
    Settings();

    // ���� ���� �� ��������
    int getBgmVol() const;             // ������� ���� ��������
    int getSfVol() const;              // ȿ���� ���� ��������
    int getFontSize() const;           // �۲� ũ�� ��������
    int getActualFontSize() const;     // �ȼ� ���� �۲� ũ�� ��ȯ

    // �ӽ� ���� �� ��������
    int getTempBgmvol() const;         // �ӽ� ������� ���� ��������
    int getTempSfVol() const;          // �ӽ� ȿ���� ���� ��������
    int getTempFontSize() const;       // �ӽ� �۲� ũ�� ��������

    // �ӽ� ���� �� �����ϱ�
    void setTempBgmVol(int volume);    // �ӽ� ������� ���� ����
    void setTempSfVol(int volume);     // �ӽ� ȿ���� ���� ����
    void setTempFontSize(int size);    // �ӽ� �۲� ũ�� ����

    // ���� ���� �� �ҷ�����
    bool saveSettings(const std::string& filename); // ������ ���Ͽ� ����
    bool loadSettings(const std::string& filename); // ������ ���Ͽ��� �ҷ�����

    // ȿ���� ����
    //bool loadSound(const std::string& name, const std::string& filepath); // ȿ���� �ε�
    //void playSound(const std::string& name);                              // ȿ���� ���

    // �ӽ� ���� ���� ���� ���� ������ �ʱ�ȭ
    void initTempSettings();    // �ӽð��� ���簪���� �ʱ�ȭ

    // �ӽ� ���� ���� ���� ���� ������ ����
    void confirmSettings();     // �ӽð��� ���������� Ȯ��

    // �ӽ� ������ ���� ���� ������ �ǵ�����
    void cancelSettings();      // �ӽð��� �ʱⰪ���� �ǵ���
};
#endif // SETTINGS_HPP