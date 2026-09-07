#include "Ashline.h"

#include "GameMapsSettings.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogAshline);

class FAshlineModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		if (UGameMapsSettings* Maps = GetMutableDefault<UGameMapsSettings>())
		{
			const FString Playable = TEXT("/Game/Ashline/Maps/ASH_Playable");
			if (FPackageName::DoesPackageExist(Playable))
			{
				Maps->SetGameDefaultMap(Playable);
#if WITH_EDITOR
				Maps->SetEditorStartupMap(Playable);
#endif
				UE_LOG(LogAshline, Log, TEXT("Startup map: %s"), *Playable);
			}
			else
			{
				Maps->SetGameDefaultMap(TEXT("/Engine/Maps/Entry"));
#if WITH_EDITOR
				Maps->SetEditorStartupMap(TEXT("/Engine/Maps/Entry"));
#endif
				UE_LOG(LogAshline, Log, TEXT("ASH_Playable.umap not in Content yet — hosting the runtime campaign on /Engine/Maps/Entry. Press Play."));
			}
		}
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FAshlineModule, Ashline, "Ashline");
