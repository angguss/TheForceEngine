#pragma once
//////////////////////////////////////////////////////////////////////
// The Force Engine Script System
//////////////////////////////////////////////////////////////////////

#include <TFE_System/types.h>
#include <vector>
struct GameObject;
struct Logic;
struct EnemyGenerator;
class Player;

enum DamageType
{
	DMG_SHOT = 0,
	DMG_FIST,
	DMG_EXPLOSION,
};

namespace TFE_LogicSystem
{
	bool init(Player* player);
	void shutdown();

	void update();

	void clearObjectLogics();
	bool registerObjectLogics(GameObject* gameObject, const std::vector<Logic>& logics, const std::vector<EnemyGenerator>& generators);

	void damageObject(GameObject* gameObject, s32 damage, DamageType type = DMG_SHOT);
	void sendPlayerCollisionTrigger(const GameObject* gameObject);
}
