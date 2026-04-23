// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXGrenade.h"
#include "EXStickyNade.generated.h"

UCLASS()
class EX_API AEXStickyNade : public AEXGrenade
{
	GENERATED_BODY()
	
public:	
	AEXStickyNade();

	virtual void Explode() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//~ Begin IEXInteractable Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	//~ End IEXInteractable Interface

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ProjectileStopped(const FHitResult& ImpactResult);

	UFUNCTION()
	void OnAttachActorDestroyed(AActor* DestroyedActor);

	bool bExploded = false;



};
