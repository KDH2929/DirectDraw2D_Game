#include "stdafx.h"
#include "../DDrawLib/DDrawDevice.h"
#include "GameOverScene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <windows.h>
#include <string>

GameOverScene::GameOverScene(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice)
    , m_hWnd(hWnd)
    , m_pTitleButton(nullptr)
    , m_pExitButton(nullptr)
    , m_pTitleText(nullptr)
{
}

GameOverScene::~GameOverScene()
{
    Cleanup();
}

void GameOverScene::Initialize()
{
    // ȭ�� ũ�⸦ DirectDraw ��ġ���� ������
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    // ��ư�� ũ�⸦ ȭ�� ũ���� ������ ���� (ȭ�� �ʺ��� 1/4, ������ 1/10)
    int buttonWidth = screenWidth / 4;
    int buttonHeight = screenHeight / 10;

    // Ÿ��Ʋ �ؽ�Ʈ ���̴� ȭ�� ������ 1/6
    int titleWidth = screenWidth / 2;
    int titleHeight = screenHeight / 6;

    // ȭ�� �߾� ��ǥ ���
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // ��ư ��ġ�� �Ʒ��� �̵���Ű�� ���� ������
    int verticalOffset = 50;

    // Title ��ư ���� ��� (�Ʒ��� offset ����)
    RECT titleButtonRect;
    titleButtonRect.left = centerX - buttonWidth / 2;
    titleButtonRect.top = centerY - buttonHeight - 10 + verticalOffset;
    titleButtonRect.right = centerX + buttonWidth / 2;
    titleButtonRect.bottom = centerY - 10 + verticalOffset;

    // Exit ��ư ���� ��� (�Ʒ��� offset ����)
    RECT exitButtonRect;
    exitButtonRect.left = centerX - buttonWidth / 2;
    exitButtonRect.top = centerY + 10 + verticalOffset;
    exitButtonRect.right = centerX + buttonWidth / 2;
    exitButtonRect.bottom = centerY + buttonHeight + 10 + verticalOffset;

    // Ÿ��Ʋ �ؽ�Ʈ ���� ��� 
    // Title ��ư ���� 40�ȼ� ���� ����
    RECT titleRect;
    titleRect.left = centerX - titleWidth / 2;
    titleRect.right = centerX + titleWidth / 2;
    titleRect.bottom = titleButtonRect.top - 40;
    titleRect.top = titleRect.bottom - titleHeight;

    // Title ��ư ���� �� �ݹ� ����
    m_pTitleButton = new UI_Button(L"Return to Title", titleButtonRect);
    m_pTitleButton->SetClickCallback([]() {
        SceneManager::GetInstance()->ChangeScene(SceneType::Title);
        });

    // ��ư ���ڻ� ���
    m_pTitleButton->SetTextColor(RGB(255, 255, 255));
    // ��ư ������ ��ο� ȸ��, �׵θ����� ������� ����
    m_pTitleButton->SetBackgroundColor(RGB(64, 64, 64));
    m_pTitleButton->SetBorderColor(RGB(255, 255, 255));

    // Exit ��ư ���� �� �ݹ� ����
    m_pExitButton = new UI_Button(L"Exit", exitButtonRect);
    m_pExitButton->SetClickCallback([]() {
        PostQuitMessage(0);
        });

    // ��ư ���ڻ� ���
    m_pExitButton->SetTextColor(RGB(255, 255, 255));
    // Exit ��ư�� ������ �������� ����
    m_pExitButton->SetBackgroundColor(RGB(64, 64, 64));
    m_pExitButton->SetBorderColor(RGB(255, 255, 255));

    m_pTitleText = new UI_Text(L"Game Over", titleRect);
    m_pTitleText->SetTextColor(RGB(255, 255, 255));

    // �ʱ� ��Ʈ ũ�� ���� (��ư�� ��ư ������ 1/2, Ÿ��Ʋ�� Ÿ��Ʋ ������ 1/2)
    m_pTitleButton->SetFontSize(buttonHeight / 2);
    m_pExitButton->SetFontSize(buttonHeight / 2);
    m_pTitleText->SetFontSize(titleHeight / 2);
}

void GameOverScene::Cleanup()
{
    if (m_pTitleButton) {
        delete m_pTitleButton;
        m_pTitleButton = nullptr;
    }
    if (m_pExitButton) {
        delete m_pExitButton;
        m_pExitButton = nullptr;
    }
    if (m_pTitleText) {
        delete m_pTitleText;
        m_pTitleText = nullptr;
    }
}

void GameOverScene::Update(float deltaTime)
{
    // ȭ�� ũ�� ��������
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    // ������ UI: ��ư ũ��� ����, ��ġ�� �������� ������Ʈ
    int buttonWidth = screenWidth / 4;
    int buttonHeight = screenHeight / 10;
    int titleWidth = screenWidth / 2;
    int titleHeight = screenHeight / 6; // Ÿ��Ʋ �ؽ�Ʈ ����

    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // ��ư ��ġ�� �Ʒ��� �̵���Ű�� ���� ������
    int verticalOffset = 50;

    // title ��ư ��ġ: ȭ�� �߾ӿ��� ���� (���� 10 �ȼ�, offset ����)
    RECT titleButtonRect;
    titleButtonRect.left = centerX - buttonWidth / 2;
    titleButtonRect.top = centerY - buttonHeight - 10 + verticalOffset;
    titleButtonRect.right = centerX + buttonWidth / 2;
    titleButtonRect.bottom = centerY - 10 + verticalOffset;

    // Exit ��ư ��ġ: ȭ�� �߾ӿ��� �Ʒ��� (���� 10 �ȼ�, offset ����)
    RECT exitButtonRect;
    exitButtonRect.left = centerX - buttonWidth / 2;
    exitButtonRect.top = centerY + 10 + verticalOffset;
    exitButtonRect.right = centerX + buttonWidth / 2;
    exitButtonRect.bottom = centerY + buttonHeight + 10 + verticalOffset;

    // Ÿ��Ʋ �ؽ�Ʈ ��ġ ������Ʈ
    // Start ��ư ���� 40�ȼ� ���� ����
    RECT titleRect;
    titleRect.left = centerX - titleWidth / 2;
    titleRect.right = centerX + titleWidth / 2;
    titleRect.bottom = titleButtonRect.top - 40;
    titleRect.top = titleRect.bottom - titleHeight;

    // ��ư �� Ÿ��Ʋ�� ��ġ�� ��Ʈ ũ�� ������Ʈ
    if (m_pTitleButton) {
        m_pTitleButton->SetRect(titleButtonRect);
        m_pTitleButton->SetFontSize(buttonHeight / 2);
        m_pTitleButton->Update(deltaTime);
    }
    if (m_pExitButton) {
        m_pExitButton->SetRect(exitButtonRect);
        m_pExitButton->SetFontSize(buttonHeight / 2);
        m_pExitButton->Update(deltaTime);
    }
    if (m_pTitleText) {
        m_pTitleText->SetRect(titleRect);
        m_pTitleText->SetFontSize(titleHeight / 2);
        m_pTitleText->Update(deltaTime);
    }

    InputManager::GetInstance().Update(deltaTime);
}

void GameOverScene::Render()
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

        if (m_pTitleText)
            m_pTitleText->Render(hdc);
        if (m_pTitleButton)
            m_pTitleButton->Render(hdc);
        if (m_pExitButton)
            m_pExitButton->Render(hdc);

        m_pDrawDevice->EndGDI(hdc);
    }
    m_pDrawDevice->OnDraw();
}

void GameOverScene::ResetInterpolation()
{
    return;
}

void GameOverScene::InterpolatePosition(float alpha)
{
    return;
}

const SceneType GameOverScene::GetSceneType()
{
    return SceneType::Title;
}
