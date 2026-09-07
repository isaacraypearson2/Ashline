#include "Presentation/AshlineAudioDirector.h"

#include "Ashline.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineEnvironmentKit.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlineWeaponVisual.h"
#include "Sound/SoundBase.h"

void UAshlineAudioDirector::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

USoundBase* UAshlineAudioDirector::Resolve(const TSoftObjectPtr<USoundBase>& Soft, const TArray<FString>& FallbackPaths) const
{
	if (!Soft.IsNull())
	{
		if (USoundBase* Loaded = Soft.LoadSynchronous())
		{
			return Loaded;
		}
	}
	for (const FString& Path : FallbackPaths)
	{
		if (USoundBase* Cue = LoadObject<USoundBase>(nullptr, *Path))
		{
			return Cue;
		}
	}
	return nullptr;
}

void UAshlineAudioDirector::PlayFire(UObject* WorldContext, FName WeaponId, const FVector& Location)
{
	USoundBase* Cue = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		Cue = Visual->FireCue.LoadSynchronous();
	}
	if (!Cue)
	{
		Cue = Resolve(DefaultFireCue, {
			UAshlineContentManifest::FireCuePath(WeaponId),
			TEXT("/Game/Ashline/Audio/Weapons/SC_Fire_Default.SC_Fire_Default"),
			TEXT("/Game/StarterContent/Audio/Explosion01.Explosion01")
		});
	}
	if (Cue)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location);
	}
}

void UAshlineAudioDirector::PlayReload(UObject* WorldContext, FName WeaponId, const FVector& Location)
{
	USoundBase* Cue = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		Cue = Visual->ReloadCue.LoadSynchronous();
	}
	if (!Cue)
	{
		Cue = Resolve(DefaultReloadCue, {
			TEXT("/Game/Ashline/Audio/Weapons/SC_Reload_Default.SC_Reload_Default")
		});
	}
	if (Cue)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location);
	}
}

void UAshlineAudioDirector::PlayHit(UObject* WorldContext, const FVector& Location)
{
	USoundBase* Cue = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(NAME_None))
	{
		Cue = Visual->HitCue.LoadSynchronous();
	}
	if (!Cue)
	{
		Cue = Resolve(DefaultHitCue, {
			TEXT("/Game/Ashline/Audio/Weapons/SC_Hit_Default.SC_Hit_Default"),
			TEXT("/Game/StarterContent/Audio/Explosion_01.Explosion_01")
		});
	}
	if (Cue)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location, 0.45f);
	}
}

void UAshlineAudioDirector::PlayFootstep(UObject* WorldContext, const FVector& Location)
{
	if (USoundBase* Cue = Resolve(DefaultFootstepCue, {
		TEXT("/Game/Ashline/Audio/Footsteps/SC_Footstep_Default.SC_Footstep_Default")
	}))
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location, 0.35f);
	}
}

void UAshlineAudioDirector::StartMusicBed(UObject* WorldContext, EAshlineMissionId MissionId)
{
	StopMusicBed();
	USoundBase* Bed = nullptr;
	if (UAshlineEnvironmentKit* Kit = UAshlinePresentationLibrary::FindEnvironmentKit(MissionId))
	{
		Bed = Kit->MusicBed.LoadSynchronous();
	}
	if (!Bed)
	{
		Bed = LoadObject<USoundBase>(nullptr, *UAshlineContentManifest::MusicBedPath(MissionId));
	}
	if (!Bed)
	{
		Bed = Resolve(FrontendMusic, {
			TEXT("/Game/Ashline/Audio/Music/SC_Bed_Frontend.SC_Bed_Frontend"),
			TEXT("/Game/StarterContent/Audio/Fire01.Fire01")
		});
	}
	if (Bed && WorldContext)
	{
		MusicComp = UGameplayStatics::SpawnSound2D(WorldContext, Bed, 0.35f, 1.f, 0.f, nullptr, true, false);
	}
	else
	{
		UE_LOG(LogAshline, Verbose, TEXT("Audio: music bed for mission %d has no cue assigned (Phase 2 import)."), static_cast<int32>(MissionId));
	}
}

void UAshlineAudioDirector::StopMusicBed()
{
	if (MusicComp)
	{
		MusicComp->Stop();
		MusicComp = nullptr;
	}
}
