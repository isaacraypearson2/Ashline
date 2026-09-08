#include "AI/AshlineAICatalog.h"

TArray<FAshlineAIArchetypeDef> UAshlineAICatalog::BuildArchetypes()
{
	TArray<FAshlineAIArchetypeDef> List;
	auto Add = [&](EAshlineAIArchetype Type, const TCHAR* Name, const TCHAR* Role, float HP, float Acc, float React, float Sight, float Speed, const TCHAR* Weapon, bool bHold, bool bFlank, const FLinearColor& Tint, const TCHAR* Alt, float Armor, float Threat, int32 Nades, bool bElite, const TCHAR* Voice, const TCHAR* Slug)
	{
		FAshlineAIArchetypeDef D;
		D.Archetype = Type;
		D.DisplayName = FText::FromString(Name);
		D.RoleDescription = FText::FromString(Role);
		D.MaxHealth = HP;
		D.Accuracy = Acc;
		D.ReactionSeconds = React;
		D.SightRange = Sight;
		D.MoveSpeed = Speed;
		D.PreferredWeaponId = Weapon;
		D.bHoldsPosition = bHold;
		D.bFlanks = bFlank;
		D.BodyTint = Tint;
		D.AlternateWeaponId = Alt;
		D.ArmorMul = Armor;
		D.Threat = Threat;
		D.GrenadeCount = Nades;
		D.bElite = bElite;
		D.VoiceLine = FText::FromString(Voice);
		D.PresentationId = FName(*FString::Printf(TEXT("DA_AI_%s"), Slug));
		List.Add(D);
	};

	Add(EAshlineAIArchetype::Rifleman, TEXT("Rifleman"), TEXT("Standard infantry. Fire and move, takes cover, pushes when suppressed fire lifts."), 100.f, 0.52f, 0.48f, 3200.f, 390.f, TEXT("WPN_AR_ASH16"), false, true, FLinearColor(0.45f, 0.12f, 0.1f), TEXT("WPN_AR_M4K"), 1.f, 1.f, 0, false, TEXT("Contact, moving."), TEXT("Rifleman"));
	Add(EAshlineAIArchetype::Breacher, TEXT("Breacher"), TEXT("Close-range assault. Clears rooms, favors shotguns and SMGs, aggressive peek."), 110.f, 0.48f, 0.32f, 1800.f, 430.f, TEXT("WPN_SHG_M870K"), false, true, FLinearColor(0.28f, 0.12f, 0.08f), TEXT("WPN_SMG_C9"), 1.05f, 1.15f, 1, false, TEXT("Breach, breach."), TEXT("Breacher"));
	Add(EAshlineAIArchetype::Marksman, TEXT("Marksman"), TEXT("Overwatch. Holds elevation, long reaction then high first-shot accuracy."), 85.f, 0.82f, 0.75f, 7000.f, 320.f, TEXT("WPN_SNP_G28L"), true, false, FLinearColor(0.22f, 0.2f, 0.12f), TEXT("WPN_DMR_SASS"), 0.9f, 1.35f, 0, false, TEXT("Overwatch set."), TEXT("Marksman"));
	Add(EAshlineAIArchetype::MachineGunner, TEXT("Machine Gunner"), TEXT("Suppression. Anchors a lane, high volume, poor mobility while firing."), 130.f, 0.40f, 0.55f, 4500.f, 300.f, TEXT("WPN_LMG_M250"), true, false, FLinearColor(0.18f, 0.14f, 0.1f), TEXT("WPN_LMG_M240"), 1.15f, 1.4f, 0, false, TEXT("Suppressing."), TEXT("Gunner"));
	Add(EAshlineAIArchetype::Officer, TEXT("Officer"), TEXT("Squad lead. Calls contact, tightens nearby AI accuracy, last to break."), 95.f, 0.60f, 0.40f, 3600.f, 380.f, TEXT("WPN_DMR_SASS"), false, true, FLinearColor(0.15f, 0.16f, 0.22f), TEXT("WPN_AR_SCARH"), 1.f, 1.5f, 1, false, TEXT("Hold the line."), TEXT("Officer"));
	Add(EAshlineAIArchetype::Scout, TEXT("Scout"), TEXT("Recon screen. Wide perception, low health, disengages and reports."), 70.f, 0.50f, 0.28f, 5000.f, 470.f, TEXT("WPN_SMG_C9"), false, true, FLinearColor(0.16f, 0.2f, 0.12f), TEXT("WPN_SMG_MPX"), 0.85f, 0.8f, 0, false, TEXT("Eyes on, breaking."), TEXT("Scout"));
	Add(EAshlineAIArchetype::Heavy, TEXT("Heavy"), TEXT("Armored pusher. Slow, high HP, forces the player to commit explosives or DMR."), 180.f, 0.45f, 0.60f, 2800.f, 260.f, TEXT("WPN_LMG_M250"), false, false, FLinearColor(0.12f, 0.12f, 0.12f), TEXT("WPN_LMG_PKM"), 1.45f, 1.7f, 0, false, TEXT("Pushing through."), TEXT("Heavy"));
	Add(EAshlineAIArchetype::CivilianIrregular, TEXT("Irregular"), TEXT("Poorly trained armed civilian. Erratic, low accuracy, high panic."), 80.f, 0.28f, 0.70f, 2200.f, 360.f, TEXT("WPN_PIS_M17A"), false, false, FLinearColor(0.32f, 0.24f, 0.16f), TEXT("WPN_PIS_G19"), 0.7f, 0.55f, 0, false, TEXT("Get back!"), TEXT("Irregular"));
	Add(EAshlineAIArchetype::Grenadier, TEXT("Grenadier"), TEXT("UGL / 40mm pressure. Holds mid, flushes cover, rifles between nades."), 105.f, 0.5f, 0.5f, 3000.f, 360.f, TEXT("WPN_AR_M4K"), true, false, FLinearColor(0.24f, 0.16f, 0.08f), TEXT("WPN_AR_ASH16"), 1.05f, 1.25f, 3, false, TEXT("Frag out."), TEXT("Grenadier"));
	Add(EAshlineAIArchetype::Elite, TEXT("Elite"), TEXT("Campaign closer. High acc, mixed kit, does not panic."), 140.f, 0.72f, 0.32f, 4200.f, 400.f, TEXT("WPN_AR_416C"), false, true, FLinearColor(0.08f, 0.09f, 0.1f), TEXT("WPN_SMG_VEC"), 1.25f, 2.1f, 2, true, TEXT("They're in the wire."), TEXT("Elite"));
	Add(EAshlineAIArchetype::Spotter, TEXT("Spotter"), TEXT("Pairs with marksman. Laser-designates, SMG if pushed."), 75.f, 0.58f, 0.3f, 6200.f, 410.f, TEXT("WPN_SMG_MPX"), true, true, FLinearColor(0.2f, 0.22f, 0.16f), TEXT("WPN_PIS_M17A"), 0.9f, 0.9f, 0, false, TEXT("Marked."), TEXT("Spotter"));
	Add(EAshlineAIArchetype::RadioOp, TEXT("Radio Op"), TEXT("Calls QRF. Low aggression until backup, then tightens the net."), 90.f, 0.5f, 0.38f, 4000.f, 400.f, TEXT("WPN_SMG_UMP"), false, true, FLinearColor(0.18f, 0.16f, 0.12f), TEXT("WPN_PIS_M17A"), 0.9f, 1.1f, 1, false, TEXT("QRF inbound."), TEXT("RadioOp"));
	Add(EAshlineAIArchetype::CQBSpecialist, TEXT("CQB"), TEXT("Room clearer. SMG/shotgun, aggressive peek, poor long range."), 100.f, 0.5f, 0.26f, 1600.f, 450.f, TEXT("WPN_PDW_P90C"), false, true, FLinearColor(0.1f, 0.1f, 0.12f), TEXT("WPN_SMG_VEC"), 1.05f, 1.2f, 1, false, TEXT("Clearing."), TEXT("CQB"));

	return List;
}

FAshlineAIArchetypeDef UAshlineAICatalog::GetArchetype(EAshlineAIArchetype Archetype)
{
	for (const FAshlineAIArchetypeDef& Def : BuildArchetypes())
	{
		if (Def.Archetype == Archetype)
		{
			return Def;
		}
	}
	return FAshlineAIArchetypeDef();
}

FAshlineDifficultyTuning UAshlineAICatalog::GetDifficulty(EAshlineDifficulty Difficulty)
{
	FAshlineDifficultyTuning T;
	switch (Difficulty)
	{
	case EAshlineDifficulty::Recruit:
		T.PlayerDamageTakenMul = 0.55f;
		T.PlayerDamageDealtMul = 1.25f;
		T.AIAccuracyMul = 0.55f;
		T.AIHealthMul = 0.75f;
		T.AIPerceptionRangeMul = 0.7f;
		T.ExtraAICount = 0;
		break;
	case EAshlineDifficulty::Regular:
		T.PlayerDamageTakenMul = 1.f;
		T.PlayerDamageDealtMul = 1.f;
		T.AIAccuracyMul = 1.f;
		T.AIHealthMul = 1.f;
		T.AIPerceptionRangeMul = 1.f;
		T.ExtraAICount = 0;
		break;
	case EAshlineDifficulty::Veteran:
		T.PlayerDamageTakenMul = 1.35f;
		T.PlayerDamageDealtMul = 0.9f;
		T.AIAccuracyMul = 1.2f;
		T.AIHealthMul = 1.15f;
		T.AIPerceptionRangeMul = 1.2f;
		T.ExtraAICount = 2;
		break;
	case EAshlineDifficulty::Extreme:
		T.PlayerDamageTakenMul = 1.75f;
		T.PlayerDamageDealtMul = 0.8f;
		T.AIAccuracyMul = 1.4f;
		T.AIHealthMul = 1.3f;
		T.AIPerceptionRangeMul = 1.4f;
		T.ExtraAICount = 4;
		break;
	}
	return T;
}
