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
				UGameMapsSettings::SetGameDefaultMap(Playable);
				Maps->EditorStartupMap = FSoftObjectPath(Playable);
				UE_LOG(LogAshline, Log, TEXT("Startup map: %s"), *Playable);
			}
			else
			{
				UGameMapsSettings::SetGameDefaultMap(TEXT("/Engine/Maps/Entry"));
				Maps->EditorStartupMap = FSoftObjectPath(TEXT("/Engine/Maps/Entry"));
				UE_LOG(LogAshline, Log, TEXT("ASH_Playable.umap not in Content yet — hosting the runtime campaign on /Engine/Maps/Entry. Press Play."));
			}
		}
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FAshlineModule, Ashline, "Ashline");
