#include "Weapons/AshlineWeaponCatalog.h"
#include <initializer_list>

namespace AshlineWeapons
{
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
		Stats.FireMode = bAuto ? EAshlineFireMode::Auto : EAshlineFireMode::Semi;
		return Stats;
	}

	static FAshlineWeaponDefinition W(
		const TCHAR* Id,
		const TCHAR* Name,
		EAshlineWeaponClass Class,
		const FAshlineWeaponStats& Stats,
		int32 Unlock,
		std::initializer_list<const TCHAR*> Attachments,
		const TCHAR* Description = TEXT(""),
		int32 Cost = 0,
		int32 Prestige = 0)
	{
		FAshlineWeaponDefinition Def;
		Def.WeaponId = Id;
		Def.DisplayName = FText::FromString(Name);
		Def.Class = Class;
		Def.BaseStats = Stats;
		Def.UnlockLevel = Unlock;
		Def.Description = FText::FromString(Description);
		Def.CreditCost = Cost;
		Def.RequiredPrestige = Prestige;
		Def.PreviewTint = FLinearColor(0.08f, 0.08f, 0.09f);
		Def.PreviewIconPath = FString::Printf(TEXT("/Game/Ashline/UI/Icons/T_WPN_%s.T_WPN_%s"), Id, Id);
		for (const TCHAR* A : Attachments)
		{
			Def.CompatibleAttachments.Add(A);
		}
		UAshlineWeaponCatalog::ApplyClassGunfeel(Def);
		return Def;
	}

	static FAshlineAttachmentDefinition A(
		const TCHAR* Id,
		const TCHAR* Name,
		EAshlineAttachmentSlot Slot,
		int32 Unlock,
		const FAshlineWeaponStats& Delta,
		int32 Cost = 0,
		const TCHAR* Description = TEXT(""))
	{
		FAshlineAttachmentDefinition Def;
		Def.AttachmentId = Id;
		Def.DisplayName = FText::FromString(Name);
		Def.Slot = Slot;
		Def.UnlockLevel = Unlock;
		Def.StatDelta = Delta;
		Def.CreditCost = Cost > 0 ? Cost : (50 + Unlock * 40);
		Def.Description = FText::FromString(Description);
		return Def;
	}

	static FAshlineWeaponStats D(
		float Damage, float RPM, int32 Mag, int32 Reserve, float Range,
		float RP, float RY, float Reload, float Hip, float Ads)
	{
		FAshlineWeaponStats X;
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
		X.bAutomatic = true;
		return X;
	}

	static TArray<FName> MergeKit(EAshlineWeaponClass Class, std::initializer_list<const TCHAR*> Extra)
	{
		TArray<FName> Kit = UAshlineWeaponCatalog::DefaultKit(Class);
		for (const TCHAR* E : Extra)
		{
			Kit.AddUnique(E);
		}
		return Kit;
	}
}

TArray<FName> UAshlineWeaponCatalog::DefaultKit(EAshlineWeaponClass Class)
{
	TArray<FName> Kit;
	auto Add = [&Kit](std::initializer_list<const TCHAR*> Ids)
	{
		for (const TCHAR* Id : Ids)
		{
			Kit.AddUnique(Id);
		}
	};

	Add({ TEXT("LSR_PEQ"), TEXT("LSR_5MW"), TEXT("AMMO_FMJ"), TEXT("AMMO_AP"), TEXT("AMMO_HP"), TEXT("AMMO_TRACER") });

	switch (Class)
	{
	case EAshlineWeaponClass::AssaultRifle:
		Add({ TEXT("OPT_RED"), TEXT("OPT_HOLO"), TEXT("OPT_IRON"), TEXT("OPT_ACOG"), TEXT("OPT_4X"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("MUZ_BRAKE"), TEXT("MUZ_FLASH"),
			TEXT("UB_GRIP"), TEXT("UB_ANGLED"), TEXT("UB_LITE"), TEXT("MAG_60"), TEXT("MAG_FAST"), TEXT("MAG_SHORT"), TEXT("STK_SOPMOD"), TEXT("STK_PRECISION"), TEXT("STK_NOSTOCK") });
		break;
	case EAshlineWeaponClass::SMG:
		Add({ TEXT("OPT_RED"), TEXT("OPT_HOLO"), TEXT("OPT_IRON"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("UB_ANGLED"), TEXT("UB_LITE"),
			TEXT("MAG_EXT_SMG"), TEXT("MAG_DRUM"), TEXT("MAG_SHORT"), TEXT("STK_WIRE"), TEXT("STK_SOPMOD"), TEXT("STK_NOSTOCK") });
		break;
	case EAshlineWeaponClass::Sniper:
		Add({ TEXT("OPT_8X"), TEXT("OPT_12X"), TEXT("OPT_THERM"), TEXT("MUZ_SUP_SN"), TEXT("MUZ_BRAKE"), TEXT("STK_PRECISION"), TEXT("UB_BIPOD"), TEXT("LSR_PEQ") });
		break;
	case EAshlineWeaponClass::Shotgun:
		Add({ TEXT("OPT_RED"), TEXT("OPT_HOLO"), TEXT("MUZ_CHOKE"), TEXT("UB_LITE"), TEXT("STK_BREACH"), TEXT("AMMO_SLUG"), TEXT("AMMO_BUCK") });
		break;
	case EAshlineWeaponClass::Sidearm:
		Add({ TEXT("OPT_RMR"), TEXT("MUZ_SUP_P"), TEXT("MAG_EXT_P"), TEXT("LSR_5MW"), TEXT("AMMO_FMJ"), TEXT("AMMO_HP") });
		break;
	case EAshlineWeaponClass::DMR:
		Add({ TEXT("OPT_ACOG"), TEXT("OPT_4X"), TEXT("OPT_8X"), TEXT("MUZ_SUP"), TEXT("MUZ_COMP"), TEXT("UB_GRIP"), TEXT("MAG_DMR25"), TEXT("STK_PRECISION") });
		break;
	case EAshlineWeaponClass::LMG:
		Add({ TEXT("OPT_HOLO"), TEXT("OPT_ACOG"), TEXT("MUZ_COMP"), TEXT("MUZ_BRAKE"), TEXT("MUZ_HEAVY"), TEXT("UB_BIPOD"), TEXT("MAG_200"), TEXT("STK_HEAVY"), TEXT("STK_SOPMOD") });
		break;
	case EAshlineWeaponClass::Launcher:
		Add({ TEXT("OPT_RED"), TEXT("AMMO_HE"), TEXT("AMMO_FMJ") });
		break;
	case EAshlineWeaponClass::Melee:
		break;
	default:
		break;
	}
	return Kit;
}

void UAshlineWeaponCatalog::ApplyClassGunfeel(FAshlineWeaponDefinition& Weapon)
{
	FAshlineWeaponStats& S = Weapon.BaseStats;
	Weapon.SupportedFireModes.Reset();

	auto Pattern = [&S](std::initializer_list<FVector2D> Kicks)
	{
		S.RecoilPattern.Reset();
		for (const FVector2D& K : Kicks)
		{
			S.RecoilPattern.Add(K);
		}
	};

	switch (Weapon.Class)
	{
	case EAshlineWeaponClass::AssaultRifle:
		Weapon.SupportedFireModes = { EAshlineFireMode::Auto, EAshlineFireMode::Burst, EAshlineFireMode::Semi };
		S.FireMode = EAshlineFireMode::Auto;
		S.bAutomatic = true;
		S.BurstCount = 3;
		S.ADSTimeSeconds = 0.22f;
		S.ADSFov = 62.f;
		S.FalloffStartMeters = 45.f;
		S.FalloffEndMeters = 280.f;
		S.MinDamageMul = 0.58f;
		S.SwayHipDegrees = 0.5f;
		S.SwayAdsDegrees = 0.12f;
		S.ArmorPenetration = 0.38f;
		S.HeadshotMultiplier = 1.75f;
		S.PelletCount = 1;
		S.RecoilRecoveryPerSecond = 10.f;
		S.RecoilAdsMul = 0.52f;
		Pattern({ {1.18f, 0.06f}, {1.05f, -0.22f}, {0.96f, 0.28f}, {0.88f, -0.18f}, {0.82f, 0.34f}, {0.78f, -0.26f}, {0.74f, 0.2f}, {0.7f, 0.12f} });
		Weapon.CollectionId = TEXT("SET_SERVICE");
		break;
	case EAshlineWeaponClass::SMG:
		Weapon.SupportedFireModes = { EAshlineFireMode::Auto, EAshlineFireMode::Semi };
		S.FireMode = EAshlineFireMode::Auto;
		S.bAutomatic = true;
		S.ADSTimeSeconds = 0.16f;
		S.ADSFov = 68.f;
		S.FalloffStartMeters = 18.f;
		S.FalloffEndMeters = 90.f;
		S.MinDamageMul = 0.42f;
		S.SwayHipDegrees = 0.42f;
		S.SwayAdsDegrees = 0.1f;
		S.ArmorPenetration = 0.22f;
		S.HeadshotMultiplier = 1.55f;
		S.RecoilRecoveryPerSecond = 14.f;
		S.RecoilAdsMul = 0.6f;
		S.MovementSpreadMul = 1.15f;
		Pattern({ {0.72f, 0.18f}, {0.68f, -0.28f}, {0.62f, 0.32f}, {0.58f, -0.22f}, {0.55f, 0.26f}, {0.52f, -0.16f} });
		Weapon.CollectionId = TEXT("SET_CQB");
		break;
	case EAshlineWeaponClass::Sniper:
		Weapon.SupportedFireModes = { EAshlineFireMode::Semi };
		S.FireMode = EAshlineFireMode::Semi;
		S.bAutomatic = false;
		S.ADSTimeSeconds = 0.38f;
		S.ADSFov = 28.f;
		S.FalloffStartMeters = 80.f;
		S.FalloffEndMeters = 900.f;
		S.MinDamageMul = 0.72f;
		S.SwayHipDegrees = 1.1f;
		S.SwayAdsDegrees = 0.18f;
		S.SwaySpeed = 0.85f;
		S.ArmorPenetration = 0.78f;
		S.HeadshotMultiplier = 2.4f;
		S.RecoilRecoveryPerSecond = 4.5f;
		S.RecoilFirstShotMul = 1.0f;
		S.RecoilAdsMul = 0.85f;
		S.EmptyReloadMul = 1.05f;
		Pattern({ {3.4f, 0.08f} });
		Weapon.CollectionId = TEXT("SET_OVERWATCH");
		break;
	case EAshlineWeaponClass::Shotgun:
		Weapon.SupportedFireModes = { EAshlineFireMode::Semi };
		S.FireMode = EAshlineFireMode::Semi;
		S.bAutomatic = false;
		S.PelletCount = 8;
		S.AmmoType = EAshlineAmmoType::Buckshot;
		S.ADSTimeSeconds = 0.24f;
		S.ADSFov = 70.f;
		S.FalloffStartMeters = 8.f;
		S.FalloffEndMeters = 28.f;
		S.MinDamageMul = 0.28f;
		S.SwayHipDegrees = 0.48f;
		S.ArmorPenetration = 0.18f;
		S.HeadshotMultiplier = 1.25f;
		S.RecoilRecoveryPerSecond = 6.f;
		Pattern({ {4.1f, 0.35f} });
		Weapon.CollectionId = TEXT("SET_BREACH");
		break;
	case EAshlineWeaponClass::Sidearm:
		Weapon.SupportedFireModes = { EAshlineFireMode::Semi };
		S.FireMode = EAshlineFireMode::Semi;
		S.bAutomatic = false;
		S.ADSTimeSeconds = 0.14f;
		S.ADSFov = 72.f;
		S.FalloffStartMeters = 12.f;
		S.FalloffEndMeters = 55.f;
		S.MinDamageMul = 0.45f;
		S.SwayHipDegrees = 0.32f;
		S.SwayAdsDegrees = 0.08f;
		S.ArmorPenetration = 0.2f;
		S.HeadshotMultiplier = 1.9f;
		S.RecoilRecoveryPerSecond = 12.f;
		S.TacticalReloadMul = 0.82f;
		Pattern({ {1.55f, 0.22f}, {1.4f, -0.18f}, {1.25f, 0.14f} });
		Weapon.CollectionId = TEXT("SET_SIDEARM");
		break;
	case EAshlineWeaponClass::DMR:
		Weapon.SupportedFireModes = { EAshlineFireMode::Semi, EAshlineFireMode::Auto };
		S.FireMode = EAshlineFireMode::Semi;
		S.bAutomatic = false;
		S.ADSTimeSeconds = 0.28f;
		S.ADSFov = 48.f;
		S.FalloffStartMeters = 60.f;
		S.FalloffEndMeters = 520.f;
		S.MinDamageMul = 0.62f;
		S.SwayHipDegrees = 0.62f;
		S.SwayAdsDegrees = 0.14f;
		S.ArmorPenetration = 0.55f;
		S.HeadshotMultiplier = 2.05f;
		S.RecoilRecoveryPerSecond = 7.5f;
		S.RecoilAdsMul = 0.48f;
		Pattern({ {1.85f, 0.06f}, {1.6f, -0.12f}, {1.45f, 0.16f}, {1.3f, -0.1f} });
		Weapon.CollectionId = TEXT("SET_MARKSMAN");
		break;
	case EAshlineWeaponClass::LMG:
		Weapon.SupportedFireModes = { EAshlineFireMode::Auto };
		S.FireMode = EAshlineFireMode::Auto;
		S.bAutomatic = true;
		S.ADSTimeSeconds = 0.34f;
		S.ADSFov = 58.f;
		S.FalloffStartMeters = 50.f;
		S.FalloffEndMeters = 420.f;
		S.MinDamageMul = 0.6f;
		S.SwayHipDegrees = 0.85f;
		S.SwayAdsDegrees = 0.22f;
		S.ArmorPenetration = 0.48f;
		S.HeadshotMultiplier = 1.6f;
		S.RecoilRecoveryPerSecond = 6.2f;
		S.RecoilFirstShotMul = 1.35f;
		S.RecoilAdsMul = 0.5f;
		S.EmptyReloadMul = 1.0f;
		S.TacticalReloadMul = 1.0f;
		S.MovementSpreadMul = 1.7f;
		Pattern({ {1.55f, 0.12f}, {1.4f, -0.2f}, {1.28f, 0.28f}, {1.15f, -0.18f}, {1.05f, 0.32f}, {0.95f, -0.22f}, {0.88f, 0.18f}, {0.82f, 0.1f} });
		Weapon.CollectionId = TEXT("SET_SUPPORT");
		break;
	case EAshlineWeaponClass::Launcher:
		Weapon.SupportedFireModes = { EAshlineFireMode::Semi };
		S.FireMode = EAshlineFireMode::Semi;
		S.bAutomatic = false;
		S.PelletCount = 1;
		S.AmmoType = EAshlineAmmoType::HE;
		S.ADSTimeSeconds = 0.42f;
		S.ADSFov = 55.f;
		S.FalloffStartMeters = 20.f;
		S.FalloffEndMeters = 350.f;
		S.MinDamageMul = 0.7f;
		S.SwayHipDegrees = 1.2f;
		S.SwayAdsDegrees = 0.28f;
		S.ArmorPenetration = 0.92f;
		S.HeadshotMultiplier = 1.1f;
		S.RecoilRecoveryPerSecond = 3.2f;
		S.RecoilFirstShotMul = 1.0f;
		S.RecoilAdsMul = 0.9f;
		S.MovementSpreadMul = 1.8f;
		Pattern({ {5.2f, 0.4f} });
		Weapon.CollectionId = TEXT("SET_DEMO");
		break;
	case EAshlineWeaponClass::Melee:
		Weapon.SupportedFireModes = { EAshlineFireMode::Semi };
		S.FireMode = EAshlineFireMode::Semi;
		S.bAutomatic = false;
		S.PelletCount = 1;
		S.ADSTimeSeconds = 0.08f;
		S.ADSFov = 78.f;
		S.FalloffStartMeters = 1.4f;
		S.FalloffEndMeters = 2.8f;
		S.MinDamageMul = 0.35f;
		S.SwayHipDegrees = 0.2f;
		S.SwayAdsDegrees = 0.06f;
		S.ArmorPenetration = 0.15f;
		S.HeadshotMultiplier = 1.4f;
		S.RecoilRecoveryPerSecond = 16.f;
		S.RecoilAdsMul = 1.f;
		S.EmptyReloadMul = 1.f;
		S.TacticalReloadMul = 1.f;
		Pattern({ {0.4f, 0.05f} });
		Weapon.CollectionId = TEXT("SET_MELEE");
		break;
	default:
		Weapon.SupportedFireModes = { S.FireMode };
		break;
	}

	if (Weapon.CompatibleAttachments.Num() == 0)
	{
		Weapon.CompatibleAttachments = DefaultKit(Weapon.Class);
	}
}

TArray<FAshlineWeaponDefinition> UAshlineWeaponCatalog::BuildRoster()
{
	using namespace AshlineWeapons;

	TArray<FAshlineWeaponDefinition> Roster;
	Roster.Reserve(40);

	auto WithKit = [](FAshlineWeaponDefinition Def, EAshlineWeaponClass Class, std::initializer_list<const TCHAR*> Extra) -> FAshlineWeaponDefinition
	{
		Def.CompatibleAttachments = MergeKit(Class, Extra);
		return Def;
	};

	Roster.Add(WithKit(W(TEXT("WPN_AR_ASH16"), TEXT("ASH-16 Service Rifle"), EAshlineWeaponClass::AssaultRifle,
		S(26.f, 720.f, 30, 180, 400.f, 1.15f, 0.38f, 2.35f, 2.4f, 0.32f, true), 1,
		{}, TEXT("Issue 5.56 service rifle. Balanced recoil, full kit rail."), 0, 0),
		EAshlineWeaponClass::AssaultRifle, { TEXT("UB_GL") }));

	Roster.Add(WithKit(W(TEXT("WPN_AR_M4K"), TEXT("M4-K Carbine"), EAshlineWeaponClass::AssaultRifle,
		S(24.f, 800.f, 30, 180, 340.f, 1.05f, 0.42f, 2.15f, 2.2f, 0.36f, true), 3,
		{}, TEXT("Short-barrel carbine. Faster ADS, snappier horizontal bounce."), 400, 0),
		EAshlineWeaponClass::AssaultRifle, {}));

	Roster.Add(WithKit(W(TEXT("WPN_AR_AK74"), TEXT("AK-74M"), EAshlineWeaponClass::AssaultRifle,
		S(30.f, 650.f, 30, 150, 420.f, 1.35f, 0.28f, 2.55f, 2.7f, 0.34f, true), 5,
		{}, TEXT("7.62-adjacent punch in a 5.45 platform. Hard first-shot, slow recovery."), 550, 0),
		EAshlineWeaponClass::AssaultRifle, { TEXT("MAG_DRUM") }));

	Roster.Add(WithKit(W(TEXT("WPN_AR_SCARH"), TEXT("SCAR-H Battle"), EAshlineWeaponClass::AssaultRifle,
		S(34.f, 600.f, 20, 120, 480.f, 1.48f, 0.32f, 2.7f, 2.8f, 0.28f, true), 12,
		{}, TEXT("Heavy battle rifle. Mag dump hurts; two-shot potential at mid."), 900, 0),
		EAshlineWeaponClass::AssaultRifle, { TEXT("MAG_DMR25") }));

	Roster.Add(WithKit(W(TEXT("WPN_AR_416C"), TEXT("416C Compact"), EAshlineWeaponClass::AssaultRifle,
		S(23.f, 850.f, 30, 180, 280.f, 0.98f, 0.48f, 2.05f, 2.05f, 0.4f, true), 8,
		{}, TEXT("CQB 5.56. High RPM, wide hip cone, excellent after a grip."), 700, 0),
		EAshlineWeaponClass::AssaultRifle, {}));

	Roster.Add(WithKit(W(TEXT("WPN_SMG_C9"), TEXT("C9 Compact"), EAshlineWeaponClass::SMG,
		S(18.f, 920.f, 25, 150, 120.f, 0.85f, 0.55f, 1.85f, 2.1f, 0.55f, true), 2,
		{}, TEXT("Folding SMG. Room-clear king, dies past 40m."), 0, 0),
		EAshlineWeaponClass::SMG, {}));

	Roster.Add(WithKit(W(TEXT("WPN_SMG_MPX"), TEXT("MPX-9"), EAshlineWeaponClass::SMG,
		S(19.f, 850.f, 30, 180, 140.f, 0.78f, 0.42f, 1.7f, 1.95f, 0.48f, true), 6,
		{}, TEXT("Closed-bolt 9mm. Tightest SMG ADS in the locker."), 500, 0),
		EAshlineWeaponClass::SMG, {}));

	Roster.Add(WithKit(W(TEXT("WPN_SMG_VEC"), TEXT("Vector .45"), EAshlineWeaponClass::SMG,
		S(21.f, 1100.f, 25, 150, 90.f, 0.62f, 0.7f, 1.95f, 2.35f, 0.62f, true), 14,
		{}, TEXT("Super-high cyclic. Melts close, burns mag, brutal yaw."), 1100, 0),
		EAshlineWeaponClass::SMG, {}));

	Roster.Add(WithKit(W(TEXT("WPN_SMG_P90"), TEXT("P90-N"), EAshlineWeaponClass::SMG,
		S(17.f, 900.f, 50, 200, 150.f, 0.7f, 0.38f, 2.4f, 1.85f, 0.5f, true), 10,
		{}, TEXT("Top-load 50rd. Hip laser + mag dump for hallways."), 800, 0),
		EAshlineWeaponClass::SMG, { TEXT("MAG_DRUM") }));

	Roster.Add(WithKit(W(TEXT("WPN_SHG_M870K"), TEXT("M870-K Breacher"), EAshlineWeaponClass::Shotgun,
		S(18.f, 70.f, 6, 30, 35.f, 4.2f, 1.1f, 0.55f, 3.8f, 1.4f, false), 4,
		{}, TEXT("Pump 12ga. Tube reload per shell. Choke + slug transforms it."), 0, 0),
		EAshlineWeaponClass::Shotgun, {}));
	Roster.Last().BaseStats.PelletCount = 8;

	Roster.Add(WithKit(W(TEXT("WPN_SHG_M1014"), TEXT("M1014 Semi"), EAshlineWeaponClass::Shotgun,
		S(16.f, 180.f, 7, 35, 32.f, 3.4f, 0.9f, 2.8f, 3.4f, 1.25f, false), 9,
		{}, TEXT("Semi 12ga. Faster follow-ups, mag-tube hybrid reload."), 650, 0),
		EAshlineWeaponClass::Shotgun, {}));
	Roster.Last().BaseStats.PelletCount = 7;

	Roster.Add(WithKit(W(TEXT("WPN_SHG_AA12"), TEXT("AA-12 Auto"), EAshlineWeaponClass::Shotgun,
		S(14.f, 300.f, 8, 32, 28.f, 2.8f, 0.8f, 3.4f, 3.2f, 1.5f, true), 18,
		{}, TEXT("Full-auto drum 12ga. Prestige hallway deletion."), 1800, 1),
		EAshlineWeaponClass::Shotgun, { TEXT("MAG_DRUM") }));
	Roster.Last().BaseStats.PelletCount = 6;
	Roster.Last().SupportedFireModes = { EAshlineFireMode::Auto, EAshlineFireMode::Semi };
	Roster.Last().BaseStats.FireMode = EAshlineFireMode::Auto;
	Roster.Last().BaseStats.bAutomatic = true;

	Roster.Add(WithKit(W(TEXT("WPN_SNP_G28L"), TEXT("G28-L Marksman"), EAshlineWeaponClass::Sniper,
		S(95.f, 48.f, 5, 30, 1200.f, 3.4f, 0.15f, 3.1f, 6.5f, 0.05f, false), 8,
		{}, TEXT("Issue DMR-length sniper. 8x/12x glass. One-shot thorax at Regular."), 0, 0),
		EAshlineWeaponClass::Sniper, {}));

	Roster.Add(WithKit(W(TEXT("WPN_SNP_M2010"), TEXT("M2010 ESR"), EAshlineWeaponClass::Sniper,
		S(110.f, 36.f, 5, 25, 1400.f, 3.9f, 0.1f, 3.4f, 7.2f, 0.04f, false), 16,
		{}, TEXT("Bolt .300. Longer ADS, harder kick, cleaner one-shot."), 1400, 0),
		EAshlineWeaponClass::Sniper, {}));

	Roster.Add(WithKit(W(TEXT("WPN_SNP_AWM"), TEXT("AWM-X"), EAshlineWeaponClass::Sniper,
		S(130.f, 28.f, 5, 20, 1600.f, 4.6f, 0.08f, 3.8f, 8.0f, 0.03f, false), 22,
		{}, TEXT("Magnum bolt. Anti-heavy. Slowest cycle in the catalog."), 2000, 0),
		EAshlineWeaponClass::Sniper, {}));

	Roster.Add(WithKit(W(TEXT("WPN_DMR_SASS"), TEXT("SASS-7 DMR"), EAshlineWeaponClass::DMR,
		S(48.f, 240.f, 20, 80, 750.f, 1.9f, 0.22f, 2.6f, 3.2f, 0.12f, false), 6,
		{}, TEXT("7.62 DMR. Semi primary, can flip auto in a pinch."), 0, 0),
		EAshlineWeaponClass::DMR, {}));

	Roster.Add(WithKit(W(TEXT("WPN_DMR_MK14"), TEXT("MK14 EBR"), EAshlineWeaponClass::DMR,
		S(52.f, 700.f, 20, 80, 700.f, 2.15f, 0.4f, 2.8f, 3.5f, 0.18f, true), 15,
		{}, TEXT("Battle DMR. Auto is a trap unless you tap."), 1200, 0),
		EAshlineWeaponClass::DMR, {}));
	Roster.Last().SupportedFireModes = { EAshlineFireMode::Semi, EAshlineFireMode::Auto };
	Roster.Last().BaseStats.FireMode = EAshlineFireMode::Semi;

	Roster.Add(WithKit(W(TEXT("WPN_DMR_SVD"), TEXT("SVD-M"), EAshlineWeaponClass::DMR,
		S(58.f, 180.f, 10, 50, 850.f, 2.3f, 0.18f, 2.9f, 3.6f, 0.1f, false), 11,
		{}, TEXT("Long 7.62. Ten-round, hard falloff past 600."), 950, 0),
		EAshlineWeaponClass::DMR, {}));

	Roster.Add(WithKit(W(TEXT("WPN_LMG_M250"), TEXT("M250 SAW"), EAshlineWeaponClass::LMG,
		S(28.f, 650.f, 100, 200, 600.f, 1.45f, 0.5f, 6.4f, 3.6f, 0.55f, true), 10,
		{}, TEXT("Issue SAW. Open bolt, 100rd. Bipod turns it honest."), 0, 0),
		EAshlineWeaponClass::LMG, {}));

	Roster.Add(WithKit(W(TEXT("WPN_LMG_M240"), TEXT("M240B"), EAshlineWeaponClass::LMG,
		S(32.f, 550.f, 100, 200, 700.f, 1.7f, 0.42f, 7.1f, 4.0f, 0.5f, true), 17,
		{}, TEXT("7.62 belt. Slower cyclic, meaner falloff curve."), 1300, 0),
		EAshlineWeaponClass::LMG, {}));

	Roster.Add(WithKit(W(TEXT("WPN_LMG_PKM"), TEXT("PKM-N"), EAshlineWeaponClass::LMG,
		S(34.f, 600.f, 100, 200, 680.f, 1.62f, 0.55f, 6.8f, 3.9f, 0.58f, true), 20,
		{}, TEXT("Open-bolt PK. Slightly worse ADS, better hip suppression."), 1500, 0),
		EAshlineWeaponClass::LMG, {}));

	Roster.Add(WithKit(W(TEXT("WPN_PIS_M17A"), TEXT("M17-A Sidearm"), EAshlineWeaponClass::Sidearm,
		S(22.f, 380.f, 17, 51, 50.f, 1.6f, 0.7f, 1.4f, 2.8f, 0.8f, false), 1,
		{}, TEXT("Issue 9mm. Reliable panic gun. RMR + HP for CQB."), 0, 0),
		EAshlineWeaponClass::Sidearm, {}));

	Roster.Add(WithKit(W(TEXT("WPN_PIS_G19"), TEXT("G19-C"), EAshlineWeaponClass::Sidearm,
		S(20.f, 420.f, 15, 60, 45.f, 1.45f, 0.62f, 1.25f, 2.5f, 0.75f, false), 4,
		{}, TEXT("Compact 9mm. Fastest swap-to-fire in the sidearm row."), 250, 0),
		EAshlineWeaponClass::Sidearm, {}));

	Roster.Add(WithKit(W(TEXT("WPN_PIS_DEAG"), TEXT("D50 Magnum"), EAshlineWeaponClass::Sidearm,
		S(55.f, 160.f, 7, 28, 70.f, 3.2f, 0.9f, 1.9f, 3.6f, 0.7f, false), 13,
		{}, TEXT("50-cal pistol. Two-shot body, cruel recovery."), 1000, 0),
		EAshlineWeaponClass::Sidearm, {}));
	Roster.Last().BaseStats.ArmorPenetration = 0.55f;
	Roster.Last().BaseStats.HeadshotMultiplier = 2.2f;

	Roster.Add(WithKit(W(TEXT("WPN_PIS_MP443"), TEXT("MP-443"), EAshlineWeaponClass::Sidearm,
		S(24.f, 340.f, 18, 54, 55.f, 1.7f, 0.58f, 1.5f, 2.9f, 0.78f, false), 7,
		{}, TEXT("Service 9x19. Slightly chunkier slide, extra mag."), 300, 0),
		EAshlineWeaponClass::Sidearm, {}));

	Roster.Add(WithKit(W(TEXT("WPN_AR_AUG"), TEXT("AUG-A3"), EAshlineWeaponClass::AssaultRifle,
		S(27.f, 680.f, 30, 180, 420.f, 1.08f, 0.3f, 2.45f, 2.15f, 0.26f, true), 11,
		{}, TEXT("Bullpup 5.56. Built-in optic feel, tight ADS, slower swap."), 850, 0),
		EAshlineWeaponClass::AssaultRifle, { TEXT("OPT_ACOG") }));

	Roster.Add(WithKit(W(TEXT("WPN_AR_FAMAS"), TEXT("FAMAS F1"), EAshlineWeaponClass::AssaultRifle,
		S(25.f, 1000.f, 25, 150, 360.f, 1.22f, 0.5f, 2.5f, 2.3f, 0.38f, true), 13,
		{}, TEXT("Burst-first bullpup. Three-round is the honest mode."), 950, 0),
		EAshlineWeaponClass::AssaultRifle, {}));
	Roster.Last().SupportedFireModes = { EAshlineFireMode::Burst, EAshlineFireMode::Semi, EAshlineFireMode::Auto };
	Roster.Last().BaseStats.FireMode = EAshlineFireMode::Burst;
	Roster.Last().BaseStats.BurstCount = 3;
	Roster.Last().BaseStats.bAutomatic = false;

	Roster.Add(WithKit(W(TEXT("WPN_SMG_UMP"), TEXT("UMP-45"), EAshlineWeaponClass::SMG,
		S(24.f, 600.f, 25, 150, 130.f, 0.92f, 0.36f, 2.05f, 2.0f, 0.42f, true), 9,
		{}, TEXT("Slow .45 SMG. Hits like a carbine inside 25m."), 650, 0),
		EAshlineWeaponClass::SMG, {}));

	Roster.Add(WithKit(W(TEXT("WPN_SMG_UZI"), TEXT("UZI-M"), EAshlineWeaponClass::SMG,
		S(18.f, 600.f, 32, 160, 80.f, 0.88f, 0.6f, 1.9f, 2.4f, 0.7f, true), 5,
		{}, TEXT("Open-bolt classic. Hip laser + mag dump."), 450, 0),
		EAshlineWeaponClass::SMG, { TEXT("STK_WIRE") }));

	Roster.Add(WithKit(W(TEXT("WPN_SHG_SPAS"), TEXT("SPAS-12"), EAshlineWeaponClass::Shotgun,
		S(17.f, 90.f, 8, 32, 34.f, 3.9f, 1.0f, 2.6f, 3.5f, 1.3f, false), 12,
		{}, TEXT("Dual-mode 12ga. Pump by default; choke + slug for doors."), 800, 0),
		EAshlineWeaponClass::Shotgun, {}));
	Roster.Last().BaseStats.PelletCount = 8;

	Roster.Add(WithKit(W(TEXT("WPN_PIS_1911"), TEXT("M1911-A"), EAshlineWeaponClass::Sidearm,
		S(32.f, 280.f, 8, 32, 48.f, 2.1f, 0.55f, 1.55f, 2.6f, 0.7f, false), 6,
		{}, TEXT(".45 ACP. Hard slap, seven in the tube plus one."), 280, 0),
		EAshlineWeaponClass::Sidearm, {}));

	Roster.Add(WithKit(W(TEXT("WPN_PIS_REV"), TEXT("Revolver .357"), EAshlineWeaponClass::Sidearm,
		S(48.f, 140.f, 6, 24, 60.f, 2.8f, 0.7f, 2.2f, 3.2f, 0.65f, false), 15,
		{}, TEXT("Wheelgun. Slow, mean, AP-friendly."), 900, 0),
		EAshlineWeaponClass::Sidearm, {}));
	Roster.Last().BaseStats.ArmorPenetration = 0.48f;

	Roster.Add(WithKit(W(TEXT("WPN_LCH_RPG"), TEXT("RPG-7V"), EAshlineWeaponClass::Launcher,
		S(220.f, 18.f, 1, 3, 380.f, 5.4f, 0.5f, 3.6f, 4.5f, 1.2f, false), 14,
		{}, TEXT("Dumbfire HE. Hitscan stand-in until projectile FX lands. Anti-heavy."), 1400, 0),
		EAshlineWeaponClass::Launcher, {}));

	Roster.Add(WithKit(W(TEXT("WPN_LCH_AT4"), TEXT("AT4-CS"), EAshlineWeaponClass::Launcher,
		S(240.f, 14.f, 1, 2, 420.f, 4.8f, 0.35f, 4.2f, 4.0f, 0.9f, false), 19,
		{}, TEXT("Disposable AT. Cleaner ADS, fewer rockets."), 1700, 0),
		EAshlineWeaponClass::Launcher, {}));

	Roster.Add(WithKit(W(TEXT("WPN_MEL_KNIFE"), TEXT("Combat knife"), EAshlineWeaponClass::Melee,
		S(78.f, 110.f, 1, 999, 2.4f, 0.35f, 0.05f, 0.42f, 1.2f, 0.4f, false), 1,
		{}, TEXT("Issue blade. Silent. Trace clamps to melee range."), 0, 0),
		EAshlineWeaponClass::Melee, {}));

	Roster.Add(WithKit(W(TEXT("WPN_MEL_TOMA"), TEXT("Tomahawk"), EAshlineWeaponClass::Melee,
		S(95.f, 85.f, 1, 999, 2.8f, 0.5f, 0.08f, 0.55f, 1.4f, 0.45f, false), 21,
		{}, TEXT("Heavier swing. Slightly longer reach, slower recovery."), 600, 0),
		EAshlineWeaponClass::Melee, {}));

	return Roster;
}

TArray<FAshlineAttachmentDefinition> UAshlineWeaponCatalog::BuildAttachments()
{
	using namespace AshlineWeapons;
	TArray<FAshlineAttachmentDefinition> List;

	List.Add(A(TEXT("OPT_RED"), TEXT("Red Dot"), EAshlineAttachmentSlot::Optic, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.25f, -0.06f), 80, TEXT("1x reflex. Hip-to-ADS speed, no zoom.")));
	List.Add(A(TEXT("OPT_HOLO"), TEXT("Holo-1"), EAshlineAttachmentSlot::Optic, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.2f, -0.05f), 100, TEXT("Holographic 1x. Slightly cleaner than iron.")));
	List.Add(A(TEXT("OPT_IRON"), TEXT("Backup irons"), EAshlineAttachmentSlot::Optic, 1, D(0, 0, 0, 0, 0, 0, 0, -0.04f, 0, 0.02f), 40, TEXT("Light irons. Fastest ADS, no zoom help.")));
	List.Add(A(TEXT("OPT_RMR"), TEXT("RMR Mini"), EAshlineAttachmentSlot::Optic, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.1f, -0.04f), 120, TEXT("Pistol/SMG mini reflex.")));
	List.Add(A(TEXT("OPT_ACOG"), TEXT("4x Combat"), EAshlineAttachmentSlot::Optic, 5, D(0, 0, 0, 0, 40.f, 0, 0, 0, 0.3f, -0.08f), 280, TEXT("4x. ADS slower, mid-lane laser.")));
	List.Add(A(TEXT("OPT_4X"), TEXT("4x Variable"), EAshlineAttachmentSlot::Optic, 7, D(0, 0, 0, 0, 50.f, 0.05f, 0, 0.05f, 0.25f, -0.09f), 320, TEXT("Cleaner 4x with a touch more sway.")));
	List.Add(A(TEXT("OPT_8X"), TEXT("8x Glass"), EAshlineAttachmentSlot::Optic, 8, D(0, 0, 0, 0, 120.f, 0.1f, 0, 0.1f, 0.8f, -0.03f), 450, TEXT("8x. DMR/sniper default.")));
	List.Add(A(TEXT("OPT_12X"), TEXT("12x Night Glass"), EAshlineAttachmentSlot::Optic, 12, D(0, 0, 0, 0, 200.f, 0.15f, 0, 0.15f, 1.2f, -0.02f), 700, TEXT("12x. Glint risk, max range.")));
	List.Add(A(TEXT("OPT_THERM"), TEXT("Thermal 4x"), EAshlineAttachmentSlot::Optic, 18, D(0, 0, 0, 0, 60.f, 0.08f, 0, 0.12f, 0.4f, -0.05f), 1200, TEXT("Thermal overlay. Heavy, prestige-adjacent cost.")));

	List.Add(A(TEXT("MUZ_SUP"), TEXT("Rifle Suppressor"), EAshlineAttachmentSlot::Muzzle, 3, D(-1.f, 0, 0, 0, -20.f, -0.15f, -0.05f, 0, -0.1f, -0.02f), 220, TEXT("Hides flash. Slight velocity loss.")));
	List.Add(A(TEXT("MUZ_SUP_SN"), TEXT("Long Suppressor"), EAshlineAttachmentSlot::Muzzle, 8, D(-2.f, 0, 0, 0, -40.f, -0.2f, 0, 0.1f, 0, 0), 360, TEXT("Sniper can. Adds ADS time.")));
	List.Add(A(TEXT("MUZ_SUP_P"), TEXT("Pistol Suppressor"), EAshlineAttachmentSlot::Muzzle, 2, D(-1.f, 0, 0, 0, -5.f, -0.1f, 0, 0, 0, 0), 160, TEXT("Pistol can.")));
	List.Add(A(TEXT("MUZ_COMP"), TEXT("Compensator"), EAshlineAttachmentSlot::Muzzle, 4, D(0, 0, 0, 0, 0, -0.25f, -0.12f, 0, 0, 0), 200, TEXT("Cuts vertical and a slice of yaw.")));
	List.Add(A(TEXT("MUZ_BRAKE"), TEXT("Muzzle Brake"), EAshlineAttachmentSlot::Muzzle, 6, D(0, 0, 0, 0, 0, -0.35f, 0.08f, 0, 0.05f, 0), 240, TEXT("Kills pitch, adds concussion yaw.")));
	List.Add(A(TEXT("MUZ_FLASH"), TEXT("Flash Hider"), EAshlineAttachmentSlot::Muzzle, 2, D(0, 0, 0, 0, 0, -0.05f, -0.04f, 0, 0, 0), 90, TEXT("Hides muzzle flash only.")));
	List.Add(A(TEXT("MUZ_CHOKE"), TEXT("Tight Choke"), EAshlineAttachmentSlot::Muzzle, 5, D(1.f, 0, 0, 0, 8.f, 0.2f, 0, 0, -0.4f, -0.2f), 180, TEXT("Tighter pellet cone. More kick.")));
	List.Add(A(TEXT("MUZ_HEAVY"), TEXT("Heavy Brake"), EAshlineAttachmentSlot::Muzzle, 10, D(0, -15.f, 0, 0, 0, -0.45f, 0.12f, 0.08f, 0.1f, 0.04f), 360, TEXT("LMG/battle-rifle brake. Kills pitch, ADS tax.")));

	List.Add(A(TEXT("UB_GRIP"), TEXT("Vertical Grip"), EAshlineAttachmentSlot::Underbarrel, 3, D(0, 0, 0, 0, 0, -0.2f, -0.1f, 0, -0.25f, -0.04f), 180, TEXT("Vertical recoil and hip.")));
	List.Add(A(TEXT("UB_ANGLED"), TEXT("Angled Grip"), EAshlineAttachmentSlot::Underbarrel, 5, D(0, 0, 0, 0, 0, -0.08f, -0.18f, 0, -0.12f, -0.06f), 200, TEXT("ADS speed + yaw control.")));
	List.Add(A(TEXT("UB_LITE"), TEXT("Weapon Light"), EAshlineAttachmentSlot::Underbarrel, 1, D(0, 0, 0, 0, 0, 0, 0, 0, -0.05f, 0), 60, TEXT("Night utility. Tiny hip help.")));
	List.Add(A(TEXT("UB_BIPOD"), TEXT("Bipod"), EAshlineAttachmentSlot::Underbarrel, 10, D(0, 0, 0, 0, 20.f, -0.4f, -0.15f, 0.2f, 0.2f, -0.1f), 320, TEXT("LMG/sniper anchor. Hurts move-spread.")));
	List.Add(A(TEXT("UB_GL"), TEXT("Grenade Launcher"), EAshlineAttachmentSlot::Underbarrel, 14, D(0, -30.f, 0, 0, 0, 0.15f, 0.1f, 0.2f, 0.2f, 0.06f), 800, TEXT("Utility. Heavier rifle.")));

	List.Add(A(TEXT("MAG_60"), TEXT("60rd Casket"), EAshlineAttachmentSlot::Magazine, 7, D(0, -20.f, 30, 0, 0, 0.1f, 0.05f, 0.35f, 0.15f, 0.04f), 340, TEXT("AR casket. Reload tax.")));
	List.Add(A(TEXT("MAG_EXT_SMG"), TEXT("40rd SMG"), EAshlineAttachmentSlot::Magazine, 4, D(0, 0, 15, 20, 0, 0.05f, 0, 0.15f, 0.05f, 0), 180, TEXT("SMG stick +15.")));
	List.Add(A(TEXT("MAG_EXT_P"), TEXT("21rd Pistol"), EAshlineAttachmentSlot::Magazine, 3, D(0, 0, 4, 8, 0, 0, 0, 0.08f, 0, 0), 120, TEXT("Pistol +4.")));
	List.Add(A(TEXT("MAG_DMR25"), TEXT("25rd DMR"), EAshlineAttachmentSlot::Magazine, 6, D(0, 0, 5, 10, 0, 0.05f, 0, 0.12f, 0, 0), 220, TEXT("DMR +5.")));
	List.Add(A(TEXT("MAG_200"), TEXT("200rd Softpack"), EAshlineAttachmentSlot::Magazine, 12, D(0, -15.f, 100, 0, 0, 0.15f, 0.08f, 1.1f, 0.25f, 0.08f), 600, TEXT("LMG belt doubling.")));
	List.Add(A(TEXT("MAG_DRUM"), TEXT("Drum Mag"), EAshlineAttachmentSlot::Magazine, 11, D(0, -25.f, 40, 20, 0, 0.18f, 0.1f, 0.55f, 0.22f, 0.06f), 480, TEXT("Drum. Heavy, slow.")));
	List.Add(A(TEXT("MAG_FAST"), TEXT("Fast Mag"), EAshlineAttachmentSlot::Magazine, 4, D(0, 0, 0, 0, 0, 0, 0, -0.28f, 0, 0), 200, TEXT("Tactical reload cut.")));
	List.Add(A(TEXT("MAG_SHORT"), TEXT("Short Mag"), EAshlineAttachmentSlot::Magazine, 2, D(0, 15.f, -10, 0, 0, -0.08f, -0.04f, -0.18f, -0.08f, -0.02f), 90, TEXT("Lighter mag. Faster handling, fewer rounds.")));

	List.Add(A(TEXT("STK_SOPMOD"), TEXT("Sopmod Stock"), EAshlineAttachmentSlot::Stock, 4, D(0, 0, 0, 0, 0, -0.12f, -0.08f, 0, -0.1f, -0.03f), 180, TEXT("All-rounder stock.")));
	List.Add(A(TEXT("STK_WIRE"), TEXT("Wire Stock"), EAshlineAttachmentSlot::Stock, 3, D(0, 20.f, 0, 0, 0, 0.1f, 0.1f, -0.1f, -0.05f, 0.05f), 140, TEXT("ADS speed, worse recoil.")));
	List.Add(A(TEXT("STK_PRECISION"), TEXT("Precision Stock"), EAshlineAttachmentSlot::Stock, 8, D(0, -10.f, 0, 0, 30.f, -0.2f, -0.05f, 0.1f, 0.2f, -0.06f), 280, TEXT("Long-range. Hip penalty.")));
	List.Add(A(TEXT("STK_BREACH"), TEXT("Breacher Stock"), EAshlineAttachmentSlot::Stock, 5, D(0, 0, 0, 0, 0, -0.3f, 0, -0.05f, -0.2f, 0), 200, TEXT("Shotgun CQB stock.")));
	List.Add(A(TEXT("STK_HEAVY"), TEXT("Heavy Stock"), EAshlineAttachmentSlot::Stock, 9, D(0, -20.f, 0, 0, 10.f, -0.22f, -0.12f, 0.15f, 0.15f, -0.04f), 260, TEXT("LMG ballast.")));
	List.Add(A(TEXT("STK_NOSTOCK"), TEXT("No Stock"), EAshlineAttachmentSlot::Stock, 6, D(0, 40.f, 0, 0, -20.f, 0.22f, 0.18f, -0.15f, -0.15f, 0.08f), 120, TEXT("CQB. ADS snap, recoil tax.")));

	List.Add(A(TEXT("LSR_PEQ"), TEXT("PEQ Laser"), EAshlineAttachmentSlot::Laser, 2, D(0, 0, 0, 0, 0, 0, 0, 0, -0.35f, -0.02f), 160, TEXT("Hip-fire laser. Night IR slot.")));
	List.Add(A(TEXT("LSR_5MW"), TEXT("5mW Laser"), EAshlineAttachmentSlot::Laser, 5, D(0, 0, 0, 0, 0, 0, 0, 0, -0.5f, 0.02f), 220, TEXT("Visible laser. Stronger hip, ADS clutter.")));

	FAshlineWeaponStats AmmoFMJ;
	List.Add(A(TEXT("AMMO_FMJ"), TEXT("FMJ"), EAshlineAttachmentSlot::Ammunition, 1, AmmoFMJ, 0, TEXT("Default ball. Balanced pen and flesh.")));
	FAshlineWeaponStats AmmoAP;
	AmmoAP.Damage = -2.f;
	AmmoAP.RangeMeters = 20.f;
	AmmoAP.ArmorPenetration = 0.25f;
	List.Add(A(TEXT("AMMO_AP"), TEXT("Armor Piercing"), EAshlineAttachmentSlot::Ammunition, 8, AmmoAP, 280, TEXT("Pen vs armor / heavies. Weaker flesh.")));
	FAshlineWeaponStats AmmoHP;
	AmmoHP.Damage = 3.f;
	AmmoHP.RangeMeters = -25.f;
	AmmoHP.ArmorPenetration = -0.15f;
	AmmoHP.MinDamageMul = -0.08f;
	List.Add(A(TEXT("AMMO_HP"), TEXT("Hollow Point"), EAshlineAttachmentSlot::Ammunition, 6, AmmoHP, 220, TEXT("Soft target damage. Dies vs plates.")));
	FAshlineWeaponStats AmmoTr;
	AmmoTr.HipFireSpread = 0.08f;
	List.Add(A(TEXT("AMMO_TRACER"), TEXT("Tracer"), EAshlineAttachmentSlot::Ammunition, 3, AmmoTr, 80, TEXT("Visible flight. Tiny hip penalty.")));
	FAshlineWeaponStats AmmoSlug;
	AmmoSlug.Damage = 28.f;
	AmmoSlug.PelletCount = -7;
	AmmoSlug.RangeMeters = 25.f;
	AmmoSlug.HipFireSpread = -1.2f;
	AmmoSlug.ADSSpread = -0.6f;
	AmmoSlug.ArmorPenetration = 0.2f;
	List.Add(A(TEXT("AMMO_SLUG"), TEXT("Slug"), EAshlineAttachmentSlot::Ammunition, 7, AmmoSlug, 240, TEXT("Single 12ga slug. Turns a breacher into a DMR-lite.")));
	FAshlineWeaponStats AmmoBuck;
	AmmoBuck.PelletCount = 2;
	AmmoBuck.HipFireSpread = 0.3f;
	List.Add(A(TEXT("AMMO_BUCK"), TEXT("00 Buck"), EAshlineAttachmentSlot::Ammunition, 1, AmmoBuck, 0, TEXT("Extra pellets. Wider cone.")));
	FAshlineWeaponStats AmmoHE;
	AmmoHE.Damage = 40.f;
	AmmoHE.RangeMeters = -30.f;
	AmmoHE.HipFireSpread = 0.2f;
	List.Add(A(TEXT("AMMO_HE"), TEXT("HE Warhead"), EAshlineAttachmentSlot::Ammunition, 14, AmmoHE, 400, TEXT("Launcher HE. More blast, less range.")));

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
	EAshlineAmmoType Ammo = Stats.AmmoType;

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

		const FAshlineWeaponStats& D = Attachment.StatDelta;
		Stats.Damage = FMath::Max(1.f, Stats.Damage + D.Damage);
		Stats.FireRateRPM = FMath::Max(10.f, Stats.FireRateRPM + D.FireRateRPM);
		Stats.MagazineSize = FMath::Max(1, Stats.MagazineSize + D.MagazineSize);
		Stats.ReserveAmmo = FMath::Max(0, Stats.ReserveAmmo + D.ReserveAmmo);
		Stats.RangeMeters = FMath::Max(5.f, Stats.RangeMeters + D.RangeMeters);
		Stats.RecoilPitch = FMath::Max(0.05f, Stats.RecoilPitch + D.RecoilPitch);
		Stats.RecoilYaw = FMath::Max(0.05f, Stats.RecoilYaw + D.RecoilYaw);
		Stats.ReloadSeconds = FMath::Max(0.2f, Stats.ReloadSeconds + D.ReloadSeconds);
		Stats.HipFireSpread = FMath::Max(0.05f, Stats.HipFireSpread + D.HipFireSpread);
		Stats.ADSSpread = FMath::Max(0.01f, Stats.ADSSpread + D.ADSSpread);
		Stats.PelletCount = FMath::Max(1, Stats.PelletCount + D.PelletCount);
		Stats.ArmorPenetration = FMath::Clamp(Stats.ArmorPenetration + D.ArmorPenetration, 0.f, 1.f);
		Stats.ADSTimeSeconds = FMath::Max(0.05f, Stats.ADSTimeSeconds + D.ADSTimeSeconds);
		Stats.FalloffStartMeters = FMath::Max(4.f, Stats.FalloffStartMeters + D.FalloffStartMeters);
		Stats.FalloffEndMeters = FMath::Max(Stats.FalloffStartMeters + 5.f, Stats.FalloffEndMeters + D.FalloffEndMeters);
		Stats.MinDamageMul = FMath::Clamp(Stats.MinDamageMul + D.MinDamageMul, 0.15f, 1.f);
		Stats.SwayHipDegrees = FMath::Max(0.02f, Stats.SwayHipDegrees + D.SwayHipDegrees);
		Stats.SwayAdsDegrees = FMath::Max(0.01f, Stats.SwayAdsDegrees + D.SwayAdsDegrees);
		Stats.MovementSpreadMul = FMath::Max(1.f, Stats.MovementSpreadMul + D.MovementSpreadMul);

		if (Attachment.Slot == EAshlineAttachmentSlot::Ammunition)
		{
			if (Attachment.AttachmentId == TEXT("AMMO_AP")) { Ammo = EAshlineAmmoType::AP; }
			else if (Attachment.AttachmentId == TEXT("AMMO_HP")) { Ammo = EAshlineAmmoType::HP; }
			else if (Attachment.AttachmentId == TEXT("AMMO_TRACER")) { Ammo = EAshlineAmmoType::Tracer; }
			else if (Attachment.AttachmentId == TEXT("AMMO_SLUG")) { Ammo = EAshlineAmmoType::Slug; }
			else if (Attachment.AttachmentId == TEXT("AMMO_BUCK")) { Ammo = EAshlineAmmoType::Buckshot; }
			else if (Attachment.AttachmentId == TEXT("AMMO_HE")) { Ammo = EAshlineAmmoType::HE; }
			else { Ammo = EAshlineAmmoType::FMJ; }
		}
	}

	ApplyAmmoType(Stats, Ammo);
	return Stats;
}

void UAshlineWeaponCatalog::ApplyAmmoType(FAshlineWeaponStats& Stats, EAshlineAmmoType Type)
{
	Stats.AmmoType = Type;
	if (Type == EAshlineAmmoType::Slug)
	{
		Stats.PelletCount = 1;
	}
}

float UAshlineWeaponCatalog::DamageAtRange(const FAshlineWeaponStats& Stats, float DistanceMeters)
{
	if (DistanceMeters <= Stats.FalloffStartMeters)
	{
		return Stats.Damage;
	}
	if (DistanceMeters >= Stats.FalloffEndMeters)
	{
		return Stats.Damage * Stats.MinDamageMul;
	}
	const float Alpha = (DistanceMeters - Stats.FalloffStartMeters) / FMath::Max(1.f, Stats.FalloffEndMeters - Stats.FalloffStartMeters);
	return Stats.Damage * FMath::Lerp(1.f, Stats.MinDamageMul, Alpha);
}

FVector2D UAshlineWeaponCatalog::RecoilKick(const FAshlineWeaponStats& Stats, int32 ShotIndex, bool bADS)
{
	FVector2D Kick(Stats.RecoilPitch, Stats.RecoilYaw);
	if (Stats.RecoilPattern.Num() > 0)
	{
		const int32 Idx = FMath::Clamp(ShotIndex, 0, Stats.RecoilPattern.Num() - 1);
		Kick = Stats.RecoilPattern[Idx];
	}
	if (ShotIndex == 0)
	{
		Kick *= Stats.RecoilFirstShotMul;
	}
	Kick *= bADS ? Stats.RecoilAdsMul : 1.f;
	return Kick;
}

TArray<FAshlineWeaponDefinition> UAshlineWeaponCatalog::WeaponsOfClass(EAshlineWeaponClass Class)
{
	TArray<FAshlineWeaponDefinition> Out;
	for (const FAshlineWeaponDefinition& Weapon : BuildRoster())
	{
		if (Weapon.Class == Class)
		{
			Out.Add(Weapon);
		}
	}
	return Out;
}

TArray<FName> UAshlineWeaponCatalog::StarterWeaponIds()
{
	return { TEXT("WPN_AR_ASH16"), TEXT("WPN_PIS_M17A") };
}

int32 UAshlineWeaponCatalog::UpgradeCostForClass(EAshlineWeaponClass Class, int32 CurrentTier)
{
	const int32 Tier = FMath::Clamp(CurrentTier, 0, MaxUpgradeTier - 1);
	int32 Base = 250;
	int32 Step = 300;
	switch (Class)
	{
	case EAshlineWeaponClass::Sidearm: Base = 150; Step = 180; break;
	case EAshlineWeaponClass::SMG: Base = 200; Step = 240; break;
	case EAshlineWeaponClass::Shotgun: Base = 220; Step = 260; break;
	case EAshlineWeaponClass::DMR: Base = 320; Step = 340; break;
	case EAshlineWeaponClass::Sniper:
	case EAshlineWeaponClass::LMG:
	case EAshlineWeaponClass::Launcher: Base = 400; Step = 420; break;
	case EAshlineWeaponClass::Melee: Base = 120; Step = 140; break;
	default: break;
	}
	return Base + Tier * Step;
}

int32 UAshlineWeaponCatalog::AttachmentUnlockCost(const FAshlineAttachmentDefinition& Attachment)
{
	return Attachment.CreditCost;
}

FString UAshlineWeaponCatalog::FireModeName(EAshlineFireMode Mode)
{
	switch (Mode)
	{
	case EAshlineFireMode::Semi: return TEXT("SEMI");
	case EAshlineFireMode::Burst: return TEXT("BURST");
	case EAshlineFireMode::Auto: return TEXT("AUTO");
	default: return TEXT("SEMI");
	}
}

FString UAshlineWeaponCatalog::AmmoTypeName(EAshlineAmmoType Type)
{
	switch (Type)
	{
	case EAshlineAmmoType::AP: return TEXT("AP");
	case EAshlineAmmoType::HP: return TEXT("HP");
	case EAshlineAmmoType::Tracer: return TEXT("TR");
	case EAshlineAmmoType::Slug: return TEXT("SLUG");
	case EAshlineAmmoType::Buckshot: return TEXT("BUCK");
	case EAshlineAmmoType::HE: return TEXT("HE");
	default: return TEXT("FMJ");
	}
}

EAshlineFireMode UAshlineWeaponCatalog::CycleFireMode(const FAshlineWeaponDefinition& Weapon, EAshlineFireMode Current)
{
	if (Weapon.SupportedFireModes.Num() == 0)
	{
		return Current;
	}
	int32 Index = Weapon.SupportedFireModes.IndexOfByKey(Current);
	Index = (Index + 1) % Weapon.SupportedFireModes.Num();
	return Weapon.SupportedFireModes[Index];
}
