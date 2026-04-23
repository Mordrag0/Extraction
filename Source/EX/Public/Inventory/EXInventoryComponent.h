// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXInventory.h"
#include "Components/ActorComponent.h"
#include "EXInventoryComponent.generated.h"

class UEXMilkJug;
class AEXCarryableObjective;
class IEXObjectiveTool;
class UEXInventoryInteract;
class UEXAnimInstance;
class USkeletalMesh;
class UEXWeapon;
class UEXMeleeWeapon;
class AEXInteract;
class AEXCharacter;
class AEXPlayerController;

struct FDeferredFireInput
{
	EWeaponInput Mode;
	/** If true, call FirePressed(), false call FireReleased() */
	bool bStartFire;

	FDeferredFireInput(EWeaponInput InMode, bool bInStartFire)
		: Mode(InMode), bStartFire(bInStartFire)
	{
	}
}; 

#define WEAPON_TIMESTAMP_BUFFER_SIZE 4 // Needs to be a power of 2

/*
* InventoryComponent is the managing class for a players inventory. For individual inventory item instances, look at UInventory and child classes.
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EX_API UEXInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEXInventoryComponent();
	bool IsInitialized() const; // Have all inventory items been replicated?
	void CheckInitialized(); 
	void OnInitialized();
	void InitializeHUD();
	void CheckPendingFire();
	void OnPossessedLocal();
	void SetPendingInventory(UEXInventory* Inventory);
	void ClearPendingInventory() { PendingInventory = nullptr; }
	UEXWeapon* GetPrimaryWeapon() const;
	void AssetsLoaded();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void ReadyForReplication() override;

protected:
	bool bClientInitialized = false;

	virtual void BeginPlay() override;
	template <class T>
	T* AddEngiTool(const TSubclassOf<T>& ToolClass);
public:
	void SetupPlayerInputComponent(UInputComponent* InputComponent);
	
	void SetOwner(AEXCharacter* Player);
	void SetController(AEXPlayerController* EXPC) { EXController = EXPC; }

	void OnRevive();
	void OnDeath();

	UFUNCTION()
	virtual UEXInventory* AddInventory(const TSubclassOf<UEXInventory> InventoryClass, int32 Idx = -1);
	UFUNCTION()
	virtual void RemoveInventory(int32 Idx);

	void OnInventoryChanged();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsEquippedInventory(const UEXInventory* Inventory) const { return Inventory == EquippedInventory; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UEXInventory* GetEquippedInventory() const { return EquippedInventory; }

	void ChangePerspective(bool bFirstToThird);

	virtual void EquipComplete(UEXInventory* Inventory);
	void SetMesh();
	virtual void UnequipComplete(UEXInventory* Inventory);

	FORCEINLINE bool IsLocallyOwnedRemote() const { return GetOwnerRole() == ROLE_AutonomousProxy; }
	FORCEINLINE bool IsAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	bool IsLocallyOwned() const;
	FORCEINLINE bool IsSimulatedProxy() const { return GetOwnerRole() == ROLE_SimulatedProxy; }

	TArray<UEXInventory*> GetInventoryList() const { return InventoryList; }

	int32 AddAmmo(float MagAmount);

	void EquipTool(EEngiTool Tool, AEXInteract* Objective, bool bLocalOnly = true);
	UEXInventory* GetTool(EEngiTool Tool);

	bool IsSpecial(UEXInventory* Inventory) const;

	template<class T>
	T* GetInventory() const
	{
		for (UEXInventory* Inventory : InventoryList)
		{
			if (Inventory->IsA(T::StaticClass()))
			{
				return Cast<T>(Inventory);
			}
		}
		return nullptr;
	}

	void RequestEquip(UEXInventory* InventoryToEquip, bool bLocalOnly = false);

	virtual void Equip(UEXInventory* InventoryToEquip, bool bLocalOnly, float ClientTimeStamp = -1.f);
	virtual void Unequip(UEXInventory* InventoryToUnequip, bool bLocalOnly);

	void ApplyDeferredFireInputs();

	void EngiToolEquipped();

	void ClearInteractObjective() { InteractObjective = nullptr; }

protected:
	UPROPERTY()
	AEXInteract* InteractObjective = nullptr;

	virtual void SelectInventory(int32 Index);

	template<int32 Index>
	void SelectInventory()
	{
		SelectInventory(Index);
	}

public:
	// Switch to ability and use when equipped
	virtual void QuickUsePressed(int32 Index);
	template<int32 Index>
	void QuickUsePressed()
	{
		QuickUsePressed(Index);
	}
	virtual void QuickUseReleased(int32 Index);
	template<int32 Index>
	void QuickUseReleased()
	{
		QuickUseReleased(Index); // #EXTODONOW
	}
	template<EWeaponInput Mode>
	void QuickUseMelee();

protected:
	
	// We send the client time stamp here, so that even if the moves aren't properly replicated, we still know the exact time a weapon was equipped on client
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_Reliable_Equip(UEXInventory* InventoryToEquip, float ClientTimeStamp);
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_Reliable_Unequip(UEXInventory* InventoryToUnequip);

public:
	UFUNCTION(Client, Reliable)
	virtual void Client_Reliable_RequestUnequip(UEXInventory* InventoryToUnequip);

	virtual void FirePressed(EWeaponInput Mode);
	virtual void FireReleased(EWeaponInput Mode);
protected:
	template<EWeaponInput Mode>
	void FirePressed()
	{
		new(DeferredFireInputs) FDeferredFireInput(Mode, true);
	}

	template<EWeaponInput Mode>
	void FireReleased()
	{
		new(DeferredFireInputs) FDeferredFireInput(Mode, false);
	}

	UFUNCTION()
	virtual void OnRep_Inventory();

public:
	UFUNCTION()
	void FirstEquip(); 

	TMap<EEngiTool, UEXInventory*> GetSpecialInventoryList() const { return SpecialInventoryList; }

	UFUNCTION()
	void OnRep_SimulatedEquippedInventory();

	TArray<FSoftObjectPath> GetAssets() const;
	bool IsAmmoFull() const;

	void SetToggleADS(bool bInToggleADS);
	bool GetToggleADS() const { return bToggleADS; }

	void SetReloadCancelsReload(bool bInReloadCancelsReload);
	bool GetReloadCancelsReload() const { return bReloadCancelsReload; }

protected:
	bool bToggleADS = false;

	bool bReloadCancelsReload = false;

	void ReinitializeInventorySettings();
public:
	float GetSpeedModifier(float ClientTimeStamp);

	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsDualWielding() const;

	void SetWantsToSprint(bool bSprinting);

	UEXInventory* GetPendingInventory() const { return PendingInventory; }

	void RecheckLowAmmo();
protected:
	// We use this buffer to track which weapon was equipped at a certain client time stamp, 
	// so we know which weapons speed modifier to use
	// Key = Client time stamp, Value = Speed modifier of the equipped weapon
	TCircularBuffer<TTuple<float, float>> WeaponSpeedModifiers 
		= TCircularBuffer<TTuple<float, float>>(WEAPON_TIMESTAMP_BUFFER_SIZE, TTuple<float, float>(-1.f, 1.f));
	int32 LatestEquipIdx = WEAPON_TIMESTAMP_BUFFER_SIZE - 1;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<UEXInventory*> InventoryList;
	TArray<UEXInventory*> QuickUseInventoryList;
	UPROPERTY()
	TMap<EEngiTool, UEXInventory*> SpecialInventoryList;

	/// Can't replicate a TMap - workaround
	UPROPERTY(ReplicatedUsing = OnRep_RepairTool)
	UEXInventoryInteract* RepairTool = nullptr;
	UFUNCTION()
	void OnRep_RepairTool(); 
	UPROPERTY(ReplicatedUsing = OnRep_PlantTool)
	UEXInventoryInteract* PlantTool = nullptr;
	UFUNCTION()
	void OnRep_PlantTool();
	UPROPERTY(ReplicatedUsing = OnRep_DefuseTool)
	UEXInventoryInteract* DefuseTool = nullptr;
	UFUNCTION()
	void OnRep_DefuseTool(); 
	UPROPERTY(ReplicatedUsing = OnRep_DeliverTool)
	UEXMilkJug* DeliverTool = nullptr;
	UFUNCTION()
	void OnRep_DeliverTool(); 
	///

	UPROPERTY(ReplicatedUsing = OnRep_SimulatedEquippedInventory)
	UEXInventory* EquippedInventory = nullptr;
	UPROPERTY()
	UEXInventory* PendingInventory = nullptr;

	bool bOwnerDead = false;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<UEXInventory>> DefaultInventory;


	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UEXMilkJug> MilkjugClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UEXInventoryInteract> RepairToolClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UEXInventoryInteract> DefuseToolClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UEXInventoryInteract> PlantToolClass = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_LowOnAmmo)
	bool bIsLowOnAmmo;

	UFUNCTION()
	void OnRep_LowOnAmmo();

	UPROPERTY()
	class AEXCharacter* EXCharacterOwner = nullptr;
	UPROPERTY()
	class AEXPlayerController* EXController = nullptr;
	// #EXTODO to reload we don't clear pending fire on release and it will reload when ready OR we check holding fire
	uint8 PendingFire = 0; 
	uint8 HoldingFire = false;

	TArray<FDeferredFireInput, TInlineAllocator<2>> DeferredFireInputs;

	UPROPERTY()
	UEXInventory* PreviousInventory = nullptr;

	UPROPERTY()
	UEXMeleeWeapon* MeleeWeapon = nullptr;

	float MinTimeBetweenTimeStampResets = -1.f;

};
