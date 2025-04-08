# typing Game
C++ 기반 타자 연습 게임 만들기

프로젝트 개요

- 개발 일정
  - 2025.03.24 ~ 2025.04.07
- 개발 특징
  - 기본 타자 연습(한/영, 코딩), 1 대 1 대결
  - 게임 화면 UI
  - MySQL 기반 사용자 데이터 관리
  - SFML 네트워크 소켓 통신 기반 멀티 플레이

기술 스택
- SFML
  - 표준 C++ 라이브러리
  - Multithreading
  - Custom UI Modules
  - 상태기반/재귀적 게임 루프
- 소켓통신
  - SFML Network
  - TCP 소켓
  - 싱글톤 패턴
-  MySQL Workbench

## 1일차

## 2일차

## 3일차
- Github 리포지토리 복제(-b 브랜치명 --single-branch 옵션 : 특정 브랜치 클론 )
    - git clone -b develop --single-branch https://github.com/peachk3/typingGame
    - git clone https://github.com/peachk3/typingGame (깃허브 주소)

## SFML 설치 (Windows 명령프롬프트)
- C:/Dev/ 로 cd 한 후
```
git clone https://github.com/microsoft/vcpkg.git

cd vcpkg
.\bootstrap-vcpkg.bat

.\vcpkg integrate install   ## visual studio 연동

vcpkg install SFML:x64-windows     ## SFML 설치
```

- 대전 버튼 클릭 시 loading 화면
    - [loading.cpp](./loading.cpp) 
    
- Server, Client 통신
    - 새 솔루션 폴더 생성 : cli -> 새 프로젝트 생성(위치 확인) : Client -> 소스파일 : [cli_main.cpp](./Client/cli%20-%20main.cpp), [NetworkManager.cpp](./Client/NetworkManager.cpp)
    - 새 솔루션 폴더 생성 : ser -> 새 프로젝트 생성(위치 확인) : Server -> 소스파일 : [server_main.cpp](./Server/server%20-%20main.cpp), [ServerManager.cpp](./Server/ServerManager.cpp)

## 4일차
- MainDisplay 소스코드 : [MainDisplayV04.cpp](./MainDisplayV04.cpp) 
