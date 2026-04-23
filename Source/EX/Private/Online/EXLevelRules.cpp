// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/EXLevelRules.h"
#include "EngineUtils.h"
#include "Online/EXTeam.h"
#include "Online/EXPlayerState.h"
#include "Player/EXCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "System/EXGameplayStatics.h"
#include "System/EXInteractable.h"
#include "Player/EXPlayerController.h"
#include "Online/EXGameModeSW.h"
#include "HUD/EXHUDWidget.h"
#include "Online/EXGameStateSW.h"
#include "Misc/EXAbility.h"
#include "EXTypes.h"
#include "System/EXInteract.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/EXDamageType.h"
#include "Online/EXGameInstance.h"
#include "Inventory/EXInventoryComponent.h"
#include "Misc/EXC4.h"
#include "System/EXProgress.h"
#include "HUD/EXPrimaryObjProgress.h"
#include "Online/EXGameModeBase.h"
#include "Online/EXGameSession.h"
#include "EXNetDefines.h"

#define LOCTEXT_NAMESPACE "EXLevelRules"

AEXLevelRules::AEXLevelRules()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AEXLevelRules::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(AEXLevelRules, Stage, PushReplicationParams::Default);
	DOREPLIFETIME_WITH_PARAMS_FAST(AEXLevelRules, CurrentVote, PushReplicationParams::Default);
}

void AEXLevelRules::BeginPlay()
{
	UEXGameplayStatics::SetLevelRules(this);
	if (!HasAuthority())
	{
		InitInteractable();
	}
	Super::BeginPlay();

	for (AEXInteract* Objective : Objectives)
	{
		NumStages = FMath::Max(NumStages, Objective->GetStage());

		Objective->GetObjectiveActiveChangedDelegate().BindUObject(this, &AEXLevelRules::PrimaryObjectiveActiveChanged);
	}

	if (HasAuthority())
	{
		GMB = Cast<AEXGameModeBase>(GetWorld()->GetAuthGameMode());
	}
	else
	{
		EXController = GetWorld()->GetFirstPlayerController<AEXPlayerController>();
	}

}

void AEXLevelRules::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UEXGameplayStatics::SetLevelRules(nullptr);
}

void AEXLevelRules::InitInteractable()
{
	// Check for all interactable actors that are placed in the level and add them to the array
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UEXInteractable::StaticClass(), InteractableActors);
	UEXGameplayStatics::GetAllActorsOfClass<AEXInteract>(GetWorld(), AEXInteract::StaticClass(), Objectives);

}

void AEXLevelRules::AddInteractable(AActor* Ability)
{
	InteractableActors.Add(Ability);
}

void AEXLevelRules::RemoveInteractable(AActor* Ability)
{
	InteractableActors.Remove(Ability);
}

void AEXLevelRules::PrimaryObjectiveActiveChanged(bool bActive)
{
	if (bActive)
	{
		NumActivePrimaryObjectives++;
	}
	else
	{
		NumActivePrimaryObjectives--;
	}
	if (GMB->GetMatchState() == MatchState::Overtime)
	{
		if (!OvertimeCheck())
		{
			GMB->EndRoundState();
		}
	}
}

void AEXLevelRules::OnRep_Stage()
{
	OnStageChanged();
}

void AEXLevelRules::VoteKick(AEXPlayerState* VoteInstigator, AEXPlayerState* Target)
{
	if (!CanVoteKick(VoteInstigator, Target))
	{
		return;
	}

	StartVote(EVote::Kick, VoteInstigator, Target->GetPlayerId());
}

int32 AEXLevelRules::GetNumVoters(EVote VoteType, AEXTeam* Team) const
{
	switch (VoteType)
	{
	case EVote::Kick:
		return Team ? Team->GetMembers().Num() : GSB->GetEXPlayerArray().Num();
	case EVote::Shuffle:
		return Team ? Team->GetMembers().Num() : -1;
	case EVote::Surrender:
		return Team ? Team->GetMembers().Num() : -1;
	case EVote::None:
	default:
		return -1;
	}
}

bool AEXLevelRules::CanVoteKick(AEXPlayerState* VoteInstigator, AEXPlayerState* Target) const
{
	if (!VoteInstigator || !Target)
	{
		return false;
	}
	if (CurrentVote.IsValid())
	{
		return false;
	}
	return true;
}

void AEXLevelRules::StartVote(EVote VoteType, AEXPlayerState* VoteInstigator, int32 Option)
{
	ensure(HasAuthority());
	const int32 NumVoters = GetNumVoters(VoteType, VoteInstigator->GetTeam());
	CurrentVote = FVoteInfo(VoteType, NumVoters, Option, CurrentVote.Idx + 1, VoteInstigator);
	CurrentVote.AddVote(VoteInstigator, true);
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXLevelRules, CurrentVote, this);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Vote, FTimerDelegate::CreateUObject(this, &AEXLevelRules::EndVote), CurrentVote.VoteDuration, false);
}

void AEXLevelRules::AddVote(AEXPlayerState* EXPS, uint8 Option)
{
	ensure(HasAuthority());
	if (!CurrentVote.IsValid())
	{
		return;
	}
	if (!CurrentVote.AddVote(EXPS, Option == 1))
	{
		return;
	}
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXLevelRules, CurrentVote, this);
	if (CurrentVote.Succeded() || CurrentVote.Failed())
	{
		EndVote();
	}
}

void AEXLevelRules::EndVote()
{
	ensure(HasAuthority());
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Vote); // In case the vote ends early, we don't want to call EndVote() twice

	if (CurrentVote.Succeded())
	{
		switch (CurrentVote.VoteType)
		{
		case EVote::Kick:
			GMB->GetGameSession()->KickPlayer(GSB->GetPlayer(CurrentVote.Option)->GetPC(), NSLOCTEXT("LevelRules", "Vote kick", "Vote kick"));
			break;
		case EVote::Shuffle:
			break;
		case EVote::Surrender:
			break;
		case EVote::None:
		default:
			ensure(0 && "Vote ended with a bad type.");
			break;
		}
	}
	CurrentVote.End();
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXLevelRules, CurrentVote, this);
}

void AEXLevelRules::OnRep_CurrentVote(const FVoteInfo& PreviousVote)
{
	if (CurrentVote.bActive && (PreviousVote.Idx != CurrentVote.Idx))
	{
		EXController->StartVote(CurrentVote);
	}
	else if (!CurrentVote.bActive)
	{
		EXController->EndVote();
	}
	else
	{
		EXController->UpdateVote(CurrentVote.NumYesVotes, CurrentVote.NumNoVotes);
	}
}

bool AEXLevelRules::OvertimeCheck() const
{
	return (NumActivePrimaryObjectives > 0);
}

void AEXLevelRules::ResetStage()
{
	Stage = 0;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXLevelRules, Stage, this);

	for (AActor* InteractableActor : InteractableActors)
	{
		InteractableActor->Reset();
	}

	OnStageChanged();
}

void AEXLevelRules::AdvanceStage()
{
	Stage++;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXLevelRules, Stage, this);

	OnStageChanged();

	if (HasAuthority())
	{
		UE_LOG(LogEXLevel, Warning, TEXT("Stage advanced to %d"), Stage);
	}
}

void AEXLevelRules::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GMB = Cast<AEXGameModeBase>(GetWorld()->GetAuthGameMode());
}

void AEXLevelRules::Reset()
{
	Super::Reset();
}

void AEXLevelRules::OnStageChanged()
{
	int32 NumActiveObjectives = 0;
	TArray<bool> CanBeDefused;
	for (AEXInteract* Objective : Objectives)
	{
		Objective->LevelStageChanged(Stage);
		if (Objective->IsPrimary() && Objective->IsObjectiveActive())
		{
			Objective->SetObjectiveIdx(NumActiveObjectives);
			for (int32 Idx = 0; Idx < Objective->GetRepeats(); Idx++)
			{
				CanBeDefused.Add(Objective->CanBeDefused());
			}
			NumActiveObjectives += Objective->GetRepeats();
		}
	}
	if (HasAuthority())
	{
		if (Stage > NumStages)
		{
			GMB->EndRoundState();
		}
		else
		{
			GMB->AdvanceStage(Stage);
		}
	}
	if (!IsNetMode(NM_DedicatedServer))
	{
		UpdatePrimaryObjectiveOnHUD(NumActiveObjectives, CanBeDefused);
	}
}

void AEXLevelRules::UpdatePrimaryObjectiveOnHUD(int32 NumActiveObjectives, TArray<bool> CanBeDefused)
{
	if (!EXController)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEXLevelRules::UpdatePrimaryObjectiveOnHUD, NumActiveObjectives, CanBeDefused));
		return;
	}
	EXController->GetHUDWidget()->GetPrimaryObjProgressWidget()->SetPrimaryObjectiveInfo(NumActiveObjectives, CanBeDefused);
}

void AEXLevelRules::Multicast_Reliable_OnStageChanged_Implementation(int32 InStage)
{
	Stage = InStage;
	if (HasAuthority())
	{
		return;
	}
	if (!EXController || !EXController->GetHUDWidget())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEXLevelRules::Multicast_Reliable_OnStageChanged_Implementation, Stage));
		return;
	}
	if (Stage > 0)
	{
		OnStageChanged();
	}
}

void AEXLevelRules::ObjectiveCompleted(AEXInteract* Objective)
{
	if (HasAuthority())
	{
		bool bAllCompleted = true;
		for (const AEXInteract* Obj : Objectives)
		{
			if (Obj->IsObjectiveActive() && Obj->IsPrimary())
			{
				const int32 CompletionsRemaining = Obj->GetProgressComp()->GetRepeats() - Obj->GetProgressComp()->GetCompletions();
				if (CompletionsRemaining > 0)
				{
					bAllCompleted = false;
					break;
				}
			}
		}
		if (bAllCompleted)
		{
			GSB->AddObjectiveTime(Objective->GetStage());
			AdvanceStage();
		}
	}
}
#undef LOCTEXT_NAMESPACE
