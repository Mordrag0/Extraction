// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EXAbility.h"
#include "EXAmmoPack.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXAmmoPack : public AEXAbility
{
	GENERATED_BODY()

public:
	AEXAmmoPack();

	virtual UEXProjectileMovementComponent* GetProjectileMovementComponent() const override { return Movement; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void PickUpAmmo(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Medpack", Meta = (DisplayName = "On Pick Up", ScriptName = "OnPickUp"))
	void K2_OnPickUp(AEXCharacter* Player);

public:

	//~ Begin IEXInteractable Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	//~ End IEXInteractable Interface
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Medpack")
	float AmmoAmount = 1.f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Medpack", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SK_Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Medpack", Meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Medpack", Meta = (AllowPrivateAccess = "true"))
	class UEXProjectileMovementComponent* Movement = nullptr;
	
};
