#pragma once
#include <windows.h>

enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2
};

class InputManager {
public:
    static InputManager& GetInstance() {
        static InputManager instance;
        return instance;
    }

    // Ű�Է� ����
    void OnKeyDown(UINT nChar);
    void OnKeyUp(UINT nChar);
    bool IsKeyDown(UINT nChar) const;
    bool IsKeyPressed(UINT nChar) const;

    // �¿�Ű�� ���� double press ���� ��ȯ
    bool IsDoublePressedLeft() const;
    bool IsDoublePressedRight() const;

    // ���콺 �Է� ó��
    void OnMouseMove(int x, int y);
    void OnMouseDown(MouseButton button);   // button: 0-��Ŭ��, 1-��Ŭ��, 2-��
    void OnMouseUp(MouseButton button);


    // ���콺 ���� Ȯ�� �޼���
    int GetMouseX() const;
    int GetMouseY() const;
    bool IsMouseButtonDown(MouseButton button) const;
    bool IsMouseButtonUp(MouseButton button) const;
    bool IsMouseButtonReleased(MouseButton button) const;       // "������ ��������" Ȯ���ϴ� �Լ�


    void Update(float deltaTime);

private:
    InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // Ű �Է� ����
    bool m_keyStates[256] = { false };
    bool m_prevKeyStates[256] = { false };

    // ���콺 �Է� ����
    int m_mouseX = 0;
    int m_mouseY = 0;
    bool m_mouseButtonStates[3] = { false, false, false };
    bool m_prevMouseButtonStates[3] = { false, false, false };


    // �¿� Ű�� ���� double press Ÿ�̸� (ms ����)
    float m_leftKeyTimer = 0.0f;
    float m_rightKeyTimer = 0.0f;

    
    bool m_leftKeyComboInputActive = false;
    bool m_rightKeyComboInputActive = false;

    bool m_leftKeyDoublePressed = false;
    bool m_rightKeyDoublePressed = false;


    // �����Է� �Ӱ�ġ (ms ����)
    static constexpr float DOUBLE_PRESS_THRESHOLD = 200.0f;   
};
