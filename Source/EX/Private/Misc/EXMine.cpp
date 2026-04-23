// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXMine.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/EXProjectileMovementComponent.h"
#include "Player/EXCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/EXGameplayStatics.h"

AEXMine::AEXMine()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	Collision = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	Collision->SetupAttachment(Mesh);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Movement = CreateDefaultSubobject<UEXProjectileMovementComponent>("Movement");
	if (HasAuthority())
	{
		Movement->OnProjectileStop.AddDynamic(this, &AEXMine::Enable);
	}

	bReplicates = true;
	SetReplicateMovement(true);

	SetCanBeDamaged(true);
}

float AEXMine::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Explode();
	return ActualDamage;
}

void AEXMine::BeginPlay()
{
	Super::BeginPlay();
	const FRotator Rot = GetActorRotation();
	SetActorRotation(FRotator(0, Rot.Yaw, 0));
}

void AEXMine::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != GetInstigator())
	{
		AEXCharacter* Player = Cast<AEXCharacter>(OtherActor);
		if (Player)
		{
			if (Cast<UCharacterMovementComponent>(Player->GetMovementComponent())->MovementMode == EMovementMode::MOVE_Walking)
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_Explode, this, &AEXMine::Explode, Delay);
			}
			else
			{
				// If player is midair, wait for him to land and then see if he is still inside the trigger area
				Player->MovementModeChangedDelegate.AddDynamic(this, &AEXMine::Trigger);
			}
		}
	}
}

void AEXMine::Trigger(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (Cast<UCharacterMovementComponent>(Character->GetMovementComponent())->MovementMode == EMovementMode::MOVE_Walking)
	{
		if (Collision->IsOverlappingActor(Character))
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Explode, this, &AEXMine::Explode, Delay);
		}
		else
		{
			Character->MovementModeChangedDelegate.RemoveDynamic(this, &AEXMine::Trigger);
		}
	}
}

void AEXMine::Explode()
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), DamageRadius, DamageType, IgnoredActors, this, GetInstigatorController());
	NetDestroy();
}

void AEXMine::PlayDeathEffects()
{
	Super::PlayDeathEffects();

	const FVector ProjDirection = -GetActorUpVector();

	const FVector StartTrace = GetActorLocation() - ProjDirection * 200;
	const FVector EndTrace = GetActorLocation() + ProjDirection * 150;
	FHitResult Impact;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool bComponentHit = GetWorld()->LineTraceSingleByChannel(Impact, StartTrace, EndTrace, ECC_WorldOnly, QueryParams);

	UEXGameplayStatics::SpawnExplosion(GetWorld(), ExplosionEffectClass, GetActorTransform(), bComponentHit, Impact);
}

void AEXMine::Enable(const FHitResult& ImpactResult)
{
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AEXMine::BeginOverlap);
}

