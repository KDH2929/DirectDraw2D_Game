#include "stdafx.h"
#include "UI_Button.h"
#include "InputManager.h"
#include "DebugManager.h"
#include <windowsx.h>

UI_Button::UI_Button(const std::wstring& text, const RECT& rect)
    : m_text(text), m_rect(rect)
{
}

void UI_Button::Update(float deltaTime)
{
    int mouseX = InputManager::GetInstance().GetMouseX();
    int mouseY = InputManager::GetInstance().GetMouseY();
    bool mouseReleased = InputManager::GetInstance().IsMouseButtonReleased(MouseButton::Left);

    // ��ư ���� �ȿ� ���콺 ��ǥ�� �ְ�, Ŭ���Ǿ����� �ݹ� ȣ��
    if (mouseReleased &&
        mouseX >= m_rect.left && mouseX <= m_rect.right &&
        mouseY >= m_rect.top && mouseY <= m_rect.bottom)
    {
        if (m_clickCallback)
        {
            m_clickCallback();
        }
    }
}

void UI_Button::Render(HDC hdc)
{
    // ��� �׸���: ������ ���� ���
    HBRUSH hBrush = CreateSolidBrush(m_bgColor);
    FillRect(hdc, &m_rect, hBrush);
    DeleteObject(hBrush);

    // �׵θ� �׸���: ������ �׵θ� ���� ���
    HBRUSH hBorderBrush = CreateSolidBrush(m_borderColor);
    FrameRect(hdc, &m_rect, hBorderBrush);
    DeleteObject(hBorderBrush);

    // �ؽ�Ʈ �׸���
    ::SetBkMode(hdc, TRANSPARENT);
    ::SetTextColor(hdc, m_textColor);

    // Arial ��Ʈ�� ���, m_fontSize�� ���̷� ����
    HFONT hFont = CreateFont(
        m_fontSize,                // ���� ����
        0,                         // ���� �� (0�̸� �⺻��)
        0,                         // ����
        0,                         // ȸ�� ����
        FW_NORMAL,                 // ����
        FALSE,                     // ���Ÿ�ü ����
        FALSE,                     // ���� ����
        FALSE,                     // ��Ҽ� ����
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Arial"                   // ��Ʈ �̸�
    );

    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // �ؽ�Ʈ�� �߾ӿ� �����Ͽ� �׸���
    DrawText(hdc, m_text.c_str(), -1, &m_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ���� ��Ʈ ���� �� ������ ��Ʈ ����
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void UI_Button::SetClickCallback(ClickCallback callback)
{
    m_clickCallback = callback;
}
