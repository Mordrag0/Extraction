// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EXWeaponAnimInstance.generated.h"

enum class EWeaponState : uint8;

/**
 * 
 */
UCLASS()
class EX_API UEXWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void Fire();
	void Reload();
	void CancelReload();

	void WeaponStateChanged(EWeaponState WeaponState);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bReload = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bCancelReload = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bFire = false;

};
