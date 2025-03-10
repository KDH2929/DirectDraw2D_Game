#include "stdafx.h"
#include "../DDrawLib/DDrawDevice.h"
#include "LoadingScene.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "UI_Text.h" 
#include <windows.h>
#include <string>

// Loading ���� ���� �迭 (4�ܰ�)
static const wchar_t* loadingStates[] = { L"Loading", L"Loading.", L"Loading..", L"Loading..." };
static const int numStates = sizeof(loadingStates) / sizeof(loadingStates[0]);

LoadingScene::LoadingScene(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice)
    , m_hWnd(hWnd)
    , m_pLoadingText(nullptr)
    , m_timer(0.0f)
    , m_currentState(0)
{
}

LoadingScene::~LoadingScene()
{
    Cleanup();
}

void LoadingScene::Initialize()
{
    // ȭ�� ũ�⸦ DirectDraw ��ġ���� ������
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    // Loading �ؽ�Ʈ ���� ���: ȭ�� �߾ӿ�, ���� ȭ���� 1/2, ���̴� ȭ���� 1/6
    int textWidth = screenWidth / 2;
    int textHeight = screenHeight / 6;
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    RECT textRect;
    textRect.left = centerX - textWidth / 2;
    textRect.right = centerX + textWidth / 2;
    // �ؽ�Ʈ�� �߾ӿ� ��ġ��Ű�� ����, �ؽ�Ʈ ������ �Ʒ����� �߾Ӻ��� �ణ �Ʒ��� ��ġ�ϰ�,
    // ������ �ڵ����� �����ǵ��� �� (���⼭�� �߾ӿ� ��ġ)
    textRect.top = centerY - textHeight / 2;
    textRect.bottom = textRect.top + textHeight;

    // UI_Text ����, �ʱ� ������ loadingStates[0] ("Loading")
    m_pLoadingText = new UI_Text(loadingStates[m_currentState], textRect);
    // �ؽ�Ʈ ����: ���
    m_pLoadingText->SetTextColor(RGB(255, 255, 255));
    // ����� ���� (�̹� ����� ������)
    m_pLoadingText->SetBkMode(TRANSPARENT);
    // �ʱ� ��Ʈ ũ�� ����: �ؽ�Ʈ ���� ������ 1/2
    m_pLoadingText->SetFontSize(textHeight / 2);
}

void LoadingScene::Cleanup()
{
    if (m_pLoadingText) {
        delete m_pLoadingText;
        m_pLoadingText = nullptr;
    }
}

void LoadingScene::Update(float deltaTime)
{
    // ȭ�� ũ�� ������Ʈ (������ UI)
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());
    int textWidth = screenWidth / 2;
    int textHeight = screenHeight / 6;
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    RECT textRect;
    textRect.left = centerX - textWidth / 2;
    textRect.right = centerX + textWidth / 2;
    textRect.top = centerY - textHeight / 2;
    textRect.bottom = textRect.top + textHeight;

    if (m_pLoadingText) {
        m_pLoadingText->SetRect(textRect);
        m_pLoadingText->SetFontSize(textHeight / 2);
        m_pLoadingText->Update(deltaTime);
    }

    // Ÿ�̸� ����
    m_timer += deltaTime;

    const float SEC_TO_MS = 1000.0f; // 1�� = 1000ms

    if (m_timer >= 1.0f * SEC_TO_MS)
    {
        m_timer -= 1.0f * SEC_TO_MS;
        m_currentState = (m_currentState + 1) % numStates;
        // UI_Text�� �ؽ�Ʈ ������Ʈ
        if (m_pLoadingText)
        {
            m_pLoadingText->SetText(loadingStates[m_currentState]);
        }
    }
}

void LoadingScene::Render()
{
    // DirectDraw�� BackBuffer�� ������ ���ϸ� ȭ�� ������ ���� ����    
    HDC hdc = nullptr;
    if (m_pDrawDevice->BeginGDI(&hdc))
    {
        // ����� ���������� ä��
        int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
        int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());
        RECT fullRect = { 0, 0, screenWidth, screenHeight };
        HBRUSH hBackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &fullRect, hBackBrush);
        DeleteObject(hBackBrush);

        if (m_pLoadingText)
            m_pLoadingText->Render(hdc);

        m_pDrawDevice->EndGDI(hdc);
    }
    m_pDrawDevice->OnDraw();
}

void LoadingScene::ResetInterpolation()
{
    return;
}

void LoadingScene::InterpolatePosition(float alpha)
{
    return;
}

const SceneType LoadingScene::GetSceneType()
{
    return SceneType::Loading;
}
