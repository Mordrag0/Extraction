// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXThrowableAbility.h"
#include "Player/EXCharacter.h"
#include "Misc/EXAbility.h"
#include "Player/EXPlayerController.h"
#include "Online/EXLevelRules.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/EXAbilityTracker.h"

UEXThrowableAbility::UEXThrowableAbility()
{
	bUnequipAbilityWhenEmpty = true;
	QuickUseType = EQuickUseType::Use;
	RecoverAmount = .8f;
	bHideOnUse = true;
}

bool UEXThrowableAbility::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::Fire(Mode, WorldTimeOverride))
	{
		return false;
	}

	if (Mode == EWeaponInput::Primary)
	{
		if (IsAuthority())
		{
			Use();
			bCanRecover = true;
		}
		if (!bShootOnFireReleased)
		{
			if (TimeToThrow <= 0.f)
			{
				Throw();
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_Throw, this, &UEXThrowableAbility::Throw, TimeToThrow, false);
			}
		}
		bCanThrow = true;
	}
	return true;
}

bool UEXThrowableAbility::StopFire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::StopFire(Mode, WorldTimeOverride))
	{
		return false;
	}
	if (Mode == EWeaponInput::Primary)
	{
		if (bShootOnFireReleased)
		{
			if (TimeToThrow <= 0.f)
			{
				Throw();
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_Throw, this, &UEXThrowableAbility::Throw, TimeToThrow, false);
			}
		}
	}
	return true;
}

void UEXThrowableAbility::Throw()
{
	if (!bCanThrow)
	{
		return;
	}
	bCanThrow = false;

	if (!IsAuthority())
	{
		return;
	}
	check(AbilityClass);
	bCanRecover = false;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwningCharacter()->GetEXController();
	SpawnParams.Instigator = GetOwningCharacter();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.bDeferConstruction = true;
	AEXAbility* SpawnedAbility = GetWorld()->SpawnActor<AEXAbility>(AbilityClass, GetSpawnTransform(), SpawnParams);
	Init(SpawnedAbility);
	UGameplayStatics::FinishSpawningActor(SpawnedAbility, GetSpawnTransform());

	if (UEXAbilityTracker* AbilityTracker = GetOwningCharacter()->GetEXController()->GetAbilityTracker())
	{
		AbilityTracker->AddAbility(SpawnedAbility);
	}
}

void UEXThrowableAbility::Init(AEXAbility* SpawnedAbility)
{
	SpawnedAbility->Init(this);
}

FTransform UEXThrowableAbility::GetSpawnTransform() const
{
	FVector Location;
	FRotator Rotation;
	GetOwningCharacter()->GetActorEyesViewPoint(Location, Rotation);
	Location += Rotation.Vector() * 100;
	return FTransform(Rotation, Location);
}

void UEXThrowableAbility::Cancel()
{
	bCanThrow = false;
	if (bCanRecover)
	{
		RecoverAbility(1.f);
	}
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Throw);
}

bool UEXThrowableAbility::CanFire(EWeaponInput Mode) const
{
	return Super::CanFire(Mode) && CanUse();
}

bool UEXThrowableAbility::CanUse(float Amount /*= 1.f*/) const
{
	// If we are in the middle of throwing, then Use() has already been called, and in that case bCanThrow will be true
	return (Super::CanUse(Amount) || bCanThrow);
}
