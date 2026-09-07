#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.generated.h"

class USkeletalMesh;
class UMaterialInterface;

UENUM(BlueprintType)
enum class EAshlineMissionId : uint8
{
	None = 0,
	ASH_01_WireCut,
	ASH_02_DustMarket,
	ASH_03_Holdfast,
	ASH_04_NightGlass,
	ASH_05_ConvoyGhost,
	ASH_06_AshHarbor,
	ASH_07_Whiteout,
	ASH_08_Catacomb,
	ASH_09_RidgeWire,
	ASH_10_FalseFlag,
	ASH_11_LastTrain,
	ASH_12_Ashline
};

UENUM(BlueprintType)
enum class EAshlineMissionState : uint8
{
	Locked,
	Available,
	InProgress,
	Completed
};

UENUM(BlueprintType)
enum class EAshlineDifficulty : uint8
{
	Recruit,
	Regular,
	Veteran,
	Extreme
};

UENUM(BlueprintType)
enum class EAshlineCameraMode : uint8
{
	FirstPerson,
	ThirdPerson
};

UENUM(BlueprintType)
enum class EAshlineWeaponClass : uint8
{
	AssaultRifle,
	SMG,
	Sniper,
	Shotgun,
	Sidearm,
	DMR,
	LMG
};

UENUM(BlueprintType)
enum class EAshlineAttachmentSlot : uint8
{
	Optic,
	Muzzle,
	Underbarrel,
	Magazine,
	Stock,
	Laser
};

UENUM(BlueprintType)
enum class EAshlineAIArchetype : uint8
{
	Rifleman,
	Breacher,
	Marksman,
	MachineGunner,
	Officer,
	Scout,
	Heavy,
	CivilianIrregular
};

UENUM(BlueprintType)
enum class EAshlineLootRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary
};

UENUM(BlueprintType)
enum class EAshlineCosmeticSlot : uint8
{
	Helmet,
	Vest,
	Pants,
	Gloves,
	Boots,
	Camo,
	Face,
	Voice,
	Charm
};

UENUM(BlueprintType)
enum class EAshlineGraphicsPreset : uint8
{
	Low,
	Medium,
	High,
	Epic,
	Cinematic,
	/** 1440p high-refresh on Radeon 9070-class (FSR3 Balanced + cheaper Lumen). */
	PC_Balanced UMETA(DisplayName = "Ashline_PC_Balanced"),
	/** Default Windows target: 1440p Ultra, Nanite/Lumen/VSM, HW RT when supported. */
	PC_Ultra UMETA(DisplayName = "Ashline_PC_Ultra")
};

UENUM(BlueprintType)
enum class EAshlinePlayPhase : uint8
{
	Frontend,
	InMission,
	Paused,
	MissionComplete,
	CampaignComplete
};

UENUM(BlueprintType)
enum class EAshlineUpscaler : uint8
{
	Off,
	MetalFXSpatial,
	MetalFXTemporal,
	/** AMD FidelityFX Super Resolution 3 (primary Windows path). */
	FSR3,
	/** Unreal Temporal Super Resolution — always available fallback. */
	TSR,
	/** NVIDIA DLSS — optional, never required. */
	DLSS
};

UENUM(BlueprintType)
enum class EAshlineSurface : uint8
{
	Auto,
	Ground,
	Concrete,
	Metal,
	Wood,
	Sand,
	Snow,
	Water,
	Foliage,
	Emissive,
	Plastic
};

USTRUCT(BlueprintType)
struct FAshlineMissionObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FName ObjectiveId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	bool bOptional = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	bool bCompleted = false;
};

USTRUCT(BlueprintType)
struct FAshlineMissionDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	EAshlineMissionId MissionId = EAshlineMissionId::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FString CodeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FText Theme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FText Briefing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FText Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FName MapAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	TArray<FAshlineMissionObjective> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	int32 RecommendedOperatorLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	int32 XPReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	int32 CrateTokensReward = 1;
};

USTRUCT(BlueprintType)
struct FAshlineWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float Damage = 24.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float FireRateRPM = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	int32 ReserveAmmo = 180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float RangeMeters = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float RecoilPitch = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float RecoilYaw = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float ReloadSeconds = 2.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float HipFireSpread = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float ADSSpread = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	bool bAutomatic = true;
};

USTRUCT(BlueprintType)
struct FAshlineWeaponDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	FName WeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	EAshlineWeaponClass Class = EAshlineWeaponClass::AssaultRifle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	FAshlineWeaponStats BaseStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	TArray<FName> CompatibleAttachments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	int32 UnlockLevel = 1;
};

USTRUCT(BlueprintType)
struct FAshlineAttachmentDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	FName AttachmentId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	EAshlineAttachmentSlot Slot = EAshlineAttachmentSlot::Optic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	FAshlineWeaponStats StatDelta;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	int32 UnlockLevel = 1;
};

USTRUCT(BlueprintType)
struct FAshlineLoadoutSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Armory")
	FName WeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Armory")
	TMap<EAshlineAttachmentSlot, FName> Attachments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Armory")
	FName SkinId;
};

USTRUCT(BlueprintType)
struct FAshlineOperatorProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	FString Callsign = TEXT("ASH-0");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	FString GivenName = TEXT("Operator");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	FName VoicePack = TEXT("VOICE_NEUTRAL");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	FName CamoId = TEXT("CAMO_FIELD");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	int32 FaceIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	int32 Prestige = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	int32 Rank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	int32 XP = 0;

	/** Equipped clothing / identity. Keys are EAshlineCosmeticSlot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	TMap<EAshlineCosmeticSlot, FName> EquippedCosmetics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Operator")
	FName EquippedCharm;
};

USTRUCT(BlueprintType)
struct FAshlineDifficultyTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Difficulty")
	float PlayerDamageTakenMul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Difficulty")
	float PlayerDamageDealtMul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Difficulty")
	float AIAccuracyMul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Difficulty")
	float AIHealthMul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Difficulty")
	float AIPerceptionRangeMul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Difficulty")
	int32 ExtraAICount = 0;
};

USTRUCT(BlueprintType)
struct FAshlineCosmeticDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FName CosmeticId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	EAshlineCosmeticSlot Slot = EAshlineCosmeticSlot::Camo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	EAshlineLootRarity Rarity = EAshlineLootRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 UnlockRank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 CreditCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 RequiredPrestige = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	bool bStarter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FLinearColor PreviewTint = FLinearColor(0.2f, 0.24f, 0.18f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	TSoftObjectPtr<USkeletalMesh> MeshOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	TSoftObjectPtr<UMaterialInterface> MaterialOverride;
};

USTRUCT(BlueprintType)
struct FAshlineWeaponSkinDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FName SkinId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FText DisplayName;

	/** None = universal (all weapons). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FName WeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	EAshlineLootRarity Rarity = EAshlineLootRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 UnlockRank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 CreditCost = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 RequiredPrestige = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	bool bStarter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FLinearColor Tint = FLinearColor(0.08f, 0.08f, 0.09f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	TSoftObjectPtr<UMaterialInterface> MaterialOverride;
};

USTRUCT(BlueprintType)
struct FAshlineRankTier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 Rank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 XPToNext = 800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 CreditGrant = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	int32 CrateGrant = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Meta")
	FName UnlockId;
};
