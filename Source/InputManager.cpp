#include "stdafx.h"
#include "InputManager.h"
#include "DebugManager.h"

void InputManager::OnKeyDown(UINT nChar) {
    if (nChar < 256)
        m_keyStates[nChar] = true;
}

void InputManager::OnKeyUp(UINT nChar) {
    if (nChar < 256)
        m_keyStates[nChar] = false;
}
bool InputManager::IsKeyDown(UINT nChar) const {
    return (nChar < 256) ? m_keyStates[nChar] : false;
}

bool InputManager::IsKeyPressed(UINT nChar) const {
    if (nChar >= 256)
        return false;

    // ���Ӱ� ������ �ľ�
    return m_keyStates[nChar] && !m_prevKeyStates[nChar];
}

void InputManager::Update(float deltaTime) {

    // ���� �����ӿ��� �ʱ�ȭó��
    // ���� ĳ���� Update -> InputMangaer Update ������ ó������ �����ϸ� ��

    m_leftKeyDoublePressed = false;
    m_rightKeyDoublePressed = false;


    // ���� Ű(VK_LEFT) ���� �Է� Ȯ��
    if (IsKeyPressed(VK_LEFT)) {

        if (m_leftKeyComboInputActive  && m_leftKeyTimer < DOUBLE_PRESS_THRESHOLD)
        {
            m_leftKeyComboInputActive = false;
            m_leftKeyDoublePressed = true;
        }

        else
        {
            m_leftKeyDoublePressed = false;
        }

        m_leftKeyComboInputActive = true;
        m_leftKeyTimer = 0.0f;
    }


    else {
        m_leftKeyTimer += deltaTime;

        if (m_leftKeyTimer >= DOUBLE_PRESS_THRESHOLD)
        {
            m_leftKeyComboInputActive = false;
            m_leftKeyDoublePressed = false;
        }
    }


    // ���� Ű(VK_RIGHT) ���� �Է� Ȯ��
    if (IsKeyPressed(VK_RIGHT)) {

        if (m_rightKeyComboInputActive && m_rightKeyTimer < DOUBLE_PRESS_THRESHOLD)
        {
            m_rightKeyComboInputActive = false;
            m_rightKeyDoublePressed = true;
        }

        else
        {
            m_rightKeyDoublePressed = false;
        }

        m_rightKeyComboInputActive = true;
        m_rightKeyTimer = 0.0f;
    }


    else {
        m_rightKeyTimer += deltaTime;

        if (m_rightKeyTimer >= DOUBLE_PRESS_THRESHOLD)
        {
            m_rightKeyComboInputActive = false;
            m_rightKeyDoublePressed = false;
        }
    }




    // ���� �������� Ű ���� ������Ʈ
    for (int i = 0; i < 256; ++i)
    {
        m_prevKeyStates[i] = m_keyStates[i];
    }

    // ���� �������� ���콺 ���� ����
    for (int i = 0; i < 3; ++i)
    {
        m_prevMouseButtonStates[i] = m_mouseButtonStates[i];
    }

}


bool InputManager::IsDoublePressedLeft() const {
    return m_leftKeyDoublePressed;
}

bool InputManager::IsDoublePressedRight() const {
    return m_rightKeyDoublePressed;
}

void InputManager::OnMouseMove(int x, int y)
{
    m_mouseX = x;
    m_mouseY = y;
}

void InputManager::OnMouseDown(MouseButton button)
{
    int index = static_cast<int>(button);
    if (index < 3) {
        m_mouseButtonStates[index] = true;
    }
}

void InputManager::OnMouseUp(MouseButton button)
{
    int index = static_cast<int>(button);
    if (index < 3) {
        m_mouseButtonStates[index] = false;
    }
}

int InputManager::GetMouseX() const
{
    return m_mouseX;
}

int InputManager::GetMouseY() const {
    return m_mouseY;
}

bool InputManager::IsMouseButtonDown(MouseButton button) const {
    int index = static_cast<int>(button);
    return (index < 3) ? m_mouseButtonStates[index] : false;
}

bool InputManager::IsMouseButtonUp(MouseButton button) const
{
    int index = static_cast<int>(button);
    return (index < 3) ? !m_mouseButtonStates[index] : false;
}

bool InputManager::IsMouseButtonReleased(MouseButton button) const
{
    int index = static_cast<int>(button);
    return (index < 3) ? (!m_mouseButtonStates[index] && m_prevMouseButtonStates[index]) : false;
}
