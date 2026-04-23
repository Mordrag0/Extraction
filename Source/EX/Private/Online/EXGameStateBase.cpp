// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameStateBase.h"
#include "System/EXGameplayStatics.h"
#include "Player/EXPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Online/EXLevelRules.h"
#include "Online/EXGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Online/EXGameModeBase.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"

void AEXGameStateBase::Init()
{
	if (bInitialized)
	{
		return;
	}
	bInitialized = true;
	LevelRules = UEXGameplayStatics::GetLevelRules(this);
	if (LevelRules) // Will be null in the editor
	{
		LevelRules->SetGameState(this);
	}
	if(!HasAuthority() || IsNetMode(NM_Standalone))
	{
		EXController = Cast<AEXPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	}

	GMB = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
}

void AEXGameStateBase::AddObjectiveTime(int32 Stage)
{
}

bool AEXGameStateBase::IsMatchInProgress() const
{
	return (GetMatchState() == MatchState::InProgress) || (GetMatchState() == MatchState::Overtime);
}

void AEXGameStateBase::Tick(float DeltaSeconds)
{
	// Don't need default GameMode tick behavior 
}

void AEXGameStateBase::SetMatchState(const FName& NewState, int32 NewStateDuration)
{
	Init();
	MatchState = NewState;
	CreateCurrentState(NewState, NewStateDuration);

	if (NewState.IsEqual(MatchState::InProgress))
	{
		RoundStartTime = GetServerWorldTimeSeconds();
	}

	GMB->ChangeState(CurrentMatchState);
}

void AEXGameStateBase::StartRound()
{
}

void AEXGameStateBase::EndRound()
{
}

void AEXGameStateBase::AdvanceRound()
{
}

bool AEXGameStateBase::GameReady() const
{
	if (HasAuthority())
	{
		return true;
	}
	return (CurrentMatchState && CurrentMatchState->IsValid());
}

void AEXGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void AEXGameStateBase::ChangeState(FEXMatchState* NewState)
{
	CurrentMatchState = NewState;
	if (CurrentMatchState && CurrentMatchState->IsValid())
	{
		OnStateChanged(CurrentMatchState->State.ToString());
	}
}

void AEXGameStateBase::OnStateChanged(const FString& StateName)
{
	if (HasAuthority())
	{
		return;
	}
	check(EXController); // If this fails, we need to call Init()
	EXController->DisplayMessage(FText::FromString(StateName));
}

void AEXGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void AEXGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	AEXPlayerState* EXPS = Cast<AEXPlayerState>(PlayerState);
	EXPlayerArray.AddUnique(EXPS);
}

void AEXGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	AEXPlayerState* EXPS = Cast<AEXPlayerState>(PlayerState);
	EXPlayerArray.Remove(EXPS);
}

AEXPlayerState* AEXGameStateBase::GetPlayer(int32 PlayerId) const
{
	for (AEXPlayerState* EXPS : EXPlayerArray)
	{
		if (EXPS->GetPlayerId() == PlayerId)
		{
			return EXPS;
		}
	}
	return nullptr;
}

