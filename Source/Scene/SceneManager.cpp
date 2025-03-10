#include "stdafx.h"
#include "SceneManager.h"
#include "DebugManager.h"
#include <chrono>
#include <future>

// �ʱ�ȭ�� �̱��� �ν��Ͻ�
SceneManager* SceneManager::s_instance = nullptr;

// �̱��� �ν��Ͻ� ����
SceneManager* SceneManager::GetInstance()
{
    return s_instance;
}

// Game Ŭ������ Initialize���� �ݵ�� ȣ���� �ʱ�ȭ �Լ�
void SceneManager::Initialize(CDDrawDevice* pDrawDevice, HWND hWnd)
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }

    s_instance = new SceneManager(pDrawDevice, hWnd);
}

// �̱��� �ν��Ͻ� ����
void SceneManager::CleanUp()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

SceneManager::SceneManager(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice),
    m_hWnd(hWnd),
    m_pCurrentScene(nullptr),
    m_pPreviousScene(nullptr),
    m_sceneLoader(pDrawDevice, hWnd),
    m_loading(false)
{
}

SceneManager::~SceneManager()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Cleanup();
        delete m_pCurrentScene;
        m_pCurrentScene = nullptr;
    }

    if (m_pPreviousScene)
    {
        m_pPreviousScene->Cleanup();
        delete m_pPreviousScene;
        m_pPreviousScene = nullptr;
    }
}

void SceneManager::ChangeScene(SceneType newSceneType)
{
    if (m_pCurrentScene)
    {
        m_pPreviousScene = m_pCurrentScene;
    }

    // ���ο� �� �ε�
    m_pCurrentScene = m_sceneLoader.LoadScene(newSceneType);
    
    SceneType DebugType = m_pCurrentScene->GetSceneType();
    LOG_MESSAGE(L"Changed Scene: " + SceneTypeToWString(DebugType));
}

void SceneManager::AsyncChangeScene(SceneType newSceneType)
{
    ChangeScene(SceneType::Loading);

    // ��׶��忡�� �� ���� �񵿱������� ����
    m_futureScene = std::async(std::launch::async, [this, newSceneType]() -> Scene* {
        return m_sceneLoader.LoadScene(newSceneType);
        });
    m_loading = true;
}

void SceneManager::Update(float deltaTime)
{
    // �񵿱� �ε� ���̸� ��׶��� �۾� ���¸� Ȯ��
    if (m_loading)
    {
        if (PollAsyncSceneLoading())
        {
            // m_futureScene.get();�� ȣ���ϸ�, std::future�� ����� �޾ƿ��� �񵿱� ������� �ڵ� ������
            Scene* loadedScene = m_futureScene.get();
            if (m_pCurrentScene)
            {
                m_pPreviousScene = m_pCurrentScene;
            }

            m_loading = false;

            m_pCurrentScene = loadedScene;
            SceneType DebugType = m_pCurrentScene->GetSceneType();
            LOG_MESSAGE(L"Changed Scene (" + SceneTypeToWString(DebugType) + L") AsyncLoad Completed.");
        }
    }

    if (m_pPreviousScene)
    {
        SceneType previousSceneType = m_pPreviousScene->GetSceneType();
        LOG_MESSAGE(L"Previous Scene (" + SceneTypeToWString(previousSceneType) + L") Memory Deallocation Start");

        m_pPreviousScene->Cleanup();
        delete m_pPreviousScene;
        m_pPreviousScene = nullptr;
        
        LOG_MESSAGE(L"Previous Scene (" + SceneTypeToWString(previousSceneType) + L") Memory Deallocation Completed");
    }

    if (m_pCurrentScene)
    {
        m_pCurrentScene->Update(deltaTime);
    }
}

void SceneManager::InterpolatePosition(float alpha)
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->InterpolatePosition(alpha);
    }
}

void SceneManager::ResetInterpolation()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->ResetInterpolation();
    }
}

void SceneManager::Render()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Render();
    }
}

// ��׶��忡�� �� �ε� ���¸� Ȯ���ϰ� ��ȯ�ϴ� �Լ�
bool SceneManager::PollAsyncSceneLoading()
{
    // �񵿱� �ε��� �Ϸ�Ǿ����� 0ms ����Ͽ� ���� Ȯ��
    if (m_futureScene.valid() &&
        m_futureScene.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
    {
        return true;
    }

    return false;
}
