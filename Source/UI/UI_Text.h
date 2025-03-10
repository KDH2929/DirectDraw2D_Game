#pragma once

#include "UI.h"
#include <string>
#include <windows.h>

class UI_Text : public UI {
public:
    // Rect�� ���� ��Ʈ ��ġ����
    // Rect �ȿ� ��Ʈ�� �׸��� ���
    UI_Text(const std::wstring& text, const RECT& rect);
    virtual ~UI_Text() = default;

    virtual bool Initialize() override { return true; }
    virtual void Update(float deltaTime) override;
    virtual void Render(HDC hdc) override;

    // �ؽ�Ʈ ����
    void SetText(const std::wstring& text) { m_text = text; }
    const std::wstring& GetText() const { return m_text; }

    // ��ġ/ũ�� ����
    void SetRect(const RECT& rect) { m_rect = rect; }
    const RECT& GetRect() const { return m_rect; }

    // �ؽ�Ʈ ���� ���� (�⺻���� ������)
    void SetTextColor(COLORREF color) { m_textColor = color; }
    COLORREF GetTextColor() const { return m_textColor; }

    // �ؽ�Ʈ ��� ���� ���� (�⺻���� ���� ó��; ���� ����� ����Ϸ��� OPAQUE ���� ����)
    void SetTextBackgroundColor(COLORREF color) { m_textBkColor = color; }
    COLORREF GetTextBackgroundColor() const { return m_textBkColor; }

    // ��� ��� ���� (�⺻���� TRANSPARENT)
    void SetBkMode(int mode) { m_bkMode = mode; }
    int GetBkMode() const { return m_bkMode; }

    // ��Ʈ ũ�� ���� (�⺻���� 14)
    void SetFontSize(int fontSize) { m_fontSize = fontSize; }
    int GetFontSize() const { return m_fontSize; }

private:
    std::wstring m_text;
    RECT m_rect;
    COLORREF m_textColor = RGB(0, 0, 0); // �⺻ �ؽ�Ʈ ����: ����
    COLORREF m_textBkColor = RGB(0, 0, 0); // �⺻ �ؽ�Ʈ ��� ���� (������ ����Ϸ��� OPAQUE ���� �Բ� ���)
    int m_bkMode = TRANSPARENT;          // �⺻ ��� ���: ����
    int m_fontSize = 14;                 // �⺻ ��Ʈ ũ��
};
