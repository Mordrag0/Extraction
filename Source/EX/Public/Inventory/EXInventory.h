// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/EXReplicatedObject.h"
#include "EXTypes.h"
#include "EX.h"
#include "EXInventory.generated.h"

class AEXCharacter;
class AEXPlayerController;
class UTexture2D;
class UAnimInstance;
class UEXAnimInstance;
class USkeletalMesh;
class UEXPersistentUser;
class UEXADSAction;
class UEXInventoryComponent;

/*
* Inventory is the class for individual item instances for a players inventory (weapons, abilities, engi tools, etc). Inventory management, look at UInventoryComponent.
*/

UCLASS(Blueprintable)
class EX_API UEXInventory : public UEXReplicatedObject
{
	GENERATED_BODY()

public:
	UEXInventory();

	void PostInitProperties() override;
protected:
	virtual void BeginPlay();
public:

	virtual bool IsInitialized() const { return true; }

	UFUNCTION(BlueprintPure)
	FName GetDisplayName() const { return DisplayName; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual USkeletalMeshComponent* GetWeaponMeshComponent() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual USkeletalMeshComponent* GetWeaponMeshComponentSecondary() const;

	UFUNCTION(BlueprintPure, Category = "Utilities")
	AEXCharacter* GetOwningCharacter() const { return EXCharacterOwner; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsDualWield() const { return bDualWield; }
	UFUNCTION(BlueprintPure)
	TSubclassOf<UEXWeaponAnimInstance> GetAnimClass() const { return AnimClass; }
	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<USkeletalMesh> GetWeaponMesh() const { return WeaponMesh; }
	UFUNCTION(BlueprintPure)
	FName GetPrimaryAttachPoint() const { return PrimaryAttachPoint; }
	UFUNCTION(BlueprintPure)
	FName GetSecondaryAttachPoint() const { return SecondaryAttachPoint; }
	UFUNCTION(BlueprintPure)
	float GetScale() const { return Scale; }

	EQuickUseType GetQuickUseType() const { return QuickUseType; }
	virtual EEngiTool GetEngiType() const { return EEngiTool::None; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsPendingUnequip() const;

	void ReleaseImmediately();

	bool IsAnAbility() const { return Ability != EAbility::None; }

	EAbility GetAbilityType() const { return Ability; }
	EWeapon GetWeaponType() const { return Weapon; }

	bool HasAmmo() const { return bCanGiveAmmo; }
	bool CanRevive() const { return bCanRevive; }
	bool CanHeal() const { return bCanHeal; }

	FORCEINLINE bool GetComponentTickEnabled() const { return bComponentTickEnabled; }
protected:
	void SetComponentTickEnabled(bool bInEnabled);
	bool bComponentTickEnabled;

	// Do we release ability as soon as it's ready? For example we tap the quick use button but it needs to be equipped first
	bool bPendingRelease = false; 

public:
	virtual void PreInitialize();
	virtual void CleanUp();

	virtual bool Tick(float DeltaTime);


	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool IsIdle() const { return WeaponState == EWeaponState::Ready; }
	//Returns true if weapon is currently equipped (does not mean this weapon is not being brought up/put down).
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool IsEquipped() const { return WeaponState != EWeaponState::Down; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool IsFiring() const { return WeaponState == EWeaponState::Firing; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool IsReloading() const { return WeaponState == EWeaponState::Reloading; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool IsEquipping() const { return WeaponState == EWeaponState::Equipping; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool IsUnequipping() const { return WeaponState == EWeaponState::Unequipping; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool IsHolding() const { return WeaponState == EWeaponState::Holding; }

	// For the server to check if a client could have reasonably completed the specified action listed.
	virtual bool IsActionNearlyComplete(EWeaponState State) const;

	virtual float GetActionTimeRemaining(EWeaponState State) const;

	UFUNCTION()
	virtual TArray<FSoftObjectPath> GetAssets() const;

	void SetVisibility(bool bInVisible);

	virtual bool FirePressed(EWeaponInput Mode);
	virtual bool FireReleased(EWeaponInput Mode);

	// #EXTODO some abilities (eg goggles) cannot be equipped at all
	UFUNCTION()
	virtual bool CanEquip() const;
	UFUNCTION()
	virtual bool Equip();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", Meta = (DisplayName = "On Equip", ScriptName = "OnEquip"))
	void K2_Equip();
	UFUNCTION()
	virtual void EquipComplete();

	UFUNCTION()
	virtual bool CanUnequip() const;
	UFUNCTION()
	virtual bool Unequip();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", Meta = (DisplayName = "On UnEquip", ScriptName = "OnUnEquip"))
	void K2_Unequip();
	UFUNCTION()
	virtual void UnequipComplete();

	UFUNCTION()
	virtual bool CanFire(EWeaponInput Mode) const;
	UFUNCTION()
	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", Meta = (DisplayName = "On Fire", ScriptName = "OnFire"))
	void K2_Fire(EWeaponInput Mode);
	UFUNCTION()
	virtual void FireComplete(EWeaponInput Mode);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", Meta = (DisplayName = "Fire Complete", ScriptName = "FireComplete"))
	void K2_FireComplete(EWeaponInput Mode);

	UFUNCTION()
	virtual bool StopFire(EWeaponInput Mode, float WorldTimeOverride);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", Meta = (DisplayName = "On Fire Released", ScriptName = "OnFireReleased"))
	void K2_StopFire(EWeaponInput Mode);

	virtual void UseAbility(float WorldTimeOverride);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", Meta = (DisplayName = "Use Ability", ScriptName = "UseAbility"))
	void K2_UseAbility();

	UFUNCTION(Server, Reliable)
	virtual void Server_Reliable_UseAbility(float WorldTimeOverride);

	UFUNCTION()
	virtual void SetWeaponState(EWeaponState InWeaponState);

	UFUNCTION()
	virtual void QuickUse(EWeaponInput Mode);

	void SetModifier(EWeaponState State, float Modifier) { Modifiers[State] = Modifier; }
	FORCEINLINE float GetModifier(EWeaponState State) const { return Modifiers[State]; }

	UFUNCTION()
	void AddUses(float Amount = 1.f);

protected:
	FName DisplayName = NAME_None;

	// If true, we fired using quickuse, so we unequip after
	bool bQuickUse = false;
	EWeaponInput QuickUseMode;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	bool bCanBeEquiped = true;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float EquipRate = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float FireRate = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float ReloadRate = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float ChangeFireModeRate = 1.f;
	
	TMap<EWeaponState, float> Modifiers;

	virtual void Cancel();
public:
	FORCEINLINE float GetUses() const { return Uses; }
	FORCEINLINE float GetMaxUses() const { return MaxUses; }
	void SetUses(float InUses) { Uses = InUses; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }
	virtual bool CanUse(float Amount = 1.f) const;
	virtual bool UsesAbility() const { return bUsesAbility; }
	void RecoverAbility(float Multiplier);

	TSoftObjectPtr<UTexture2D> GetIcon(EIconSize Size) const;

	float GetMovementModifier() const { return MovementModifier; }
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float MovementModifier = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	bool bUsesAbility = false;
	UPROPERTY(Transient)
	float Uses;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float MaxUses = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float CooldownTime = 10.f;
	// Ratio of cooldown lowered when ability is picked up
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", Meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
	float RecoverAmount = .5f;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	bool bShootOnFireReleased = false;

	bool UpdateCooldowns(float DeltaTime);
	UFUNCTION()
	void Use(float Amount = 1.f);
	UFUNCTION(Client, Reliable)
	void Client_Reliable_SetUses(float Amount);
public:
	float ReduceCooldown(float OldUses, float Seconds);

	virtual bool CanSprint() const;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	bool bPreventSprintWhenActive = true;

	UFUNCTION()
	void HitOnClient(AActor* Actor, bool bHeadShot);

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	bool bUsedWhenDead = false;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	EQuickUseType QuickUseType = EQuickUseType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetWeaponMesh)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = IsDualWield)
	bool bDualWield = false;
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetPrimaryAttachPoint)
	FName PrimaryAttachPoint = FName("GripPoint");
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetSecondaryAttachPoint)
	FName SecondaryAttachPoint = FName("GripPoint2");
	UPROPERTY()
	AEXCharacter* EXCharacterOwner = nullptr;
	UPROPERTY()
	UEXInventoryComponent* InventoryOwner = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float Scale = 1.f;

	UPROPERTY(ReplicatedUsing = OnRep_Visibility)
	bool bVisible = true;
	UFUNCTION()
	void OnRep_Visibility();

	UPROPERTY()
	EWeaponState WeaponState = EWeaponState::Down;
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Rifle;
	UPROPERTY(EditDefaultsOnly)
	bool bCanGiveAmmo = false;
	UPROPERTY(EditDefaultsOnly)
	bool bCanRevive = false;
	UPROPERTY(EditDefaultsOnly)
	bool bCanHeal = false;
	UPROPERTY(EditDefaultsOnly)
	bool bHideOnUse = false;

private:

	UPROPERTY(EditDefaultsOnly)
	EWeapon Weapon = EWeapon::None;
	UPROPERTY(EditDefaultsOnly)
	EAbility Ability = EAbility::None;

protected:
	
	FTimerHandle TimerHandle_Equip;
	FTimerHandle TimerHandle_Unequip;
	FTimerHandle TimerHandle_Fire; // Used by abilities, weapons use fire modes

	virtual bool IsReadyToPerformAction(EWeaponState Action) const;

public:
	virtual void FinishCurrentState(EWeaponState State);
	virtual void CancelReload() {}
protected:
	// For abilities only, things like medpacks can't be equipped when on cooldown
	bool bUnequipAbilityWhenEmpty = false;

	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconKillFeed = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconSmall = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconMedium = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconLarge = nullptr;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetAnimClass)
	TSubclassOf<UEXWeaponAnimInstance> AnimClass;

	UPROPERTY()
	const UEXPersistentUser* PersistentUser = nullptr;
	UPROPERTY()
	AEXPlayerController* EXController = nullptr;
	bool bPreInit = false;

public:
	virtual void LoadSettings() {}
};
