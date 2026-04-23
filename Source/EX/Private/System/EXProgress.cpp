// Fill out your copyright notice in the Description page of Project Settings.

#include "System/EXProgress.h"
#include "Net/UnrealNetwork.h"
#include "EX.h"
#include "Online/EXLevelRules.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXCharacter.h"
#include "UObject/UObjectBaseUtility.h"
#include "Online/EXPlayerState.h"
#include "Player/EXPlayerController.h"
#include "HUD/EXHUDWidget.h"
#include "GameFramework/GameStateBase.h"
#include "System/EXInteract.h"
#include "System/EXGameplayStatics.h"
#include "Net/Core/PushModel/PushModel.h"
#include "EXNetDefines.h"


UEXProgress::UEXProgress()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicatedByDefault(true);
}

void UEXProgress::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(UEXProgress, Goal, PushReplicationParams::Default);
	DOREPLIFETIME_WITH_PARAMS_FAST(UEXProgress, ReplicatedProgress, PushReplicationParams::Default);
	DOREPLIFETIME_WITH_PARAMS_FAST(UEXProgress, Completions, PushReplicationParams::Default);
}

void UEXProgress::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.SetTickFunctionEnable(false);

	Objective = Cast<AEXInteract>(GetOwner());
	if (Objective)
	{
		Stage = Objective->GetStage();
	}
	LevelRules = UEXGameplayStatics::GetLevelRules(this);
}

void UEXProgress::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bProgressing && (ProgressModifier > 0))
	{
		AddProgress(Player, DeltaTime);
	}
}

void UEXProgress::ResetProgress()
{
	SetProgress(0);
	SetCompletions(0);
	OnReset.Broadcast();
	UpdateHUD();
}

void UEXProgress::StartProgress(AEXPlayerController* InPlayer, float InProgressModifier /*= 1.f*/)
{
	ProgressModifier = InProgressModifier;
	Player = InPlayer;
	bProgressing = true;
	if (TotalExpForCompletion > 0.f)
	{
		CurrentRationStartedOn = GetRatio();
	}
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UEXProgress::StopProgress(AEXPlayerController* InPlayer)
{
	if (!bProgressing) // If it's not progressing the progress has already been stopped and that's what triggered the StopInteract which called this again
	{
		return;
	}
	PrimaryComponentTick.SetTickFunctionEnable(false);
	bProgressing = false;
	if (bResetOnStop && Progress < Goal)
	{
		ResetProgress();
	}
	if (GetOwner()->HasAuthority())
	{
		if (TotalExpForCompletion > 0.f)
		{
			const float DeltaProgress = GetRatio() - CurrentRationStartedOn;
			AEXPlayerState* PS = InPlayer->GetPlayerState<AEXPlayerState>();
			PS->AddScore(DeltaProgress * TotalExpForCompletion, EScoreType::Objective, true);
		}
	}
}

void UEXProgress::Complete(AEXPlayerController* InPlayer)
{
	if (bProgressing)
	{
		StopProgress(InPlayer);
	}
	SetProgress(0.f);
	SetCompletions(Completions + 1);

	if (GetOwner()->HasAuthority())
	{
		OnCompleted.Broadcast(InPlayer);
	}
}

void UEXProgress::SetGoal(float InGoal)
{
	Goal = InGoal;
	MARK_PROPERTY_DIRTY_FROM_NAME(UEXProgress, Goal, this);
}

void UEXProgress::SetCompletions(int32 InCompletions)
{
	Completions = InCompletions;
	MARK_PROPERTY_DIRTY_FROM_NAME(UEXProgress, Completions, this);
}

void UEXProgress::UpdateHUD()
{
	if (EXCharacterOwner && (EXCharacterOwner->IsLocallyControlled()))
	{
		// Check the stage to prevent objectives that were already passed from changing the progress on HUD
		if ((Stage == 0) || (LevelRules->GetStage() == Stage)) 
		{
			EXCharacterOwner->GetEXController()->GetHUDWidget()->SetInteractProgress(Progress / Goal);
		}
	}
}

void UEXProgress::AddProgress(AEXPlayerController* InPlayer, float DeltaProgress)
{
	SetProgress(Progress + DeltaProgress * ProgressModifier);

	if (Progress >= Goal)
	{
		Complete(InPlayer);
	}
	UpdateHUD();
}

void UEXProgress::SetProgress(float InProgress)
{
	const int32 OldInt = FMath::FloorToInt(Progress);
	Progress = InProgress;
	const int32 NewInt = FMath::FloorToInt(Progress);
	if (NewInt != OldInt)
	{
		ProgressUpdate.Broadcast(Progress);
	}

	uint16 NewReplicatedProgress = FMath::Floor(Progress * 10); // #EXTODO
	if (ReplicatedProgress != NewReplicatedProgress)
	{
		ReplicatedProgress = NewReplicatedProgress;
		MARK_PROPERTY_DIRTY_FROM_NAME(UEXProgress, ReplicatedProgress, this);
	}
}

void UEXProgress::OnRep_Progress()
{
	Progress = ((float)ReplicatedProgress) / 10;
	ProgressUpdate.Broadcast(Progress);

	UpdateHUD();
}

void UEXProgress::OnRep_Goal()
{
	UpdateHUD();
}

void UEXProgress::OnRep_Completions()
{
	if (Objective)
	{
		Objective->SetCompletions(this, Completions);
	}
}

void UEXProgress::AddProgressInstant(AEXPlayerController* InPlayer, float DeltaProgress)
{
	check(GetOwner()->HasAuthority());
	SetProgress(Progress + DeltaProgress * ProgressModifier);
	if (Progress >= Goal)
	{
		Complete(InPlayer);
	}

	if ((TotalExpForCompletion > 0) && InPlayer)
	{
		AEXPlayerState* PS = InPlayer->GetPlayerState<AEXPlayerState>();
		PS->AddScore((DeltaProgress / Goal) * TotalExpForCompletion, ScoreType);
	}
	UpdateHUD();
}


