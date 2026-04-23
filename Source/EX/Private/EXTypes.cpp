// Fill out your copyright notice in the Description page of Project Settings.


#include "EXTypes.h"
#include "Online/EXGameStateSW.h"

FSquadMerc FSquad::GetMerc(EMerc Type) const
{
	for (FSquadMerc Merc : Mercs)
	{
		if (Merc.Type == Type)
		{
			return Merc;
		}
	}
	check(0);
	return FSquadMerc();
}

void FSquad::SwapAugments(EMerc MercType, TArray<EAugment> InAugments)
{
	for (FSquadMerc& Merc : Mercs)
	{
		if (Merc.Type == MercType)
		{
			Merc.SelectedAugments = InAugments;
			break;
		}
	}
}

void FSquad::ReplaceMerc(EMerc OldMerc, EMerc NewMerc)
{
	for (int32 Idx = 0; Idx < SelectedMercs.Num(); Idx++)
	{
		if (SelectedMercs[Idx] == OldMerc)
		{
			SelectedMercs[Idx] = NewMerc;
		}
		else if (SelectedMercs[Idx] == NewMerc)
		{
			SelectedMercs[Idx] = OldMerc;
		}
	}
}

EGameResult FRoundScore::WinsAgaints(const FRoundScore& Other) const
{
	if (CompletedObjectives != Other.CompletedObjectives)
	{
		// One team completed more objectives than the other
		return (CompletedObjectives > Other.CompletedObjectives)
			? EGameResult::Win : EGameResult::Lose;
	}
	if (CompletedObjectives > 0)
	{
		if (ObjectiveScores[CompletedObjectives - 1].Num() != Other.ObjectiveScores[CompletedObjectives - 1].Num())
		{
			// One team did more repetitions of the last objective than the other
			return (ObjectiveScores[CompletedObjectives - 1].Num() > Other.ObjectiveScores[CompletedObjectives - 1].Num())
				? EGameResult::Win : EGameResult::Lose;
		}
	}

	// Both teams did the same amount of objectives
	if (!FMath::IsNearlyEqual(LastObjectiveProgress, Other.LastObjectiveProgress))
	{
		// One team got further on the last objective
		return (LastObjectiveProgress > Other.LastObjectiveProgress)
			? EGameResult::Win : EGameResult::Lose;
	}
	else if (CompletedObjectives == 0)
	{
		// Neither team touched the first objective
		return EGameResult::Draw;
	}
	const int32 LastObjRepetitions = ObjectiveScores[CompletedObjectives - 1].Num();
	const float LastObjectiveTime = ObjectiveScores[CompletedObjectives - 1][LastObjRepetitions - 1];
	const float OtherLastObjectiveTime = Other.ObjectiveScores[CompletedObjectives - 1][LastObjRepetitions - 1];
	if (!FMath::IsNearlyEqual(LastObjectiveTime, OtherLastObjectiveTime))
	{
		// Teams completed same amount of objectives and one team did the last completed objective faster
		return (LastObjectiveTime < OtherLastObjectiveTime)
			? EGameResult::Win : EGameResult::Lose;
	}
	// Both teams did the last objective at the same time, with same amount of progress
	return EGameResult::Draw;
}

void FRoundScore::ObjectiveCompleted(float Time, int32 Stage)
{
	if (Stage >= CompletedObjectives)
	{
		CompletedObjectives++;
		ObjectiveScores.Add(FObjectiveScores());
	}
	ObjectiveScores.Last().Add(Time);
}

void FPlayerStats::CalculateCredits()
{
	Credits = 1000;
}

void FPlayerStats::CalculateTransient()
{
	Kills = 0;
	Deaths = 0;
	Assists = 0;
	TotalExp = 0;
	for (const TTuple<EMerc, FMercStatsGame>& MercKVP : MercStats)
	{
		for (const TTuple<EWeapon, FWeaponStatsGame>& WeaponKVP : MercKVP.Value.WeaponStats)
		{
			Kills += WeaponKVP.Value.Kills;
		}
		for (const TTuple<EAbility, FAbilityStatsGame>& AbilityKVP : MercKVP.Value.AbilityStats)
		{
			Kills += AbilityKVP.Value.Kills;
		}
		Deaths += MercKVP.Value.Deaths;
		Assists += MercKVP.Value.Assists;

		TotalExp += MercKVP.Value.ExpSupport + MercKVP.Value.ExpCombat + MercKVP.Value.ExpObjective;
	}

}

