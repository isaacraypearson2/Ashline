#include "Weapons/AshlineWeaponCatalog.h"
#include <initializer_list>

namespace AshlineWeapons
{
	static FAshlineWeaponDefinition W(
		const TCHAR* Id,
		const TCHAR* Name,
		EAshlineWeaponClass Class,
		const FAshlineWeaponStats& Stats,
		int32 Unlock,
		std::initializer_list<const TCHAR*> Attachments)
	{
		FAshlineWeaponDefinition Def;
		Def.WeaponId = Id;
		Def.DisplayName = FText::FromString(Name);
		Def.Class = Class;
		Def.BaseStats = Stats;
		Def.UnlockLevel = Unlock;
		for (const TCHAR* A : Attachments)
		{
			Def.CompatibleAttachments.Add(A);
		}
		return Def;
	}

	static FAshlineAttachmentDefinition A(
		const TCHAR* Id,
		const TCHAR* Name,
		EAshlineAttachmentSlot Slot,
		int32 Unlock,
		const FAshlineWeaponStats& Delta)
	{
		FAshlineAttachmentDefinition Def;
		Def.AttachmentId = Id;
		Def.DisplayName = FText::FromString(Name);
		Def.Slot = Slot;
		Def.UnlockLevel = Unlock;
		Def.StatDelta = Delta;
		return Def;
	}

	static FAshlineWeaponStats S(
		float Damage,
		float RPM,
		int32 Mag,
		int32 Reserve,
		float Range,
		float RecoilP,
		float RecoilY,
		float Reload,
		float Hip,
		float Ads,
		bool bAuto,
		float AdsSpeed = 1.f,
		float MoveMul = 1.f,
		float Noise = 40.f,
		float Headshot = 1.5f,
		float Pen = 0.f)
	{
		FAshlineWeaponStats Stats;
		Stats.Damage = Damage;
		Stats.FireRateRPM = RPM;
		Stats.MagazineSize = Mag;
		Stats.ReserveAmmo = Reserve;
		Stats.RangeMeters = Range;
		Stats.RecoilPitch = RecoilP;
		Stats.RecoilYaw = RecoilY;
		Stats.ReloadSeconds = Reload;
		Stats.HipFireSpread = Hip;
		Stats.ADSSpread = Ads;
		Stats.bAutomatic = bAuto;
		Stats.ADSSpeedMul = AdsSpeed;
		Stats.MoveSpeedMul = MoveMul;
		Stats.NoiseMeters = Noise;
		Stats.HeadshotMultiplier = Headshot;
		Stats.ArmorPenetration = Pen;
		return Stats;
	}
}

TArray<FAshlineWeaponDefinition> UAshlineWeaponCatalog::BuildRoster()
{
	using namespace AshlineWeapons;

	TArray<FAshlineWeaponDefinition> Roster;
	Roster.Reserve(7);

	Roster.Add(W(TEXT("WPN_AR_ASH16"), TEXT("ASH-16 Service Rifle"), EAshlineWeaponClass::AssaultRifle,
		S(26.f, 720.f, 30, 180, 400.f, 1.15f, 0.38f, 2.35f, 2.4f, 0.32f, true, 1.f, 1.f, 42.f, 1.5f, 0.1f), 1,
		{ TEXT("OPT_HOLO"), TEXT("OPT_REFLEX"), TEXT("OPT_ACOG"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("MUZ_BRAKE"), TEXT("UB_GRIP"), TEXT("UB_ANGLED"), TEXT("UB_LITE"), TEXT("MAG_60"), TEXT("MAG_QUICK"), TEXT("STK_SOPMOD"), TEXT("STK_LIGHT"), TEXT("LSR_PEQ"), TEXT("LSR_IR") }));

	Roster.Add(W(TEXT("WPN_SMG_C9"), TEXT("C9 Compact"), EAshlineWeaponClass::SMG,
		S(18.f, 920.f, 25, 150, 120.f, 0.85f, 0.55f, 1.85f, 2.1f, 0.55f, true, 1.15f, 1.08f, 36.f, 1.35f, 0.f), 2,
		{ TEXT("OPT_HOLO"), TEXT("OPT_REFLEX"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("UB_ANGLED"), TEXT("MAG_EXT_SMG"), TEXT("MAG_DRUM_SMG"), TEXT("MAG_QUICK"), TEXT("STK_WIRE"), TEXT("STK_LIGHT"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_SNP_G28L"), TEXT("G28-L Marksman"), EAshlineWeaponClass::Sniper,
		S(95.f, 48.f, 5, 30, 1200.f, 3.4f, 0.15f, 3.1f, 6.5f, 0.05f, false, 0.72f, 0.88f, 55.f, 2.2f, 0.35f), 8,
		{ TEXT("OPT_8X"), TEXT("OPT_12X"), TEXT("MUZ_SUP_SN"), TEXT("MUZ_BRAKE"), TEXT("STK_PRECISION"), TEXT("LSR_PEQ"), TEXT("LSR_IR") }));

	Roster.Add(W(TEXT("WPN_SHG_M870K"), TEXT("M870-K Breacher"), EAshlineWeaponClass::Shotgun,
		S(18.f, 70.f, 6, 30, 35.f, 4.2f, 1.1f, 0.55f, 3.8f, 1.4f, false, 1.05f, 0.96f, 48.f, 1.2f, 0.05f), 4,
		{ TEXT("OPT_HOLO"), TEXT("OPT_REFLEX"), TEXT("MUZ_CHOKE"), TEXT("UB_LITE"), TEXT("STK_BREACH"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_PIS_M17A"), TEXT("M17-A Sidearm"), EAshlineWeaponClass::Sidearm,
		S(22.f, 380.f, 17, 51, 50.f, 1.6f, 0.7f, 1.4f, 2.8f, 0.8f, false, 1.2f, 1.05f, 28.f, 1.4f, 0.f), 1,
		{ TEXT("OPT_RMR"), TEXT("MUZ_SUP_P"), TEXT("MAG_EXT_P"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_DMR_SASS"), TEXT("SASS-7 DMR"), EAshlineWeaponClass::DMR,
		S(48.f, 240.f, 20, 80, 750.f, 1.9f, 0.22f, 2.6f, 3.2f, 0.12f, false, 0.85f, 0.94f, 44.f, 1.8f, 0.22f), 6,
		{ TEXT("OPT_ACOG"), TEXT("OPT_8X"), TEXT("OPT_REFLEX"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("MUZ_BRAKE"), TEXT("UB_GRIP"), TEXT("MAG_DMR25"), TEXT("STK_PRECISION"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_LMG_M250"), TEXT("M250 SAW"), EAshlineWeaponClass::LMG,
		S(28.f, 650.f, 100, 200, 600.f, 1.45f, 0.5f, 6.4f, 3.6f, 0.55f, true, 0.8f, 0.82f, 52.f, 1.45f, 0.18f), 10,
		{ TEXT("OPT_HOLO"), TEXT("OPT_ACOG"), TEXT("MUZ_COMP"), TEXT("MUZ_BRAKE"), TEXT("UB_BIPOD"), TEXT("MAG_200"), TEXT("STK_SOPMOD"), TEXT("LSR_PEQ") }));

	return Roster;
}

TArray<FAshlineAttachmentDefinition> UAshlineWeaponCatalog::BuildAttachments()
{
	using namespace AshlineWeapons;

	TArray<FAshlineAttachmentDefinition> List;
	auto Zero = S(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, true);

	auto D = [&](float Damage, float RPM, int32 Mag, int32 Reserve, float Range, float RP, float RY, float Reload, float Hip, float Ads)
	{
		FAshlineWeaponStats X = Zero;
		X.Damage = Damage;
		X.FireRateRPM = RPM;
		X.MagazineSize = Mag;
		X.ReserveAmmo = Reserve;
		X.RangeMeters = Range;
		X.RecoilPitch = RP;
		X.RecoilYaw = RY;
		X.ReloadSeconds = Reload;
		X.HipFireSpread = Hip;
		X.ADSSpread = Ads;
		return X;
	};

	auto Finish = [](FAshlineAttachmentDefinition Def, int32 Cost, float AdsSpeed, float Move, float Noise, float Sway, float Recov, float Fov)
	{
		Def.CreditCost = Cost;
		Def.StatDelta.ADSSpeedMul = AdsSpeed;
		Def.StatDelta.MoveSpeedMul = Move;
		Def.StatDelta.NoiseMeters = Noise;
		Def.StatDelta.SwayDegrees = Sway;
		Def.StatDelta.RecoilRecovery = Recov;
		Def.StatDelta.ADSFovScale = Fov;
		return Def;
	};

	List.Add(Finish(A(TEXT("OPT_HOLO"), TEXT("Holo-1"), EAshlineAttachmentSlot::Optic, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.2f, -0.05f)), 0, 0.04f, 0.f, 0.f, -0.04f, 0.f, -0.02f));
	List.Add(Finish(A(TEXT("OPT_REFLEX"), TEXT("Reflex"), EAshlineAttachmentSlot::Optic, 3, D(0, 0, 0, 0, 0, 0, 0, 0, -0.25f, -0.06f)), 150, 0.08f, 0.f, 0.f, -0.05f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("OPT_RMR"), TEXT("RMR Mini"), EAshlineAttachmentSlot::Optic, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.1f, -0.04f)), 80, 0.06f, 0.f, 0.f, -0.02f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("OPT_ACOG"), TEXT("4x Combat"), EAshlineAttachmentSlot::Optic, 5, D(0, 0, 0, 0, 40.f, 0, 0, 0, 0.3f, -0.08f)), 350, -0.08f, -0.02f, 0.f, 0.04f, 0.f, -0.18f));
	List.Add(Finish(A(TEXT("OPT_8X"), TEXT("8x Glass"), EAshlineAttachmentSlot::Optic, 8, D(0, 0, 0, 0, 120.f, 0.1f, 0, 0.1f, 0.8f, -0.03f)), 600, -0.16f, -0.04f, 0.f, 0.08f, 0.f, -0.32f));
	List.Add(Finish(A(TEXT("OPT_12X"), TEXT("12x Night Glass"), EAshlineAttachmentSlot::Optic, 12, D(0, 0, 0, 0, 200.f, 0.15f, 0, 0.15f, 1.2f, -0.02f)), 900, -0.22f, -0.06f, 0.f, 0.12f, 0.f, -0.42f));

	List.Add(Finish(A(TEXT("MUZ_SUP"), TEXT("Rifle Suppressor"), EAshlineAttachmentSlot::Muzzle, 3, D(-1.f, 0, 0, 0, -20.f, -0.15f, -0.05f, 0, -0.1f, -0.02f)), 250, -0.04f, -0.02f, -22.f, -0.02f, 0.05f, 0.f));
	List.Add(Finish(A(TEXT("MUZ_SUP_SN"), TEXT("Long Suppressor"), EAshlineAttachmentSlot::Muzzle, 8, D(-2.f, 0, 0, 0, -40.f, -0.2f, 0, 0.1f, 0, 0)), 500, -0.06f, -0.04f, -30.f, 0.f, 0.04f, 0.f));
	List.Add(Finish(A(TEXT("MUZ_SUP_P"), TEXT("Pistol Suppressor"), EAshlineAttachmentSlot::Muzzle, 2, D(-1.f, 0, 0, 0, -5.f, -0.1f, 0, 0, 0, 0)), 180, 0.f, 0.f, -14.f, 0.f, 0.02f, 0.f));
	List.Add(Finish(A(TEXT("MUZ_COMP"), TEXT("Compensator"), EAshlineAttachmentSlot::Muzzle, 4, D(0, 0, 0, 0, 0, -0.25f, -0.12f, 0, 0, 0)), 220, 0.f, 0.f, 4.f, -0.04f, 0.12f, 0.f));
	List.Add(Finish(A(TEXT("MUZ_BRAKE"), TEXT("Muzzle Brake"), EAshlineAttachmentSlot::Muzzle, 6, D(0, 0, 0, 0, 0, -0.35f, -0.04f, 0, 0.05f, 0)), 280, 0.f, 0.f, 8.f, -0.02f, 0.18f, 0.f));
	List.Add(Finish(A(TEXT("MUZ_CHOKE"), TEXT("Tight Choke"), EAshlineAttachmentSlot::Muzzle, 5, D(1.f, 0, 0, 0, 8.f, 0.2f, 0, 0, -0.4f, -0.2f)), 200, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f));

	List.Add(Finish(A(TEXT("UB_GRIP"), TEXT("Vertical Grip"), EAshlineAttachmentSlot::Underbarrel, 3, D(0, 0, 0, 0, 0, -0.2f, -0.1f, 0, -0.25f, -0.04f)), 180, 0.04f, -0.01f, 0.f, -0.06f, 0.08f, 0.f));
	List.Add(Finish(A(TEXT("UB_ANGLED"), TEXT("Angled Grip"), EAshlineAttachmentSlot::Underbarrel, 5, D(0, 0, 0, 0, 0, -0.08f, -0.16f, 0, -0.12f, -0.02f)), 200, 0.1f, 0.01f, 0.f, -0.03f, 0.1f, 0.f));
	List.Add(Finish(A(TEXT("UB_LITE"), TEXT("Weapon Light"), EAshlineAttachmentSlot::Underbarrel, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.05f, 0)), 0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("UB_BIPOD"), TEXT("Bipod"), EAshlineAttachmentSlot::Underbarrel, 10, D(0, 0, 0, 0, 20.f, -0.4f, -0.15f, 0.2f, 0.2f, -0.1f)), 450, -0.1f, -0.08f, 0.f, -0.12f, 0.2f, 0.f));

	List.Add(Finish(A(TEXT("MAG_60"), TEXT("60rd Casket"), EAshlineAttachmentSlot::Magazine, 7, D(0, -20.f, 30, 0, 0, 0.1f, 0.05f, 0.35f, 0.15f, 0.04f)), 400, -0.06f, -0.04f, 0.f, 0.04f, -0.04f, 0.f));
	List.Add(Finish(A(TEXT("MAG_EXT_SMG"), TEXT("40rd SMG"), EAshlineAttachmentSlot::Magazine, 4, D(0, 0, 15, 20, 0, 0.05f, 0, 0.15f, 0.05f, 0)), 180, 0.f, -0.01f, 0.f, 0.02f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("MAG_DRUM_SMG"), TEXT("50rd Drum"), EAshlineAttachmentSlot::Magazine, 9, D(0, -15.f, 25, 25, 0, 0.12f, 0.06f, 0.28f, 0.1f, 0.03f)), 420, -0.08f, -0.05f, 0.f, 0.05f, -0.05f, 0.f));
	List.Add(Finish(A(TEXT("MAG_EXT_P"), TEXT("21rd Pistol"), EAshlineAttachmentSlot::Magazine, 3, D(0, 0, 4, 8, 0, 0, 0, 0.08f, 0, 0)), 120, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("MAG_DMR25"), TEXT("25rd DMR"), EAshlineAttachmentSlot::Magazine, 6, D(0, 0, 5, 10, 0, 0.05f, 0, 0.12f, 0, 0)), 260, 0.f, -0.01f, 0.f, 0.f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("MAG_200"), TEXT("200rd Softpack"), EAshlineAttachmentSlot::Magazine, 12, D(0, -15.f, 100, 0, 0, 0.15f, 0.08f, 1.1f, 0.25f, 0.08f)), 700, -0.12f, -0.08f, 0.f, 0.06f, -0.06f, 0.f));
	List.Add(Finish(A(TEXT("MAG_QUICK"), TEXT("Quick Mag"), EAshlineAttachmentSlot::Magazine, 4, D(0, 0, 0, 0, 0, 0.04f, 0, -0.28f, 0.04f, 0)), 220, 0.05f, 0.f, 0.f, 0.f, 0.04f, 0.f));

	List.Add(Finish(A(TEXT("STK_SOPMOD"), TEXT("Sopmod Stock"), EAshlineAttachmentSlot::Stock, 4, D(0, 0, 0, 0, 0, -0.12f, -0.08f, 0, -0.1f, -0.03f)), 200, 0.02f, 0.f, 0.f, -0.05f, 0.1f, 0.f));
	List.Add(Finish(A(TEXT("STK_WIRE"), TEXT("Wire Stock"), EAshlineAttachmentSlot::Stock, 3, D(0, 20.f, 0, 0, 0, 0.1f, 0.1f, -0.1f, -0.05f, 0.05f)), 160, 0.08f, 0.04f, 0.f, 0.04f, -0.04f, 0.f));
	List.Add(Finish(A(TEXT("STK_LIGHT"), TEXT("Light Stock"), EAshlineAttachmentSlot::Stock, 6, D(0, 10.f, 0, 0, 0, 0.06f, 0.04f, -0.08f, 0, 0.02f)), 240, 0.1f, 0.05f, 0.f, 0.02f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("STK_PRECISION"), TEXT("Precision Stock"), EAshlineAttachmentSlot::Stock, 8, D(0, -10.f, 0, 0, 30.f, -0.2f, -0.05f, 0.1f, 0.2f, -0.06f)), 380, -0.08f, -0.03f, 0.f, -0.1f, 0.16f, 0.f));
	List.Add(Finish(A(TEXT("STK_BREACH"), TEXT("Breacher Stock"), EAshlineAttachmentSlot::Stock, 5, D(0, 0, 0, 0, 0, -0.3f, 0, -0.05f, -0.2f, 0)), 220, 0.06f, 0.02f, 0.f, 0.f, 0.08f, 0.f));

	List.Add(Finish(A(TEXT("LSR_PEQ"), TEXT("PEQ Laser"), EAshlineAttachmentSlot::Laser, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.35f, -0.02f)), 140, 0.02f, 0.f, 0.f, -0.03f, 0.f, 0.f));
	List.Add(Finish(A(TEXT("LSR_IR"), TEXT("IR Laser"), EAshlineAttachmentSlot::Laser, 7, D(0, 0, 0, 0, 0, 0, 0, 0, -0.2f, -0.03f)), 320, 0.f, 0.f, -2.f, -0.02f, 0.f, 0.f));

	return List;
}

bool UAshlineWeaponCatalog::FindWeapon(FName WeaponId, FAshlineWeaponDefinition& OutWeapon)
{
	for (const FAshlineWeaponDefinition& Weapon : BuildRoster())
	{
		if (Weapon.WeaponId == WeaponId)
		{
			OutWeapon = Weapon;
			return true;
		}
	}
	return false;
}

bool UAshlineWeaponCatalog::FindAttachment(FName AttachmentId, FAshlineAttachmentDefinition& OutAttachment)
{
	for (const FAshlineAttachmentDefinition& Attachment : BuildAttachments())
	{
		if (Attachment.AttachmentId == AttachmentId)
		{
			OutAttachment = Attachment;
			return true;
		}
	}
	return false;
}

FAshlineWeaponStats UAshlineWeaponCatalog::ComposeStats(const FAshlineWeaponDefinition& Weapon, const TMap<EAshlineAttachmentSlot, FName>& Attachments)
{
	FAshlineWeaponStats Stats = Weapon.BaseStats;
	for (const TPair<EAshlineAttachmentSlot, FName>& Pair : Attachments)
	{
		FAshlineAttachmentDefinition Attachment;
		if (!FindAttachment(Pair.Value, Attachment))
		{
			continue;
		}
		if (!Weapon.CompatibleAttachments.Contains(Attachment.AttachmentId))
		{
			continue;
		}

		Stats.Damage = FMath::Max(1.f, Stats.Damage + Attachment.StatDelta.Damage);
		Stats.FireRateRPM = FMath::Max(10.f, Stats.FireRateRPM + Attachment.StatDelta.FireRateRPM);
		Stats.MagazineSize = FMath::Max(1, Stats.MagazineSize + Attachment.StatDelta.MagazineSize);
		Stats.ReserveAmmo = FMath::Max(0, Stats.ReserveAmmo + Attachment.StatDelta.ReserveAmmo);
		Stats.RangeMeters = FMath::Max(5.f, Stats.RangeMeters + Attachment.StatDelta.RangeMeters);
		Stats.RecoilPitch = FMath::Max(0.05f, Stats.RecoilPitch + Attachment.StatDelta.RecoilPitch);
		Stats.RecoilYaw = FMath::Max(0.05f, Stats.RecoilYaw + Attachment.StatDelta.RecoilYaw);
		Stats.ReloadSeconds = FMath::Max(0.2f, Stats.ReloadSeconds + Attachment.StatDelta.ReloadSeconds);
		Stats.HipFireSpread = FMath::Max(0.05f, Stats.HipFireSpread + Attachment.StatDelta.HipFireSpread);
		Stats.ADSSpread = FMath::Max(0.01f, Stats.ADSSpread + Attachment.StatDelta.ADSSpread);
		Stats.ADSSpeedMul = FMath::Clamp(Stats.ADSSpeedMul + Attachment.StatDelta.ADSSpeedMul, 0.45f, 1.6f);
		Stats.MoveSpeedMul = FMath::Clamp(Stats.MoveSpeedMul + Attachment.StatDelta.MoveSpeedMul, 0.7f, 1.25f);
		Stats.SprintToFireSeconds = FMath::Max(0.05f, Stats.SprintToFireSeconds + Attachment.StatDelta.SprintToFireSeconds);
		Stats.SwayDegrees = FMath::Max(0.02f, Stats.SwayDegrees + Attachment.StatDelta.SwayDegrees);
		Stats.ArmorPenetration = FMath::Clamp(Stats.ArmorPenetration + Attachment.StatDelta.ArmorPenetration, 0.f, 1.f);
		Stats.HeadshotMultiplier = FMath::Max(1.f, Stats.HeadshotMultiplier + Attachment.StatDelta.HeadshotMultiplier);
		Stats.RecoilRecovery = FMath::Clamp(Stats.RecoilRecovery + Attachment.StatDelta.RecoilRecovery, 0.4f, 2.f);
		Stats.NoiseMeters = FMath::Max(4.f, Stats.NoiseMeters + Attachment.StatDelta.NoiseMeters);
		Stats.ADSFovScale = FMath::Clamp(Stats.ADSFovScale + Attachment.StatDelta.ADSFovScale, 0.4f, 1.15f);
	}
	return Stats;
}

bool UAshlineWeaponCatalog::IsWeaponUnlockedAtRank(FName WeaponId, int32 Rank)
{
	FAshlineWeaponDefinition Weapon;
	return FindWeapon(WeaponId, Weapon) && Rank >= Weapon.UnlockLevel;
}

bool UAshlineWeaponCatalog::IsAttachmentUnlockedAtRank(FName AttachmentId, int32 Rank)
{
	FAshlineAttachmentDefinition Attachment;
	return FindAttachment(AttachmentId, Attachment) && Rank >= Attachment.UnlockLevel;
}

bool UAshlineWeaponCatalog::IsAttachmentCompatible(FName WeaponId, FName AttachmentId)
{
	FAshlineWeaponDefinition Weapon;
	return FindWeapon(WeaponId, Weapon) && Weapon.CompatibleAttachments.Contains(AttachmentId);
}

TArray<FName> UAshlineWeaponCatalog::StarterAttachmentIds(FName WeaponId)
{
	return AttachmentsUnlockedByRank(WeaponId, 1);
}

TArray<FName> UAshlineWeaponCatalog::AttachmentsUnlockedByRank(FName WeaponId, int32 Rank)
{
	TArray<FName> Ids;
	FAshlineWeaponDefinition Weapon;
	if (!FindWeapon(WeaponId, Weapon))
	{
		return Ids;
	}
	for (const FName& AttachmentId : Weapon.CompatibleAttachments)
	{
		if (IsAttachmentUnlockedAtRank(AttachmentId, Rank))
		{
			Ids.AddUnique(AttachmentId);
		}
	}
	return Ids;
}
