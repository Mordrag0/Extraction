// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameStateDM.h"
#include "Online/EXGameInstance.h"
#include "Online/EXGameModeDM.h"
#include "Net/UnrealNetwork.h"
#include "Online/EXPlayerState.h"

void AEXGameStateDM::Init()
{
	Super::Init();

	GM = GetWorld()->GetAuthGameMode<AEXGameModeDM>();
}

FGameResultDM AEXGameStateDM::SerializeGameScore() const
{
	TArray<AEXPlayerState*> Players;
	for (APlayerState* PS : PlayerArray)
	{
		Players.Add(Cast<AEXPlayerState>(PS));
	}
	for (APlayerState* PS : GM->InactivePlayerArray)
	{
		AEXPlayerState* EXPS = Cast<AEXPlayerState>(PS);
		Players.AddUnique(EXPS);
	}
	TArray<FPlayerStats> PlayerStats;
	for (AEXPlayerState* EXPS : Players)
	{
		if (EXPS)
		{
			PlayerStats.Add(EXPS->GetStats());
		}
	}
	return FGameResultDM(GetWorld()->GetMapName(), PlayerStats);
}

void AEXGameStateDM::SendGameResult() const
{
	UEXGameInstance* GI = GetGameInstance<UEXGameInstance>();
	GI->SubmitGameResult(SerializeGameScore());
}

void AEXGameStateDM::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXGameStateDM, CurrentMatchStateDM);
	DOREPLIFETIME(AEXGameStateDM, Spectators);
}

TArray<AEXPlayerState*> AEXGameStateDM::GetSpectatorMembers() const
{
	return Spectators;
}

void AEXGameStateDM::AddSpectator(AEXPlayerState* PS)
{
	Spectators.Add(PS);
}

void AEXGameStateDM::RemoveSpectator(AEXPlayerState* PS)
{
	Spectators.Remove(PS);
}

void AEXGameStateDM::CreateCurrentState(const FName& NewState, int32 NewStateDuration)
{
	CurrentMatchStateDM =
		FEXMatchStateDM(
			NewState,
			GetServerWorldTimeSeconds(),
			NewStateDuration
		);
	ChangeState(&CurrentMatchStateDM);
}

void AEXGameStateDM::OnRep_CurrentMatchState()
{
	ChangeState(&CurrentMatchStateDM);
}
