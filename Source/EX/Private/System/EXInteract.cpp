// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EXInteract.h"
#include "System/EXProgress.h"
#include "Player/EXCharacter.h"
#include "Online/EXGameModeSW.h"
#include "Online/EXTeam.h"
#include "Online/EXGameStateBase.h"
#include "Online/EXGameModeSW.h"
#include "Player/EXPlayerController.h"
#include "HUD/EXHUDWidget.h"
#include "HUD/EXPrimaryObjProgress.h"
#include "System/EXGameplayStatics.h"
#include "Online/EXLevelRules.h"

AEXInteract::AEXInteract()
{
	bReplicates = false; // Not all AEXInteract classes need to replicate, but if they do, set it in the child class
	bAlwaysRelevant = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

bool AEXInteract::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (bPrimary && ProgressComp)
	{
		if (ProgressComp->GetRepeats() == ProgressComp->GetCompletions())
		{
			return false;
		}
	}
	return (!bNeedEyeContact || bLookAt) && !IsSomeoneFasterInteracting(Player) && (State == EInteractableState::Active) && GS->IsMatchInProgress();
}

bool AEXInteract::StartInteract_Implementation(AEXCharacter* Player)
{
	if(HasAuthority())
	{
		if (bOneInteractAtOnce && InteractingPlayer)
		{
			InteractingPlayer->StopInteract(this, true);
		}
	}
	InteractingPlayer = Player;
	if (bNeedEyeContact && InteractingPlayer && InteractingPlayer->IsLocallyControlled())
	{
		PrimaryActorTick.SetTickFunctionEnable(true);
	}
	return true;
}

bool AEXInteract::StopInteract_Implementation(AEXCharacter* Player)
{
	InteractingPlayer = nullptr;
	if (bNeedEyeContact && InteractingPlayer && InteractingPlayer->IsLocallyControlled())
	{
		PrimaryActorTick.SetTickFunctionEnable(false);
	}
	return true;
}

void AEXInteract::Reset()
{
	Super::Reset();
	State = EInteractableState::NotReached;
}

void AEXInteract::SetProgressComp(UEXProgress* InProgressComp)
{
	ClearProgressComp();
	ProgressComp = InProgressComp;
	DelegateHandle_OnUpdate = ProgressComp->ProgressUpdate.AddUObject(this, &AEXInteract::ProgressUpdated);
	DelegateHandle_OnCompleted = ProgressComp->OnCompleted.AddUObject(this, &AEXInteract::OnCompleted);
	DelegateHandle_OnReset = ProgressComp->OnReset.AddUObject(this, &AEXInteract::OnProgressReset);
}

void AEXInteract::ClearProgressComp()
{
	if (ProgressComp)
	{
		ProgressComp->ProgressUpdate.Remove(DelegateHandle_OnUpdate);
		ProgressComp->OnCompleted.Remove(DelegateHandle_OnCompleted);
		ProgressComp->OnReset.Remove(DelegateHandle_OnReset);
	}
	ProgressComp = nullptr;
}

void AEXInteract::OnActivated()
{
}

void AEXInteract::ProgressUpdated(float Value)
{
	if (IsPrimary() && EXController && EXController->IsLocalController() && EXController->GetHUDWidget() && (LevelRules->GetStage() == Stage))
	{
		// Update progress on HUD
		EXController->GetHUDWidget()->GetPrimaryObjProgressWidget()->SetProgress(ObjectiveIdx, NumCompletions, Value / ProgressComp->GetGoal());
	}
}

int32 AEXInteract::GetRepeats() const
{
	return ProgressComp ? ProgressComp->GetRepeats() : -1;
}

void AEXInteract::SetCompletions(UEXProgress* InProgress, int32 Completions)
{
	if (InProgress != GetProgressComp())
	{
		// Non primary progress comp, so ignore it
		return;
	}
	NumCompletions = Completions;

	if (IsPrimary() && EXController && EXController->IsLocalController() && EXController->GetHUDWidget() && (LevelRules->GetStage() == Stage))
	{
		// Update progress on HUD
		EXController->GetHUDWidget()->GetPrimaryObjProgressWidget()->SetRepeatsCompleted(ObjectiveIdx, Completions);
	}
}

void AEXInteract::OnProgressReset()
{
}

void AEXInteract::OnCompleted(AEXPlayerController* Player)
{
	if (!ProgressComp)
	{
		return;
	}
	LevelRules->ObjectiveCompleted(this);
}

AEXCharacter* AEXInteract::GetInteractingPlayer() const
{
	return InteractingPlayer;
}

bool AEXInteract::IsSomeoneFasterInteracting(const AEXCharacter* Player) const
{
	if (!InteractingPlayer) // Nobody currently repairing
	{
		return false;
	}

	return Player->BetterEngiThan(InteractingPlayer, RequiredTool);
}

void AEXInteract::BeginPlay()
{
	Super::BeginPlay();

	// Note: do not call Reset() here, because some actors (eg c4) get destroyed in reset
	// Instead we call reset from LevelRules when we start a round

	if (HasAuthority())
	{
		GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
		check(GM);
	}
	GS = GetWorld()->GetGameState<AEXGameStateBase>();

	if (!IsNetMode(NM_DedicatedServer))
	{
		EXController = Cast<AEXPlayerController>(GetWorld()->GetFirstPlayerController());
	}

	LevelRules = UEXGameplayStatics::GetLevelRules(this);
}

void AEXInteract::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bNeedEyeContact)
	{
		if (InteractingPlayer && InteractingPlayer->IsLocallyControlled())
		{
			// #POTENTIALCHEAT this is client side, but server should maybe check it too
			AActor* Target = UEXGameplayStatics::GetActorLookingAt(InteractingPlayer, InteractingPlayer->GetMaxTraceRange());
			if (this != Target)
			{
				InteractingPlayer->StopInteract(this, false);
			}
		}
	}
}

void AEXInteract::SetObjectiveActive(bool bInActive)
{
	// Only primary objectives can put the game to overtime
	if (!IsPrimary() || (bObjectiveActive == bInActive))
	{
		return;
	}
	bObjectiveActive = bInActive;
	if (IsPrimary())
	{
		PrimaryObjectiveActiveChanged.Execute(bObjectiveActive);
	}
}

void AEXInteract::LevelStageChanged(int32 LevelStage)
{
	if ((Stage == 0) && (LevelStage == 1))
	{
		SetActive();
	}
	else if (LevelStage > Stage)
	{
		Passed();
	}
	else if (LevelStage == Stage)
	{
		SetActive();
	}
}

void AEXInteract::Passed()
{
	State = EInteractableState::Passed;
}

void AEXInteract::SetActive()
{
	State = EInteractableState::Active;
}

void AEXInteract::StopInteraction()
{
	if (InteractingPlayer)
	{
		InteractingPlayer->StopInteract(this, HasAuthority());
	}
}
