// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/EXPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Player/EXCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Online/EXGameModeSW.h"
#include "HUD/EXHUDWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Online/EXPlayerState.h"
#include "Online/EXLevelRules.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/EXWeapon.h"
#include "Inventory/EXInventory.h"
#include "HUD/EXDeathScreen.h"
#include "Online/EXGameModeBase.h"
#include "Online/EXAssaultCourseGame.h"
#include "HUD/EXAssaultCourseWidget.h"
#include "Online/EXGameStateSW.h"
#include "Engine/Texture.h"
#include "Inventory/EXDamageType.h"
#include "PLayer/EXPersistentUser.h"
#include "Player/EXLocalPlayer.h"
#include "Online/EXTeam.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "EX.h"
#include "Inventory/EXInventoryComponent.h"
#include "HUD/EXObjectiveTimes.h"
#include "HUD/EXInGameMenu.h"
#include "HUD/EXGameStatus.h"
#include "System/EXGameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HUD/EXPlayerContextMenu.h"
#include "Online/EXGameInstance.h"
#include "Player/EXPlayerCameraManager.h"
#include "System/EXInteract.h"
#include "Online/EXAdminMessage.h"
#include "HUD/EXAssaultCourseScores.h"
#include "Sound/SoundCue.h"
#include "Engine/AssetManager.h"
#include "HUD/EXScoreboardBase.h"
#include "HUD/EXTextSelectPanel.h"
#include "Misc/EXInputInfo.h"
#include "Inventory/EXAbilityTracker.h"

#if WITH_EDITOR
#define ADMINCHECK(x) LogAdminCommand(x)
#else
#define ADMINCHECK(x) if (!GMB || !GMB->IsAdmin(this)) { ClientReceiveLocalizedMessage(UEXAdminMessage::StaticClass(), 4); return; } LogAdminCommand(x)
#endif
#define HUDCHECK if (!HUD) { return; }


AEXPlayerController::AEXPlayerController()
{
	// #PAUSE
	bShouldPerformFullTickWhenPaused = true;

	bCanPossessWithoutAuthority = false;
}

void AEXPlayerController::Client_Reliable_LoadAssets_Implementation(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs)
{
	LoadAssets(Mercs);
}

void AEXPlayerController::Client_Reliable_Init_Implementation(const TArray<TSoftClassPtr<AEXCharacter>>& ClassesToLoadOnNewPlayer, const TArray<FSquadMerc>& InSquad)
{
	// The BP classes (from the new player) 
	TArray<TSoftClassPtr<AEXCharacter>> ClassesToLoad;
	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	TMap<EMerc, TSoftClassPtr<AEXCharacter>> MercClasses = GI->GetMercClasses();
	for (const FSquadMerc& Merc : InSquad)
	{
		ClassesToLoad.Add(MercClasses[Merc.Type]);
	}
	TArray<TSoftClassPtr<AEXCharacter>> AllClasses = ClassesToLoadOnNewPlayer;
	AllClasses.Append(ClassesToLoad);
	LoadAssets(AllClasses);

	SetSelectedMercs(ClassesToLoad); // Set players squad

	if (HUD) 
	{
		HUD->SetIcons(ClassesToLoad);
	}

	bClientDataLoaded = true;

	TArray<AEXCharacter*> Players;
	UEXGameplayStatics::GetAllActorsOfClass<AEXCharacter>(this, AEXCharacter::StaticClass(), Players);
	for (AEXCharacter* APlayer : Players)
	{
		// Equipped weapons will be null at this point, so initialize them by reequipping 
		APlayer->GetInventoryComponent()->AssetsLoaded();
	}

	GI->HideTravelWidget();
	UE_LOG(LogEXController, Warning, TEXT("Client_Reliable_Init_Implementation"));
}

void AEXPlayerController::SetSelectedMercs(const TArray<TSoftClassPtr<AEXCharacter>>& InSelectedMercs)
{
	SelectedMercs = InSelectedMercs;
	SelectedPlayerClass = SelectedMercs[0].Get();
}

void AEXPlayerController::Server_Reliable_OnClientInit_Implementation()
{
#if WITH_EDITOR && !USE_MASTER_SERVER_WITH_EDITOR
	TArray<FSquadMerc> DefaultSquad = { FSquadMerc::CreateDefault(0), FSquadMerc::CreateDefault(1), FSquadMerc::CreateDefault(2) };
	FString Name = FString::Printf(TEXT("Player %d"), MyEXPS->GetSteamID());
	if (GMB)
	{
		GMB->LoadNewPLayer(MyEXPS->GetSteamID(), DefaultSquad, Name);
	}
#endif
}

void AEXPlayerController::DisplayMessage(FText Message)
{
	HUDCHECK;

	HUD->DisplayMessage(Message);
}

void AEXPlayerController::OnDeath()
{
	SetInteractionIconVisibility(false);

	if (AbilityTracker) 
	{
		AbilityTracker->OnCharacterDeath(EXCharacter);
	}
}

void AEXPlayerController::Reset()
{
	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		MyPawn->Reset();
	}

	Super::Reset();
}

void AEXPlayerController::LoadAssets(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs)
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

void AEXPlayerController::UnloadAssets(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs)
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	// Get the CDO and load its assets
	TArray<FSoftObjectPath> AssetsToLoad;
	for (const TSoftClassPtr<AEXCharacter>& MercClass : Mercs)
	{
		// Load the BP class
		AEXCharacter* MercCDO = MercClass->GetDefaultObject<AEXCharacter>();
		TArray<FSoftObjectPath> MercAssets = MercCDO->GetAssets();
		for (const FSoftObjectPath& AssetRef : MercAssets)
		{
			StreamableManager.Unload(AssetRef);
			UE_LOG(LogEXAssets, Verbose, TEXT("Unloading %s"), *AssetRef.GetAssetName());
		}
		StreamableManager.Unload(MercClass.ToSoftObjectPath());
		UE_LOG(LogEXAssets, Verbose, TEXT("Unloading %s"), *MercClass->GetName());
	}
}

bool AEXPlayerController::CanRestartPlayer()
{
	if (!bAssetsLoaded || !bTimeSynced)
	{
		return false;
	}

	return Super::CanRestartPlayer();
}

void AEXPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	EXCharacter = Cast<AEXCharacter>(InPawn);
}

void AEXPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AEXPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	EXCharacter = Cast<AEXCharacter>(InPawn);
}

void AEXPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	EXCharacter = GetPawn<AEXCharacter>();
}

void AEXPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEXPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

void AEXPlayerController::SpawnPlayerCameraManager()
{
	Super::SpawnPlayerCameraManager();

	PCM = Cast<AEXPlayerCameraManager>(PlayerCameraManager);
}

void AEXPlayerController::UpdateVote(int32 Yeses, int32 Nos)
{
	HUDCHECK;

	HUD->UpdateVote(Yeses, Nos);
}

void AEXPlayerController::StartVote(const FVoteInfo& Vote)
{
	switch (Vote.VoteType)
	{
	case EVote::Kick: 
	{
		AEXPlayerState* Target = GSB->GetPlayer(Vote.Option);
		if (!Target)
		{
			return;
		}
		if (bTeamGame)
		{
			if (bTeamGame && !Target->OnSameTeam(MyEXPS))
			{
				return;
			}
		}
		const FString PlayerName = Target->GetPlayerName();
		HUD->VoteKick(PlayerName, Vote.GetRequiredVotes(), Vote.VoteDuration);
		break;
	}
	case EVote::Shuffle: 
	{
		HUD->VoteShuffle(Vote.GetRequiredVotes(), Vote.VoteDuration);
		break;
	}
	case EVote::Surrender: 
	{

		break;
	}
	case EVote::None:
	default:
		return;
	}

	if (MyEXPS == Vote.Instigator)
	{
		// Automatically vote yes on votes the player starts himself
		OnVote(true);
	}
	else
	{
		bCanVote = true;
	}
}

void AEXPlayerController::EndVote()
{
	bCanVote = false;
	HUD->EndVote();
}

void AEXPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Respawn", IE_Pressed, this, &AEXPlayerController::Respawn).bConsumeInput = false;

	InputComponent->BindAction("ShowScoreboard", IE_Pressed, this, &AEXPlayerController::ShowScoreboard);
	InputComponent->BindAction("ShowScoreboard", IE_Released, this, &AEXPlayerController::HideScoreboard);

	InputComponent->BindAction("PrimaryMerc", IE_Pressed, this, &AEXPlayerController::MercSelect<0>);
	InputComponent->BindAction("SecondaryMerc", IE_Pressed, this, &AEXPlayerController::MercSelect<1>);
	InputComponent->BindAction("TertiaryMerc", IE_Pressed, this, &AEXPlayerController::MercSelect<2>);

	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &AEXPlayerController::PushToTalk<true>);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &AEXPlayerController::PushToTalk<false>);

	InputComponent->BindAction("Cancel", IE_Pressed, this, &AEXPlayerController::Cancel);

	InputComponent->BindAction("QuickChat", IE_Pressed, this, &AEXPlayerController::QuickChat);
	InputComponent->BindAction("OpenAdminPanel", IE_Pressed, this, &AEXPlayerController::OpenAdminPanel);
	InputComponent->BindAction("VoteYes", IE_Pressed, this, &AEXPlayerController::VoteYes);
	InputComponent->BindAction("VoteNo", IE_Pressed, this, &AEXPlayerController::VoteNo);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AEXPlayerController::Click).bConsumeInput = false;

	InputComponent->BindAction("One", IE_Pressed, this, &AEXPlayerController::QuickSelect<0>).bConsumeInput = false;
	InputComponent->BindAction("Two", IE_Pressed, this, &AEXPlayerController::QuickSelect<1>).bConsumeInput = false;
	InputComponent->BindAction("Three", IE_Pressed, this, &AEXPlayerController::QuickSelect<2>).bConsumeInput = false;
	InputComponent->BindAction("Four", IE_Pressed, this, &AEXPlayerController::QuickSelect<3>).bConsumeInput = false;
	InputComponent->BindAction("Five", IE_Pressed, this, &AEXPlayerController::QuickSelect<4>).bConsumeInput = false;
	InputComponent->BindAction("Six", IE_Pressed, this, &AEXPlayerController::QuickSelect<5>).bConsumeInput = false;
	InputComponent->BindAction("Seven", IE_Pressed, this, &AEXPlayerController::QuickSelect<6>).bConsumeInput = false;
	InputComponent->BindAction("Eight", IE_Pressed, this, &AEXPlayerController::QuickSelect<7>).bConsumeInput = false;
	InputComponent->BindAction("Nine", IE_Pressed, this, &AEXPlayerController::QuickSelect<8>).bConsumeInput = false;
	InputComponent->BindAction("Zero", IE_Pressed, this, &AEXPlayerController::QuickSelect<9>).bConsumeInput = false;

	InputComponent->BindAction("ShowObjectives", IE_Pressed, this, &AEXPlayerController::ShowObjectives<true>);
	InputComponent->BindAction("ShowObjectives", IE_Released, this, &AEXPlayerController::ShowObjectives<false>);

	InputComponent->BindAction("Test", IE_Pressed, this, &AEXPlayerController::TestServer);
}

void AEXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LevelRules = UEXGameplayStatics::GetLevelRules(this);
	if (HasAuthority())
	{
		GMB = GetWorld()->GetAuthGameMode<AEXGameModeBase>(); 
		AbilityTracker = NewObject<UEXAbilityTracker>(this, UEXAbilityTracker::StaticClass(), FName("AbilityTracker"));
	}
	GSB = GetWorld()->GetGameState<AEXGameStateBase>();
	GSSW = GetWorld()->GetGameState<AEXGameStateSW>();
	if (IsLocalController())
	{
		CreateHUD();

		PersistentUser = GetPersistentUser();
		bHitMarkersEnabled = PersistentUser->GetHitMarkers();
		bHideCrosshairWhenADS = !PersistentUser->GetADSCrosshair();
#if WITH_EDITOR
		SetDefaultFOV(80.f); // #EDITORDEFAULTS
#else
		SetDefaultFOV(PersistentUser->GetFOV());
#endif

	}

	if (!HasAuthority())
	{
		Server_Reliable_OnClientInit();
	}
}

bool AEXPlayerController::CanRespawn() const
{
	if (IsNetMode(NM_Standalone))
	{
		return !EXCharacter;
	}
	if (EXCharacter && !EXCharacter->IsDead())
	{
		return false;
	}
	if (!bClientDataLoaded)
	{
		return false;
	}

	if (!MyEXPS)
	{
		return false;
	}
	if (!GSB || !GSB->GameReady())
	{
		return false;
	}
	if (GSB->IsRoundInProgress() && !bCanRespawn)
	{
		return false;
	}

	if (GSSW && (!MyEXPS->GetTeam() || MyEXPS->GetTeam()->IsSpectating()))
	{
		return false;
	}
	return true;
}

void AEXPlayerController::Respawn()
{
	if (CanRespawn())
	{
		if (HUD) 
		{
			HUD->SetDeathScreenVisibility(false);
		}

		// We need a selected player class if were playing in GM, but don't if were playing in GMB thats not GM
		if(ensure(!GMB || !!SelectedPlayerClass))
		{
			Server_Reliable_Respawn();
		}
	}
}

void AEXPlayerController::Server_Reliable_ChoosePawnClass_Implementation(UClass* PawnClass)
{
	SelectedPlayerClass = PawnClass;
}

bool AEXPlayerController::Server_Reliable_ChoosePawnClass_Validate(UClass* PawnClass)
{
	return true;
}

void AEXPlayerController::SetCanRespawn(bool bInCanRespawn)
{
	bCanRespawn = bInCanRespawn;
	Client_Reliable_SetCanRespawn(bInCanRespawn);
}

void AEXPlayerController::Client_Reliable_SetCanRespawn_Implementation(bool bInCanRespawn)
{
	bCanRespawn = bInCanRespawn;

	if (HUD)
	{
		HUD->SetCanRespawn(bCanRespawn);
	}
}

void AEXPlayerController::ShowScoreboard()
{
	if (Scoreboard)
	{
		Scoreboard->Show();
	}
	else if (UEXAssaultCourseScores* ACScores = HUD->GetAssaultCourseScores())
	{
		ACScores->Show();
	}
}

void AEXPlayerController::HideScoreboard()
{
	if (Scoreboard)
	{
		Scoreboard->Close();
	}
	else if (UEXAssaultCourseScores* ACScores = HUD->GetAssaultCourseScores())
	{
		ACScores->Hide();
	}
}

void AEXPlayerController::Click()
{
	if (Scoreboard && Scoreboard->IsOpen())
	{
		Scoreboard->BeginUIMode();
	}
}

template<bool bSpeaking>
void AEXPlayerController::PushToTalk()
{
	if (IsNetMode(NM_Standalone))
	{
		return;
	}
	if (bSpeaking)
	{
		UE_LOG(LogTemp, Warning, TEXT("Begin speaking"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("End speaking"));
	}
	ToggleSpeaking(bSpeaking);

	HUDCHECK;
	HUD->ToggleSpeaking(bSpeaking);
}

void AEXPlayerController::Cancel()
{
	if (IsValid(this))
	{
		return;
	}

	HUDCHECK;

	if (HUD->GetChatWidget() && HUD->GetChatWidget()->IsTyping())
	{
		HUD->GetChatWidget()->StopTyping(true);
	}
	else if (QuickChatWindow && QuickChatWindow->IsOpen())
	{
		QuickChatWindow->Close();
	}
	else if (AdminPanel && AdminPanel->IsOpen())
	{
		AdminPanel->Close();
	}
	else if (InGameMenu->IsOpen())
	{
		InGameMenu->Close();
	}
	else
	{
		InGameMenu->Open();
	}
}

void AEXPlayerController::QuickChat()
{
	OpenTextPanel(QuickChatWindow, QuickChatOptions);
}

void AEXPlayerController::OpenAdminPanel()
{
	OpenTextPanel(AdminPanel, AdminCommands);
}

void AEXPlayerController::OpenTextPanel(UEXTextSelectPanel* Panel, const FTextPanelCategory& Options)
{
	if (Panel)
	{
		if (Panel->IsOpen())
		{
			Panel->Close();
		}
		else
		{
			Panel->Open();
			Panel->SetSelectedCategory(Options);
		}
	}
}

template<int32 Index>
void AEXPlayerController::QuickSelect()
{
	if (QuickChatWindow && QuickChatWindow->IsOpen())
	{
		QuickSelectTextPanel(QuickChatWindow, Index);
	}
	else if (AdminPanel && AdminPanel->IsOpen())
	{
		QuickSelectTextPanel(AdminPanel, Index);
	}
	else
	{
		AEXTeam* Team = MyEXPS ? MyEXPS->GetTeam() : nullptr;
		if (Team && Team->IsSpectating())
		{
			// #EXTODO2
			/*AEXTeam* TeamToSpec = bAlt ? LevelRules->GetDefenders() : LevelRules->GetAttackers();
			if (TeamToSpec->GetMembers().IsValidIndex(Index))
			{
				AEXPlayerState* PlayerToSpec = TeamToSpec->GetMembers()[Index];
				AEXCharacter* CharacterToSpec = PlayerToSpec ? PlayerToSpec->GetPawn<AEXCharacter>() : nullptr;
				if (CharacterToSpec)
				{
					UE_LOG(LogTemp, Warning, TEXT("Possessing"));
					Possess(CharacterToSpec);
				}
			}*/
		}
	}
}

void AEXPlayerController::QuickSelectTextPanel(UEXTextSelectPanel* Panel, int32 Idx)
{
	FTextPanelCategory CurrentCategory = Panel->GetCurrentCategory();
	if(CurrentCategory.Categories.IsValidIndex(Idx) && CurrentCategory.Categories[Idx].Func)
	{
		(this->*CurrentCategory.Categories[Idx].Func)(CurrentCategory.Categories[Idx]);
		if (CurrentCategory.Categories[Idx].Categories.Num() == 0)
		{
			// Close leaf
			Panel->Close();
		}
	}
	else
	{
		// Invalid index, so close it
		Panel->Close();
	}
}

void AEXPlayerController::Server_Unreliable_Vote_Implementation(bool bVote)
{
	LevelRules->AddVote(MyEXPS, bVote);
}

void AEXPlayerController::VoteYes()
{
	if (!bCanVote)
	{
		return;
	}
	Server_Unreliable_Vote(true);
	OnVote(true);
}

void AEXPlayerController::VoteNo()
{
	if (!bCanVote)
	{
		return;
	}
	Server_Unreliable_Vote(false);
	OnVote(false);
}

void AEXPlayerController::OnVote(bool bVote)
{
	HUDCHECK;
	HUD->Vote(bVote);
	bCanVote = false;
}

#pragma region Panel
void AEXPlayerController::ExpandPanel(FTextPanelCategory& Cat)
{
	Cat.Panel->SetSelectedCategory(Cat);
}

void AEXPlayerController::SayQuickChat(FTextPanelCategory& Cat)
{
	Say(Cat.Name.ToString(), false, true);
}

void AEXPlayerController::SayQuickChatTeam(FTextPanelCategory& Cat)
{
	Say(Cat.Name.ToString(), true, true);
}

void AEXPlayerController::StartVoteKick(FTextPanelCategory& Cat)
{
	Server_Reliable_VoteKickPlayer(GSB->GetPlayer(Cat.Option));
}

void AEXPlayerController::AdminChangeMap(FTextPanelCategory& Cat)
{
	Server_Reliable_AdminChangeLevel(ValidMapNames[Cat.Option]);
}

void AEXPlayerController::AdminStartMatch(FTextPanelCategory& Cat)
{
	Server_Reliable_AdminStartMatch();
}
#pragma endregion Panel

void AEXPlayerController::CreateKickOptions(FTextPanelCategory& Cat)
{
	TArray<FTextPanelCategory> PlayerList;
	TArray<AEXPlayerState*> KickablePlayers = bTeamGame ? MyEXPS->GetTeam()->GetMembers() : GSB->GetEXPlayerArray();
	for (AEXPlayerState* EXPS : KickablePlayers)
	{
		if (EXPS == MyEXPS)
		{
			// Skip self, because no point starting a vote to kick yourself
			continue;
		}
		FTextPanelCategory PlayerLine(&AEXPlayerController::StartVoteKick, Cat.Panel, FText::FromString(EXPS->GetPlayerName()), EXPS->GetPlayerId());
		PlayerList.Add(PlayerLine);
	}
	Cat.Categories = PlayerList;
	Cat.Panel->SetSelectedCategory(Cat);
}

void AEXPlayerController::CreateMapOptions(FTextPanelCategory& Cat)
{
	TArray<FTextPanelCategory> MapList;
	int32 Idx = 0;
	for (const FName& Name : ValidMapNames)
	{
		FTextPanelCategory PlayerLine(&AEXPlayerController::AdminChangeMap, Cat.Panel, FText::FromName(Name), Idx++);
		MapList.Add(PlayerLine);
	}
	Cat.Categories = MapList;
	Cat.Panel->SetSelectedCategory(Cat);
}

void AEXPlayerController::CreateHUD()
{
	ensure(IsLocalController());

	if (IsNetMode(NM_Standalone)) // Playing offline
	{
		if (OfflineHUDClass)
		{
			HUD = CreateWidget<UEXHUDWidget>(this, OfflineHUDClass);
			HUD->AddToViewport();
			HUD->SetOwner(this);
			HUD->InitAssaultCourse();
		}
	}
	else
	{
		if (HUDClass)
		{
			HUD = CreateWidget<UEXHUDWidget>(this, HUDClass);
			HUD->AddToViewport();
			HUD->SetOwner(this); // If PlayerState is not yet replicated, we will repeat the call in OnRep_PlayerState
			if (SelectedMercs.Num() > 0)
			{
				HUD->SetIcons(SelectedMercs);
			}
		}
	}
	QuickChatWindow = HUD->GetQuickChatWindow();
	AdminPanel = HUD->GetAdminPanel();
	CreateQuickChat();
	CreateAdminPanel(false);

	if (InGameMenuClass)
	{
		InGameMenu = CreateWidget<UEXInGameMenu>(this, InGameMenuClass);
		InGameMenu->SetVisibility(ESlateVisibility::Hidden);
		InGameMenu->AddToViewport();
	}

	if (ScoreboardClass)
	{
		Scoreboard = CreateWidget<UEXScoreboardBase>(this, ScoreboardClass);
		Scoreboard->AddToViewport();
		Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	}
	if (EndScoreboardClass)
	{
		RoundEndScreen = CreateWidget<UEXScoreboardBase>(this, EndScoreboardClass);
		RoundEndScreen->AddToViewport();
		RoundEndScreen->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsLocalController())
	{
		UEXGameplayStatics::RefreshTeamColors(this);
	}
}

void AEXPlayerController::CreateQuickChat()
{
	FTextPanelCategory QuickChat1(&AEXPlayerController::ExpandPanel, QuickChatWindow, NSLOCTEXT("Quickchat", "Command", "Command"),
		{
			FTextPanelCategory(&AEXPlayerController::SayQuickChat, QuickChatWindow, NSLOCTEXT("Quickchat", "Yes.", "Yes.")),
			FTextPanelCategory(&AEXPlayerController::SayQuickChat, QuickChatWindow, NSLOCTEXT("Quickchat", "No.", "No.")),
		});
	FTextPanelCategory QuickChat2(&AEXPlayerController::ExpandPanel, QuickChatWindow, NSLOCTEXT("Quickchat", "Team Command", "Team Command"),
		{
			FTextPanelCategory(&AEXPlayerController::SayQuickChatTeam, QuickChatWindow, NSLOCTEXT("Quickchat", "Yes.", "Yes.")),
			FTextPanelCategory(&AEXPlayerController::SayQuickChatTeam, QuickChatWindow, NSLOCTEXT("Quickchat", "No.", "No.")),
		});
	if (bTeamGame)
	{
		QuickChatOptions = FTextPanelCategory(&AEXPlayerController::ExpandPanel, QuickChatWindow, NSLOCTEXT("Quickchat", "Quickchat", "Quickchat"), { QuickChat1, QuickChat2 });
	}
	else
	{
		QuickChatOptions = FTextPanelCategory(&AEXPlayerController::ExpandPanel, QuickChatWindow, NSLOCTEXT("Quickchat", "Quickchat", "Quickchat"), { QuickChat1 });
	}
}

void AEXPlayerController::CreateAdminPanel(bool bInAdmin)
{
	FTextPanelCategory Votes(&AEXPlayerController::ExpandPanel, AdminPanel, NSLOCTEXT("AdminPanel", "Start vote", "Start vote"),
		{
			FTextPanelCategory(&AEXPlayerController::CreateKickOptions, AdminPanel, NSLOCTEXT("AdminPanel", "Kick player", "Kick player")),
			FTextPanelCategory(&AEXPlayerController::ExpandPanel, AdminPanel, NSLOCTEXT("AdminPanel", "Shuffle teams", "Shuffle teams")),
		});
		
	FTextPanelCategory Game(&AEXPlayerController::ExpandPanel, AdminPanel, NSLOCTEXT("AdminPanel", "Admin", "Admin"),
		{
			FTextPanelCategory(&AEXPlayerController::CreateMapOptions, AdminPanel, NSLOCTEXT("AdminPanel", "Change map", "Change map")),
			FTextPanelCategory(&AEXPlayerController::AdminStartMatch, AdminPanel, NSLOCTEXT("AdminPanel", "Start match", "Start match")),
		});
	if (bInAdmin)
	{
		AdminCommands = FTextPanelCategory(&AEXPlayerController::ExpandPanel, AdminPanel, NSLOCTEXT("AdminPanel", "Admin panel", "Admin panel"), { Votes, Game });
	}
	else
	{
		AdminCommands = FTextPanelCategory(&AEXPlayerController::ExpandPanel, AdminPanel, NSLOCTEXT("AdminPanel", "Admin panel", "Admin panel"), { Votes });
	}
}

void AEXPlayerController::ShowObjectives(bool bShow)
{
	TArray<AEXInteract*> Objectives = LevelRules->GetObjectives();
	for (AEXInteract* Objective : Objectives)
	{
		Objective->ChangeIconVisibility(bShow);
	}
}

void AEXPlayerController::HitOnClient(bool bHeadshot)
{
	HUDCHECK;

	if (bHitMarkersEnabled)
	{
		HUD->ShowHitMarker();
	}
	if (bHeadshot)
	{
		PlayHeadshotSound();
	}
}

void AEXPlayerController::ClientWasKicked_Implementation(const FText& KickReason)
{
	// #EXTODO2 implement was kicked notification
}

void AEXPlayerController::Client_Reliable_PauseStatusChanged_Implementation(bool bNewPaused)
{
	PauseStatusChanged(bNewPaused);
}

void AEXPlayerController::PauseStatusChanged(bool bNewPaused)
{
	if (HasAuthority())
	{
		Client_Reliable_PauseStatusChanged(bNewPaused);
	}
	else
	{
		HUDCHECK;

		HUD->SetGamePaused(bNewPaused);
	}
}

void AEXPlayerController::Client_Reliable_SetInteractProgressVisibility_Implementation(bool bVisible)
{
	HUDCHECK;
	HUD->SetInteractProgressVisibility(bVisible);
}

void AEXPlayerController::CharacterRevived()
{
	// Once revived, if the character goes down again, he can't respawn until the next wave
	SetCanRespawn(false);
	HUDCHECK;
	if (HUD)
	{
		HUD->SetDeathScreenVisibility(false);
	}
}

void AEXPlayerController::CharacterDied()
{
	if (PCM)
	{
		PCM->ResetFOV();
	}
	if (HUD)
	{
		HUD->SetDeathScreenVisibility(true);
	}
	if (GMB)
	{
		GMB->CharacterDied(this);
	}
}

void AEXPlayerController::AllowRespawn()
{
	// To catch the spawn wave, the player must be down or gibbed
	const AEXCharacter* ControlledCharacter = Cast<AEXCharacter>(GetPawn());
	if (HasAuthority() && (!ControlledCharacter || ControlledCharacter->IsDead()))
	{
		AEXTeam* Team = MyEXPS ? MyEXPS->GetTeam() : nullptr;
		if (!Team || !Team->IsSpectating())
		{
			SetCanRespawn(true);
		}
	}
}

void AEXPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetHUDWidget())
	{
		// If it's not yet initialized, then this call is done at initialization
		GetHUDWidget()->SetOwner(this);
	}
}

void AEXPlayerController::OnTeamChanged(AEXTeam* Team)
{
	if (IsLocalController())
	{
		if (!HUD)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEXPlayerController::OnTeamChanged, Team));
			return;
		}
		if(Team)
		{
			HUD->SetOwnerInSpectate(Team->IsSpectating());
			HUD->OnTeamChanged(Team);
		}
	}
	UEXGameplayStatics::RefreshTeamColors(this);
}

void AEXPlayerController::UpdateAmmo(const UEXWeapon* Weapon)
{
	HUDCHECK;

	HUD->UpdateAmmo(Weapon);
}

void AEXPlayerController::UpdateInventory(const UEXInventory* Inventory)
{
	HUDCHECK;

	HUD->UpdateInventory(Inventory);
}

void AEXPlayerController::SetScopeVisibility(bool bVisible)
{
	HUDCHECK;

	HUD->SetScopeVisibility(bVisible);
}

void AEXPlayerController::SetCrosshairVisibility(bool bVisible)
{
	HUDCHECK;

	if (bHideCrosshairWhenADS)
	{
		HUD->SetCrosshairVisibility(bVisible);
	}
}

void AEXPlayerController::SetSpread(float Val)
{
	HUDCHECK;

	HUD->SetSpread(Val);
}

void AEXPlayerController::SetSpotted(bool bSpotted)
{
	HUDCHECK;

	HUD->SetSpotted(bSpotted);
}

void AEXPlayerController::MercSelect(int32 Index)
{
	if (!SelectedMercs.IsValidIndex(Index) || !SelectedMercs[Index])
	{
		return;
	}
	SelectedPlayerClass = SelectedMercs[Index].Get();
	Server_Reliable_ChoosePawnClass(SelectedPlayerClass);
	if (HUD)
	{
		HUD->ChangeMercSelection(Index);
	}
}

void AEXPlayerController::OpenChat(bool bTeam)
{
	HUDCHECK;

	if (HUD->GetChatWidget())
	{
		HUD->GetChatWidget()->StartTyping(bTeam);
	}
}

void AEXPlayerController::OnScoreAdded(int32 AddedScore, EScoreType Type)
{
	HUDCHECK;

	HUD->OnScoreAdded(AddedScore, Type);
}

void AEXPlayerController::SetInteractionIconVisibility(bool bVisible)
{
	HUDCHECK;

	if (!HasActorBegunPlay())
	{
		return;
	}
	if (bVisible != bPickUpIconVisible)
	{
		bPickUpIconVisible = bVisible;
		HUD->SetInteractIconVisible(bPickUpIconVisible);
	}
}

void AEXPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	/*AEXGameState* GameState = GetWorld()->GetGameState<AEXGameState>();
	if (GameState != nullptr)
	{
		GameState->SetMatchState(EMatchState::WaitingTravel);
	}*/

	// #EXTODO2 Leave voice chat here

	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
}

void AEXPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	UEXPersistentUser* PU = GetPersistentUser();
	if (PU)
	{
		PU->TellInputAboutKeybindings();
		if (PCM)
		{
			PCM->SetScopedSensitivityScale(PU->GetScopeSensitivityScale());
		}
	}
}

void AEXPlayerController::PostInitProperties()
{
	Super::PostInitProperties();

}

void AEXPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void AEXPlayerController::AddToKillFeed(const FString& DeadPlayer, UTexture2D* KillIcon, const FString& Killer)
{
	HUDCHECK;

	HUD->AddToKillFeed(DeadPlayer, KillIcon, Killer);
}

void AEXPlayerController::RefreshGameStatusWidget(AEXTeam* Attackers, AEXTeam* Defenders)
{
	UEXGameStatus* GameStatusWidget = HUD->GetGameStatus();
	if (GameStatusWidget)
	{
		GameStatusWidget->Refresh(Attackers, Defenders);
	}
}

void AEXPlayerController::Server_Reliable_Respawn_Implementation()
{
	if (CanRespawn())
	{
		AEXCharacter* ControlledCharacter = Cast<AEXCharacter>(GetPawn());
		if (!ControlledCharacter || ControlledCharacter->IsDead())
		{
			if (ControlledCharacter) // This controller is still controlling an ungibbed character
			{
				ControlledCharacter->SelfKill();
			}
			else 
			{ 
				APawn* MyPawn = GetPawn();
				if (MyPawn)
				{ // Currently spectating (without a team)
					MyPawn->Destroy();
				}
			}
			SetCanRespawn(false);

			if (IsNetMode(NM_Standalone))
			{
				if (AEXAssaultCourseGame* ACGM = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>())
				{
					ACGM->RestartPlayer(this);
					ACGM->ResetCourse();
				}

			}
			else
			{
				GMB->RestartPlayer(this);
			}
		}
	}
}

void AEXPlayerController::AssaultCourseReset()
{
	AEXAssaultCourseGame* GM = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
	if (GM)
	{
		GM->Reset();
	}
}

void AEXPlayerController::Client_Reliable_EnableAdmin_Implementation()
{
	bAdmin = true;
	HUDCHECK;
	if (AdminPanel->IsOpen())
	{
		// Because the options can change when the user gets admin rights, we want to make sure the panel resets
		AdminPanel->Close();
	}
	CreateAdminPanel(true);
}

void AEXPlayerController::SetDefaultFOV(float FOV)
{
	if (PCM)
	{
		PCM->SetDefaultFOV(FOV);
		PCM->SetFOV(FOV);
	}
}

void AEXPlayerController::PlayHeadshotSound()
{
	if (HeadshotSound)
	{
		UGameplayStatics::PlaySound2D(this, HeadshotSound, UEXGameplayStatics::GetMasterVolume());
	}
}

void AEXPlayerController::SetFOV(float FOV)
{
	UEXLocalPlayer* LP = Cast<UEXLocalPlayer>(GetLocalPlayer());
	UEXPersistentUser* PU = LP ? LP->GetPersistentUser() : nullptr;
	if (PU)
	{
		PU->SetFOV(FOV);
		PU->SaveIfDirty();
		SetDefaultFOV(FOV);
	}
}

void AEXPlayerController::SwitchTeam()
{
	if (!HasAuthority())
	{
		Server_Reliable_SwitchTeam();
	}
}

void AEXPlayerController::Spectate()
{
	if (!HasAuthority())
	{
		Server_Reliable_Spectate();
	}
}

void AEXPlayerController::Server_Reliable_Spectate_Implementation()
{
	AEXTeam* Team = MyEXPS ? MyEXPS->GetTeam() : nullptr;
	const bool bSpectating = Team && Team->IsSpectating();
	if (!bSpectating)
	{
		GMB->AddToSpectators(this);
	}
}

void AEXPlayerController::SetSensitivity(float Value)
{
	UEXLocalPlayer* LP = Cast<UEXLocalPlayer>(GetLocalPlayer());
	UEXPersistentUser* PU = LP ? LP->GetPersistentUser() : nullptr;
	if (PU)
	{
		PU->SetAimSensitivity(Value);
		PU->SaveIfDirty();
		PU->TellInputAboutKeybindings();
	}
}

void AEXPlayerController::SetAltInputs()
{
	UEXLocalPlayer* LP = Cast<UEXLocalPlayer>(GetLocalPlayer());
	UEXPersistentUser* PU = LP ? LP->GetPersistentUser() : nullptr;
	if (PU)
	{
		UEXInputInfo* InputInfo = UEXGameplayStatics::GetInputInfoCDO();
		PU->SetChangedInputs(InputInfo->AltInputs);
		PU->SaveIfDirty();
		PU->TellInputAboutKeybindings();
	}
}

void AEXPlayerController::SetMasterVolume(float Value)
{
	PersistentUser->SetMasterVolume(Value);
	PersistentUser->SaveIfDirty();
}

void AEXPlayerController::Server_Reliable_SwitchTeam_Implementation()
{
	if (!GMB->IsTeamSwitchAllowed())
	{
		UE_LOG(LogEXController, Warning, TEXT("Trying to switch teams when its not allowed"));
		return;
	}
	GMB->SwitchTeam(this);
}

bool AEXPlayerController::Server_Reliable_SwitchTeam_Validate()
{
	return true;
}

void AEXPlayerController::Disconnect()
{
	if (IsLocalController())
	{
		UEXGameInstance* GameInstance = GetGameInstance<UEXGameInstance>();
		GameInstance->ReturnToMainMenu();
	}
	else
	{
		ClientReturnToMainMenuWithTextReason(NSLOCTEXT("Session", "Disconnected", "Disconnected"));
	}
	OnDisconnect();
}

#pragma region Admin

void AEXPlayerController::AdminLogin(const FString& Password)
{
	if (!HasAuthority())
	{
		Server_Reliable_AdminLogin(Password);
	}
}

void AEXPlayerController::Server_Reliable_AdminLogin_Implementation(const FString& Password)
{
	if (GMB)
	{
		bool bSuccess = GMB->RequestAdmin(this, Password);
	}
}

void AEXPlayerController::AdminSetRoundTime(const uint32 Time)
{
	if (!HasAuthority())
	{
		Server_Reliable_AdminSetRoundTime(Time);
	}
}

void AEXPlayerController::Server_Reliable_AdminSetRoundTime_Implementation(const uint32 Time)
{
	ADMINCHECK("SetRoundTime");

	if ((Time > 0) && (Time < 3600))
	{
		GMB->AdminSetTime(Time);
	}

}

void AEXPlayerController::AdminEndState()
{
	if (!HasAuthority())
	{
		Server_Reliable_AdminEndState();
	}
}

void AEXPlayerController::AdminSwapTeams()
{
	if (!HasAuthority())
	{
		Server_Reliable_AdminSwapTeams();
	}
}

void AEXPlayerController::AdminRestartLevel()
{
if (!HasAuthority())
{
	Server_Reliable_AdminRestartLevel();
}
}

void AEXPlayerController::AdminRestartRound()
{
	if (!HasAuthority())
	{
		Server_Reliable_AdminRestartRound();
	}
}

void AEXPlayerController::AdminSetNumPlayersRequiredToStart(uint8 NumPlayers)
{
	if (!HasAuthority())
	{
		Server_Reliable_AdminSetNumPlayersRequiredToStart(NumPlayers);
	}
}

void AEXPlayerController::Server_Reliable_AdminRestartLevel_Implementation()
{
	ADMINCHECK("AdminRestartLevel");

	if (GMB)
	{
		GMB->RestartGame();
	}
}

void AEXPlayerController::Server_Reliable_AdminRestartRound_Implementation()
{
	ADMINCHECK("AdminRestartRound");

	if (GMB)
	{
		GMB->RestartRound();
	}
}

void AEXPlayerController::Server_Reliable_AdminChangeLevel_Implementation(const FName& MapName)
{
	ADMINCHECK("AdminChangeLevel");

	if (ValidMapNames.Contains(MapName))
	{
		UGameplayStatics::OpenLevel(this, MapName);
	}
	else
	{
		ClientReceiveLocalizedMessage(UEXAdminMessage::StaticClass(), 3);
	}
}

void AEXPlayerController::Server_Reliable_AdminSwapTeams_Implementation()
{
	ADMINCHECK("AdminSwapTeams");

	if (GMB)
	{
		GMB->SwapTeams();
	}
}

void AEXPlayerController::Server_Reliable_AdminStartMatch_Implementation()
{
	ADMINCHECK("AdminStartMatch");

	GMB->ForceStartMatch();
}

void AEXPlayerController::Server_Reliable_AdminEndState_Implementation()
{
	ADMINCHECK("AdminEndState");

	GMB->EndRoundState();
}

void AEXPlayerController::Server_Reliable_AdminSetNumPlayersRequiredToStart_Implementation(uint8 NumPlayers)
{
	ADMINCHECK("ChangeNumPlayersRequiredToStart");

	GMB->SetNumPlayersRequiredToStart(NumPlayers);
}

void AEXPlayerController::AdminExecuteCommand()
{
	if (HasAuthority())
	{
		
	}
	else
	{
		Server_Reliable_AdminExecuteCommand();
	}
}

void AEXPlayerController::Server_Reliable_AdminExecuteCommand_Implementation()
{
	AdminExecuteCommand();
}

void AEXPlayerController::VoteKickPlayer(AEXPlayerState* Target)
{
	Server_Reliable_VoteKickPlayer(Target);
}

void AEXPlayerController::Server_Reliable_VoteKickPlayer_Implementation(AEXPlayerState* Target)
{
	LevelRules->VoteKick(MyEXPS, Target);
}

void AEXPlayerController::HideHUD()
{
	HUD->SetVisibility(ESlateVisibility::Hidden);
}

void AEXPlayerController::ShowHUD()
{
	HUD->SetVisibility(ESlateVisibility::HitTestInvisible);
}

#pragma endregion Admin

#pragma region TestCommands

void AEXPlayerController::Test()
{
	//UEXGameplayStatics::RefreshTeamColors(this, true);
	HUD->OnScoreAdded(FMath::Rand() % 200, EScoreType::Combat);
}

void AEXPlayerController::TestServer()
{
	Server_Reliable_Test();
}

void AEXPlayerController::Server_Reliable_Test_Implementation()
{
	//EXCharacter->Revive(30, this, EMerc::Aura);

	/*TArray<AEXPlayerState*> PSS;
	UEXGameplayStatics::GetAllActorsOfClass<AEXPlayerState>(this, AEXPlayerState::StaticClass(), PSS);
	for (AEXPlayerState* PS : PSS)
	{
		AEXTeam* Team = PS->GetTeam();
		if (Team->IsAttacking())
		{
			UE_LOG(LogTemp, Error, TEXT("%s att"), *PS->GetPlayerName());
		}
		else if (Team->IsDefending())
		{
			UE_LOG(LogTemp, Error, TEXT("%s def"), *PS->GetPlayerName());
		}
		else if (Team->IsSpectating())
		{
			UE_LOG(LogTemp, Error, TEXT("%s spec"), *PS->GetPlayerName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s none"), *PS->GetPlayerName());
		}
	}*/
}

void AEXPlayerController::TestShowPlayerStats()
{
	Server_Reliable_RequestPlayerStats();
}

void AEXPlayerController::Give()
{
	Server_Reliable_Give();
}

void AEXPlayerController::SendData()
{
	Server_Reliable_SendData();
}

void AEXPlayerController::LogAdminCommand(FString LogText)
{
	const FDateTime Now = FDateTime::Now();
	UE_LOG(LogEXAdmin, Log, TEXT("%s: %s [%s]"), *Now.ToString(), *GetName(), *LogText);
}

void AEXPlayerController::Server_Reliable_SendData_Implementation()
{
	GSB->SendGameResult();
}

void AEXPlayerController::AddBots(int32 Count)
{
	Server_Reliable_AddBots(Count);
}

void AEXPlayerController::Server_Reliable_AddBots_Implementation(int32 Count)
{
	GMB->AddBots(Count);
}

void AEXPlayerController::TestTeamHUD()
{
	UEXGameplayStatics::RefreshTeamColors(this);
}

void AEXPlayerController::Server_Reliable_Give_Implementation()
{
	AEXCharacter* ControlledCharacter = Cast<AEXCharacter>(GetPawn());
	UEXInventoryComponent* InventoryComp = ControlledCharacter->GetInventoryComponent();
	for (UEXInventory* Inventory : InventoryComp->GetInventoryList())
	{
		if (Inventory->UsesAbility())
		{
			Inventory->AddUses(Inventory->GetMaxUses());
		}
	}
}

void AEXPlayerController::Server_Reliable_RequestPlayerStats_Implementation()
{
	TArray<FPlayerStats> PlayerStats;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AEXPlayerController* PC = Cast<AEXPlayerController>(*It);
		AEXPlayerState* PS = PC ? PC->GetPlayerState<AEXPlayerState>() : nullptr;
		if (PS)
		{
			FPlayerStats Stats = PS->GetStats();
			PlayerStats.Add(Stats);
		}
	}
	Client_Reliable_DisplayPlayerStats(PlayerStats);
}

void AEXPlayerController::Client_Reliable_DisplayPlayerStats_Implementation(const TArray<FPlayerStats>& Stats)
{
	for (const FPlayerStats& PlayerStats : Stats)
	{
		//ClientMessage(PlayerStats.ToString());
	}
}

#pragma endregion TestCommands

void AEXPlayerController::OverridePlayerState(AEXPlayerState* InPS)
{
	MyEXPS = InPS;
}

void AEXPlayerController::Client_Reliable_SetTimer_Implementation(float WorldTime, int32 InRoundSeconds, int32 InSpawnSeconds, int32 InSpawnWave)
{
	if (!GSB)
	{
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AEXPlayerController::Client_Reliable_SetTimer_Implementation,
			WorldTime, InRoundSeconds, InSpawnSeconds, InSpawnWave);
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
		return;
	}
	const float LocalWorldTime = GSB->GetServerWorldTimeSeconds();
	const float Diff = LocalWorldTime - WorldTime;

	float IntPart;
	float FloatPart = FMath::Modf(Diff, &IntPart);
	int32 IntPart2 = FMath::RoundToInt(IntPart);

	RoundSeconds = InRoundSeconds - IntPart2;
	SpawnSeconds = InSpawnSeconds - IntPart2;
	if (SpawnSeconds < 0)
	{
		SpawnSeconds += SpawnWave;
	}
	SpawnWave = InSpawnWave;

	UpdateTimers(RoundSeconds, SpawnSeconds);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Second, this, &AEXPlayerController::Second, 1.f, true, 1 - FloatPart);
}

void AEXPlayerController::Second()
{
	--RoundSeconds;
	if (SpawnWave > 0)
	{
		--SpawnSeconds;
		if (SpawnSeconds < 0)
		{
			SpawnSeconds += SpawnWave;
		}
	}
	if (RoundSeconds >= 0) 
	{
		UpdateTimers(RoundSeconds, SpawnSeconds);
	}
}

void AEXPlayerController::Report(const AEXPlayerState* PS)
{
	if (PS != MyEXPS)
	{
		//ReportWindow->Show(PS);
	}
}

void AEXPlayerController::Server_Reliable_Report_Implementation(const AEXPlayerState* PS, EReportReason Reason, const FString& Message)
{

}

void AEXPlayerController::UpdateTimers(int32 InRoundSeconds, int32 InSpawnSeconds)
{
	HUDCHECK;

	HUD->MatchStateTimeUpdate(InSpawnSeconds, InRoundSeconds);
}

void AEXPlayerController::Client_Reliable_OnMatchStateChanged_Implementation(const FName& NewMatchState)
{
	HUDCHECK;

	HUD->OnMatchStateChanged(NewMatchState);
}

void AEXPlayerController::Client_Reliable_SetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}

void AEXPlayerController::Client_Reliable_StartOnlineGame_Implementation()
{
	if (!MyEXPS)
	{	
		// Keep retrying until player state is replicated
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &AEXPlayerController::Client_Reliable_StartOnlineGame_Implementation, 0.2f, false);
		return;
	}
	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && Sessions->GetNamedSession(MyEXPS->SessionName))
		{
			UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *MyEXPS->SessionName.ToString());
			Sessions->StartSession(MyEXPS->SessionName);
		}
	}

}

void AEXPlayerController::Client_Reliable_EndOnlineGame_Implementation()
{
	if (MyEXPS)
	{
		IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && Sessions->GetNamedSession(MyEXPS->SessionName))
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *MyEXPS->SessionName.ToString());
				Sessions->EndSession(MyEXPS->SessionName);
			}
		}
	}
}

