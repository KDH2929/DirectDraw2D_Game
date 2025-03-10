#pragma once

#include "Scene.h"
#include <windows.h>

class CDDrawDevice;
class UI_Text;

class LoadingScene : public Scene {
public:
    LoadingScene(CDDrawDevice* pDrawDevice, HWND hWnd);
    virtual ~LoadingScene();

    virtual void Initialize() override;
    virtual void Cleanup() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    virtual void ResetInterpolation() override;
    virtual void InterpolatePosition(float alpha) override;

    virtual const SceneType GetSceneType() override;

private:
    CDDrawDevice* m_pDrawDevice;  // DirectDraw ��ġ ������
    HWND m_hWnd;                  // �������� ����� ������ �ڵ�
    UI_Text* m_pLoadingText;      // �߾ӿ� ǥ���� �ؽ�Ʈ (Loading ����)

    float m_timer;                // �ؽ�Ʈ ���� ���� Ÿ�̸�
    int m_currentState;           // ���� �ε� �ؽ�Ʈ ���� �ε���
};
