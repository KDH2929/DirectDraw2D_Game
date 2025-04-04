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
    // 화면 크기를 DirectDraw 장치에서 가져옴
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    // 버튼의 크기를 화면 크기의 비율로 정함 (화면 너비의 1/4, 높이의 1/10)
    int buttonWidth = screenWidth / 4;
    int buttonHeight = screenHeight / 10;

    // 타이틀 텍스트 높이는 화면 높이의 1/6
    int titleWidth = screenWidth / 2;
    int titleHeight = screenHeight / 6;

    // 화면 중앙 좌표 계산
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // 버튼 위치를 아래로 이동시키기 위한 오프셋
    int verticalOffset = 50;

    // Title 버튼 영역 계산 (아래로 offset 적용)
    RECT titleButtonRect;
    titleButtonRect.left = centerX - buttonWidth / 2;
    titleButtonRect.top = centerY - buttonHeight - 10 + verticalOffset;
    titleButtonRect.right = centerX + buttonWidth / 2;
    titleButtonRect.bottom = centerY - 10 + verticalOffset;

    // Exit 버튼 영역 계산 (아래로 offset 적용)
    RECT exitButtonRect;
    exitButtonRect.left = centerX - buttonWidth / 2;
    exitButtonRect.top = centerY + 10 + verticalOffset;
    exitButtonRect.right = centerX + buttonWidth / 2;
    exitButtonRect.bottom = centerY + buttonHeight + 10 + verticalOffset;

    // 타이틀 텍스트 영역 계산 
    // Title 버튼 위에 40픽셀 간격 적용
    RECT titleRect;
    titleRect.left = centerX - titleWidth / 2;
    titleRect.right = centerX + titleWidth / 2;
    titleRect.bottom = titleButtonRect.top - 40;
    titleRect.top = titleRect.bottom - titleHeight;

    // Title 버튼 생성 및 콜백 설정
    m_pTitleButton = new UI_Button(L"Return to Title", titleButtonRect);
    m_pTitleButton->SetClickCallback([]() {
        SceneManager::GetInstance()->ChangeScene(SceneType::Title);
        });

    // 버튼 글자색 흰색
    m_pTitleButton->SetTextColor(RGB(255, 255, 255));
    // 버튼 배경색을 어두운 회색, 테두리색을 흰색으로 설정
    m_pTitleButton->SetBackgroundColor(RGB(64, 64, 64));
    m_pTitleButton->SetBorderColor(RGB(255, 255, 255));

    // Exit 버튼 생성 및 콜백 설정
    m_pExitButton = new UI_Button(L"Exit", exitButtonRect);
    m_pExitButton->SetClickCallback([]() {
        PostQuitMessage(0);
        });

    // 버튼 글자색 흰색
    m_pExitButton->SetTextColor(RGB(255, 255, 255));
    // Exit 버튼도 동일한 색상으로 설정
    m_pExitButton->SetBackgroundColor(RGB(64, 64, 64));
    m_pExitButton->SetBorderColor(RGB(255, 255, 255));

    m_pTitleText = new UI_Text(L"Game Over", titleRect);
    m_pTitleText->SetTextColor(RGB(255, 255, 255));

    // 초기 폰트 크기 설정 (버튼은 버튼 높이의 1/2, 타이틀은 타이틀 높이의 1/2)
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
    // 화면 크기 가져오기
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    // 반응형 UI: 버튼 크기는 고정, 위치만 동적으로 업데이트
    int buttonWidth = screenWidth / 4;
    int buttonHeight = screenHeight / 10;
    int titleWidth = screenWidth / 2;
    int titleHeight = screenHeight / 6; // 타이틀 텍스트 높이

    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // 버튼 위치를 아래로 이동시키기 위한 오프셋
    int verticalOffset = 50;

    // title 버튼 위치: 화면 중앙에서 위쪽 (간격 10 픽셀, offset 적용)
    RECT titleButtonRect;
    titleButtonRect.left = centerX - buttonWidth / 2;
    titleButtonRect.top = centerY - buttonHeight - 10 + verticalOffset;
    titleButtonRect.right = centerX + buttonWidth / 2;
    titleButtonRect.bottom = centerY - 10 + verticalOffset;

    // Exit 버튼 위치: 화면 중앙에서 아래쪽 (간격 10 픽셀, offset 적용)
    RECT exitButtonRect;
    exitButtonRect.left = centerX - buttonWidth / 2;
    exitButtonRect.top = centerY + 10 + verticalOffset;
    exitButtonRect.right = centerX + buttonWidth / 2;
    exitButtonRect.bottom = centerY + buttonHeight + 10 + verticalOffset;

    // 타이틀 텍스트 위치 업데이트
    // Start 버튼 위에 40픽셀 간격 적용
    RECT titleRect;
    titleRect.left = centerX - titleWidth / 2;
    titleRect.right = centerX + titleWidth / 2;
    titleRect.bottom = titleButtonRect.top - 40;
    titleRect.top = titleRect.bottom - titleHeight;

    // 버튼 및 타이틀의 위치와 폰트 크기 업데이트
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
    // DirectDraw의 BackBuffer에 렌더을 안하면 화면 깜빡임 문제 생김    
    HDC hdc = nullptr;
    if (m_pDrawDevice->BeginGDI(&hdc))
    {
        // 배경을 검정색으로 채움
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
