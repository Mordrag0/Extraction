// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXDummy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Online/EXAssaultCourseGame.h"
#include "EX.h"
#include "Misc/EXDummyAnimInstance.h"

AEXDummy::AEXDummy()
{
	Scene = CreateDefaultSubobject<USceneComponent>("RootScene");
	RootComponent = Scene;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>("CollisionComp");
	CollisionComp->SetupAttachment(Scene);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Ability, ECR_Overlap);


	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(Scene);

	SetCanBeDamaged(true);
}

void AEXDummy::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEXDummy::AbilityHit);

	AnimInst = Cast<UEXDummyAnimInstance>(Mesh->GetAnimInstance());

	Reset();
}

void AEXDummy::Reset()
{
	Super::Reset();
	bDown = false;
	Health = MaxHealth;

	if (AnimInst)
	{
		AnimInst->Reset();
	}
}

void AEXDummy::AbilityHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEXAssaultCourseGame* GM = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
	if (GM->IsActive())
	{
		if (Type == EDummyType::Ability)
		{
			OtherActor->Destroy();
			Die();
		}
	}
}

float AEXDummy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	AEXAssaultCourseGame* GM = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
	ActualDamage = ((Type == EDummyType::Damage) && !bDown && GM && GM->IsActive()) ? ActualDamage : 0;

	Health -= ActualDamage;
	if (Health <= 0.f)
	{
		Die();
	}
	else
	{
		PlayHitEffects();
	}
	return ActualDamage;
}

void AEXDummy::PlayHitEffects_Implementation()
{
	if (AnimInst)
	{
		AnimInst->OnHit();
	}
}

void AEXDummy::PlayDeathEffects_Implementation()
{
	if (AnimInst)
	{
		AnimInst->OnDead();
	}
}

void AEXDummy::Die()
{
	if (bDown)
	{
		return;
	}
	bDown = true;
	Scored();
	PlayDeathEffects();
}

void AEXDummy::Scored()
{
	AEXAssaultCourseGame* Game = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
	if (Game)
	{
		Game->AddScore(Score);
	}
}
