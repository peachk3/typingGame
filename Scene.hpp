// Scene.hpp
#ifndef SCENE_HPP
#define SCENE_HPP

enum class Scene {
    LOGIN,
    SIGNUP,
    MAIN_MENU,      // ó�� ȭ��, ����� ������ ����
    PROFILE,
    IMAGESELECT,
    SETTING,
    FILE_SELECT,    // ���� ��� ���� ȭ��
    TYPING_GAME,    // ���� ���� ȭ��
    RESULT,         // ���� ������ ��� ȭ��
    EXIT            // ���� ���� (�ʿ� ��)
};
#endif