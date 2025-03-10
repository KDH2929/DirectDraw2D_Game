#pragma once

#include "Scene.h"
#include "SceneLoader.h"
#include "SceneTypes.h"
#include <windows.h>
#include <vector>
#include <future>


class CDDrawDevice; // ���� ����

class SceneManager
{
public:
    // �̱���
    static SceneManager* GetInstance();

    // Game Ŭ������ Initialize���� �ݵ�� ȣ�� �� ��
    static void Initialize(CDDrawDevice* pDrawDevice, HWND hWnd);
    static void CleanUp();

    // ���� �� ��ȯ: ���� ���� �����ϰ�, �� ���� ���������� ���� �� �ʱ�ȭ
    void ChangeScene(SceneType newSceneType);

    // �񵿱� �� ��ȯ: �ε� ���� ǥ���� ��, ��׶��忡�� �� �� ����
    void AsyncChangeScene(SceneType newSceneType);

    void Update(float deltaTime);
    void InterpolatePosition(float alpha);
    void ResetInterpolation();
    void Render();

private:
    // ������ �� �Ҹ��ڴ� private
    SceneManager(CDDrawDevice* pDrawDevice, HWND hWnd);
    ~SceneManager();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // ��׶��忡�� �� �ε� ���¸� Ȯ��
    bool PollAsyncSceneLoading();

private:
    static SceneManager* s_instance;

    Scene* m_pCurrentScene;
    Scene* m_pPreviousScene;
    SceneLoader m_sceneLoader;              // �� ������ SceneLoader�� ���� ó��
    CDDrawDevice* m_pDrawDevice;
    HWND m_hWnd;

    // �񵿱� �ε� ���� ���
    std::future<Scene*> m_futureScene;
    bool m_loading;
};
