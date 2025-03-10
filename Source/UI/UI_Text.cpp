#include "stdafx.h"
#include "UI_Text.h"
#include <windows.h>

UI_Text::UI_Text(const std::wstring& text, const RECT& rect)
    : m_text(text), m_rect(rect)
{
}

void UI_Text::Update(float deltaTime)
{

}

void UI_Text::Render(HDC hdc)
{
    // ��� ��� ���� �� �ؽ�Ʈ ���� ����
    // hdc�� ���ڷ� �Ѱܾ� �Կ� ����
    // Windows API �Լ��� ȣ���� �� ���� ���� ������(::)�� ����Ͽ� ȣ�� �� �� (���� �̸��� �Լ��� ����)
    ::SetBkMode(hdc, m_bkMode);
    ::SetTextColor(hdc, m_textColor);

    // ���� OPAQUE ���� �ؽ�Ʈ ����� ǥ���ϰ� �ʹٸ�, ��� ���� ����
    if (m_bkMode == OPAQUE) {
        SetBkColor(hdc, m_textBkColor);
    }

    // ��Ʈ ����: m_fontSize�� ����Ͽ� ��Ʈ ũ�⸦ ����
    HFONT hFont = CreateFont(
        m_fontSize,                // ���� ����
        0,                         // ���� �� (0�̸� �⺻��)
        0,                         // ����
        0,                         // ȸ�� ����
        FW_NORMAL,                 // ����
        FALSE,                     // ���Ÿ� ����
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

    // �ؽ�Ʈ ������: m_rect ������ �ؽ�Ʈ�� �׸���
    // DT_CENTER (���� �߾�), DT_VCENTER (���� �߾�)
    DrawText(hdc, m_text.c_str(), -1, &m_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ���� ��Ʈ ���� �� ������ ��Ʈ ����
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}
