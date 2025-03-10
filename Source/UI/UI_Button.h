#pragma once
#include "UI.h"
#include <string>
#include <functional>
#include <windows.h>


// ����, �׵θ� ����, �ؽ�Ʈ ����, ��Ʈ ũ�⸦ ��������
class UI_Button : public UI {
public:
    // Ŭ�� �� ������ �ݹ� �Լ�
    using ClickCallback = std::function<void()>;

    UI_Button(const std::wstring& text, const RECT& rect);
    virtual ~UI_Button() = default;

    virtual bool Initialize() override { return true; }
    virtual void Update(float deltaTime) override;
    virtual void Render(HDC hdc) override;

    // Ŭ�� �� �ݹ� �Լ� ����
    void SetClickCallback(ClickCallback callback);

    // ��ư ���� ������Ʈ
    void SetRect(const RECT& rect) { m_rect = rect; }
    const RECT& GetRect() const { return m_rect; }

    // �߰� �Ӽ� ���� �Լ���
    void SetBackgroundColor(COLORREF color) { m_bgColor = color; }
    void SetBorderColor(COLORREF color) { m_borderColor = color; }
    void SetTextColor(COLORREF color) { m_textColor = color; }
    void SetFontSize(int size) { m_fontSize = size; }

private:
    std::wstring m_text;
    RECT m_rect;
    ClickCallback m_clickCallback;

    // �⺻ ���� �� ��Ʈ ũ�� (�ʱⰪ ����)
    COLORREF m_bgColor = RGB(180, 180, 180);
    COLORREF m_borderColor = RGB(0, 0, 0);
    COLORREF m_textColor = RGB(0, 0, 0);
    int m_fontSize = 14;
};
