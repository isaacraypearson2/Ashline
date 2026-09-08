#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineEquipmentCatalog.generated.h"

/** SP lethals / tacticals / field gear. Locker data — throwables land with authored FX. */
UCLASS()
class ASHLINE_API UAshlineEquipmentCatalog : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ashline|Equipment")
	static TArray<FAshlineEquipmentDefinition> BuildRoster();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Equipment")
	static bool FindEquipment(FName EquipmentId, FAshlineEquipmentDefinition& OutEquipment);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Equipment")
	static TArray<FAshlineEquipmentDefinition> OfSlot(EAshlineEquipmentSlot Slot);

	UFUNCTION(BlueprintPure, Category = "Ashline|Equipment")
	static TArray<FName> StarterIds();

	UFUNCTION(BlueprintPure, Category = "Ashline|Equipment")
	static FString SlotName(EAshlineEquipmentSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Equipment")
	static void BindContentPaths(FAshlineEquipmentDefinition& Equipment);
};
