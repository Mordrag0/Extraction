// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EXPlacedAbility.h"
#include "EXAmmoStation.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXAmmoStation : public AEXPlacedAbility
{
	GENERATED_BODY()

public:
	AEXAmmoStation();

protected:

	UFUNCTION()
	void GiveAmmo();

	UPROPERTY(EditDefaultsOnly, Category = "AmmoStation")
	float MagAmount = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "AmmoStation")
	float Frequency = 5.f;


	virtual void BeginPlay() override;

	FTimerHandle TimerHandle_GiveAmmo;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComp = nullptr;
};
