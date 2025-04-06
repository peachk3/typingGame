// Scene.hpp
#ifndef SCENE_HPP
#define SCENE_HPP

enum class Scene {
    MAIN_MENU,      // 처음 화면, 사용자 프로필 포함
    FILE_SELECT,    // 파일 목록 선택 화면
    TYPING_GAME,    // 실제 게임 화면
    RESULT,         // 게임 끝나고 결과 화면
    EXIT            // 종료 상태 (필요 시)
};
#endif