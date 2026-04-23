// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXPlacedAbility.h"
#include "EXHealingStation.generated.h"

UCLASS()
class EX_API AEXHealingStation : public AEXPlacedAbility
{
	GENERATED_BODY()
	
public:	
	AEXHealingStation();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "HealingStation")
	float HealingRate = 10.f;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComp = nullptr;

};
