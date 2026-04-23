// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXDropOffLocation.h"
#include "Inventory/EXMilkJug.h"
#include "Components/SceneComponent.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"
#include "System/EXProgress.h"
#include "HUD/EXProgressWidget.h"
#include "Misc/EXCarryableObjective.h"
#include "Components/WidgetComponent.h"
#include "Player/EXPlayerController.h"
#include "Online/EXObjectiveMessage.h"
#include "Online/EXGameModeBase.h"

AEXDropOffLocation::AEXDropOffLocation()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	DeliverWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DeliverProgressbar"));
	DeliverWidgetComp->SetupAttachment(Root);
	DeliverWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	DeliveryProgressComp = CreateDefaultSubobject<UEXProgress>("DropoffProgress");

	bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;
}

void AEXDropOffLocation::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority())
	{
		DeliverWidget = Cast<UEXProgressWidget>(DeliverWidgetComp->GetUserWidgetObject());
		if (DeliverWidget)
		{
			DeliverWidget->SetProgressComponent(DeliveryProgressComp);
			DeliverWidget->SetColor(FColor::Yellow);
		
		}
	}

	for (AEXCarryableObjective* MilkJug : Carryables)
	{
		MilkJug->SetStage(Stage);
	}
	ensure(Carryables.Num() == DeliveryProgressComp->GetRepeats() && "The number of carryables doesn't match the number of repeats.");
	SetProgressComp(DeliveryProgressComp);
	GMB = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
}

bool AEXDropOffLocation::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Super::CanInteract_Implementation(Player, bLookAt))
	{
		return false;
	}
	
	return !!Cast<UEXMilkJug>(Player->GetInventoryComponent()->GetEquippedInventory());
}

bool AEXDropOffLocation::StartInteract_Implementation(AEXCharacter* Player)
{
	Super::StartInteract_Implementation(Player);

	if (HasAuthority())
	{
		// Start progress immediately, because we already need the proper tool (milkjug) in hand to interact
		StartProgress(Player);
		SetObjectiveActive(true);
	}
	return true;
}

bool AEXDropOffLocation::StopInteract_Implementation(AEXCharacter* Player)
{
	Super::StopInteract_Implementation(Player);

	if (HasAuthority())
	{
		DeliveryProgressComp->StopProgress(Player->GetEXController());
		SetObjectiveActive(false);
	}
	return true;
}

void AEXDropOffLocation::StartProgress(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		DeliveryProgressComp->StartProgress(Player->GetEXController());
	}
}

void AEXDropOffLocation::OnCompleted(AEXPlayerController* Player)
{
	SetObjectiveActive(false);

	Super::OnCompleted(Player);

	if (!HasAuthority())
	{
		return;
	}

	AEXCharacter* PlayerChar = Player ? Player->GetEXCharacter() : nullptr;
	if (PlayerChar)
	{
		UEXMilkJug* MilkJug = Cast<UEXMilkJug>(PlayerChar->GetInventoryComponent()->GetEquippedInventory());
		AEXCarryableObjective* MilkJugActor = MilkJug ? MilkJug->GetMilkJugActor() : nullptr;
		if (MilkJugActor)
		{
			MilkJugActor->OnDelivered();
		}
		else
		{
			UE_LOG(LogEXWeapon, Error, TEXT("Failed to remove the milkjug"));
		}

		PlayerChar->StopInteract(this, true);
		PlayerChar->GetInventoryComponent()->Unequip(MilkJug, false);
		if (Player) 
		{
			GMB->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 2, Player->GetPlayerState<AEXPlayerState>());
		}
	}

	ShowCarryable(GetProgressComp()->GetCompletions());
}

void AEXDropOffLocation::Reset()
{
	Super::Reset();

	for (AEXCarryableObjective* Carryable : Carryables)
	{
		Carryable->SetVisible(false);
	}
}

void AEXDropOffLocation::SetActive()
{
	Super::SetActive();

	if (Carryables.Num() > 0)
	{
		Carryables[0]->SetVisible(true);
	}
}

void AEXDropOffLocation::ShowCarryable(int32 Idx)
{
	if (Carryables.IsValidIndex(Idx))
	{
		Carryables[Idx]->SetVisible(true);
	}
}

