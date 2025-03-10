#pragma once


// 32��Ʈ ��Ʈ����ũ ����� �浹 ���̾ ���� Ÿ�� (����Ƽ�� Layer����)
using CollisionLayer = unsigned int;

// ��� ���̾�� �浹�ϵ��� �ϴ� �⺻ ����ũ
const CollisionLayer DEFAULT_COLLISION_LAYER_MASK = 0xFFFFFFFF;
const CollisionLayer NO_COLLISION_LAYER_MASK = 0x00000000;

// �� ���̾�
const CollisionLayer DEFAULT_COLLISION_LAYER = 1 << 0;         // (0b00...000001)
const CollisionLayer OVERLAP_COLLISION_LAYER = 1 << 1;         // (0b00...000010)
const CollisionLayer CHARACTER_COLLISION_LAYER = 1 << 2;     // (0b00...000100)       
const CollisionLayer TILE_COLLISION_LAYER = 1 << 3;                // (0b00...001000)
const CollisionLayer MONSTER_COLLISION_LAYER = 1 << 4;