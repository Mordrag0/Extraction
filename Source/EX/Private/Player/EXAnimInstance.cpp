// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/EXAnimInstance.h"
#include "Player/EXCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/EXCharacterMovement.h"
#include "Inventory/EXInventory.h"
#include "EX.h"
#include "Inventory/EXWeaponAnimInstance.h"

void UEXAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EXCharacter = Cast<AEXCharacter>(TryGetPawnOwner());
	if (EXCharacter)
	{
		EXCharacterMovement = Cast<UEXCharacterMovement>(EXCharacter->GetCharacterMovement());
	}
	EquippedWeaponState = EWeaponState::Ready;
}

void UEXAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		return;
	}
	if (!EXCharacter || !EXCharacterMovement)
	{
		return;
	}

	bSprinting = EXCharacterMovement->IsSprinting();
	const float WeaponModifier = EXCharacterMovement->GetWeaponModifier();

	const FVector Velocity = EXCharacter->GetVelocity();
	const FVector LocalVelocity = EXCharacter->GetActorRotation().UnrotateVector(Velocity);
	WalkForward = LocalVelocity.X / (EXCharacterMovement->MaxWalkSpeed * WeaponModifier);
	WalkRight = LocalVelocity.Y / (EXCharacterMovement->MaxWalkSpeed * WeaponModifier);
	if (bSprinting)
	{
		WalkForward = 2.f;
	}
	const FRotator LookDirection = EXCharacter->GetAimOffsets();
	Pitch = LookDirection.Pitch;
}

void UEXAnimInstance::WeaponStateChanged(EWeaponState WeaponState, float Modifier)
{
	EquippedWeaponState = WeaponState;
	if (!EXCharacter->IsNetMode(NM_DedicatedServer))
	{
		OnWeaponStateChanged(WeaponState, Modifier);
		if (WeaponAnim)
		{
			WeaponAnim->WeaponStateChanged(WeaponState);
		}
	}
	if (EXCharacter->IsLocallyControlled())
	{
		UE_LOG(LogEXAnim, Log, TEXT("%s"), *UEnum::GetValueAsString(WeaponState));
	}
}

void UEXAnimInstance::WeaponTypeChanged(EWeaponType WeaponType)
{
	if (EquippedWeaponType != WeaponType)
	{
		EquippedWeaponType = WeaponType;
		if (!EXCharacter->IsNetMode(NM_DedicatedServer))
		{
			OnWeaponTypeChanged(WeaponType);
		}
		if (EXCharacter->IsLocallyControlled())
		{
			UE_LOG(LogEXAnim, Log, TEXT("%s"), *UEnum::GetValueAsString(WeaponType));
		}
	}
}

void UEXAnimInstance::StartSlowRes()
{
	bSlowRes = true;
}

void UEXAnimInstance::StopSlowRes()
{
	bSlowRes = false;
}

void UEXAnimInstance::SetADS(bool bVal)
{
	bADS = bVal; 
	ADSChanged();
}

void UEXAnimInstance::SetDead(bool bInDead)
{
	bDead = bInDead;
}

void UEXAnimInstance::SetGibbed(bool bInGibbed)
{
	bGibbed = bInGibbed;
}

void UEXAnimInstance::SetIsFalling(bool bInFalling)
{
	bInAir = bInFalling;
}

void UEXAnimInstance::SetCrouchAmount(float InCrouchAmount)
{
	Crouching = InCrouchAmount;
}
