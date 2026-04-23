// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXAmmoPack.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Misc/EXProjectileMovementComponent.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"


AEXAmmoPack::AEXAmmoPack()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>("CollisionComp");
	RootComponent = CollisionComp;

	SK_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("SK_Mesh");
	SK_Mesh->SetupAttachment(CollisionComp);

	Movement = CreateDefaultSubobject<UEXProjectileMovementComponent>("Movement");
	Movement->SetUpdatedComponent(CollisionComp);

	bReplicates = true;
	SetReplicateMovement(true);
}

void AEXAmmoPack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEXAmmoPack::PickUpAmmo);
	}
}

void AEXAmmoPack::PickUpAmmo(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEXCharacter* Player = Cast<AEXCharacter>(OtherActor);
	if (Player && !Player->GetInventoryComponent()->IsAmmoFull())
	{
		Player->GetInventoryComponent()->AddAmmo(AmmoAmount);
		K2_OnPickUp(Player);
		NetDestroy();
	}
}

bool AEXAmmoPack::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Movement->IsStopped())
	{
		return false;
	}
	return Super::CanInteract_Implementation(Player, bLookAt);
}
