// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXFragNadeInventory.h"
#include "Misc/EXFragNade.h"
#include "EX.h"
#include "Inventory/EXInventoryComponent.h"

UEXFragNadeInventory::UEXFragNadeInventory()
{
}

bool UEXFragNadeInventory::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::Fire(Mode, WorldTimeOverride))
	{
		return false;
	}

	if (Mode != EWeaponInput::Primary)
	{
		return true;
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MaxHold, this, &UEXFragNadeInventory::MaxHold, MaxHoldTime, false);
	return true;
}

bool UEXFragNadeInventory::CanUse(float Amount /*= 1.f*/) const
{
	// If we are in the middle of throwing, then Use() has already been called, and in that case bCanThrow will be true
	return (Super::CanUse(Amount) || bCanThrow);
}

bool UEXFragNadeInventory::StopFire(EWeaponInput Mode, float WorldTimeOverride)
{
	TimeRemaining = GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_MaxHold)
		? GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_MaxHold) : 0.f;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MaxHold);
	if (!Super::StopFire(Mode, WorldTimeOverride))
	{
		return false;
	}
	return true;
}

void UEXFragNadeInventory::MaxHold()
{
	const float WorldTime = GetWorld()->GetTimeSeconds();
	StopFire(EWeaponInput::Primary, WorldTime);
}

void UEXFragNadeInventory::Throw()
{
	if (bCanThrow)
	{
		Super::Throw();
	}
}

void UEXFragNadeInventory::BeginPlay()
{
	Super::BeginPlay();	
	
	MaxHoldTime = TimeToExplode - TimeToThrow;
}

void UEXFragNadeInventory::Init(AEXAbility* SpawnedAbility)
{
	Super::Init(SpawnedAbility);

	AEXFragNade* FragNade = Cast<AEXFragNade>(SpawnedAbility);
	UE_LOG(LogEXAbility, Warning, TEXT("UEXFragNadeInventory: Time to explode: %f"), TimeRemaining);
	FragNade->SetTimeToExplode(TimeRemaining);
}
