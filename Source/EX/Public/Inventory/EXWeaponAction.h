// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/EXReplicatedObject.h"
#include "EXTypes.h"
#include "EXWeaponAction.generated.h"

class AEXCharacter;
class UEXWeapon;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EX_API UEXWeaponAction : public UEXReplicatedObject
{
	GENERATED_BODY()

public:	
	UEXWeaponAction();

	UFUNCTION(BlueprintPure)
	AEXCharacter* GetOwningCharacter() const { return OwningCharacter; }
	UFUNCTION(BlueprintPure)
	UEXWeapon* GetOwningWeapon() const { return OwningWeapon; }
	UFUNCTION(BlueprintPure)
	UEXInventoryComponent* GetOwningInventory() const { return OwningInventory; }

	UFUNCTION()
	virtual void Initialize(UEXWeapon* Weapon, EWeaponInput Type);
	UFUNCTION()
	virtual bool IsInitialized() const { return bInitialized; }

	UFUNCTION(BlueprintPure, Category = "Action")
	bool IsHeldDown() const { return bHeldDown; }

	UFUNCTION(BlueprintPure, Category = "Action")
	bool IsToggled() const { return bToggled; }

	void SetToggled(bool bVal) { bToggled = bVal; }

	UFUNCTION()
	virtual bool CanFire() const;
	UFUNCTION()
	virtual bool Fire(float WorldTimeOverride);
	UFUNCTION()
	virtual void StopFire(float WorldTimeOverride);
	UFUNCTION()
	virtual void ToggleOff(float WorldTimeOverride);

	virtual float GetSpread(float WorldTimeOverride) const;

	virtual uint8 GetAmmoPerShot() const { return 0; }

	virtual void FinishEarly() {}

protected:
	UFUNCTION()
	virtual void FireComplete();

	UFUNCTION(BlueprintImplementableEvent, Category = "Action", Meta = (DisplayName = "On Fire", ScriptName = "OnFire"))
	void K2_OnFire();
	UFUNCTION(BlueprintImplementableEvent, Category = "Action", Meta = (DisplayName = "On Stop Fire", ScriptName = "OnStopFire"))
	void K2_OnStopFire();
	UFUNCTION(BlueprintImplementableEvent, Category = "Action", Meta = (DisplayName = "On Fire Complete", ScriptName = "OnFireComplete"))
	void K2_OnFireComplete();

public:

	bool GetPreventsSprint() const { return bPreventsSprint; }
	bool IsFiring() const { return bFiring; }
	virtual float GetTimeRemaining() const { return 0.f; }
	virtual void Cancel(float WorldTimeOverride);
protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_Fire(float WorldTimeOverride);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_StopFire(float WorldTimeOverride);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_Cancel(float WorldTimeOverride);

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	float FireRate = 1.f;

	bool bHeldDown = false;

	bool bToggled = false;

	bool bFiring = false;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bPreventsSprint = true;

	UPROPERTY(BlueprintGetter = GetOwningCharacter)
	AEXCharacter* OwningCharacter = nullptr;
	UPROPERTY(BlueprintGetter = GetOwningWeapon)
	UEXWeapon* OwningWeapon = nullptr;
	UPROPERTY(BlueprintGetter = GetOwningInventory)
	UEXInventoryComponent* OwningInventory = nullptr;

	EWeaponInput InputType;
private:
	bool bInitialized = false;

};
