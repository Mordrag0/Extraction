// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXJumpPad.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Player/EXCharacter.h"
#include "Misc/EXProjectileMovementComponent.h"

AEXJumpPad::AEXJumpPad()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>("BoxCollisionComp");
	RootComponent = CollisionComp;
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Pad");
	Mesh->SetupAttachment(CollisionComp);


	BoostCollisionComp = CreateDefaultSubobject<UBoxComponent>("BoostCollisionComp");
	BoostCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoostCollisionComp->SetupAttachment(CollisionComp);

	Movement = CreateDefaultSubobject<UEXProjectileMovementComponent>("Movement");
	Movement->bRotationFollowsVelocity = true;
	Movement->bRotationRemainsVertical = true;
	Movement->bShouldBounce = true;
	if (HasAuthority())
	{
		Movement->OnProjectileStop.AddDynamic(this, &AEXJumpPad::Enable);
	}

	bReplicates = true;
	SetReplicateMovement(true);
}

void AEXJumpPad::Enable(const FHitResult& ImpactResult)
{
	if (bEnabled)
	{
		return;
	}
	bEnabled = true;
	BoostCollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BoostCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEXJumpPad::Launch);
}

void AEXJumpPad::Launch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEXCharacter* Player = CastChecked<AEXCharacter>(OtherActor);
	const FVector PlayerVelocity = Player->GetVelocity();
	FVector2D Fwd(PlayerVelocity.X, PlayerVelocity.Y);
	float FwdVelocity = FMath::Clamp(Fwd.Size(), ForwardLaunchMin, ForwardLaunchMax);
	float UpVelocity = FMath::Clamp(FMath::Abs(PlayerVelocity.Z), UpwardLaunchMin, UpwardLaunchMax);
	Fwd.Normalize();
	const FVector LaunchVelocity(Fwd * FwdVelocity, UpVelocity);
	Player->LaunchCharacter(LaunchVelocity, true, true);
}

void AEXJumpPad::BeginPlay()
{
	Super::BeginPlay();

	HP = MaxHP;
}

void AEXJumpPad::Kill()
{
	BoostCollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	NetDestroy();
}

float AEXJumpPad::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ActualDamage = FMath::Min(ActualDamage, HP);
	HP -= ActualDamage;

	if (FMath::IsNearlyZero(HP))
	{
		Kill();
	}

	return ActualDamage;
}

