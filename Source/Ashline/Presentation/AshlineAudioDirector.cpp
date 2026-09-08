#include "Presentation/AshlineAudioDirector.h"
#include "Presentation/AshlineLoad.h"

#include "Ashline.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineEnvironmentKit.h"
#include "Presentation/AshlineLoad.h"
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
		if (USoundBase* Loaded = AshlineLoad::Soft(Soft))
		{
			return Loaded;
		}
	}
	for (const FString& Path : FallbackPaths)
	{
		if (USoundBase* Cue = AshlineLoad::Object<USoundBase>(Path))
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
		Cue = AshlineLoad::Soft(Visual->FireCue);
	}
	if (!Cue)
	{
		TArray<FString> Paths = {
			UAshlineContentManifest::FireCuePath(WeaponId),
			TEXT("/Game/Ashline/Audio/Weapons/SC_Fire_Default.SC_Fire_Default")
		};
		Paths.Append(UAshlineContentManifest::EngineFireSoundCandidates());
		Cue = Resolve(DefaultFireCue, Paths);
	}
	if (Cue)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location, SFXGain() * 0.85f);
	}
}

void UAshlineAudioDirector::PlayReload(UObject* WorldContext, FName WeaponId, const FVector& Location)
{
	USoundBase* Cue = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		Cue = AshlineLoad::Soft(Visual->ReloadCue);
	}
	if (!Cue)
	{
		TArray<FString> Paths = { UAshlineContentManifest::ReloadCuePath() };
		Paths.Append(UAshlineContentManifest::EngineReloadSoundCandidates());
		Cue = Resolve(DefaultReloadCue, Paths);
	}
	if (Cue)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location, SFXGain() * 0.7f);
	}
}

void UAshlineAudioDirector::PlayHit(UObject* WorldContext, const FVector& Location, bool bFlesh)
{
	USoundBase* Cue = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(NAME_None))
	{
		Cue = AshlineLoad::Soft(Visual->HitCue);
	}
	if (!Cue)
	{
		TArray<FString> Paths = {
			bFlesh ? TEXT("/Game/Ashline/Audio/Weapons/SC_Hit_Flesh.SC_Hit_Flesh") : TEXT("/Game/Ashline/Audio/Weapons/SC_Hit_Default.SC_Hit_Default")
		};
		Paths.Append(UAshlineContentManifest::EngineHitSoundCandidates());
		Cue = Resolve(DefaultHitCue, Paths);
	}
	if (Cue)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location, SFXGain() * (bFlesh ? 0.55f : 0.4f), bFlesh ? 0.92f : 1.15f);
	}
}

void UAshlineAudioDirector::PlayFootstep(UObject* WorldContext, const FVector& Location, EAshlineSurface Surface)
{
	TArray<FString> Paths = { UAshlineContentManifest::FootstepCuePath(Surface), TEXT("/Game/Ashline/Audio/Footsteps/SC_Footstep_Default.SC_Footstep_Default") };
	Paths.Append(UAshlineContentManifest::EngineFootstepSoundCandidates());
	if (USoundBase* Cue = Resolve(DefaultFootstepCue, Paths))
	{
		float Pitch = 1.f;
		switch (Surface)
		{
		case EAshlineSurface::Metal: Pitch = 1.18f; break;
		case EAshlineSurface::Wood: Pitch = 0.92f; break;
		case EAshlineSurface::Sand: Pitch = 0.84f; break;
		case EAshlineSurface::Snow: Pitch = 0.8f; break;
		case EAshlineSurface::Water: Pitch = 0.7f; break;
		case EAshlineSurface::Concrete: Pitch = 1.05f; break;
		default: break;
		}
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location, SFXGain() * 0.32f, Pitch);
	}
}

void UAshlineAudioDirector::StartMusicBed(UObject* WorldContext, EAshlineMissionId MissionId)
{
	StopMusicBed();
	bCombatLayer = false;
	USoundBase* Bed = nullptr;
	if (UAshlineEnvironmentKit* Kit = UAshlinePresentationLibrary::FindEnvironmentKit(MissionId))
	{
		Bed = AshlineLoad::Soft(Kit->MusicBed);
	}
	if (!Bed)
	{
		Bed = AshlineLoad::Object<USoundBase>(UAshlineContentManifest::MusicBedPath(MissionId));
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
		MusicComp = UGameplayStatics::SpawnSound2D(WorldContext, Bed, MusicGain() * 0.35f, 1.f, 0.f, nullptr, true, false);
	}
	else
	{
		UE_LOG(LogAshline, Verbose, TEXT("Audio: music bed for mission %d has no cue assigned."), static_cast<int32>(MissionId));
	}

	StartAmbience(WorldContext, MissionId);
}

void UAshlineAudioDirector::StopMusicBed()
{
	if (MusicComp)
	{
		MusicComp->Stop();
		MusicComp = nullptr;
	}
	StopAmbience();
	bCombatLayer = false;
}

void UAshlineAudioDirector::StartAmbience(UObject* WorldContext, EAshlineMissionId MissionId)
{
	StopAmbience();
	USoundBase* Bed = nullptr;
	if (UAshlineEnvironmentKit* Kit = UAshlinePresentationLibrary::FindEnvironmentKit(MissionId))
	{
		Bed = AshlineLoad::Soft(Kit->AmbienceBed);
	}
	if (!Bed)
	{
		Bed = AshlineLoad::Object<USoundBase>(UAshlineContentManifest::AmbiencePath(MissionId));
	}
	if (!Bed)
	{
		Bed = Resolve(DefaultAmbience, {
			TEXT("/Game/Ashline/Audio/Ambience/SC_Amb_Default.SC_Amb_Default"),
			TEXT("/Game/StarterContent/Audio/Fire01.Fire01")
		});
	}
	if (Bed && WorldContext)
	{
		AmbienceComp = UGameplayStatics::SpawnSound2D(WorldContext, Bed, MusicGain() * 0.22f, 1.f, 0.f, nullptr, true, false);
	}
}

void UAshlineAudioDirector::StopAmbience()
{
	if (AmbienceComp)
	{
		AmbienceComp->Stop();
		AmbienceComp = nullptr;
	}
}

void UAshlineAudioDirector::NotifyCombat(UObject* WorldContext)
{
	if (bCombatLayer)
	{
		return;
	}
	bCombatLayer = true;
	PlayStinger(WorldContext);
	if (MusicComp)
	{
		MusicComp->SetVolumeMultiplier(MusicGain() * 0.55f);
	}
}

void UAshlineAudioDirector::PlayStinger(UObject* WorldContext)
{
	USoundBase* Cue = nullptr;
	Cue = Resolve(CombatStingerCue, {
		UAshlineContentManifest::CombatStingerPath(),
		TEXT("/Game/StarterContent/Audio/Explosion01.Explosion01"),
		TEXT("/Game/StarterContent/Audio/Explosion_01.Explosion_01")
	});
	if (Cue)
	{
		UGameplayStatics::PlaySound2D(WorldContext, Cue, SFXGain() * 0.55f);
	}
}

void UAshlineAudioDirector::PlayUI(UObject* WorldContext, bool bConfirm)
{
	const TSoftObjectPtr<USoundBase>& Soft = bConfirm ? UIConfirmCue : UIBackCue;
	if (USoundBase* Cue = Resolve(Soft, UAshlineContentManifest::EngineUISoundCandidates()))
	{
		UGameplayStatics::PlaySound2D(WorldContext, Cue, SFXGain() * 0.35f, bConfirm ? 1.1f : 0.85f);
	}
}

void UAshlineAudioDirector::PlayExplosion(UObject* WorldContext, const FVector& Location)
{
	TArray<FString> Paths = UAshlineContentManifest::EngineExplosionSoundCandidates();
	if (USoundBase* Cue = Resolve(ExplosionCue, Paths))
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContext, Cue, Location, SFXGain() * 0.9f);
	}
}

void UAshlineAudioDirector::SetBusVolumes(float Master, float SFX, float Music)
{
	MasterVolume = FMath::Clamp(Master, 0.f, 1.f);
	SFXVolume = FMath::Clamp(SFX, 0.f, 1.f);
	MusicVolume = FMath::Clamp(Music, 0.f, 1.f);
	if (MusicComp)
	{
		MusicComp->SetVolumeMultiplier(MusicGain() * (bCombatLayer ? 0.55f : 0.35f));
	}
	if (AmbienceComp)
	{
		AmbienceComp->SetVolumeMultiplier(MusicGain() * 0.22f);
	}
}
