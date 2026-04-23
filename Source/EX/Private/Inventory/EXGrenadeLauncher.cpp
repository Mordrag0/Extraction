// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXGrenadeLauncher.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXGrenade.h"

UEXGrenadeLauncher::UEXGrenadeLauncher()
{
	bUsesAbility = true;
}

void UEXGrenadeLauncher::PerformShot()
{
	Use();
	if (!IsLocallyOwned())
	{
		return;
	}
	const FRotator Rot = GetOwningCharacter()->GetControlRotation();
	const FVector Loc = GetWeaponMeshComponent()->GetComponentLocation();
	Server_Reliable_PerformShot(Loc, Rot);
}

bool UEXGrenadeLauncher::CanFire(EWeaponInput Mode) const
{
	return Super::CanFire(Mode) && CanUse();
}

void UEXGrenadeLauncher::Server_Reliable_PerformShot_Implementation(const FVector Loc, const FRotator Rot)
{
	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwningCharacter();
		SpawnParams.Instigator = GetOwningCharacter();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FVector SpawnLoc = Loc + Rot.RotateVector(ProjectileSpawnOffset);
		const FRotator SpawnRot(Rot.Pitch - 90.f, Rot.Yaw, 0.f);
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLoc, SpawnRot, SpawnParams);
	}
}

bool UEXGrenadeLauncher::Server_Reliable_PerformShot_Validate(const FVector Loc, const FRotator Rot)
{
	return true;
}

