// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

/**
 * FEnhoneyGameplayTags
 *
 * Singleton containing native gameplay tags
 */
struct RPGDEMO_API FEnhoneyGameplayTags
{
public:
	static const FEnhoneyGameplayTags& Get() { return EnhoneyGameplayTags; }

	static void InitializeGameplayTags();

public:

#pragma region AttributeTag
	/** Attribute Tag*/

	// PrimaryAttribute
	FGameplayTag Attribute_Primary_Strength;
	FGameplayTag Attribute_Primary_Constitution;
	FGameplayTag Attribute_Primary_Intelligence;
	FGameplayTag Attribute_Primary_Defense;

	// SecondaryAttribute
	FGameplayTag Attribute_Secondary_BasicAttackPower;
	FGameplayTag Attribute_Secondary_ExtraAttackPower;
	FGameplayTag Attribute_Secondary_ExtraPercentageAttack;
	FGameplayTag Attribute_Secondary_AttackPower;

	FGameplayTag Attribute_Secondary_DefenseCapacity;

	FGameplayTag Attribute_Secondary_ElementProficiency;

	FGameplayTag Attribute_Secondary_ArmorPenetration;

	FGameplayTag Attribute_Secondary_PhysicalDamageBonus;
	FGameplayTag Attribute_Secondary_FlameDamageBonus;
	FGameplayTag Attribute_Secondary_FrostDamageBonus;
	FGameplayTag Attribute_Secondary_PhysicalDamageResistance;
	FGameplayTag Attribute_Secondary_FlameDamageResistance;
	FGameplayTag Attribute_Secondary_FrostDamageResistance;

	FGameplayTag Attribute_Secondary_CriticalChance;
	FGameplayTag Attribute_Secondary_CriticalRatio;

	FGameplayTag Attribute_Secondary_MaxHealth;
	FGameplayTag Attribute_Secondary_MaxMana;
	FGameplayTag Attribute_Secondary_MaxArcane;

	// VitalAttribute
	FGameplayTag Attribute_Vital_Health;
	FGameplayTag Attribute_Vital_Mana;
	FGameplayTag Attribute_Vital_Arcane;
#pragma endregion AttributeTag

#pragma region AbilityTag
	/** 固有主动技能*/
	// 玩家技能
	FGameplayTag Ability_Offensive_Inherent;
	FGameplayTag Ability_Offensive_Inherent_CommonAttack_Melee;			// 普通攻击--近战
	FGameplayTag Ability_Offensive_Inherent_CommonAttack_FlyKnife;		// 普通攻击--飞刀
	FGameplayTag Ability_Offensive_Inherent_CommonAttack_ShootArrow;	// 普通攻击--弩箭
	FGameplayTag Ability_Offensive_Inherent_InteractWithNPC;			// 和NPC交互

	// 敌人技能--LittleDragon
	FGameplayTag Ability_Offensive_Inherent_LittleDragon_FireBall;
	FGameplayTag Ability_Offensive_Inherent_LittleDragon_FrostBall;
	FGameplayTag Ability_Offensive_Inherent_LittleDragon_Melee;

	// 通用技能Tag
	FGameplayTag Ability_Offensive_Inherent_HitReaction;	// 受击反应

	/** 可变主动技能*/
	FGameplayTag Ability_Offensive_Variable;

	/** 固有被动技能*/
	FGameplayTag Ability_Passive_Inherent;

	/** 可变被动技能*/
	FGameplayTag Ability_Passive_Variable;

#pragma endregion AbilityTag

#pragma region AbilityDamageType
	FGameplayTag AbilityDamageType_Physic;		// 伤害类型--物理
	FGameplayTag AbilityDamageType_Flame;		// 伤害类型--火焰
	FGameplayTag AbilityDamageType_Frost;		// 伤害类型--冰霜

#pragma endregion AbilityDamageType


#pragma region AbilityCooldownTag
	FGameplayTag Cooldown_Inherent_EnemyLocking;

#pragma endregion AbilityCooldownTag

#pragma region AbilityEventTag
	FGameplayTag AbilityEventTag_CommonAttack_Melee;			// 近战普通攻击命中敌人

	FGameplayTag AbilityEventTag_ListenCombat_Enter;			// 进入连击监听状态
	FGameplayTag AbilityEventTag_ListenCombat_Quit;				// 退出连击监听状态
	FGameplayTag AbilityEventTag_ComboAttack_Reset;				// 重置是否可执行连击
	FGameplayTag AbilityEventTag_ComboAttack_ExecuteCombo;		// 执行连击

	FGameplayTag AbilityEventTag_RemoteAttack_Fire;				// 远程攻击--开火

	FGameplayTag AbilityEventTag_HitReaction_Finish;			// 受击动画播放完毕

	// LittleDragon
	FGameplayTag AbilityEventTag_MeleeCollisionEnabled_Mouth;	// 启用或关闭嘴部DamageBox的碰撞
	FGameplayTag AbilityEventTag_MeleeCollisionEnabled_Tail;	// 启用或关闭尾部DamageBox的碰撞
#pragma endregion AbilityEventTag

#pragma region WeaponTag
	FGameplayTag Weapon_CurvedKnife;	// 弯刀
	FGameplayTag Weapon_FlyKnife;		// 飞刀
	FGameplayTag Weapon_Crossbow;		// 弓弩
#pragma endregion WeaponTag

// 材料
#pragma region MaterialTag
	// 可贩卖的材料
	FGameplayTag Material_Economic_Slag;		// 矿渣
	FGameplayTag Material_Economic_DragonScale; // 龙鳞
	FGameplayTag Material_Economic_DragonEye;	// 巨龙之眼
	FGameplayTag Material_Economic_GoldBar;		// 金条


	// 贵重/武器升级材料
	FGameplayTag Material_Valuable_DragonTooth;	// 龙牙
	FGameplayTag Material_Valuable_DragonBone;	// 龙骨
	FGameplayTag Material_Valuable_Rubine;		// 红宝石
	FGameplayTag Material_Valuable_Emerald;		// 绿宝石
	FGameplayTag Material_Valuable_Sapphire;	// 蓝宝石
	FGameplayTag Material_Valuable_Amethyst;	// 紫宝石
	FGameplayTag Material_Valuable_Obsidian;	// 黑曜石
	FGameplayTag Material_Valuable_CastPopper;	// 铸铜
	FGameplayTag Material_Valuable_ColdIron;	// 寒铁
	FGameplayTag Material_Valuable_Zixi;		// 紫锡
	FGameplayTag Material_Valuable_GengJin;		// 庚金

	// 任务材料
	FGameplayTag Material_Task_Chalcedony;				// 石髓
	FGameplayTag Material_Task_DiamondRuneStone;		// 菱形符文石
	FGameplayTag Material_Task_RoundRuneStone;			// 圆形符文石
	FGameplayTag Material_Task_ElfFeather;				// 精灵之羽
	
	// 食物材料
	FGameplayTag Material_Food_BlueMushroom;		// 蓝蘑菇
	FGameplayTag Material_Food_CompoundPotion;		// 复合药剂
	FGameplayTag Material_Food_DragonVeinFruit;		// 龙脉果
	FGameplayTag Material_Food_ElfFruit;			// 精灵果
	FGameplayTag Material_Food_ForbiddenPotion;		// 禁忌药水
	FGameplayTag Material_Food_GrilledBirdLegs;		// 烤鸟腿
	FGameplayTag Material_Food_GrilledDragonMeat;	// 烤龙肉
	FGameplayTag Material_Food_HoneyPotion;			// 蜂蜜药水
	FGameplayTag Material_Food_JinGua;				// 堇瓜
	FGameplayTag Material_Food_LifePotion;			// 生命药水
	FGameplayTag Material_Food_ManaPotion;			// 魔法药水
	FGameplayTag Material_Food_Morels;				// 羊肚菌
	FGameplayTag Material_Food_Pumpkin;				// 南瓜
	FGameplayTag Material_Food_RoastedPork;			// 烤大肉
	FGameplayTag Material_Food_WoodMushroom;		// 树菌

#pragma endregion MaterialTag

	/** 敌人类型*/
#pragma region EnemyTypeTag
	FGameplayTag EnemyType_Common_LittleDragon;		// 小型龙
	FGameplayTag EnemyType_Boss_GiantDragon;		// 巨龙
	FGameplayTag EnemyType_Boss_Berserker;			// 战狂
#pragma endregion EnemyTypeTag

	/** 任务类型*/
#pragma region TaskType
	FGameplayTag TaskType_KillEnemy_LittleDragon;		// 击杀小型龙
	FGameplayTag TaskType_KillEnemy_GiantDragon;		// 击杀巨龙
	FGameplayTag TaskType_KillEnemy_Berserker;			// 击杀战狂

	FGameplayTag TaskType_CommitMat_Chalcedony;			// 提交石髓
	FGameplayTag TaskType_CommitMat_DiamondRuneStone;	// 提交菱形符文石
	FGameplayTag TaskType_CommitMat_RoundRuneStone;		// 提交圆形符文石
	FGameplayTag TaskType_CommitMat_ElfFeather;			// 提交精灵之羽

#pragma endregion TaskType



private:
	static FEnhoneyGameplayTags EnhoneyGameplayTags;
};