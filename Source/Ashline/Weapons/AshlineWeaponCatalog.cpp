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
		bool bAuto)
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
		Stats.PelletCount = 1;
		return Stats;
	}
}

TArray<FAshlineWeaponDefinition> UAshlineWeaponCatalog::BuildRoster()
{
	using namespace AshlineWeapons;

	TArray<FAshlineWeaponDefinition> Roster;
	Roster.Reserve(13);

	FAshlineWeaponDefinition Ash16 = W(TEXT("WPN_AR_ASH16"), TEXT("ASH-16 Service Rifle"), EAshlineWeaponClass::AssaultRifle,
		S(26.f, 720.f, 30, 180, 400.f, 1.15f, 0.38f, 2.35f, 2.4f, 0.32f, true), 1,
		{ TEXT("OPT_HOLO"), TEXT("OPT_REDDOT"), TEXT("OPT_ACOG"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("MUZ_BRAKE"), TEXT("UB_GRIP"), TEXT("UB_ANGLED"), TEXT("UB_LITE"), TEXT("MAG_60"), TEXT("STK_SOPMOD"), TEXT("LSR_PEQ"), TEXT("AMMO_FMJ"), TEXT("AMMO_TRACER"), TEXT("AMMO_AP") });
	Roster.Add(Ash16);

	Roster.Add(W(TEXT("WPN_AR_M4C"), TEXT("M4-C Carbine"), EAshlineWeaponClass::AssaultRifle,
		S(24.f, 800.f, 30, 180, 320.f, 1.05f, 0.42f, 2.15f, 2.2f, 0.28f, true), 3,
		{ TEXT("OPT_HOLO"), TEXT("OPT_REDDOT"), TEXT("OPT_ACOG"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("UB_ANGLED"), TEXT("MAG_60"), TEXT("STK_SOPMOD"), TEXT("LSR_PEQ"), TEXT("AMMO_FMJ"), TEXT("AMMO_TRACER") }));

	Roster.Add(W(TEXT("WPN_SMG_C9"), TEXT("C9 Compact"), EAshlineWeaponClass::SMG,
		S(18.f, 920.f, 25, 150, 120.f, 0.85f, 0.55f, 1.85f, 2.1f, 0.55f, true), 2,
		{ TEXT("OPT_HOLO"), TEXT("OPT_REDDOT"), TEXT("MUZ_SUP"), TEXT("UB_GRIP"), TEXT("MAG_EXT_SMG"), TEXT("STK_WIRE"), TEXT("LSR_PEQ"), TEXT("AMMO_HP"), TEXT("AMMO_SUB") }));

	Roster.Add(W(TEXT("WPN_PDW_P90C"), TEXT("P90-C PDW"), EAshlineWeaponClass::PDW,
		S(16.f, 900.f, 50, 200, 140.f, 0.7f, 0.4f, 2.05f, 1.9f, 0.48f, true), 7,
		{ TEXT("OPT_HOLO"), TEXT("OPT_REDDOT"), TEXT("MUZ_SUP"), TEXT("UB_LITE"), TEXT("LSR_PEQ"), TEXT("AMMO_FMJ"), TEXT("AMMO_TRACER") }));

	Roster.Add(W(TEXT("WPN_SMG_UMP"), TEXT("UMP-K"), EAshlineWeaponClass::SMG,
		S(22.f, 600.f, 25, 125, 160.f, 0.95f, 0.35f, 2.0f, 2.0f, 0.42f, true), 5,
		{ TEXT("OPT_HOLO"), TEXT("MUZ_SUP"), TEXT("UB_GRIP"), TEXT("MAG_EXT_SMG"), TEXT("STK_WIRE"), TEXT("AMMO_HP"), TEXT("AMMO_SUB") }));

	FAshlineWeaponDefinition G28 = W(TEXT("WPN_SNP_G28L"), TEXT("G28-L Marksman"), EAshlineWeaponClass::Sniper,
		S(95.f, 48.f, 5, 30, 1200.f, 3.4f, 0.15f, 3.1f, 6.5f, 0.05f, false), 8,
		{ TEXT("OPT_8X"), TEXT("OPT_12X"), TEXT("MUZ_SUP_SN"), TEXT("STK_PRECISION"), TEXT("LSR_PEQ"), TEXT("AMMO_AP"), TEXT("AMMO_TRACER") });
	Roster.Add(G28);

	FAshlineWeaponDefinition M870 = W(TEXT("WPN_SHG_M870K"), TEXT("M870-K Breacher"), EAshlineWeaponClass::Shotgun,
		S(18.f, 70.f, 6, 30, 35.f, 4.2f, 1.1f, 0.55f, 3.8f, 1.4f, false), 4,
		{ TEXT("OPT_HOLO"), TEXT("MUZ_CHOKE"), TEXT("UB_LITE"), TEXT("STK_BREACH"), TEXT("AMMO_SLUG"), TEXT("AMMO_HP") });
	M870.BaseStats.PelletCount = 8;
	Roster.Add(M870);

	FAshlineWeaponDefinition AA12 = W(TEXT("WPN_SHG_AA12"), TEXT("AA-12 Auto"), EAshlineWeaponClass::Shotgun,
		S(14.f, 300.f, 8, 40, 28.f, 3.4f, 1.4f, 2.8f, 4.2f, 1.6f, true), 11,
		{ TEXT("OPT_HOLO"), TEXT("MUZ_CHOKE"), TEXT("UB_LITE"), TEXT("MAG_DRUM"), TEXT("AMMO_SLUG") });
	AA12.BaseStats.PelletCount = 6;
	Roster.Add(AA12);

	Roster.Add(W(TEXT("WPN_PIS_M17A"), TEXT("M17-A Sidearm"), EAshlineWeaponClass::Sidearm,
		S(22.f, 380.f, 17, 51, 50.f, 1.6f, 0.7f, 1.4f, 2.8f, 0.8f, false), 1,
		{ TEXT("OPT_RMR"), TEXT("MUZ_SUP_P"), TEXT("MAG_EXT_P"), TEXT("LSR_PEQ"), TEXT("AMMO_HP"), TEXT("AMMO_FMJ") }));

	Roster.Add(W(TEXT("WPN_PIS_1911"), TEXT("M1911-A"), EAshlineWeaponClass::Sidearm,
		S(32.f, 240.f, 8, 32, 45.f, 2.1f, 0.9f, 1.55f, 3.0f, 0.9f, false), 4,
		{ TEXT("OPT_RMR"), TEXT("MUZ_SUP_P"), TEXT("LSR_PEQ"), TEXT("AMMO_HP"), TEXT("AMMO_AP") }));

	Roster.Add(W(TEXT("WPN_DMR_SASS"), TEXT("SASS-7 DMR"), EAshlineWeaponClass::DMR,
		S(48.f, 240.f, 20, 80, 750.f, 1.9f, 0.22f, 2.6f, 3.2f, 0.12f, false), 6,
		{ TEXT("OPT_ACOG"), TEXT("OPT_8X"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("MAG_DMR25"), TEXT("STK_PRECISION"), TEXT("AMMO_AP"), TEXT("AMMO_TRACER") }));

	Roster.Add(W(TEXT("WPN_BR_FAL"), TEXT("FAL-E Battle Rifle"), EAshlineWeaponClass::BattleRifle,
		S(42.f, 650.f, 20, 80, 600.f, 1.7f, 0.4f, 2.5f, 2.8f, 0.22f, true), 9,
		{ TEXT("OPT_ACOG"), TEXT("OPT_HOLO"), TEXT("MUZ_BRAKE"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("STK_HEAVY"), TEXT("AMMO_FMJ"), TEXT("AMMO_AP") }));

	Roster.Add(W(TEXT("WPN_LMG_M250"), TEXT("M250 SAW"), EAshlineWeaponClass::LMG,
		S(28.f, 650.f, 100, 200, 600.f, 1.45f, 0.5f, 6.4f, 3.6f, 0.55f, true), 10,
		{ TEXT("OPT_HOLO"), TEXT("OPT_ACOG"), TEXT("MUZ_COMP"), TEXT("UB_BIPOD"), TEXT("MAG_200"), TEXT("STK_SOPMOD"), TEXT("AMMO_FMJ"), TEXT("AMMO_TRACER") }));

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

	List.Add(A(TEXT("OPT_HOLO"), TEXT("Holo-1"), EAshlineAttachmentSlot::Optic, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.2f, -0.05f)));
	List.Add(A(TEXT("OPT_REDDOT"), TEXT("Red Dot"), EAshlineAttachmentSlot::Optic, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.15f, -0.03f)));
	List.Add(A(TEXT("OPT_RMR"), TEXT("RMR Mini"), EAshlineAttachmentSlot::Optic, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.1f, -0.04f)));
	List.Add(A(TEXT("OPT_ACOG"), TEXT("4x Combat"), EAshlineAttachmentSlot::Optic, 5, D(0, 0, 0, 0, 40.f, 0, 0, 0, 0.3f, -0.08f)));
	List.Add(A(TEXT("OPT_8X"), TEXT("8x Glass"), EAshlineAttachmentSlot::Optic, 8, D(0, 0, 0, 0, 120.f, 0.1f, 0, 0.1f, 0.8f, -0.03f)));
	List.Add(A(TEXT("OPT_12X"), TEXT("12x Night Glass"), EAshlineAttachmentSlot::Optic, 12, D(0, 0, 0, 0, 200.f, 0.15f, 0, 0.15f, 1.2f, -0.02f)));

	List.Add(A(TEXT("MUZ_SUP"), TEXT("Rifle Suppressor"), EAshlineAttachmentSlot::Muzzle, 3, D(-1.f, 0, 0, 0, -20.f, -0.15f, -0.05f, 0, -0.1f, -0.02f)));
	List.Add(A(TEXT("MUZ_SUP_SN"), TEXT("Long Suppressor"), EAshlineAttachmentSlot::Muzzle, 8, D(-2.f, 0, 0, 0, -40.f, -0.2f, 0, 0.1f, 0, 0)));
	List.Add(A(TEXT("MUZ_SUP_P"), TEXT("Pistol Suppressor"), EAshlineAttachmentSlot::Muzzle, 2, D(-1.f, 0, 0, 0, -5.f, -0.1f, 0, 0, 0, 0)));
	List.Add(A(TEXT("MUZ_COMP"), TEXT("Compensator"), EAshlineAttachmentSlot::Muzzle, 4, D(0, 0, 0, 0, 0, -0.25f, -0.12f, 0, 0, 0)));
	List.Add(A(TEXT("MUZ_BRAKE"), TEXT("Muzzle Brake"), EAshlineAttachmentSlot::Muzzle, 5, D(0, 0, 0, 0, 0, -0.35f, 0.05f, 0, 0.05f, 0)));
	List.Add(A(TEXT("MUZ_CHOKE"), TEXT("Tight Choke"), EAshlineAttachmentSlot::Muzzle, 5, D(1.f, 0, 0, 0, 8.f, 0.2f, 0, 0, -0.4f, -0.2f)));

	List.Add(A(TEXT("UB_GRIP"), TEXT("Vertical Grip"), EAshlineAttachmentSlot::Underbarrel, 3, D(0, 0, 0, 0, 0, -0.2f, -0.1f, 0, -0.25f, -0.04f)));
	List.Add(A(TEXT("UB_ANGLED"), TEXT("Angled Grip"), EAshlineAttachmentSlot::Underbarrel, 4, D(0, 0, 0, 0, 0, -0.08f, -0.18f, 0, -0.15f, -0.02f)));
	List.Add(A(TEXT("UB_LITE"), TEXT("Weapon Light"), EAshlineAttachmentSlot::Underbarrel, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.05f, 0)));
	List.Add(A(TEXT("UB_BIPOD"), TEXT("Bipod"), EAshlineAttachmentSlot::Underbarrel, 10, D(0, 0, 0, 0, 20.f, -0.4f, -0.15f, 0.2f, 0.2f, -0.1f)));

	List.Add(A(TEXT("MAG_60"), TEXT("60rd Casket"), EAshlineAttachmentSlot::Magazine, 7, D(0, -20.f, 30, 0, 0, 0.1f, 0.05f, 0.35f, 0.15f, 0.04f)));
	List.Add(A(TEXT("MAG_EXT_SMG"), TEXT("40rd SMG"), EAshlineAttachmentSlot::Magazine, 4, D(0, 0, 15, 20, 0, 0.05f, 0, 0.15f, 0.05f, 0)));
	List.Add(A(TEXT("MAG_EXT_P"), TEXT("21rd Pistol"), EAshlineAttachmentSlot::Magazine, 3, D(0, 0, 4, 8, 0, 0, 0, 0.08f, 0, 0)));
	List.Add(A(TEXT("MAG_DMR25"), TEXT("25rd DMR"), EAshlineAttachmentSlot::Magazine, 6, D(0, 0, 5, 10, 0, 0.05f, 0, 0.12f, 0, 0)));
	List.Add(A(TEXT("MAG_200"), TEXT("200rd Softpack"), EAshlineAttachmentSlot::Magazine, 12, D(0, -15.f, 100, 0, 0, 0.15f, 0.08f, 1.1f, 0.25f, 0.08f)));
	List.Add(A(TEXT("MAG_DRUM"), TEXT("20rd Drum"), EAshlineAttachmentSlot::Magazine, 11, D(0, -10.f, 12, 12, 0, 0.2f, 0.1f, 0.55f, 0.2f, 0.06f)));

	List.Add(A(TEXT("STK_SOPMOD"), TEXT("Sopmod Stock"), EAshlineAttachmentSlot::Stock, 4, D(0, 0, 0, 0, 0, -0.12f, -0.08f, 0, -0.1f, -0.03f)));
	List.Add(A(TEXT("STK_WIRE"), TEXT("Wire Stock"), EAshlineAttachmentSlot::Stock, 3, D(0, 20.f, 0, 0, 0, 0.1f, 0.1f, -0.1f, -0.05f, 0.05f)));
	List.Add(A(TEXT("STK_PRECISION"), TEXT("Precision Stock"), EAshlineAttachmentSlot::Stock, 8, D(0, -10.f, 0, 0, 30.f, -0.2f, -0.05f, 0.1f, 0.2f, -0.06f)));
	List.Add(A(TEXT("STK_BREACH"), TEXT("Breacher Stock"), EAshlineAttachmentSlot::Stock, 5, D(0, 0, 0, 0, 0, -0.3f, 0, -0.05f, -0.2f, 0)));
	List.Add(A(TEXT("STK_HEAVY"), TEXT("Heavy Stock"), EAshlineAttachmentSlot::Stock, 9, D(0, -15.f, 0, 0, 20.f, -0.25f, -0.1f, 0.12f, 0.1f, -0.04f)));

	List.Add(A(TEXT("LSR_PEQ"), TEXT("PEQ Laser"), EAshlineAttachmentSlot::Laser, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.35f, -0.02f)));
	List.Add(A(TEXT("LSR_IR"), TEXT("IR Laser"), EAshlineAttachmentSlot::Laser, 8, D(0, 0, 0, 0, 0, 0, 0, 0, -0.2f, -0.01f)));

	List.Add(A(TEXT("AMMO_FMJ"), TEXT("FMJ"), EAshlineAttachmentSlot::Ammunition, 2, D(2.f, 0, 0, 0, 15.f, 0.05f, 0, 0, 0, 0)));
	List.Add(A(TEXT("AMMO_HP"), TEXT("Hollow Point"), EAshlineAttachmentSlot::Ammunition, 3, D(4.f, 0, 0, 0, -8.f, 0.08f, 0, 0, 0.05f, 0)));
	List.Add(A(TEXT("AMMO_AP"), TEXT("Armor Piercing"), EAshlineAttachmentSlot::Ammunition, 8, D(3.f, 0, 0, 0, 40.f, 0.12f, 0, 0, 0, 0)));
	List.Add(A(TEXT("AMMO_TRACER"), TEXT("Tracer"), EAshlineAttachmentSlot::Ammunition, 4, D(0, 0, 0, 0, 0, 0.04f, 0.04f, 0, 0, 0)));
	List.Add(A(TEXT("AMMO_SUB"), TEXT("Subsonic"), EAshlineAttachmentSlot::Ammunition, 6, D(-2.f, -40.f, 0, 0, -15.f, -0.1f, -0.05f, 0, -0.1f, 0)));
	List.Add(A(TEXT("AMMO_SLUG"), TEXT("Slug"), EAshlineAttachmentSlot::Ammunition, 5, D(22.f, 0, 0, 0, 25.f, 0.4f, 0.1f, 0, -1.5f, -0.4f)));

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
		if (Attachment.Slot == EAshlineAttachmentSlot::Ammunition && Attachment.AttachmentId == TEXT("AMMO_SLUG"))
		{
			Stats.PelletCount = 1;
		}
		else if (Attachment.StatDelta.PelletCount != 0)
		{
			Stats.PelletCount = FMath::Max(1, Stats.PelletCount + Attachment.StatDelta.PelletCount);
		}
	}
	return Stats;
}
