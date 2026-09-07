#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AshlineTypes.h"
#include "AshlineOperatorCreator.generated.h"

UCLASS(BlueprintType)
class ASHLINE_API UAshlineOperatorCreator : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ashline|Operator")
	static TArray<FName> GetVoicePacks();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Operator")
	static TArray<FName> GetCamos();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Operator")
	static TArray<FName> GetCosmeticsForSlot(EAshlineCosmeticSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Operator")
	static FAshlineOperatorProfile MakeDefault();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Operator")
	static bool Validate(const FAshlineOperatorProfile& Profile, FText& OutError);
};
