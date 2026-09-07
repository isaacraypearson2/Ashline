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
		return Stats;
	}
}

TArray<FAshlineWeaponDefinition> UAshlineWeaponCatalog::BuildRoster()
{
	using namespace AshlineWeapons;

	TArray<FAshlineWeaponDefinition> Roster;
	Roster.Reserve(7);

	Roster.Add(W(TEXT("WPN_AR_ASH16"), TEXT("ASH-16 Service Rifle"), EAshlineWeaponClass::AssaultRifle,
		S(26.f, 720.f, 30, 180, 400.f, 1.15f, 0.38f, 2.35f, 2.4f, 0.32f, true), 1,
		{ TEXT("OPT_HOLO"), TEXT("OPT_ACOG"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("UB_LITE"), TEXT("MAG_60"), TEXT("STK_SOPMOD"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_SMG_C9"), TEXT("C9 Compact"), EAshlineWeaponClass::SMG,
		S(18.f, 920.f, 25, 150, 120.f, 0.85f, 0.55f, 1.85f, 2.1f, 0.55f, true), 2,
		{ TEXT("OPT_HOLO"), TEXT("MUZ_SUP"), TEXT("UB_GRIP"), TEXT("MAG_EXT_SMG"), TEXT("STK_WIRE"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_SNP_G28L"), TEXT("G28-L Marksman"), EAshlineWeaponClass::Sniper,
		S(95.f, 48.f, 5, 30, 1200.f, 3.4f, 0.15f, 3.1f, 6.5f, 0.05f, false), 8,
		{ TEXT("OPT_8X"), TEXT("OPT_12X"), TEXT("MUZ_SUP_SN"), TEXT("STK_PRECISION"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_SHG_M870K"), TEXT("M870-K Breacher"), EAshlineWeaponClass::Shotgun,
		S(18.f, 70.f, 6, 30, 35.f, 4.2f, 1.1f, 0.55f, 3.8f, 1.4f, false), 4,
		{ TEXT("OPT_HOLO"), TEXT("MUZ_CHOKE"), TEXT("UB_LITE"), TEXT("STK_BREACH") }));

	Roster.Add(W(TEXT("WPN_PIS_M17A"), TEXT("M17-A Sidearm"), EAshlineWeaponClass::Sidearm,
		S(22.f, 380.f, 17, 51, 50.f, 1.6f, 0.7f, 1.4f, 2.8f, 0.8f, false), 1,
		{ TEXT("OPT_RMR"), TEXT("MUZ_SUP_P"), TEXT("MAG_EXT_P"), TEXT("LSR_PEQ") }));

	Roster.Add(W(TEXT("WPN_DMR_SASS"), TEXT("SASS-7 DMR"), EAshlineWeaponClass::DMR,
		S(48.f, 240.f, 20, 80, 750.f, 1.9f, 0.22f, 2.6f, 3.2f, 0.12f, false), 6,
		{ TEXT("OPT_ACOG"), TEXT("OPT_8X"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("MAG_DMR25"), TEXT("STK_PRECISION") }));

	Roster.Add(W(TEXT("WPN_LMG_M250"), TEXT("M250 SAW"), EAshlineWeaponClass::LMG,
		S(28.f, 650.f, 100, 200, 600.f, 1.45f, 0.5f, 6.4f, 3.6f, 0.55f, true), 10,
		{ TEXT("OPT_HOLO"), TEXT("OPT_ACOG"), TEXT("MUZ_COMP"), TEXT("UB_BIPOD"), TEXT("MAG_200"), TEXT("STK_SOPMOD") }));

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
	List.Add(A(TEXT("OPT_RMR"), TEXT("RMR Mini"), EAshlineAttachmentSlot::Optic, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.1f, -0.04f)));
	List.Add(A(TEXT("OPT_ACOG"), TEXT("4x Combat"), EAshlineAttachmentSlot::Optic, 5, D(0, 0, 0, 0, 40.f, 0, 0, 0, 0.3f, -0.08f)));
	List.Add(A(TEXT("OPT_8X"), TEXT("8x Glass"), EAshlineAttachmentSlot::Optic, 8, D(0, 0, 0, 0, 120.f, 0.1f, 0, 0.1f, 0.8f, -0.03f)));
	List.Add(A(TEXT("OPT_12X"), TEXT("12x Night Glass"), EAshlineAttachmentSlot::Optic, 12, D(0, 0, 0, 0, 200.f, 0.15f, 0, 0.15f, 1.2f, -0.02f)));

	List.Add(A(TEXT("MUZ_SUP"), TEXT("Rifle Suppressor"), EAshlineAttachmentSlot::Muzzle, 3, D(-1.f, 0, 0, 0, -20.f, -0.15f, -0.05f, 0, -0.1f, -0.02f)));
	List.Add(A(TEXT("MUZ_SUP_SN"), TEXT("Long Suppressor"), EAshlineAttachmentSlot::Muzzle, 8, D(-2.f, 0, 0, 0, -40.f, -0.2f, 0, 0.1f, 0, 0)));
	List.Add(A(TEXT("MUZ_SUP_P"), TEXT("Pistol Suppressor"), EAshlineAttachmentSlot::Muzzle, 2, D(-1.f, 0, 0, 0, -5.f, -0.1f, 0, 0, 0, 0)));
	List.Add(A(TEXT("MUZ_COMP"), TEXT("Compensator"), EAshlineAttachmentSlot::Muzzle, 4, D(0, 0, 0, 0, 0, -0.25f, -0.12f, 0, 0, 0)));
	List.Add(A(TEXT("MUZ_CHOKE"), TEXT("Tight Choke"), EAshlineAttachmentSlot::Muzzle, 5, D(1.f, 0, 0, 0, 8.f, 0.2f, 0, 0, -0.4f, -0.2f)));

	List.Add(A(TEXT("UB_GRIP"), TEXT("Vertical Grip"), EAshlineAttachmentSlot::Underbarrel, 3, D(0, 0, 0, 0, 0, -0.2f, -0.1f, 0, -0.25f, -0.04f)));
	List.Add(A(TEXT("UB_LITE"), TEXT("Weapon Light"), EAshlineAttachmentSlot::Underbarrel, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.05f, 0)));
	List.Add(A(TEXT("UB_BIPOD"), TEXT("Bipod"), EAshlineAttachmentSlot::Underbarrel, 10, D(0, 0, 0, 0, 20.f, -0.4f, -0.15f, 0.2f, 0.2f, -0.1f)));

	List.Add(A(TEXT("MAG_60"), TEXT("60rd Casket"), EAshlineAttachmentSlot::Magazine, 7, D(0, -20.f, 30, 0, 0, 0.1f, 0.05f, 0.35f, 0.15f, 0.04f)));
	List.Add(A(TEXT("MAG_EXT_SMG"), TEXT("40rd SMG"), EAshlineAttachmentSlot::Magazine, 4, D(0, 0, 15, 20, 0, 0.05f, 0, 0.15f, 0.05f, 0)));
	List.Add(A(TEXT("MAG_EXT_P"), TEXT("21rd Pistol"), EAshlineAttachmentSlot::Magazine, 3, D(0, 0, 4, 8, 0, 0, 0, 0.08f, 0, 0)));
	List.Add(A(TEXT("MAG_DMR25"), TEXT("25rd DMR"), EAshlineAttachmentSlot::Magazine, 6, D(0, 0, 5, 10, 0, 0.05f, 0, 0.12f, 0, 0)));
	List.Add(A(TEXT("MAG_200"), TEXT("200rd Softpack"), EAshlineAttachmentSlot::Magazine, 12, D(0, -15.f, 100, 0, 0, 0.15f, 0.08f, 1.1f, 0.25f, 0.08f)));

	List.Add(A(TEXT("STK_SOPMOD"), TEXT("Sopmod Stock"), EAshlineAttachmentSlot::Stock, 4, D(0, 0, 0, 0, 0, -0.12f, -0.08f, 0, -0.1f, -0.03f)));
	List.Add(A(TEXT("STK_WIRE"), TEXT("Wire Stock"), EAshlineAttachmentSlot::Stock, 3, D(0, 20.f, 0, 0, 0, 0.1f, 0.1f, -0.1f, -0.05f, 0.05f)));
	List.Add(A(TEXT("STK_PRECISION"), TEXT("Precision Stock"), EAshlineAttachmentSlot::Stock, 8, D(0, -10.f, 0, 0, 30.f, -0.2f, -0.05f, 0.1f, 0.2f, -0.06f)));
	List.Add(A(TEXT("STK_BREACH"), TEXT("Breacher Stock"), EAshlineAttachmentSlot::Stock, 5, D(0, 0, 0, 0, 0, -0.3f, 0, -0.05f, -0.2f, 0)));

	List.Add(A(TEXT("LSR_PEQ"), TEXT("PEQ Laser"), EAshlineAttachmentSlot::Laser, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.35f, -0.02f)));

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
	}
	return Stats;
}
