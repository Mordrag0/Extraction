// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXPlacingAbility.h"
#include "Components/StaticMeshComponent.h"
#include "Player/EXCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SceneComponent.h"

AEXPlacingAbility::AEXPlacingAbility()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	PrimaryActorTick.bCanEverTick = true;

}

void AEXPlacingAbility::SetCharacter(AEXCharacter* InPlayer)
{
	Player = InPlayer;
}

void AEXPlacingAbility::BeginPlay()
{
	Super::BeginPlay();
	
	if (MaterialInstance)
	{
		DynamicMaterial = Mesh->CreateDynamicMaterialInstance(0, MaterialInstance);
	}
	SetVaildPlacement(false);
}

void AEXPlacingAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Adjust();
}

void AEXPlacingAbility::SetVaildPlacement(bool bValid)
{
	if ((bValid != bCurrentlyValidPlacement) && DynamicMaterial)
	{
		bCurrentlyValidPlacement = bValid;
		DynamicMaterial->SetVectorParameterValue("Color", bCurrentlyValidPlacement ? ValidColor : InvalidColor);
	}
}

void AEXPlacingAbility::Adjust()
{
	if (!Player)
	{
		return;
	}
	FVector PlayerLoc;
	FRotator PlayerRot;
	Player->GetActorEyesViewPoint(PlayerLoc, PlayerRot);
	FVector AbilitiLoc = PlayerLoc + PlayerRot.Vector() * Dist;
	const FRotator AbilityRot(0, Player->GetActorRotation().Yaw, 0);
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Player);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, PlayerLoc, AbilitiLoc, ECC_Visibility, QueryParams);

	if (bHit && (Hit.Normal.Z >= MinNormalZ))
	{
		SetActorLocationAndRotation(Hit.Location, AbilityRot, false, nullptr, ETeleportType::TeleportPhysics);
		TSet<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);
		if (OverlappingActors.Num() == 0)
		{
			SetVaildPlacement(true);
		}
		else
		{
			SetVaildPlacement(false);
		}
	}
	else
	{
		SetActorLocationAndRotation(AbilitiLoc, AbilityRot, false, nullptr, ETeleportType::TeleportPhysics);
		SetVaildPlacement(false);
	}

}
