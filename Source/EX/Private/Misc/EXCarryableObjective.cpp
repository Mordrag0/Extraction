// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXCarryableObjective.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Inventory/EXInventory.h"
#include "Inventory/EXMilkJug.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"
#include "DrawDebugHelpers.h"
#include "Online/EXTeam.h"
#include "Online/EXPlayerState.h"
#include "System/EXProgress.h"
#include "HUD/EXProgressWidget.h"
#include "Online/EXLevelRules.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Online/EXGameModeSW.h"
#include "Online/EXObjectiveMessage.h"
#include "Net/UnrealNetwork.h"
#include "EXNetDefines.h"

#define LOCTEXT_NAMESPACE "EXCarryableObjective"


AEXCarryableObjective::AEXCarryableObjective()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	TimerWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("TimerProgressbar"));
	TimerWidgetComp->SetupAttachment(Root);
	TimerWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	TimerComp = CreateDefaultSubobject<UEXProgress>("TimerComp");
	TimerComp->SetResetOnStop(false);

	ReturnWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ReturnProgressbar"));
	ReturnWidgetComp->SetupAttachment(Root);
	ReturnWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	ReturnComp = CreateDefaultSubobject<UEXProgress>("ReturnComp");

	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(false);

	SetCanBeDamaged(false);
}

void AEXCarryableObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(AEXCarryableObjective, bEnabled, PushReplicationParams::Default);
	DOREPLIFETIME_WITH_PARAMS_FAST(AEXCarryableObjective, Loc, PushReplicationParams::Default);
	DOREPLIFETIME_WITH_PARAMS_FAST(AEXCarryableObjective, Rot, PushReplicationParams::Default);
}

void AEXCarryableObjective::BeginPlay()
{
	Super::BeginPlay();

	StartingLocation = GetActorLocation();
	StartingRotation = GetActorRotation();

	if (HasAuthority())
	{
		ReturnComp->OnCompleted.AddUObject(this, &AEXCarryableObjective::ReturnCarryable);
		TimerComp->OnCompleted.AddUObject(this, &AEXCarryableObjective::ReturnCarryable);
	}
	else
	{
		TimerWidget = Cast<UEXProgressWidget>(TimerWidgetComp->GetUserWidgetObject());
		if (TimerWidget)
		{
			TimerWidget->SetProgressComponent(TimerComp);
			TimerWidget->SetColor(TimerProgressbarColor);
		}
		ReturnWidget = Cast<UEXProgressWidget>(ReturnWidgetComp->GetUserWidgetObject());
		if (ReturnWidget)
		{
			ReturnWidget->SetProgressComponent(ReturnComp);
			ReturnWidget->SetColor(ReturnProgressbarColor);
		}
	}
}

bool AEXCarryableObjective::IsInSpawn() const
{
	// Doing it this way, we don't have to replicate another variable to get this info
	// besides it shouldn't be a problem if you can't interact with a carryable that's nearly in spawn even if it was picked up and dropped
	// even if it has a reset timer running
	return (FVector::DistSquared(GetActorLocation(), StartingLocation) < 1.f);
}

void AEXCarryableObjective::OnRep_Enabled()
{
	SetVisible(bEnabled);
}

void AEXCarryableObjective::OnRep_Loc()
{
	SetActorLocation(Loc);
}

void AEXCarryableObjective::OnRep_Rot()
{
	SetActorRotation(Rot);
}

void AEXCarryableObjective::SetLocAndRot(const FVector& InLoc, const FRotator& InRot)
{
	SetActorLocationAndRotation(InLoc, InRot, false, nullptr, ETeleportType::TeleportPhysics);
	Loc = InLoc;
	Rot = InRot;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXCarryableObjective, Loc, this);
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXCarryableObjective, Rot, this);
}

bool AEXCarryableObjective::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Super::CanInteract_Implementation(Player, bLookAt))
	{
		return false;
	}

	AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
	AEXTeam* Team = PS ? PS->GetTeam() : nullptr;
	return !IsInSpawn() || (Team && Team->IsAttacking());
}

bool AEXCarryableObjective::StartInteract_Implementation(AEXCharacter* Player)
{
	Super::StartInteract_Implementation(Player);

	AEXPlayerState* const PS = Player->GetPlayerState<AEXPlayerState>();
	AEXTeam* const Team = PS ? PS->GetTeam() : nullptr;
	if (Team)
	{
		if (Team->IsAttacking())
		{
			UEXMilkJug* MilkJug = CastChecked<UEXMilkJug>(Player->GetInventoryComponent()->GetTool(RequiredTool));
			if(MilkJug)
			{
				MilkJug->SetJugActor(this);
				// Note: Start interact should equip local only, so that if start interact fails on server, we don't equip the tool at all, just send a correction to the client
				Player->GetInventoryComponent()->EquipTool(RequiredTool, this);
				OnPickedUp(Player->GetEXController());
			}
		}
		else if (Team->IsDefending() && !IsInSpawn() && !ReturningPlayer) // Defenders can't interact if the milkjug is in its spawn place
		{
			// Start progress immediately 
			StartProgress(Player);
		}
	}
	return true;
}

bool AEXCarryableObjective::StopInteract_Implementation(AEXCharacter* Player)
{
	Super::StopInteract_Implementation(Player);

	if (HasAuthority())
	{
		AEXPlayerState* const PS = Player->GetPlayerState<AEXPlayerState>();
		AEXTeam* const Team = PS ? PS->GetTeam() : nullptr;
		if (Team && Team->IsDefending() && !IsInSpawn()) // Defenders can't interact if the milkjug is in its spawn place
		{
			ReturnComp->StopProgress(Player->GetEXController());
			ReturningPlayer = nullptr;
		}
	}
	return true;
}

void AEXCarryableObjective::OnDelivered()
{
	bDelivered = true;
	SetVisible(false);
}

void AEXCarryableObjective::OnPickedUp(AEXPlayerController* Player)
{
	if (!HasAuthority())
	{
		return;
	}
	SetVisible(false);

	TimerComp->StopProgress(nullptr);

	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 1, Player->GetPlayerState<APlayerState>());
}

void AEXCarryableObjective::OnDropped(const FVector Location, const FRotator Rotation)
{
	if (!HasAuthority())
	{
		return;
	}
	if (bDelivered)
	{
		return;
	}
	SetVisible(true);

	FVector AdjustedLocation = Location;
	FRotator AdjustedRotation = Rotation;

	// Try to find a spawn position
	GetWorld()->FindTeleportSpot(this, AdjustedLocation, AdjustedRotation);
	// Put the carryable on the floor
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, AdjustedLocation, AdjustedLocation - FVector::UpVector * 250, ECC_WorldOnly))
	{
		AdjustedLocation = HitResult.ImpactPoint;
	}
	SetLocAndRot(AdjustedLocation, AdjustedRotation);

	TimerComp->StartProgress(nullptr);
}

void AEXCarryableObjective::ReturnCarryable(AEXPlayerController* Player)
{
	if (HasAuthority())
	{
		if (Player)
		{
			GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 0, Player->GetPlayerState<APlayerState>());
		}
		else
		{
			GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 12);
		}
		ResetCarryable();
	}
}

void AEXCarryableObjective::Reset()
{
	ResetCarryable();

	Super::Reset();
}

void AEXCarryableObjective::ResetCarryable()
{
	if (HasAuthority())
	{
		if (ReturningPlayer)
		{
			ReturningPlayer->StopInteract(this, true);
			ReturningPlayer = nullptr;
		}
		SetLocAndRot(StartingLocation, StartingRotation);
		TimerComp->ResetProgress();
	}
}

void AEXCarryableObjective::SetVisible(bool bVisible)
{
	if (bVisible != bEnabled)
	{
		SetActorEnableCollision(bVisible);
		SetActorHiddenInGame(!bVisible);
		bEnabled = bVisible;
		MARK_PROPERTY_DIRTY_FROM_NAME(AEXCarryableObjective, bEnabled, this);
	}
}


void AEXCarryableObjective::StartProgress(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		ReturnComp->StartProgress(Player->GetEXController());
		ReturningPlayer = Player;
	}
}

#undef LOCTEXT_NAMESPACE

