// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Inventory/EXInventory.h"
#include "EXAnimInstance.generated.h"

class UEXWeaponAnimInstance;

/**
 * 
 */
UCLASS()
class EX_API UEXAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	


public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void WeaponStateChanged(EWeaponState WeaponState, float Modifier);
	void WeaponTypeChanged(EWeaponType WeaponType);


	UFUNCTION(BlueprintImplementableEvent, Category = "Character Animation")
	void CancelReload();

	void SetWeaponAnimInst(UEXWeaponAnimInstance* InWeaponAnimInst, UEXWeaponAnimInstance* InAnimInstSecondary)
	{
		WeaponAnim = InWeaponAnimInst; 
		WeaponAnimSecondary = InAnimInstSecondary;
	}
	void StartSlowRes();
	void StopSlowRes();

	void SetADS(bool bVal);

	void SetDead(bool bInDead);
	void SetGibbed(bool bInGibbed);
	void SetIsFalling(bool bInFalling);
	void SetCrouchAmount(float InCrouchAmount);
protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Character Animation")
	void OnWeaponStateChanged(EWeaponState WeaponState, float Modifier);
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Animation")
	void OnWeaponTypeChanged(EWeaponType WeaponType);
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Animation")
	void ADSChanged();

	bool bSlowRes = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation")
	class AEXCharacter* EXCharacter = nullptr;
	UPROPERTY()
	class UEXCharacterMovement* EXCharacterMovement = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	float WalkForward = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	float WalkRight = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	bool bInAir = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	bool bDead = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	bool bGibbed = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	bool bADS = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	bool bSprinting = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	float Crouching = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	float Pitch = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	EWeaponState EquippedWeaponState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Animation")
	EWeaponType EquippedWeaponType;

	UPROPERTY()
	UEXWeaponAnimInstance* WeaponAnim = nullptr;
	UPROPERTY()
	UEXWeaponAnimInstance* WeaponAnimSecondary = nullptr;

};
