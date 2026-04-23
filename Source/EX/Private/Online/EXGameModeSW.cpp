// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/EXGameModeSW.h"
#include "Player/EXCharacter.h"
#include "Online/EXPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Player/EXPlayerController.h"
#include "EX.h"
#include "Online/EXLevelRules.h"
#include "Misc/EXSpawnArea.h"
#include "Online/EXTeam.h"
#include "Kismet/GameplayStatics.h"
#include "EXTypes.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Online/EXGameSession.h"
#include "Online/EXGameStateSW.h"
#include "GameFramework/SpectatorPawn.h"
#include "Engine/PlayerStartPIE.h"
#include "Online/EXGameMessage.h"
#include "Online/EXGameInstance.h"
#include "System/EXGameplayStatics.h"
#include "Engine/LevelScriptActor.h"
#include "System/EXInteract.h"
#include "Misc/EXFlag.h"


AEXGameModeSW::AEXGameModeSW()
{
	bAllowTeamSwitch = true;
	bTeamGame = true;
}

bool AEXGameModeSW::MustSpectate_Implementation(APlayerController* NewPlayerController) const
{
	if (Super::MustSpectate_Implementation(NewPlayerController))
	{
		return true;
	}

	if (!Attackers || !Defenders)
	{
		return true;
	}

	if (Attackers->IsFull() && Defenders->IsFull())
	{
		return true;
	}

	return false;
}

bool AEXGameModeSW::IsInitialized() const
{
	return !!Attackers && !!Defenders && !!Spectators;
}

void AEXGameModeSW::InitTeams()
{
	if (IsInitialized())
	{
		return;
	}

	// Create teams and set first spawns
	Attackers = GetWorld()->SpawnActor<AEXTeam>(TeamClass);
	Attackers->SetType(ETeam::Attack);
	Attackers->SetStartedOnAtt(true);
	Defenders = GetWorld()->SpawnActor<AEXTeam>(TeamClass);
	Defenders->SetType(ETeam::Defense);
	Defenders->SetStartedOnAtt(false);
	Spectators = GetWorld()->SpawnActor<AEXTeam>(TeamClass);
	Spectators->SetType(ETeam::Spectator);
	ResetSpawns(); // Initialize spawns

	// Add players already connected to teams
	for (AEXPlayerController* Player : ConnectedPlayers)
	{
		AutoAssignTeam(Player, false);
	}

	GSB->SetTeams(Attackers, Defenders, Spectators);
}

void AEXGameModeSW::ResetSpawns()
{
	if (!IsInitialized())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEXGameModeSW::ResetSpawns);
		return;
	}

	const TArray<AEXSpawnArea*> AttackerSpawns = LevelRules->GetAttackerSpawns();
	const TArray<AEXSpawnArea*> DefenderSpawns = LevelRules->GetDefenderSpawns();
	if (ensure((AttackerSpawns.Num() > 0) && (DefenderSpawns.Num() > 0)))
	{
		Attackers->SetSpawnArea(AttackerSpawns[0]);
		Defenders->SetSpawnArea(DefenderSpawns[0]);
	}
}


int32 AEXGameModeSW::GetNumPlayers()
{
	return GetAttackers()->NumMembers() + GetDefenders()->NumMembers();
}

int32 AEXGameModeSW::GetNumSpectators()
{
	return GetSpectators()->NumMembers();
}

void AEXGameModeSW::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (IsNetMode(NM_Standalone))
	{
		if (!Attackers && !Defenders)
		{
			InitTeams();
		}
	}
	else
	{
		// If players should start as spectators, leave them in the spectator state
		bool bForceSpec = bStartPlayersAsSpectators || MustSpectate(NewPlayer);
		AutoAssignTeam(Cast<AEXPlayerController>(NewPlayer), bForceSpec);
	}
}

void AEXGameModeSW::AutoAssignTeam(AEXPlayerController* Player, bool bForceSpec)
{
	check(HasAuthority());
	AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
	check(PS && !PS->GetTeam());

	// Add the player to the team with fewer members or to the defenders if they are equal
	AEXTeam* Team = (Attackers->NumMembers() < Defenders->NumMembers()) ? Attackers : Defenders;
#if WITH_EDITOR
	// In editor prefer attackers
	Team = (Attackers->NumMembers() <= Defenders->NumMembers()) ? Attackers : Defenders;
#endif
	if (Team->IsFull() || bForceSpec)
	{
		Team = Spectators;
	}
	if (Team == Attackers)
	{
		UE_LOG(LogEXGameMode, Warning, TEXT("Player added to Attackers"));
	}
	else if (Team == Defenders)
	{
		UE_LOG(LogEXGameMode, Warning, TEXT("Player added to Defenders"));
	}
	else
	{
		UE_LOG(LogEXGameMode, Warning, TEXT("Player added to Spectators"));
	}
	TArray<AActor*> PSS;
	UGameplayStatics::GetAllActorsOfClass(this, AEXPlayerState::StaticClass(), PSS);
	// #ServerTeam
	Team->AddToTeam(PS);
}

void AEXGameModeSW::BeginPlay()
{
	Super::BeginPlay();

	ResetSpawns();
}

void AEXGameModeSW::InitGameState()
{
	Super::InitGameState();

	GSB = GetGameState<AEXGameStateSW>();
	if (GSB)
	{
		GSB->Init();
		GSB->SetMatchStates(AttackSpawnWaveTime, DefenseSpawnWaveTime, NumRounds);
	}
}

bool AEXGameModeSW::CanDealDamage(class AEXPlayerState* DamageCauser, class AEXPlayerState* DamagedPlayer) const
{
	if (bFriendlyFire)
	{
		return true;
	}

	// Allow damage to self
	if (DamagedPlayer == DamageCauser)
	{
		return true;
	}

	// Compare teams
	return DamageCauser && DamagedPlayer && (!DamageCauser->OnSameTeam(DamagedPlayer));
}

void AEXGameModeSW::ChangeState(FEXMatchState* InMatchState)
{
	FEXMatchStateSW* MS = (FEXMatchStateSW*)InMatchState;
	UE_LOG(LogEXLevel, Warning, TEXT("ChangeState to %s"), *(MS->State.ToString()));
	if (MS->Duration == 0) // Skip state 
	{
		EndRoundState();
		return;
	}
	CurrentState = MS->State;

	if (CurrentState.IsEqual(MatchState::InProgress) 
	 || CurrentState.IsEqual(MatchState::PreRound) 
	 || CurrentState.IsEqual(MatchState::PostRound))
	{
		AttSpawnWaveDuration = (MS->State.IsEqual(MatchState::InProgress)) ? MS->AttSpawnWaveDuration : 1;
		DefSpawnWaveDuration = (MS->State.IsEqual(MatchState::InProgress)) ? MS->DefSpawnWaveDuration : 1;

		AttSpawnSeconds = AttSpawnWaveDuration;
		DefSpawnSeconds = DefSpawnWaveDuration;

		RoundSeconds = MS->Duration;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Second, this, &AEXGameModeSW::Second, 1.f, true);

		OnMatchStateChanged();
	}
}

void AEXGameModeSW::OnMatchStateChanged()
{
	for (AEXPlayerController* PC : ConnectedPlayers)
	{
		PC->Client_Reliable_OnMatchStateChanged(CurrentState);
		UpdateClientTimers(PC);
	}
}

void AEXGameModeSW::UpdateClientTimers(AEXPlayerController* PC)
{
	AEXPlayerState* EXPS = PC->GetPlayerState<AEXPlayerState>();
	if (EXPS)
	{
		bool bAtt = !EXPS->GetTeam() || (EXPS->GetTeam()->GetType() == ETeam::Attack);
		int32 SpawnSeconds = bAtt ? AttSpawnSeconds : DefSpawnSeconds;
		int32 SpawnWave = bAtt ? AttSpawnWaveDuration : DefSpawnWaveDuration;
		PC->Client_Reliable_SetTimer(GSB->GetServerWorldTimeSeconds(), RoundSeconds, SpawnSeconds, SpawnWave);
	}
}

void AEXGameModeSW::Second()
{
	if (AttSpawnSeconds == 0) // Spawn wave
	{
		AttSpawnSeconds = AttSpawnWaveDuration;
		SpawnWave(ETeam::Attack);
	}
	if (DefSpawnSeconds == 0) // Spawn wave
	{
		DefSpawnSeconds = DefSpawnWaveDuration;
		SpawnWave(ETeam::Defense);
	}
	--AttSpawnSeconds;
	--DefSpawnSeconds;

	Super::Second();
}

void AEXGameModeSW::HalfTime()
{
	UE_LOG(LogGameMode, Verbose, TEXT("HalfTime %s"), *GetName());

	bFullReset = false;
	ResetLevel();
	bFullReset = true;
	SwapTeams();
	SetMatchState(MatchState::PreRound);
}

void AEXGameModeSW::RestartRound()
{
	if (!IsMatchInProgress())
	{
		// Nothing to reset
		return;
	}
	bFullReset = false;
	ResetLevel();
	bFullReset = true;
	GSB->ResetRound();
	SetMatchState(MatchState::PreRound);
}

void AEXGameModeSW::ResetLevel()
{
	ResetSpawns();

	Super::ResetLevel();
}

void AEXGameModeSW::SwapTeams()
{
	// Swap teams
	Swap(Attackers, Defenders);

	// Set the new type for both teams
	Attackers->SetType(ETeam::Attack);
	Defenders->SetType(ETeam::Defense);

	KillAllPlayers();
}

void AEXGameModeSW::FlagCaptured(AEXFlag* Flag, bool bRecaptured)
{
	if (bRecaptured)
	{
		SetSpawnsForStage(LevelRules->GetStage());
	}
	else
	{
		Attackers->SetSpawnArea(Flag->GetAttackerSpawn());
		Defenders->SetSpawnArea(Flag->GetDefenderSpawn());
	}
}

void AEXGameModeSW::AdvanceStage(int32 Stage)
{
	SetSpawnsForStage(Stage);
}

void AEXGameModeSW::SetSpawnsForStage(int32 Stage)
{
	const TArray<AEXSpawnArea*> AttackerSpawns = LevelRules->GetAttackerSpawns();
	const TArray<AEXSpawnArea*> DefenderSpawns = LevelRules->GetDefenderSpawns();
	if (ensure(AttackerSpawns.Num() > Stage))
	{
		Attackers->SetSpawnArea(AttackerSpawns[Stage]);
	}
	if (ensure(DefenderSpawns.Num() > Stage))
	{
		Defenders->SetSpawnArea(DefenderSpawns[Stage]);
	}
}

void AEXGameModeSW::SpawnWave(ETeam TeamType)
{
	UE_LOG(LogEXGameMode, Log, TEXT("Spawn wave"));

	AEXTeam* Team = (TeamType == ETeam::Attack) ? Attackers : (TeamType == ETeam::Defense) ? Defenders : nullptr;
	for (AEXPlayerState* Player : Team->GetMembers())
	{
		AEXPlayerController* PC = Player ? Cast<AEXPlayerController>(Player->GetOwner()) : nullptr;
		if (PC)
		{
			PC->AllowRespawn();
		}
	}
}

AEXTeam* AEXGameModeSW::GetAttackersOnStart() const
{
	return Attackers->GetStartedOnAtt() ? Attackers : Defenders;
}

AEXTeam* AEXGameModeSW::GetDefendersOnStart() const
{
	return Attackers->GetStartedOnAtt() ? Defenders : Attackers;
}

void AEXGameModeSW::AddToSpectators(AEXPlayerController* PC)
{
	// #ServerTeam
	Spectators->AddToTeam(PC->GetPlayerState<AEXPlayerState>());
}

void AEXGameModeSW::SwitchTeam(AEXPlayerController* PC)
{
	check(HasAuthority());
	UE_LOG(LogEXController, Warning, TEXT("%s switching teams"), *GetName());
	AEXPlayerState* EXPS = PC->GetPS();
	AEXTeam* Team = EXPS ? EXPS->GetTeam() : nullptr;
	const bool bWasDefending = Team && Team->IsDefending();
	AEXTeam* NewTeam = bWasDefending ? Attackers : Defenders;
	// #ServerTeam
	NewTeam->AddToTeam(EXPS);

	UpdateClientTimers(PC);
}

AActor* AEXGameModeSW::ChoosePlayerStart_Implementation(AController* Player)
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

	AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
	AEXTeam* Team = PS ? PS->GetTeam() : nullptr;
	return Team ? Team->GetPlayerStart(Cast<AEXPlayerController>(Player)) 
		: UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
}

void AEXGameModeSW::EndRoundState()
{
	if (MatchState.IsEqual(MatchState::PreRound))
	{
		StartRound();
	}
	else if (MatchState.IsEqual(MatchState::InProgress))
	{
		SetMatchState(LevelRules->OvertimeCheck() ? MatchState::Overtime : MatchState::PostRound);
	}
	else if (MatchState.IsEqual(MatchState::Overtime))
	{
		SetMatchState(MatchState::PostRound);
	}
	else if (MatchState.IsEqual(MatchState::PostRound))
	{
		if (GSB->IsOnLastRound())
		{
			EndMatch();
		}
		else
		{
			HalfTime();
		}
	}

}

