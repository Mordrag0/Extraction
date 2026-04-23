// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXDefib.h"
#include "Player/EXCharacter.h"
#include "Online/EXPlayerState.h"
#include "EXTypes.h"
#include "Inventory/EXDamageType.h"
#include "System/EXGameplayStatics.h"

UEXDefib::UEXDefib()
{
	WeaponType = EWeaponType::Defib;
	PrimaryAttachPoint = FName("Palm");
	SecondaryAttachPoint = FName("Palm2");

	bShootOnFireReleased = true;
}

void UEXDefib::Server_Unreliable_TraceHit_Implementation(const FHitResult& HitResult, float ShotCharge)
{
	AEXCharacter* OtherPlayer = Cast<AEXCharacter>(HitResult.GetActor());
	if (OtherPlayer)
	{
		AEXCharacter* Owner = GetOwningCharacter();
		AController* EventInstigator = Owner->GetController();
		const bool bSameTeam = UEXGameplayStatics::SameTeam(Owner, OtherPlayer);
		const bool bDead = OtherPlayer->IsDead();
		if (bSameTeam)
		{
			if (bDead)
			{
				OtherPlayer->Revive(ShotCharge * 100.f, EventInstigator, Owner->GetMercType());
			}
		}
		else
		{
			FEXPointDamageEvent PointDamageEvent = FEXPointDamageEvent(Damage, HitResult, (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal(), DamageType, GetOwningCharacter()->GetMercType());
			OtherPlayer->TakeDamage(Damage, FDamageEvent(), EventInstigator, Owner);
		}
	}
}
