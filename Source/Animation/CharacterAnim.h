#pragma once
#include "SpriteAnim.h"


enum class CharacterAnimState {
    Idle,
    Run,
    Attack,       // 2 Combo Attack
    Death,
    Hurt,
    Jump,        // Jump
    UpToFall,    // Jump �� �������� ����
    Fall,           // ���� Fall
    EdgeGrab,
    EdgeIdle,
    WallSlide,
    Crouch,
    Dash,
    DashAttack,
    Slide,
    LadderGrab
};


using CharacterAnim = SpriteAnim<CharacterAnimState>;