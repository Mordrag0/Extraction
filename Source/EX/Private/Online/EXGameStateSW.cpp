// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/EXGameStateSW.h"
#include "Online/EXGameModeSW.h"
#include "Net/UnrealNetwork.h"
#include "Online/EXLevelRules.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXPlayerController.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/EXDamageType.h"
#include "Engine/Canvas.h"
#include "EX.h"
#include "Online/CoreOnline.h"
#include "Online/EXPlayerState.h"
#include "Online/EXGameInstance.h"
#include "Online/EXTeam.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "EXTypes.h"
#include "System/EXProgress.h"
#include "System/EXInteract.h"
#include "Player/EXCharacter.h"
#include "System/EXGameplayStatics.h"

#define LOCTEXT_NAMESPACE "EXGameState"

AEXGameStateSW::AEXGameStateSW()
{
}

void AEXGameStateSW::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXGameStateSW, Round);
	DOREPLIFETIME(AEXGameStateSW, NumRounds);
	DOREPLIFETIME(AEXGameStateSW, RoundScores);
	DOREPLIFETIME(AEXGameStateSW, CurrentMatchStateSW);
	DOREPLIFETIME(AEXGameStateSW, Attackers);
	DOREPLIFETIME(AEXGameStateSW, Defenders);
	DOREPLIFETIME(AEXGameStateSW, Spectators);
}

void AEXGameStateSW::AdvanceRound()
{
	Round++;
	RoundScores.Add(FRoundScore());

	for (APlayerState* PS : PlayerArray)
	{
		AEXPlayerState* EXPS = Cast<AEXPlayerState>(PS);
		EXPS->StartRound();
	}
}

void AEXGameStateSW::StartRound()
{
	AdvanceRound();
}

void AEXGameStateSW::EndRound()
{
	float LastObjectiveProgress = 0.f;
	for (AEXInteract* Objective : LevelRules->GetObjectives())
	{
		if ((Objective->GetState() == EInteractableState::Active))
		{
			UEXProgress* ObjProgressComp = Objective->GetProgressComp();
			// Resettable objectives should be at 0, but check anyway
			if (ObjProgressComp && !ObjProgressComp->CanBeReset())
			{
				LastObjectiveProgress = FMath::Max(LastObjectiveProgress, ObjProgressComp->GetProgress());
			}
		}
	}
}

void AEXGameStateSW::Init()
{
	Super::Init();
	if (HasAuthority())
	{
		GM = Cast<AEXGameModeSW>(GetWorld()->GetAuthGameMode());
	}
}

void AEXGameStateSW::AddObjectiveTime(int32 Stage)
{
	const float ObjectiveTime = GetServerWorldTimeSeconds() - RoundStartTime;
	UE_LOG(LogEXObjective, Warning, TEXT("%f"), ObjectiveTime);

	if (RoundScores.IsValidIndex(Round - 1))
	{
		RoundScores[Round - 1].ObjectiveCompleted(ObjectiveTime, Stage);
	}
	else
	{
		UE_LOG(LogEXGameState, Error, TEXT("Can't add score."));
	}
}

void AEXGameStateSW::SendGameResult() const
{
	UEXGameInstance* GI = GetGameInstance<UEXGameInstance>();
	GI->SubmitGameResult(SerializeGameScore());
}

void AEXGameStateSW::CreateCurrentState(const FName& NewState, int32 NewStateDuration)
{
	CurrentMatchStateSW = 
		FEXMatchStateSW(
			NewState,
			GetServerWorldTimeSeconds(),
			NewStateDuration,
			AttSpawnWaveDuration,
			DefSpawnWaveDuration,
			SpawnTimeBeginOffset,
			SpawnTimeBeginOffset
	);
	ChangeState(&CurrentMatchStateSW);
}

void AEXGameStateSW::OnRep_CurrentMatchState()
{
	ChangeState(&CurrentMatchStateSW);
}

void AEXGameStateSW::SetTeams(AEXTeam* InAttackers, AEXTeam* InDefenders, AEXTeam* InSpectators)
{
	Attackers = InAttackers;
	Defenders = InDefenders;
	Spectators = InSpectators;
}

TArray<AEXPlayerState*> AEXGameStateSW::GetSpectatorMembers() const
{
	return Spectators->GetMembers();
}

FTeamResult AEXGameStateSW::GetTeamScore(AEXTeam* Team, TArray<AEXPlayerState*> InactivePlayers) const
{
	FTeamResult TeamResult;
	for (AEXPlayerState* Player : Team->GetMembers())
	{
		TeamResult.Players.Add(Player->GetStats());
	}
	for (AEXPlayerState* Player : InactivePlayers)
	{
		TeamResult.Players.Add(Player->GetStats());
	}
	if (RoundScores.Num() > 1)
	{
		if (Team->GetStartedOnAtt())
		{
			TeamResult.ObjectiveTimes = RoundScores[0];
		}
		else
		{
			TeamResult.ObjectiveTimes = RoundScores[1];
		}
	}
	return TeamResult;
}

FGameResultSW AEXGameStateSW::SerializeGameScore() const
{
	bool bRanked = GM->IsRanked();
	FGameResultSW GameResult(bRanked, GetWorld()->GetMapName());

	TArray<AEXPlayerState*> InactiveAttackers;
	TArray<AEXPlayerState*> InactiveDefenders;

	for (APlayerState* PS : GM->InactivePlayerArray)
	{
		// #EXTODO2 include players that end in spec
		AEXPlayerState* EXPS = Cast<AEXPlayerState>(PS);
		if (GM->GetAttackers()->GetMembers().Contains(EXPS)
			|| GM->GetDefenders()->GetMembers().Contains(EXPS))
		{
			ensure(0);
			continue;
		}
		if (EXPS->GetTeam()->GetStartedOnAtt())
		{
			InactiveAttackers.Add(EXPS);
		}
		else if (!EXPS->GetTeam()->GetStartedOnAtt())
		{
			InactiveDefenders.Add(EXPS);
		}
	}

	GameResult.Att = GetTeamScore(GM->GetAttackersOnStart(), InactiveAttackers);
	GameResult.Def = GetTeamScore(GM->GetDefendersOnStart(), InactiveDefenders);

	GameResult.AttGameResult = GameResult.Att.ObjectiveTimes.WinsAgaints(GameResult.Def.ObjectiveTimes);

	return GameResult;
}

void AEXGameStateSW::Reset()
{
	Super::Reset();
	RoundScores.Empty();
}

void AEXGameStateSW::ResetRound()
{
	--Round;
	RoundScores.RemoveAt(Round);

	// Revert player stats to before the round
	for (APlayerState* PS : PlayerArray)
	{
		AEXPlayerState* EXPS = Cast<AEXPlayerState>(PS);
		EXPS->ResetRound();
	}
}

void AEXGameStateSW::SetMatchStates(uint8 InAttSpawnWaveDuration, uint8 InDefSpawnWaveDuration, uint8 InNumRounds)
{
	check((InNumRounds > 0) && "Bad gamemode defaults");
	NumRounds = InNumRounds;
	AttSpawnWaveDuration = InAttSpawnWaveDuration;
	DefSpawnWaveDuration = InDefSpawnWaveDuration;
}

bool AEXGameStateSW::IsOnLastRound() const
{
	return (Round == NumRounds);
}

#undef LOCTEXT_NAMESPACE

