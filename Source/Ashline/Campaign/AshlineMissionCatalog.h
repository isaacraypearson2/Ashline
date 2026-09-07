#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineMissionCatalog.generated.h"

UCLASS()
class ASHLINE_API UAshlineMissionCatalog : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	static TArray<FAshlineMissionDefinition> BuildCampaign();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	static bool FindMission(EAshlineMissionId MissionId, FAshlineMissionDefinition& OutMission);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	static EAshlineMissionId NextMission(EAshlineMissionId Current);

	UFUNCTION(BlueprintPure, Category = "Ashline|Campaign")
	static int32 MissionIndex(EAshlineMissionId MissionId);
};
