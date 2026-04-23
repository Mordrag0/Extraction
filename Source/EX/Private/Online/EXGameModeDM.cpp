// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameModeDM.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Online/EXGameStateDM.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/GameStateBase.h"
#include "Online/EXPlayerState.h"
#include "Player/EXPlayerController.h"
#include "Misc/EXPlayerStart.h"

AActor* AEXGameModeDM::ChoosePlayerStart_Implementation(AController* Player)
{
	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			if (Cast<APlayerStartPIE>(*It))
			{
				return *It;
			}
		}
	}

	if (PlayerStarts.Num() == 0)
	{
		return nullptr;
	}

	int32 SpawnIdx = FMath::RandHelper(PlayerStarts.Num());
	return PlayerStarts[SpawnIdx];
}

void AEXGameModeDM::InitGameState()
{
	Super::InitGameState();
	
	GSB = GetGameState<AEXGameStateDM>();
}

int32 AEXGameModeDM::GetNumPlayers()
{
	return ConnectedPlayers.Num() - Spectators.Num();
}

int32 AEXGameModeDM::GetNumSpectators()
{
	return Spectators.Num();
}

void AEXGameModeDM::AddToSpectators(AEXPlayerController* PC)
{
	Spectators.Add(PC);
	GSB->AddSpectator(PC->GetPlayerState<AEXPlayerState>());
}

void AEXGameModeDM::SwitchTeam(AEXPlayerController* PC)
{
	Spectators.Remove(PC);
	GSB->RemoveSpectator(PC->GetPlayerState<AEXPlayerState>());
}

void AEXGameModeDM::CharacterDied(AEXPlayerController* PC)
{
	PC->AllowRespawn();
}

void AEXGameModeDM::ChangeState(FEXMatchState* InMatchState)
{
	Super::ChangeState(InMatchState);
}

void AEXGameModeDM::BeginPlay()
{
	Super::BeginPlay();

}

