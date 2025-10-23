// Copyright Enhoney.

#include "EnhoneyGameplayTags.h"

#include "GameplayTagsManager.h"

FEnhoneyGameplayTags FEnhoneyGameplayTags::EnhoneyGameplayTags;

void FEnhoneyGameplayTags::InitializeGameplayTags()
{

#pragma region InputTag
	// 可变技能匹配的InputTag
	EnhoneyGameplayTags.InputTag_E =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.E"), FString("InputTag E Key"));
	EnhoneyGameplayTags.InputTag_Q = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Q"), FString("InputTag Q Key"));
	EnhoneyGameplayTags.InputTag_R = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.R"), FString("InputTag R Key"));

#pragma endregion InputTag

#pragma region AbilityStatus
	// 可变技能的状态
	EnhoneyGameplayTags.AbilityStatus_Locked = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityStatus.Locked"), FString("AbilityStatus Locked"));
	EnhoneyGameplayTags.AbilityStatus_Eligible = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityStatus.Eligible"), FString("AbilityStatus Eligible"));
	EnhoneyGameplayTags.AbilityStatus_Unlocked = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityStatus.Unlocked"), FString("AbilityStatus Unlocked"));
	EnhoneyGameplayTags.AbilityStatus_Equipped = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityStatus.Equipped"), FString("AbilityStatus Equipped"));
#pragma endregion AbilityStatus
#pragma region AttributeTag
	EnhoneyGameplayTags.Attribute_Primary_Strength =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Strength"), FString("Player's Strength"));
	EnhoneyGameplayTags.Attribute_Primary_Constitution =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Constitution"), FString("Player's Constitution"));
	EnhoneyGameplayTags.Attribute_Primary_Intelligence =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Intelligence"), FString("Player's Intelligence"));
	EnhoneyGameplayTags.Attribute_Primary_Defense =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Primary.Defense"), FString("Player's Defense"));

	EnhoneyGameplayTags.Attribute_Secondary_BasicAttackPower =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.BasicAttackPower"), FString("Player's BasicAttackPower"));
	EnhoneyGameplayTags.Attribute_Secondary_ExtraAttackPower =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.ExtraAttackPower"), FString("Player's ExtraAttackPower"));
	EnhoneyGameplayTags.Attribute_Secondary_ExtraPercentageAttack =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.ExtraPercentageAttack"), FString("Player's ExtraPercentageAttack"));
	EnhoneyGameplayTags.Attribute_Secondary_AttackPower =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.AttackPower"), FString("Player's AttackPower"));

	EnhoneyGameplayTags.Attribute_Secondary_DefenseCapacity =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.DefenseCapacity"), FString("Player's DefenseCapacity"));

	EnhoneyGameplayTags.Attribute_Secondary_ElementProficiency =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.ElementProficiency"), FString("Player's ElementProficiency"));

	EnhoneyGameplayTags.Attribute_Secondary_ArmorPenetration =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.ArmorPenetration"), FString("Player's ArmorPenetration"));

	EnhoneyGameplayTags.Attribute_Secondary_PhysicalDamageBonus =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.PhysicalDamageBonus"), FString("Player's PhysicalDamageBonus"));
	EnhoneyGameplayTags.Attribute_Secondary_FlameDamageBonus =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.FlameDamageBonus"), FString("Player's FlameDamageBonus"));
	EnhoneyGameplayTags.Attribute_Secondary_FrostDamageBonus =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.FrostDamageBonus"), FString("Player's FrostDamageBonus"));
	EnhoneyGameplayTags.Attribute_Secondary_PhysicalDamageResistance =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.PhysicalDamageResistance"), FString("Player's PhysicalDamageResistance"));
	EnhoneyGameplayTags.Attribute_Secondary_FlameDamageResistance =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.FlameDamageResistance"), FString("Player's FlameDamageResistance"));
	EnhoneyGameplayTags.Attribute_Secondary_FrostDamageResistance =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.FrostDamageResistance"), FString("Player's FrostDamageResistance"));

	EnhoneyGameplayTags.Attribute_Secondary_CriticalChance =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.CriticalChance"), FString("Player's CriticalChance"));
	EnhoneyGameplayTags.Attribute_Secondary_CriticalRatio =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.CriticalRatio"), FString("Player's CriticalRatio"));

	EnhoneyGameplayTags.Attribute_Secondary_MaxHealth =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.MaxHealth"), FString("Player's MaxHealth"));
	EnhoneyGameplayTags.Attribute_Secondary_MaxMana =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.MaxMana"), FString("Player's MaxMana"));
	EnhoneyGameplayTags.Attribute_Secondary_MaxArcane =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Secondary.MaxArcane"), FString("Player's MaxArcane"));

	EnhoneyGameplayTags.Attribute_Vital_Health =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Vital.Health"), FString("Player's Health"));
	EnhoneyGameplayTags.Attribute_Vital_Mana =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Vital.Mana"), FString("Player's Mana"));
	EnhoneyGameplayTags.Attribute_Vital_Arcane =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attribute.Vital.Arcane"), FString("Player's Arcane"));
#pragma endregion AttributeTag

#pragma region AbilityTag
	EnhoneyGameplayTags.Ability_Offensive_Inherent =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent"), FString("Inherent Offensive Ability"));
	EnhoneyGameplayTags.Ability_Offensive_Inherent_CommonAttack_Melee =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.CommonAttack.Melee"), FString("Inherent Offensive Ability for Melee CommonAttack"));
	EnhoneyGameplayTags.Ability_Offensive_Inherent_CommonAttack_FlyKnife =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.CommonAttack.FlyKnife"), FString("Inherent Offensive Ability for FlyKnife CommonAttack"));
	EnhoneyGameplayTags.Ability_Offensive_Inherent_CommonAttack_ShootArrow =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.CommonAttack.ShootArrow"), FString("Inherent Offensive Ability for ShootArrow CommonAttack"));
	EnhoneyGameplayTags.Ability_Offensive_Inherent_InteractWithNPC =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.InteractWithNPC"), FString("Inherent Offensive Ability for Interact With NPC"));

	EnhoneyGameplayTags.Ability_Offensive_Inherent_LittleDragon_FireBall =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.LittleDragon.FireBall"), FString("Inherent Offensive Ability for LittleDragon FireBall"));
	EnhoneyGameplayTags.Ability_Offensive_Inherent_LittleDragon_FrostBall =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.LittleDragon.FrostBall"), FString("Inherent Offensive Ability for LittleDragon FrostBall"));
	EnhoneyGameplayTags.Ability_Offensive_Inherent_LittleDragon_Melee =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.LittleDragon.Melee"), FString("Inherent Offensive Ability for LittleDragon Melee"));
	

	// 受击反应
	EnhoneyGameplayTags.Ability_Offensive_Inherent_HitReaction =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Inherent.HitReaction"), FString("Inherent Offensive Ability for HitReaction"));
	
	EnhoneyGameplayTags.Ability_Offensive_Variable =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Variable"), FString("Variable Offensive Ability"));
	EnhoneyGameplayTags.Ability_Offensive_Variable_IceShield = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Variable.IceShield"), FString("Variable Offensive Ability IceShield"));
	EnhoneyGameplayTags.Ability_Offensive_Variable_FireShield = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Variable.FireShield"), FString("Variable Offensive Ability FireShield"));
	EnhoneyGameplayTags.Ability_Offensive_Variable_FireBall = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Variable.FireBall"), FString("Variable Offensive Ability FireBall"));
	EnhoneyGameplayTags.Ability_Offensive_Variable_IceBall =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Variable.IceBall"), FString("Variable Offensive Ability IceBall"));
	EnhoneyGameplayTags.Ability_Offensive_Variable_StarFallen =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Variable.StarFallen"), FString("Variable Offensive Ability StarFallen"));
	EnhoneyGameplayTags.Ability_Offensive_Variable_LimboIce =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Offensive.Variable.LimboIce"), FString("Variable Offensive Ability LimboIce"));

	EnhoneyGameplayTags.Ability_Passive_Inherent =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.Inherent"), FString("Inherent Passive Ability"));

	EnhoneyGameplayTags.Ability_Passive_Variable =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.Variable"), FString("Variable Passive Ability"));

#pragma endregion AbilityTag


#pragma region AbilityDamageType
	EnhoneyGameplayTags.AbilityDamageType_Physic =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityDamageType.Physic"), FString("Physical Type Damage"));
	EnhoneyGameplayTags.AbilityDamageType_Flame =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityDamageType.Flame"), FString("Flame Type Damage"));
	EnhoneyGameplayTags.AbilityDamageType_Frost =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityDamageType.Frost"), FString("Frost Type Damage"));

#pragma endregion 

#pragma region AbilityCooldownTag
	EnhoneyGameplayTags.Cooldown_Inherent_EnemyLocking =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Inherent.EnemyLocking"), FString("EnemyLocking Ability Cooldown Tag"));


#pragma endregion AbilityCooldownTag

#pragma region AbilityEventTag
	EnhoneyGameplayTags.AbilityEventTag_CommonAttack_Melee =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.CommonAttack.Melee"), FString("Player CommonAttack Melee"));

	EnhoneyGameplayTags.AbilityEventTag_ListenCombat_Enter =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.ListenCombat.Enter"), FString("Enter ListenCombat State"));
	EnhoneyGameplayTags.AbilityEventTag_ListenCombat_Quit =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.ListenCombat.Quit"), FString("Quit ListenCombat State"));
	EnhoneyGameplayTags.AbilityEventTag_ComboAttack_Reset =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.ComboAttack.Reset"), FString("Reset ComboAttack"));
	EnhoneyGameplayTags.AbilityEventTag_ComboAttack_ExecuteCombo =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.ComboAttack.ExecuteCombo"), FString("ExecuteCombo ComboAttack"));
	
	EnhoneyGameplayTags.AbilityEventTag_RemoteAttack_Fire =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.RemoteAttack.Fire"), FString("RemoteAttack Fire"));

	EnhoneyGameplayTags.AbilityEventTag_HitReaction_Finish =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.HitReaction.Finish"), FString("AbilityEventTag HitReaction Finish"));

	EnhoneyGameplayTags.AbilityEventTag_MeleeCollisionEnabled_Mouth =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.MeleeCollisionEnabled.Mouth"), FString("AbilityEventTag Mouth Melee Collision Enabled"));
	EnhoneyGameplayTags.AbilityEventTag_MeleeCollisionEnabled_Tail =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AbilityEventTag.MeleeCollisionEnabled.Tail"), FString("AbilityEventTag Tail Melee Collision Enabled"));
	
#pragma endregion AbilityEventTag


#pragma region WeaponTag
	EnhoneyGameplayTags.Weapon_CurvedKnife =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.CurvedKnife"), FString("CurvedKnife"));
	EnhoneyGameplayTags.Weapon_FlyKnife =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.FlyKnife"), FString("FlyKnife"));
	EnhoneyGameplayTags.Weapon_Crossbow =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.Crossbow"), FString("Crossbow"));
#pragma endregion WeaponTag

// 材料
#pragma region MaterialTag
	// 可售卖材料
	EnhoneyGameplayTags.Material_Economic_Slag =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Economic.Slag"), FString("Economic Material Slag"));
	EnhoneyGameplayTags.Material_Economic_DragonScale =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Economic.DragonScale"), FString("Economic Material DragonScale"));
	EnhoneyGameplayTags.Material_Economic_DragonEye =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Economic.DragonEye"), FString("Economic Material DragonEye"));
	EnhoneyGameplayTags.Material_Economic_GoldBar =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Economic.GoldBar"), FString("Economic Material GoldBar"));

	// 贵重/武器升级材料
	EnhoneyGameplayTags.Material_Valuable_DragonTooth =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.DragonTooth"), FString("Valuable Material DragonTooth"));
	EnhoneyGameplayTags.Material_Valuable_DragonBone =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.DragonBone"), FString("Valuable Material DragonBone"));
	EnhoneyGameplayTags.Material_Valuable_Rubine =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.Rubine"), FString("Valuable Material Rubine"));
	EnhoneyGameplayTags.Material_Valuable_Emerald =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.Emerald"), FString("Valuable Material Emerald"));
	EnhoneyGameplayTags.Material_Valuable_Sapphire =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.Sapphire"), FString("Valuable Material Sapphire"));
	EnhoneyGameplayTags.Material_Valuable_Amethyst =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.Amethyst"), FString("Valuable Material Amethyst"));
	EnhoneyGameplayTags.Material_Valuable_Obsidian =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.Obsidian"), FString("Valuable Material Obsidian"));
	EnhoneyGameplayTags.Material_Valuable_CastPopper =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.CastPopper"), FString("Valuable Material CastPopper"));
	EnhoneyGameplayTags.Material_Valuable_ColdIron =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.ColdIron"), FString("Valuable Material ColdIron"));
	EnhoneyGameplayTags.Material_Valuable_Zixi =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.Zixi"), FString("Valuable Material Zixi"));
	EnhoneyGameplayTags.Material_Valuable_GengJin =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Valuable.GengJin"), FString("Valuable Material GengJin"));


	// 任务材料
	EnhoneyGameplayTags.Material_Task_Chalcedony =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Task.Chalcedony"), FString("Task Material Chalcedony"));
	EnhoneyGameplayTags.Material_Task_DiamondRuneStone =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Task.DiamondRuneStone"), FString("Task Material DiamondRuneStone"));
	EnhoneyGameplayTags.Material_Task_RoundRuneStone =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Task.RoundRuneStone"), FString("Task Material RoundRuneStone"));
	EnhoneyGameplayTags.Material_Task_ElfFeather =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Task.ElfFeather"), FString("Task Material ElfFeather"));

	// 食物材料
	EnhoneyGameplayTags.Material_Food_BlueMushroom =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.BlueMushroom"), FString("Material Food BlueMushroom"));
	EnhoneyGameplayTags.Material_Food_CompoundPotion =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.CompoundPotion"), FString("Material Food CompoundPotion"));
	EnhoneyGameplayTags.Material_Food_DragonVeinFruit =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.DragonVeinFruit"), FString("Material Food DragonVeinFruit"));
	EnhoneyGameplayTags.Material_Food_ElfFruit =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.ElfFruit"), FString("Material Food ElfFruit"));
	EnhoneyGameplayTags.Material_Food_ForbiddenPotion =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.ForbiddenPotion"), FString("Material Food ForbiddenPotion"));
	EnhoneyGameplayTags.Material_Food_GrilledBirdLegs =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.GrilledBirdLegs"), FString("Material Food GrilledBirdLegs"));
	EnhoneyGameplayTags.Material_Food_GrilledDragonMeat =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.GrilledDragonMeat"), FString("Material Food GrilledDragonMeat"));
	EnhoneyGameplayTags.Material_Food_HoneyPotion =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.HoneyPotion"), FString("Material Food HoneyPotion"));
	EnhoneyGameplayTags.Material_Food_JinGua =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.JinGua"), FString("Material Food JinGua"));
	EnhoneyGameplayTags.Material_Food_LifePotion =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.LifePotion"), FString("Material Food LifePotion"));
	EnhoneyGameplayTags.Material_Food_ManaPotion =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.ManaPotion"), FString("Material Food ManaPotion"));
	EnhoneyGameplayTags.Material_Food_Morels =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.Morels"), FString("Material Food Morels"));
	EnhoneyGameplayTags.Material_Food_Pumpkin =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.Pumpkin"), FString("Material Food Pumpkin"));
	EnhoneyGameplayTags.Material_Food_RoastedPork =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.RoastedPork"), FString("Material Food RoastedPork"));
	EnhoneyGameplayTags.Material_Food_WoodMushroom =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Material.Food.WoodMushroom"), FString("Material Food WoodMushroom"));
	

#pragma endregion MaterialTag

#pragma region EnemyTypeTag
	EnhoneyGameplayTags.EnemyType_Common_LittleDragon =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("EnemyType.Common.LittleDragon"), FString("EnemyType Common LittleDragon"));
	EnhoneyGameplayTags.EnemyType_Boss_GiantDragon =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("EnemyType.Boss.GiantDragon"), FString("EnemyType Boss GiantDragon"));
	EnhoneyGameplayTags.EnemyType_Boss_Berserker =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("EnemyType.Boss.Berserker"), FString("EnemyType Boss Berserker"));
#pragma endregion EnemyTypeTag

#pragma region TaskType
	EnhoneyGameplayTags.TaskType_KillEnemy_LittleDragon =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaskType.KillEnemy.LittleDragon"), FString("TaskType KillEnemy LittleDragon"));
	EnhoneyGameplayTags.TaskType_KillEnemy_GiantDragon =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaskType.KillEnemy.GiantDragon"), FString("TaskType KillEnemy GiantDragon"));
	EnhoneyGameplayTags.TaskType_KillEnemy_Berserker =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaskType.KillEnemy.Berserker"), FString("TaskType KillEnemy Berserker"));

	EnhoneyGameplayTags.TaskType_CommitMat_Chalcedony =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaskType.CommitMat.Chalcedony"), FString("TaskType CommitMat Chalcedony"));
	EnhoneyGameplayTags.TaskType_CommitMat_DiamondRuneStone =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaskType.CommitMat.DiamondRuneStone"), FString("TaskType CommitMat DiamondRuneStone"));
	EnhoneyGameplayTags.TaskType_CommitMat_RoundRuneStone =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaskType.CommitMat.RoundRuneStone"), FString("TaskType CommitMat RoundRuneStone"));
	EnhoneyGameplayTags.TaskType_CommitMat_ElfFeather =
		UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaskType.CommitMat.ElfFeather"), FString("TaskType CommitMat ElfFeather"));

#pragma endregion TaskType

}
