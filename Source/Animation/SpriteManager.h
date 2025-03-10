#pragma once
#include <vector>
#include <Windows.h>
#include <memory>
#include "../ImageData/ImageData.h"

struct SpriteFrame {
    RECT srcRect;           // ��������Ʈ ��Ʈ ������ �ش� �������� ����
};

class SpriteManager {
public:
    SpriteManager();
    ~SpriteManager();

    bool SetSpriteSheet(CImageData* spriteSheet, int frameWidth, int frameHeight);
    const CImageData* GetSpriteSheet() const;     // �������Լ� : ��ü�� ��� ������ ������ �� ������, const ��ü������ ȣ�Ⱑ�� 


    // ��ü ������ �� ��ȯ
    int GetFrameCount() const;

    // ������ �ε����� ������ �ҽ� �簢�� ��ȯ
    const RECT& GetFrameRect(int index) const;

    int GetFrameWidth() const;
    int GetFrameHeight() const;


private:
    void GenerateFrames();

    CImageData* m_spriteSheet;      // ��������Ʈ ��Ʈ �̹��� ������
    int m_frameWidth;               // �� �������� �ʺ�
    int m_frameHeight;              // �� �������� ����
    std::vector<RECT> m_frames;     // ���� �������� �ҽ� �簢�� ���
};
