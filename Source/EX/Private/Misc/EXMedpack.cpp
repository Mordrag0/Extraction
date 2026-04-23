// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXMedpack.h"
#include "Components/BoxComponent.h"
#include "Player/EXCharacter.h"
#include "Misc/EXProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

AEXMedpack::AEXMedpack()
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

void AEXMedpack::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEXMedpack::PickUpMed);
	}

}

void AEXMedpack::PickUpMed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEXCharacter* Player = Cast<AEXCharacter>(OtherActor);
	if (Player)
	{
		if (Player->GetHealth() < Player->GetMaxHealth())
		{
			if (HealAmount > 0.f)
			{
				float Heal = HealAmount;
				if (!Movement->IsStopped())
				{
					Heal += DirectHitHeal;
				}
				Player->InstaHeal(Heal, GetInstigatorController());
			}
			if (HealingRate > 0.f)
			{
				Player->SetHealingRate(HealingRate, GetInstigatorController());
			}
			K2_OnPickUp(Player);
			Destroy();
		}
	}
}

bool AEXMedpack::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Movement->IsStopped())
	{
		return false;
	}
	return Super::CanInteract_Implementation(Player, bLookAt);
}
