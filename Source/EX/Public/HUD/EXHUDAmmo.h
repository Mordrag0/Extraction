// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "EXHUDAmmo.generated.h"

class UEXWeapon;
class UTextBlock;

/**
 * 
 */
UCLASS()
class EX_API UEXHUDAmmo : public UEXHUDElement
{
	GENERATED_BODY()

public:
	void UpdateAmmo(const UEXWeapon* Weapon);
protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UTextBlock* Ammo = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UTextBlock* TotalAmmo = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	FText EmptyText = INVTEXT("--");
};
