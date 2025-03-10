#pragma once

#include "Scene.h"
#include "TileMap2D.h"
#include "Character.h"
#include "Monster.h"
#include "Portal.h"
#include "DeathOverlapBox.h"
#include <vector>
#include <windows.h>
#include <memory>

// ���� ����
class CTGAImage;
class CImageData;
class CDDrawDevice;


class Stage2Scene : public Scene
{
public:
    Stage2Scene(CDDrawDevice* pDrawDevice, HWND hWnd);
    virtual ~Stage2Scene();

    virtual void Initialize() override;
    virtual void Cleanup() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    virtual void ResetInterpolation() override;
    virtual void InterpolatePosition(float alpha) override;

    virtual const SceneType GetSceneType() override;

private:
    void UpdateCamera(int screenWidth, int screenHeight);

private:
    // DirectDraw ��ġ �� ������ �ڵ�
    CDDrawDevice* m_pDrawDevice;
    HWND m_hWnd;

    // �̹��� ���ҽ���
    CImageData* m_pPlayerImgData = nullptr;
    CImageData* m_pCentipedeImgData = nullptr;
    CTGAImage* m_pBackgroundImage = nullptr;

    // �ش� Ŭ������ ImgData�� ����
    // �����Ҵ������� ���ص� �˾Ƽ� ��ü�� ������ �Ҵ������� ����
    std::unique_ptr<CImageData> m_pPortalImgData = nullptr;

    // Ÿ�ϸ�
    TileMap2D* m_TileMap = nullptr;

    // ��� �̹��� ũ��
    int m_bgWidth = 0;
    int m_bgHeight = 0;

    // ī�޶� ó���� ������
    int m_cameraOffsetX = 0;
    int m_cameraOffsetY = 0;
    int m_playerRenderX = 0;
    int m_playerRenderY = 0;
    int m_backgroundPosX = 0;
    int m_backgroundPosY = 0;


    // ���� ������Ʈ:
    Character* m_pPlayer = nullptr;
    std::unique_ptr<Portal> m_pPortal = nullptr;
    std::vector<Monster*> m_vMonsters;
    std::unique_ptr<DeathOverlapBox> m_pDeathOverlapBox = nullptr;

};
