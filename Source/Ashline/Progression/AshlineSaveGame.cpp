#include "Progression/AshlineSaveGame.h"
#include "Campaign/AshlineMissionCatalog.h"

void UAshlineSaveGame::SeedNewCampaign()
{
	Operator = FAshlineOperatorProfile();
	Difficulty = EAshlineDifficulty::Regular;
	PreferredCamera = EAshlineCameraMode::FirstPerson;
	CrateTokens = 0;
	CratesOpened = 0;
	PrestigeLevel = 0;
	MissionsCompleted = 0;
	CollectedCosmetics.Reset();
	Missions.Reset();
	Armory.Reset();

	const TArray<FAshlineMissionDefinition> Campaign = UAshlineMissionCatalog::BuildCampaign();
	for (const FAshlineMissionDefinition& Mission : Campaign)
	{
		FAshlineMissionProgress Progress;
		Progress.MissionId = Mission.MissionId;
		Progress.State = (Mission.MissionId == EAshlineMissionId::ASH_01_WireCut)
			? EAshlineMissionState::Available
			: EAshlineMissionState::Locked;
		Missions.Add(Progress);
	}

	auto Own = [this](const TCHAR* Id)
	{
		FAshlineOwnedWeapon W;
		W.WeaponId = Id;
		W.UpgradeTier = 0;
		Armory.Add(W);
	};
	Own(TEXT("WPN_AR_ASH16"));
	Own(TEXT("WPN_PIS_M17A"));

	Primary.WeaponId = TEXT("WPN_AR_ASH16");
	Secondary.WeaponId = TEXT("WPN_PIS_M17A");
}

FAshlineMissionProgress* UAshlineSaveGame::FindMissionProgress(EAshlineMissionId MissionId)
{
	return const_cast<FAshlineMissionProgress*>(
		static_cast<const UAshlineSaveGame*>(this)->FindMissionProgress(MissionId));
}

const FAshlineMissionProgress* UAshlineSaveGame::FindMissionProgress(EAshlineMissionId MissionId) const
{
	for (const FAshlineMissionProgress& Progress : Missions)
	{
		if (Progress.MissionId == MissionId)
		{
			return &Progress;
		}
	}
	return nullptr;
}

void UAshlineSaveGame::UnlockNext(EAshlineMissionId Completed)
{
	if (FAshlineMissionProgress* Current = FindMissionProgress(Completed))
	{
		Current->State = EAshlineMissionState::Completed;
	}

	MissionsCompleted = FMath::Max(MissionsCompleted, UAshlineMissionCatalog::MissionIndex(Completed) + 1);

	const EAshlineMissionId Next = UAshlineMissionCatalog::NextMission(Completed);
	if (FAshlineMissionProgress* NextProgress = FindMissionProgress(Next))
	{
		if (NextProgress->State == EAshlineMissionState::Locked)
		{
			NextProgress->State = EAshlineMissionState::Available;
		}
	}
}

void UAshlineSaveGame::GrantXP(int32 Amount)
{
	Operator.XP += FMath::Max(0, Amount);
	while (Operator.XP >= 1000 && Operator.Rank < 50)
	{
		Operator.XP -= 1000;
		++Operator.Rank;
	}
}
