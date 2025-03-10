#pragma once

#include <windows.h>

// �߻�Ŭ����

class UI {
public:
    UI() = default;
    virtual ~UI() = default;

    virtual bool Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(HDC hdc) = 0;
};