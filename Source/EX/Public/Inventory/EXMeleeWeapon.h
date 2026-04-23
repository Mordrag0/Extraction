// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeapon.h"
#include "EXMeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXMeleeWeapon : public UEXWeapon
{
	GENERATED_BODY()
	
public:
	UEXMeleeWeapon();

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void EnableDamage();
	UFUNCTION(BlueprintCallable, Category = "Melee")
	void EnableDamageSecondary();
	UFUNCTION(BlueprintCallable, Category = "Melee")
	void DisableDamage();
	UFUNCTION(BlueprintCallable, Category = "Melee")
	void DisableDamageSecondary();

	void BeginPlay() override;

	UFUNCTION()
	void MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_MeleeHit(const FHitResult& SweepResult);
	// #EXTODORELIABLE
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnImpactEffect(const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UDamageType> DamageType = nullptr;

	TArray<AActor*> IgnoreActors;
};
