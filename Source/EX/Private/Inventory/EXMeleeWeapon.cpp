// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXMeleeWeapon.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"

UEXMeleeWeapon::UEXMeleeWeapon()
{
	bUsesAbility = false;

	WeaponType = EWeaponType::Melee;
	QuickUseType = EQuickUseType::Use;
}

void UEXMeleeWeapon::EnableDamage()
{
	GetOwningCharacter()->GetWeaponMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UEXMeleeWeapon::EnableDamageSecondary()
{
	GetOwningCharacter()->GetWeaponMeshComponentSecondary()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UEXMeleeWeapon::DisableDamage()
{
	GetOwningCharacter()->GetWeaponMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IgnoreActors.Empty(1);
	IgnoreActors.Add(GetOwningCharacter());
}

void UEXMeleeWeapon::DisableDamageSecondary()
{
	GetOwningCharacter()->GetWeaponMeshComponentSecondary()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IgnoreActors.Empty(1);
	IgnoreActors.Add(GetOwningCharacter());
}

void UEXMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyOwned())
	{
		GetOwningCharacter()->GetWeaponMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &UEXMeleeWeapon::MeleeHit);
	}
	DisableDamage();
}

void UEXMeleeWeapon::MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsEquipped())
	{ 
		// This will be true for non equipped melee weapons in the inventory (for example: milk jug when knife is equipped)
		return;
	}
	if (!IgnoreActors.Contains(OtherActor) && OtherActor->CanBeDamaged())
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee hit"));
		IgnoreActors.Add(OtherActor);
		SpawnImpactEffect(SweepResult);
		Server_Reliable_MeleeHit(SweepResult);
		if (SweepResult.GetActor()->CanBeDamaged())
		{
			HitOnClient(SweepResult.GetActor(), false);
		}
	}
	else
	{

	}
}

void UEXMeleeWeapon::Multicast_SpawnImpactEffect_Implementation(const FHitResult& SweepResult)
{
	if (IsSimulatedProxy())
	{
		SpawnImpactEffect(SweepResult);
	}
}

void UEXMeleeWeapon::Server_Reliable_MeleeHit_Implementation(const FHitResult& SweepResult)
{
	Multicast_SpawnImpactEffect(SweepResult);

	if (!SweepResult.GetActor()->CanBeDamaged())
	{
		return;
	}

	FPointDamageEvent PointDamageEvent = FPointDamageEvent(Damage, SweepResult, (SweepResult.TraceEnd - SweepResult.TraceStart).GetSafeNormal(), DamageType);
	SweepResult.GetActor()->TakeDamage(Damage, PointDamageEvent, GetOwningCharacter()->GetController(), GetOwningCharacter());
}

bool UEXMeleeWeapon::Server_Reliable_MeleeHit_Validate(const FHitResult& SweepResult)
{
	return true;
}
