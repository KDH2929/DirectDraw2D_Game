#include "stdafx.h"
#include "../DDrawLib/DDrawDevice.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <windows.h>
#include <string>

TitleScene::TitleScene(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice)
    , m_hWnd(hWnd)
    , m_pStartButton(nullptr)
    , m_pExitButton(nullptr)
    , m_pTitleText(nullptr)
{
}

TitleScene::~TitleScene()
{
    Cleanup();
}

void TitleScene::Initialize()
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

    // Start ��ư ���� ��� (�Ʒ��� offset ����)
    RECT startButtonRect;
    startButtonRect.left = centerX - buttonWidth / 2;
    startButtonRect.top = centerY - buttonHeight - 10 + verticalOffset;
    startButtonRect.right = centerX + buttonWidth / 2;
    startButtonRect.bottom = centerY - 10 + verticalOffset;

    // Exit ��ư ���� ��� (�Ʒ��� offset ����)
    RECT exitButtonRect;
    exitButtonRect.left = centerX - buttonWidth / 2;
    exitButtonRect.top = centerY + 10 + verticalOffset;
    exitButtonRect.right = centerX + buttonWidth / 2;
    exitButtonRect.bottom = centerY + buttonHeight + 10 + verticalOffset;

    // Ÿ��Ʋ �ؽ�Ʈ ���� ��� 
    // Start ��ư ���� 40�ȼ� ���� ����
    RECT titleRect;
    titleRect.left = centerX - titleWidth / 2;
    titleRect.right = centerX + titleWidth / 2;
    titleRect.bottom = startButtonRect.top - 40;
    titleRect.top = titleRect.bottom - titleHeight;

    // Start ��ư ���� �� �ݹ� ����
    m_pStartButton = new UI_Button(L"Start", startButtonRect);
    m_pStartButton->SetClickCallback([]() {
        SceneManager::GetInstance()->ChangeScene(SceneType::Stage1);
        });

    // ��ư ���ڻ� ���
    m_pStartButton->SetTextColor(RGB(255, 255, 255));
    // ��ư ������ ��ο� ȸ��, �׵θ����� ������� ����
    m_pStartButton->SetBackgroundColor(RGB(64, 64, 64));
    m_pStartButton->SetBorderColor(RGB(255, 255, 255));

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

    // Ÿ��Ʋ �ؽ�Ʈ ���� ("Demo Game")
    m_pTitleText = new UI_Text(L"Demo Game", titleRect);
    // Ÿ��Ʋ �ؽ�Ʈ ���
    m_pTitleText->SetTextColor(RGB(255, 255, 255));

    // �ʱ� ��Ʈ ũ�� ���� (��ư�� ��ư ������ 1/2, Ÿ��Ʋ�� Ÿ��Ʋ ������ 1/2)
    m_pStartButton->SetFontSize(buttonHeight / 2);
    m_pExitButton->SetFontSize(buttonHeight / 2);
    m_pTitleText->SetFontSize(titleHeight / 2);
}

void TitleScene::Cleanup()
{
    if (m_pStartButton) {
        delete m_pStartButton;
        m_pStartButton = nullptr;
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

void TitleScene::Update(float deltaTime)
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

    // Start ��ư ��ġ: ȭ�� �߾ӿ��� ���� (���� 10 �ȼ�, offset ����)
    RECT startRect;
    startRect.left = centerX - buttonWidth / 2;
    startRect.top = centerY - buttonHeight - 10 + verticalOffset;
    startRect.right = centerX + buttonWidth / 2;
    startRect.bottom = centerY - 10 + verticalOffset;

    // Exit ��ư ��ġ: ȭ�� �߾ӿ��� �Ʒ��� (���� 10 �ȼ�, offset ����)
    RECT exitRect;
    exitRect.left = centerX - buttonWidth / 2;
    exitRect.top = centerY + 10 + verticalOffset;
    exitRect.right = centerX + buttonWidth / 2;
    exitRect.bottom = centerY + buttonHeight + 10 + verticalOffset;

    // Ÿ��Ʋ �ؽ�Ʈ ��ġ ������Ʈ
    // Start ��ư ���� 40�ȼ� ���� ����
    RECT titleRect;
    titleRect.left = centerX - titleWidth / 2;
    titleRect.right = centerX + titleWidth / 2;
    titleRect.bottom = startRect.top - 40;
    titleRect.top = titleRect.bottom - titleHeight;

    // ��ư �� Ÿ��Ʋ�� ��ġ�� ��Ʈ ũ�� ������Ʈ
    if (m_pStartButton) {
        m_pStartButton->SetRect(startRect);
        m_pStartButton->SetFontSize(buttonHeight / 2);
        m_pStartButton->Update(deltaTime);
    }
    if (m_pExitButton) {
        m_pExitButton->SetRect(exitRect);
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

void TitleScene::Render()
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
        if (m_pStartButton)
            m_pStartButton->Render(hdc);
        if (m_pExitButton)
            m_pExitButton->Render(hdc);

        m_pDrawDevice->EndGDI(hdc);
    }
    m_pDrawDevice->OnDraw();
}

void TitleScene::ResetInterpolation()
{
    return;
}

void TitleScene::InterpolatePosition(float alpha)
{
    return;
}

const SceneType TitleScene::GetSceneType()
{
    return SceneType::Title;
}
