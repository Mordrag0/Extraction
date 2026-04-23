// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/EXInteractable.h"
#include "Inventory/EXInventory.h"
#include "EXAbility.generated.h"

class AEXCharacter;
class UEXInventory;
class UEXProjectileMovementComponent;
class UParticleSystem;
class USoundCue;

UCLASS()
class EX_API AEXAbility : public AActor , public IEXInteractable // #Interactable
{
	GENERATED_BODY()
	
public:	
	AEXAbility();

	virtual void Init(UEXInventory* InventoryOwner);

	virtual void PickUp(AEXCharacter* Player);

	void NetDestroy();
protected:

	virtual bool CanPickUp(const AEXCharacter* Player) const;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bCanBePickedUp = true;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bCanBeRecycled = true;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bNeedsEyeContactToInteract = true;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float LifeTime = 0.f;

	UPROPERTY()
	UEXInventory* Inventory = nullptr;


	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UParticleSystem* DeathEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	USoundCue* DeathSoundEffect;

	UPROPERTY(ReplicatedUsing = OnRep_Destroyed)
	bool bDestroyed;
	UFUNCTION()
	void OnRep_Destroyed();

	virtual void PlayDeathEffects();
public:
	//~ Begin IEXInteractable Interface
	virtual bool CanInteract_Implementation(const AEXCharacter * Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
	virtual bool TapInteract() const override;
	//~ End IEXInteractable Interface

	virtual void PostNetReceiveLocationAndRotation() override;
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

	virtual UEXProjectileMovementComponent* GetProjectileMovementComponent() const { return nullptr; }


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
