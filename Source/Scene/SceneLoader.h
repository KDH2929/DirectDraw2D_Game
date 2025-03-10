#pragma once
#include "Scene.h"
#include "SceneTypes.h"
#include <windows.h>

// ������� ��ȯ������ �׻� ������ ��

class CDDrawDevice;

class SceneLoader {
public:
    SceneLoader(CDDrawDevice* pDrawDevice, HWND hWnd);
    ~SceneLoader();

    // ������ SceneType�� ���� Scene ��ü�� �����Ͽ� ��ȯ
    Scene* LoadScene(SceneType type);

private:
    CDDrawDevice* m_pDrawDevice;
    HWND m_hWnd;
};
