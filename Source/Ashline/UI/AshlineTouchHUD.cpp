#include "UI/AshlineTouchHUD.h"

void UAshlineTouchHUD::NativeConstruct()
{
	Super::NativeConstruct();

#if PLATFORM_IOS || PLATFORM_TVOS
	bShowVirtualStick = true;
	bShowFireButton = true;
	bShowAimButton = true;
	bShowCameraToggle = true;
	ConfigureTouchLayout();
#else
	bShowVirtualStick = false;
	bShowFireButton = false;
#endif
}
