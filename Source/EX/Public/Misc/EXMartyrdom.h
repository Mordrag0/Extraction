// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXAbilityInventory.h"
#include "EXMartyrdom.generated.h"

class AEXGrenade;

/**
 * 
 */
UCLASS()
class EX_API UEXMartyrdom : public UEXAbilityInventory
{
	GENERATED_BODY()
	
public:
	UEXMartyrdom();

protected:
	void Explode();


	UPROPERTY(EditDefaultsOnly, Category = "Martyrdom")
	float Delay = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Martyrdom")
	TSubclassOf<AEXGrenade> GrenadeClass = nullptr;

	FTimerHandle TimerHandle_Explode;


};
