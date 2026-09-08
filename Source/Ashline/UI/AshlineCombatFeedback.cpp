#include "UI/AshlineCombatFeedback.h"

void UAshlineCombatFeedback::Tick(float DeltaTime)
{
	HitMarkerRemaining = FMath::Max(0.f, HitMarkerRemaining - DeltaTime);
	KillConfirmRemaining = FMath::Max(0.f, KillConfirmRemaining - DeltaTime);
	DamagePulse = FMath::Max(0.f, DamagePulse - DeltaTime * 0.85f);
	InteractRemaining = FMath::Max(0.f, InteractRemaining - DeltaTime);
	SubtitleRemaining = FMath::Max(0.f, SubtitleRemaining - DeltaTime);
	if (InteractRemaining <= 0.f)
	{
		InteractPrompt.Reset();
	}
	if (SubtitleRemaining <= 0.f)
	{
		Subtitle.Reset();
	}

	for (int32 i = KillFeed.Num() - 1; i >= 0; --i)
	{
		KillFeed[i].Remaining -= DeltaTime;
		if (KillFeed[i].Remaining <= 0.f)
		{
			KillFeed.RemoveAt(i);
		}
	}
}

TStatId UAshlineCombatFeedback::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAshlineCombatFeedback, STATGROUP_Tickables);
}

void UAshlineCombatFeedback::NotifyHitMarker(bool bKilled)
{
	HitMarkerRemaining = bKilled ? 0.28f : 0.16f;
	bLastHitWasKill = bKilled;
}

void UAshlineCombatFeedback::NotifyKillConfirm(const FString& TargetName)
{
	++MissionKills;
	KillConfirmName = TargetName.IsEmpty() ? TEXT("HOSTILE") : TargetName;
	KillConfirmRemaining = 1.6f;
	bLastHitWasKill = true;
	HitMarkerRemaining = FMath::Max(HitMarkerRemaining, 0.28f);

	FAshlineKillFeedItem Item;
	Item.Text = FString::Printf(TEXT("KIA  %s"), *KillConfirmName);
	Item.Remaining = 3.2f;
	KillFeed.Insert(Item, 0);
	if (KillFeed.Num() > 5)
	{
		KillFeed.SetNum(5);
	}
}

void UAshlineCombatFeedback::NotifyDamageTaken(float NormalizedHealth)
{
	DamagePulse = FMath::Clamp(1.15f - NormalizedHealth, 0.35f, 1.f);
}

void UAshlineCombatFeedback::SetInteractPrompt(const FString& Prompt)
{
	InteractPrompt = Prompt;
	InteractRemaining = 0.2f;
}

void UAshlineCombatFeedback::ClearInteractPrompt()
{
	InteractPrompt.Reset();
	InteractRemaining = 0.f;
}

void UAshlineCombatFeedback::SetSubtitle(const FString& Text, float Seconds)
{
	Subtitle = Text;
	SubtitleRemaining = Seconds;
}

void UAshlineCombatFeedback::NotifyObjective(const FString& Text)
{
	FAshlineKillFeedItem Item;
	Item.Text = Text;
	Item.Remaining = 4.f;
	KillFeed.Insert(Item, 0);
	if (KillFeed.Num() > 5)
	{
		KillFeed.SetNum(5);
	}
}

void UAshlineCombatFeedback::ResetMission()
{
	MissionKills = 0;
	KillFeed.Reset();
	HitMarkerRemaining = 0.f;
	KillConfirmRemaining = 0.f;
	DamagePulse = 0.f;
	InteractPrompt.Reset();
	Subtitle.Reset();
}
