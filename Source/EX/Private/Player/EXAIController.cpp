// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXAIController.h"
#include "Misc/EXTargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Inventory/EXInventory.h"

void AEXAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

}

bool AEXAIController::HasPath() const
{
	UPathFollowingComponent* PFC = GetPathFollowingComponent();
	if (!PFC)
	{
		return false;
	}
	return PFC->GetPath().IsValid();
}

bool AEXAIController::CanFire() const
{
	if (!EXCharacter)
	{
		return false;
	}
	UEXInventory* Inventory = EXCharacter->GetInventoryComponent()->GetEquippedInventory();
	return Inventory && Inventory->CanFire(EWeaponInput::Primary);
}

bool AEXAIController::IsFiring() const
{
	if (!EXCharacter)
	{
		return false;
	}
	UEXInventory* Inventory = EXCharacter->GetInventoryComponent()->GetEquippedInventory();
	return Inventory && Inventory->IsFiring();
}

void AEXAIController::Fire(float Time)
{
	if (!EXCharacter)
	{
		return;
	}
	if (bFiring)
	{
		return;
	}
	bFiring = true;
	if (Time > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &AEXAIController::StopFire, Time, false);
	}

	EXCharacter->GetInventoryComponent()->FirePressed(EWeaponInput::Primary);
}

void AEXAIController::StopFire()
{
	if (!EXCharacter)
	{
		return;
	}
	if (!bFiring)
	{
		return;
	}
	bFiring = false;

	EXCharacter->GetInventoryComponent()->FireReleased(EWeaponInput::Primary);
}

void AEXAIController::BeginPlay()
{
	Super::BeginPlay();

	/*TArray<AActor*> TargetPoints;
	UGameplayStatics::GetAllActorsOfClass(this, AEXTargetPoint::StaticClass(), TargetPoints);

	
	if (TargetPoints.Num() > 0)
	{
		AActor* Target = TargetPoints[FMath::Rand() % TargetPoints.Num()];

		UAIBlueprintHelperLibrary::SimpleMoveToActor(this, Target);
	}*/
}

void AEXAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	EXCharacter = Cast<AEXCharacter>(InPawn);
	EXCharacter->SetPlayerDefaults();
}

void AEXAIController::OnUnPossess()
{
	Super::OnUnPossess();

	EXCharacter = nullptr;
}
