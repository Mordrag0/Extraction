// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameModeBase.h"
#include "Player/EXBaseController.h"
#include "Online/EXPlayerState.h"
#include "Online/EXGameInstance.h"
#include "Player/EXCharacter.h"
#include "Online/EXGameSession.h"
#include "System/EXGameplayStatics.h"
#include "Online/EXOnlineBeaconHost.h"
#include "Online/EXOnlineBeaconHostObject.h"
#include "Online/EXLevelRules.h"
#include "Online/EXAdminMessage.h"
#include "Player/EXPlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameSession.h"
#include "Engine/LevelScriptActor.h"
#include "Online/EXGameStateBase.h"
#include "Online/EXGameMessage.h"
#include "System/EXInteract.h"
#include "GameFramework/SpectatorPawn.h"
#include "EXTypes.h"
#include "Online/EXTeam.h"
#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXInventory.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/EXPlayerStart.h"
#include "Inventory/EXAbilityTracker.h"

AEXGameModeBase::AEXGameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AEXGameModeBase::AllowTextMessage_Implementation(const FString& Msg, bool bIsTeamMessage, AEXBaseController* Sender) const
{
	return true;
}

bool AEXGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate /*= FCanUnpause()*/)
{
	bool bPaused = Super::SetPause(PC, CanUnpauseDelegate);
	if (!bPaused)
	{
		return false;
	}
	PauseStatusChanged(true);
	return true;
}

bool AEXGameModeBase::ClearPause()
{
	bool bPauseCleared = Super::ClearPause();
	if (!bPauseCleared)
	{
		return false;
	}
	PauseStatusChanged(false);
	return true;
}

void AEXGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	AEXPlayerController* NewPC = Cast<AEXPlayerController>(NewPlayer);
	Players.Add(NewPC);
}

void AEXGameModeBase::StartToLeaveMap()
{
	Super::StartToLeaveMap();

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->MSDisconnect();
}

bool AEXGameModeBase::ShouldResetActor(AActor* ActorToReset)
{
	if (ActorToReset->IsA<AGameState>()
	 || ActorToReset->IsA<AEXLevelRules>()
	 || ActorToReset->IsA<APlayerState>()
	 || ActorToReset->IsA<AGameSession>()
	)
	{
		return false;
	}

	return true;
}

void AEXGameModeBase::PauseStatusChanged(bool bNewPaused)
{
	for (AEXPlayerController* PC : Players)
	{
		PC->PauseStatusChanged(bNewPaused);
	}
}

FString AEXGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	AEXPlayerState* EXPS = NewPlayerController->GetPlayerState<AEXPlayerState>();
#if WITH_EDITOR
	//PS->SetSteamID(PlayerIdx == 3 ? 1 : PlayerIdx); // For testing rejoin when playing with 2 clients - [Open 127.0.0.1:17777]
	EXPS->SetSteamID(PlayerIdx);
	++PlayerIdx;
#else
	uint64 SteamId = UniqueId->IsValid() ? (*(uint64*)UniqueId->GetBytes()) : 0;
	EXPS->SetSteamID(SteamId);
#endif

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	AEXPlayerController* PC = Cast<AEXPlayerController>(NewPlayerController);
	if (PC)
	{
		GI->RequestPlayerInfo(EXPS);

		ConnectedPlayers.Add(PC);
	}

	return ErrorMessage;
}

void AEXGameModeBase::Logout(AController* Exiting)
{
	AEXPlayerController* PC = Cast<AEXPlayerController>(Exiting);
	ConnectedPlayers.Remove(PC);

	AEXPlayerState* PS = PC->GetPlayerState<AEXPlayerState>();
	// #ServerTeam
	if (PS->GetTeam())
	{
		PS->GetTeam()->RemoveFromTeam(PS);
	}
	if (UEXAbilityTracker* AbilityTracker = PC->GetAbilityTracker())
	{
		AbilityTracker->DestroyAbilities();
	}

	Super::Logout(Exiting);
}

void AEXGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	LevelRules = UEXGameplayStatics::GetLevelRules(this);
	if (LevelRules)
	{
		LevelRules->InitInteractable();
	}
	Super::InitGame(MapName, Options, ErrorMessage);

	EXGameSession = Cast<AEXGameSession>(GameSession);
}

TSubclassOf<AGameSession> AEXGameModeBase::GetGameSessionClass() const
{
	return AEXGameSession::StaticClass();
}

void AEXGameModeBase::BeginPlay()
{
	LevelRules = UEXGameplayStatics::GetLevelRules(this);
	Super::BeginPlay();

	InitTeams();

	OBHost = GetWorld()->SpawnActor<AEXOnlineBeaconHost>(AEXOnlineBeaconHost::StaticClass(), FTransform());
	if (OBHost && OBHost->Start())
	{
		OBHostObject = GetWorld()->SpawnActor<AEXOnlineBeaconHostObject>(AEXOnlineBeaconHostObject::StaticClass(), FTransform());
		OBHost->AddHost(OBHostObject);
	}

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->MSConnect();

	bWarmupTimerDone = false;

	// #PAUSE
	GetWorld()->bIsCameraMoveableWhenPaused = true;


	// #Spawn
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), OutActors);
	for (AActor* A : OutActors)
	{
		AEXPlayerStart* EXPlayerStart = Cast<AEXPlayerStart>(A);
		if (EXPlayerStart && EXPlayerStart->IsEnabled())
		{
			PlayerStarts.Add(EXPlayerStart);
		}
	}

#if WITH_EDITOR
	ForceStartMatch();
#endif
}

void AEXGameModeBase::WarmUpTimerDone()
{
	bWarmupTimerDone = true;
}

void AEXGameModeBase::StartMatchIfConditionsMet()
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		// Check to see if we should start the match
		if (ReadyToStartMatch())
		{
			UE_LOG(LogGameMode, Log, TEXT("GameMode returned ReadyToStartMatch"));
			StartMatch();
		}
		else // Start timer after first login
		{
			FTimerManager& TimerManager = GetWorld()->GetTimerManager();
			if (!TimerManager.IsTimerActive(TimerHandle_WarmupTimer))
			{
				TimerManager.SetTimer(TimerHandle_WarmupTimer, this, &AEXGameModeBase::WarmUpTimerDone, WarmupMaxDuration);
			}
		}
	}
}

void AEXGameModeBase::SetNumPlayersRequiredToStart(uint8 InNumPlayersRequiredToStart)
{
	NumPlayersRequiredToStart = InNumPlayersRequiredToStart;
	StartMatchIfConditionsMet();
}

int32 AEXGameModeBase::GetStateDuration(const FName& InMatchState) const
{
	if (InMatchState == MatchState::PreRound)
	{
		return PreRoundTime;
	}
	else if (InMatchState == MatchState::InProgress)
	{
		return RoundTime;
	}
	else if (InMatchState == MatchState::PostRound)
	{
		return PostRoundTime;
	}
	else
	{
		return -1;
	}
}

void AEXGameModeBase::Second()
{
	--RoundSeconds;
	if (RoundSeconds < 0) // Out of time
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Second);
		EndRoundState();
	}
}

void AEXGameModeBase::EndRoundState()
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
		EndMatch();
	}
}

void AEXGameModeBase::AddBots(int32 Count)
{
	if (PlayerStarts.Num() == 0)
	{
		return;
	}
	for (int32 Idx = 0; Idx < Count; Idx++)
	{
		AEXPlayerStart* PlayerStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
		APawn* Pawn = UAIBlueprintHelperLibrary::SpawnAIFromClass(this, BotClass, BotBehaviorTree, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
		AEXCharacter* EXCharacter = Cast<AEXCharacter>(Pawn);
		if (EXCharacter)
		{
			Bots.Add(EXCharacter);
		}
	}
}

void AEXGameModeBase::RemoveAllBots()
{
	for (AEXCharacter* EXCharacter : Bots)
	{
		if(IsValid(EXCharacter))
		{
			EXCharacter->SelfKill();
		}
		Bots.Remove(EXCharacter);
	}
}

void AEXGameModeBase::AdvanceStage(int32 Stage)
{
}

void AEXGameModeBase::FlagCaptured(AEXFlag* Flag, bool bRecaptured)
{

}

bool AEXGameModeBase::RequestAdmin(AEXPlayerController* Player, const FString& Password)
{
	if (Admins.Contains(Player))
	{
		Player->ClientReceiveLocalizedMessage(UEXAdminMessage::StaticClass(), 2, Player->GetPlayerState<AEXPlayerState>());
		return false;
	}
	for (const FString& ServerPW : ServerPasswords)
	{
		if (ServerPW.Equals(Password, ESearchCase::IgnoreCase))
		{
			Admins.Add(Player);
			UE_LOG(LogEXAdmin, Log, TEXT("%s logged in as admin with password %s"), *Player->GetName(), *Password);
			Player->Client_Reliable_EnableAdmin();
			Player->ClientReceiveLocalizedMessage(UEXAdminMessage::StaticClass(), 0, Player->GetPlayerState<AEXPlayerState>());
			return true;
		}
	}
	Player->ClientReceiveLocalizedMessage(UEXAdminMessage::StaticClass(), 1, Player->GetPlayerState<AEXPlayerState>());
	return false;
}

void AEXGameModeBase::ResetLevel()
{
	UE_LOG(LogGameMode, Verbose, TEXT("Reset %s"), *GetName());

	// Reset ALL controllers first
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AController* Controller = Iterator->Get();
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (PlayerController)
		{
			PlayerController->ClientReset();
		}
		Controller->Reset();
	}

	// Reset all actors (except controllers, the GameMode, and any other actors specified by ShouldResetActor())
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* A = *It;
		if (IsValid(A) && (A != this) && !A->IsA<AController>() && !A->IsA<ACharacter>() && (bFullReset || ShouldResetActor(A)))
		{
			A->Reset();
		}
	}

	// Reset the GameMode
	if (bFullReset)
	{
		Reset();
	}

	// Notify the level script that the level has been reset
	ALevelScriptActor* LevelScript = GetWorld()->GetLevelScriptActor();
	if (LevelScript)
	{
		LevelScript->LevelReset();
	}
}

void AEXGameModeBase::LoadNewPLayer(uint64 SteamId, const TArray<FSquadMerc>& Squad, const FString& Name)
{
	AEXPlayerController* NewPC = GetPlayer(SteamId);
	AEXPlayerState* NewPS = NewPC->GetPlayerState<AEXPlayerState>();
	NewPS->SetPlayerName(Name);
	NewPS->SetSquad(Squad);

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	TMap<EMerc, TSoftClassPtr<AEXCharacter>> MercClasses = GI->GetMercClasses();
	TArray<TSoftClassPtr<AEXCharacter>> ClassesToLoad;

	// The BP classes (from the new player) that all players must load
	for (const FSquadMerc& Merc : Squad)
	{
		ClassesToLoad.Add(MercClasses[Merc.Type]);
	}
	LoadServerAssets(ClassesToLoad);
	TArray<TSoftClassPtr<AEXCharacter>> ClassesToLoadOnNewPlayer;
	for (AEXPlayerController* Player : ConnectedPlayers)
	{
		if (Player != NewPC)
		{
			Player->Client_Reliable_LoadAssets(ClassesToLoad);
			// The BP classes that the new player has to load (from previous players)
			for (const TSoftClassPtr<AEXCharacter>& MercClass : Player->GetSelectedMercs())
			{
				ClassesToLoadOnNewPlayer.AddUnique(MercClass);
			}
		}
	}
	NewPC->Client_Reliable_Init(ClassesToLoadOnNewPlayer, Squad);
	NewPC->SetSelectedMercs(ClassesToLoad); // Set players squad
	NewPC->SetClientDataLoaded();
	UE_LOG(LogEXController, Warning, TEXT("LoadNewPlayer"));
}

AEXPlayerController* AEXGameModeBase::GetPlayer(uint64 SteamId) const
{
	for (AEXPlayerController* PC : ConnectedPlayers)
	{
		if (PC->GetPlayerState<AEXPlayerState>()->GetSteamID() == SteamId)
		{
			return PC;
		}
	}
	UE_LOG(LogEXLevel, Error, TEXT("NO PC FOUND [%llu]"), SteamId);
	return nullptr;
}

void AEXGameModeBase::LoadServerAssets(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs)
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	// Get the CDO and load its assets
	TArray<FSoftObjectPath> AssetsToLoad;
	for (const TSoftClassPtr<AEXCharacter>& MercClass : Mercs)
	{
		// Load the BP class
		LoadedAssets.Add(StreamableManager.LoadSynchronous(MercClass));
		UE_LOG(LogEXAssets, Verbose, TEXT("Loading %s"), *MercClass->GetName());
		AEXCharacter* MercCDO = MercClass->GetDefaultObject<AEXCharacter>();
		TArray<FSoftObjectPath> MercAssets = MercCDO->GetAssets();
		for (const FSoftObjectPath& AssetRef : MercAssets)
		{
			LoadedAssets.Add(StreamableManager.LoadSynchronous(AssetRef));
			UE_LOG(LogEXAssets, Verbose, TEXT("Loading %s"), *AssetRef.GetAssetName());
		}
	}
}

void AEXGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Update spectator location for client
	AEXPlayerController* NewPC = Cast<AEXPlayerController>(NewPlayer);
	if (NewPC && !NewPC->GetPawn())
	{
		NewPC->Client_Reliable_SetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation());
	}

	// Notify new player if match is already in progress
	if (NewPC && IsMatchInProgress())
	{
		NewPC->Client_Reliable_StartOnlineGame();
	}

	StartMatchIfConditionsMet();
}

bool AEXGameModeBase::ReadyToStartMatch_Implementation()
{
	// If bDelayed Start is set, wait for a manual match start
	if (bDelayedStart)
	{
		return false;
	}

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		// Start when everyone connects or the timer expires
		if ((NumPlayers >= NumPlayersRequiredToStart) /*&& bWarmupTimerDone*/)
		{
			return true;
		}
	}
	return false;
}

void AEXGameModeBase::ForceStartMatch()
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		UE_LOG(LogGameMode, Log, TEXT("ForceStartMatch"));
		StartMatch();
	}
}

void AEXGameModeBase::StartMatch()
{
	if (HasMatchStarted())
	{
		// Already started
		return;
	}

	//Let the game session override the StartMatch function, in case it wants to wait for arbitration
	if (GameSession->HandleStartMatchRequest())
	{
		return;
	}

	// Skip calling Super::StartMatch

	Super::HandleMatchHasStarted();
	StartPreRound();
}

void AEXGameModeBase::InitGameState()
{
	Super::InitGameState();

	GSB = GetGameState<AEXGameStateBase>();
}

void AEXGameModeBase::AdminSetTime(uint32 Time)
{
	RoundSeconds = Time;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Second);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Second, this, &AEXGameModeBase::Second, 1.f, true);
	OnMatchStateChanged();
}

void AEXGameModeBase::ChangeState(FEXMatchState* InMatchState)
{
	CurrentState = InMatchState->State;
	RoundSeconds = InMatchState->Duration;
	if (RoundSeconds > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Second, this, &AEXGameModeBase::Second, 1.f, true);
	}
	OnMatchStateChanged();
}

void AEXGameModeBase::OnMatchStateChanged()
{
	for (AEXPlayerController* PC : ConnectedPlayers)
	{
		PC->Client_Reliable_OnMatchStateChanged(CurrentState);
		PC->Client_Reliable_SetTimer(GSB->GetServerWorldTimeSeconds(), RoundSeconds, 0, 0);
	}
}

void AEXGameModeBase::OnMatchStateSet()
{
	GSB->SetMatchState(MatchState, GetStateDuration(MatchState));
	FGameModeEvents::OnGameModeMatchStateSetEvent().Broadcast(MatchState);

	// Call change callbacks
	if (MatchState.IsEqual(MatchState::WaitingToStart))
	{
		HandleMatchIsWaitingToStart();
	}
	else if (MatchState.IsEqual(MatchState::PreRound))
	{
		HandleMatchPreRound();
	}
	else if (MatchState.IsEqual(MatchState::InProgress))
	{
		HandleMatchHasStarted();
	}
	else if (MatchState.IsEqual(MatchState::Overtime))
	{
		HandleOvertime();
	}
	else if (MatchState.IsEqual(MatchState::PostRound))
	{
		HandleMatchPostRound();
	}
	else if (MatchState.IsEqual(MatchState::WaitingPostMatch))
	{
		HandleMatchHasEnded();
	}
	else if (MatchState.IsEqual(MatchState::LeavingMap))
	{
		HandleLeavingMap();
	}
	else if (MatchState.IsEqual(MatchState::Aborted))
	{
		HandleMatchAborted();
	}
}

void AEXGameModeBase::HandleMatchHasStarted()
{
	// Not calling super on purpose, its called in StartMatch

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AEXPlayerController* PC = Cast<AEXPlayerController>(*It);
		if (AEXCharacter* Char = PC->GetEXCharacter())
		{
			Char->StartTracking();
		}
	}
	LevelRules->AdvanceStage(); // Go to Stage 1
}

void AEXGameModeBase::HandleMatchPreRound()
{
	GSB->StartRound();

	LevelRules->ResetStage();
}

void AEXGameModeBase::HandleOvertime()
{
	BroadcastLocalized(this, UEXGameMessage::StaticClass(), 1);
}

void AEXGameModeBase::HandleMatchPostRound()
{
	for (AEXInteract* Objective : LevelRules->GetObjectives())
	{
		Objective->StopInteraction();
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AEXPlayerController* PC = Cast<AEXPlayerController>(*It);
		if (AEXCharacter* Char = PC->GetEXCharacter())
		{
			Char->StopTracking();
		}
	}

	GSB->EndRound();
}

void AEXGameModeBase::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void AEXGameModeBase::StartPreRound()
{
	SetMatchState(MatchState::PreRound);
}

void AEXGameModeBase::StartRound()
{
	SetMatchState(MatchState::InProgress);
	BroadcastLocalized(this, UEXGameMessage::StaticClass(), 0);
}

void AEXGameModeBase::StartPostRound()
{
	SetMatchState(MatchState::PostRound);
}

float AEXGameModeBase::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	AEXCharacter* DamagedPawn = Cast<AEXCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		AEXPlayerState* DamagedPlayerState = DamagedPawn->GetPlayerState<AEXPlayerState>();
		AEXPlayerState* InstigatorPlayerState = EventInstigator->GetPlayerState<AEXPlayerState>();

		// Check for friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			return 0.f;
		}
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)&DamageEvent;

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PointDamageEvent->HitInfo.PhysMaterial.Get());
			switch (SurfaceType)
			{
				case SURFACE_Body: break;
				case SURFACE_Head: ActualDamage *= HeadShotMultiplier; break;
				case SURFACE_Limb: ActualDamage *= LimbMultiplier; break;
				case SURFACE_LowerLimb: ActualDamage *= LowerLimbMultiplier; break;
				default: break;
			}
		}

	}

	return ActualDamage;
}

void AEXGameModeBase::KillAllPlayers()
{
	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(this, AEXCharacter::StaticClass(), Characters);
	for (AActor* Player : Characters)
	{
		AEXCharacter* Character = Cast<AEXCharacter>(Player);
		Character->SelfKill();
	}
}

AActor* AEXGameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName /*= TEXT("")*/)
{
	return ChoosePlayerStart(Player);
}

UClass* AEXGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	UClass* SelectedClass = nullptr;
	AEXPlayerController* PC = Cast<AEXPlayerController>(InController);
	if (PC)
	{
		SelectedClass = PC->GetSelectedClass();
	} 
	return SelectedClass ? SelectedClass : SpectatorClass;
}

void AEXGameModeBase::EndMatch()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Second);

	Super::EndMatch();

	GSB->SendGameResult();
}

