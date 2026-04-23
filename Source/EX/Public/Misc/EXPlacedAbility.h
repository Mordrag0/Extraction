// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXAbility.h"
#include "EXPlacedAbility.generated.h"

class AEXCharacter;

UCLASS()
class EX_API AEXPlacedAbility : public AEXAbility
{
	GENERATED_BODY()
	
public:	
	AEXPlacedAbility();

	virtual void Remove();

	virtual void Die();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void SetPlayer(AEXCharacter* Player);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float MaxHealth = 100.f;

	float Health = 0.f;
protected:

	UPROPERTY()
	class AEXCharacter* PlayerOwner = nullptr;
	UPROPERTY()
	class AEXTeam* Team = nullptr;

public:
	virtual void PickUp(AEXCharacter* Player) override;

};
