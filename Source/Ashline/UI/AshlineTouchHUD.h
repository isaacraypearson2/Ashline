#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AshlineTouchHUD.generated.h"

UCLASS()
class ASHLINE_API UAshlineTouchHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Ashline|iOS")
	void ConfigureTouchLayout();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|iOS")
	bool bShowVirtualStick = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|iOS")
	bool bShowFireButton = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|iOS")
	bool bShowAimButton = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|iOS")
	bool bShowCameraToggle = true;
};
