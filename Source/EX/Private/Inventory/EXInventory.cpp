// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXInventory.h"
#include "Inventory/EXInventoryComponent.h"
#include "Player/EXCharacter.h"
#include "EX.h"
#include "Player/EXPlayerController.h"
#include "Inventory/EXWeapon.h"
#include "System/EXGameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EXLocalPlayer.h"
#include "EXNetDefines.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// #DEBUG
static TAutoConsoleVariable<int32> CVarLogWeaponState(
	TEXT("x.LogWeaponState"),
	0,
	TEXT("Log weapon state changes to console.\n")
	TEXT("  0: off\n")
	TEXT("  1: on\n"),
	ECVF_Cheat);
#endif

UEXInventory::UEXInventory()
{
	int32 Count = (int32)EWeaponState::Max;
	for (int32 StateIdx = 0; StateIdx < Count; StateIdx++)
	{
		EWeaponState State = (EWeaponState)StateIdx;
		Modifiers.Add(State, 1.f);
	}
}

void UEXInventory::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}
	BeginPlay();
}

void UEXInventory::BeginPlay()
{
	PreInitialize();

	if (bUsesAbility)
	{
		SetComponentTickEnabled(true);
	}
	Uses = MaxUses;
}

USkeletalMeshComponent* UEXInventory::GetWeaponMeshComponent() const
{
	return GetOwningCharacter()->GetWeaponMeshComponent();
}

USkeletalMeshComponent* UEXInventory::GetWeaponMeshComponentSecondary() const
{
	return GetOwningCharacter()->GetWeaponMeshComponentSecondary();
}

void UEXInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(UEXInventory, bVisible, PushReplicationParams::SimulatedOnly);
}

bool UEXInventory::IsPendingUnequip() const
{
	UEXInventory* PendingInventory = InventoryOwner->GetPendingInventory();
	return ((PendingInventory != nullptr) && (PendingInventory != this));
}

void UEXInventory::ReleaseImmediately()
{
	bPendingRelease = true;
}

void UEXInventory::SetComponentTickEnabled(bool bInEnabled)
{
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		return;
	}
	bComponentTickEnabled = bInEnabled;
}

void UEXInventory::PreInitialize()
{
	if (bPreInit)
	{
		return;
	}
	bPreInit = true;
	EXCharacterOwner = Cast<AEXCharacter>(GetOwningActor());
	InventoryOwner = EXCharacterOwner ? EXCharacterOwner->GetInventoryComponent() : nullptr;
	EXController = EXCharacterOwner ? Cast<AEXPlayerController>(EXCharacterOwner->GetController()) : nullptr;

	if (IsLocallyOwned())
	{
		UEXLocalPlayer* LP = EXController ? Cast<UEXLocalPlayer>(EXController->GetLocalPlayer()) : nullptr;
		PersistentUser = LP ? LP->GetPersistentUser() : nullptr;
	}

	ensure(EXCharacterOwner && ("could not establish inventory owner on initialize"));
	ensure(InventoryOwner && ("could not establish inventory owner component on initialize"));
	if (IsLocallyOwned()) {

		ensure(EXController && ("could not establish inventory controller on initialize"));
	}
}

void UEXInventory::CleanUp()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

bool UEXInventory::Tick(float DeltaTime)
{
	return UpdateCooldowns(DeltaTime);
}

bool UEXInventory::UpdateCooldowns(float DeltaTime)
{
	if (Uses < MaxUses)
	{
		Uses = ReduceCooldown(Uses, DeltaTime);
		return true;
	}
	return false;
}

bool UEXInventory::IsActionNearlyComplete(EWeaponState State) const
{
	// Allowing for some variance, determined by MAX_VARIANCE_ALLOWED
	if (WeaponState != State)
	{
		UE_LOG(LogEXInventory, Error, TEXT("IsActionNearlyComplete checking wrong state (%s, %s)"),
			*UEnum::GetValueAsString(WeaponState),
			*UEnum::GetValueAsString(State));
		return false;
	}

	return (GetActionTimeRemaining(State) < MAX_VARIANCE_ALLOWED);
}

float UEXInventory::GetActionTimeRemaining(EWeaponState State) const
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	switch (State)
	{
	case EWeaponState::Equipping:
	{
		return TimerManager.GetTimerRemaining(TimerHandle_Equip);
	}
	case EWeaponState::Unequipping:
	{
		return TimerManager.GetTimerRemaining(TimerHandle_Unequip);
	}
	case EWeaponState::Firing:
	{
		return TimerManager.GetTimerRemaining(TimerHandle_Fire);
	}
	default:
	{
		return 0.f;
	}
	}
}

TArray<FSoftObjectPath> UEXInventory::GetAssets() const
{
	TArray<FSoftObjectPath> Assets;
	if (!WeaponMesh.IsNull())
	{
		Assets.AddUnique(WeaponMesh.ToSoftObjectPath());
	}
	if (!IconKillFeed.IsNull())
	{
		Assets.AddUnique(IconKillFeed.ToSoftObjectPath());
	}
	if (!IconSmall.IsNull())
	{
		Assets.AddUnique(IconSmall.ToSoftObjectPath());
	}
	if (!IconMedium.IsNull())
	{
		Assets.AddUnique(IconMedium.ToSoftObjectPath());
	}
	if (!IconLarge.IsNull())
	{
		Assets.AddUnique(IconLarge.ToSoftObjectPath());
	}

	return Assets;
}

void UEXInventory::SetVisibility(bool bInVisible)
{
	if (bVisible == bInVisible)
	{
		return;
	}
	bVisible = bInVisible;
	MARK_PROPERTY_DIRTY_FROM_NAME(UEXInventory, bVisible, this);
	if (!GetOwningCharacter()->IsNetMode(NM_DedicatedServer))
	{
		if (!bPreInit)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UEXInventory::SetVisibility, bInVisible));
			return;
		}
		GetWeaponMeshComponent()->SetVisibility(bInVisible, true);
		GetWeaponMeshComponentSecondary()->SetVisibility(bInVisible && bDualWield, true);
	}
}

bool UEXInventory::FirePressed(EWeaponInput Mode)
{
	// Weapons override this to enable toggle, but inventory has no toggle option, 
	// because it would be incompatible with quickuse since you only press quickuse once to perform everything
	const float WorldTime = GetWorld()->GetTimeSeconds();
	return Fire(Mode, WorldTime);
}

bool UEXInventory::FireReleased(EWeaponInput Mode)
{
	const float WorldTime = GetWorld()->GetTimeSeconds();
	return StopFire(Mode, WorldTime);
}

bool UEXInventory::CanEquip() const
{
	if (IsSimulatedProxy())
	{
		return true;
	}

	if (IsEquipped() && !IsUnequipping())
	{
		return false;
	}

	if (bUnequipAbilityWhenEmpty && !CanUse())
	{
		return false;
	}

	return true;
}

bool UEXInventory::Equip()
{
	if (!CanEquip())
	{
		return false;
	}

	GetOwningCharacter()->WeaponTypeChanged(WeaponType);
	SetWeaponState(EWeaponState::Equipping);

	K2_Equip();
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(TimerHandle_Equip);
	TimerManager.SetTimer(TimerHandle_Equip, this, &UEXInventory::EquipComplete, EquipRate * Modifiers[EWeaponState::Equipping]);

	return true;
}

void UEXInventory::EquipComplete()
{
	SetWeaponState(EWeaponState::Ready);

	InventoryOwner->EquipComplete(this);

	// If we're quickusing, that means we want to Fire as soon as possible
	// Unless we've queued unequip, which is a way for the player to cancel quickuse

	if (IsLocallyOwned() && bQuickUse)
	{
		if (IsUnequipping())
		{
			bQuickUse = false;
		}
		else
		{
			const float WorldTime = GetWorld()->GetTimeSeconds();
			Fire(QuickUseMode, WorldTime);
			if (bPendingRelease)
			{
				StopFire(QuickUseMode, WorldTime);
				bPendingRelease = false;
			}
		}
	}
}

bool UEXInventory::CanUnequip() const // #CHECKING_NEARLY_COMPLETE
{
	if (!IsReadyToPerformAction(EWeaponState::Unequipping))
	{
		return false;
	}

	return InventoryOwner->IsEquippedInventory(this);
}

bool UEXInventory::Unequip()
{
	if (IsUnequipping())
	{
		return false;
	}
	if (!CanUnequip())
	{
		return false;
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	{
		if (WeaponState == EWeaponState::Holding)
		{
			Cancel();
		}
	}
	SetWeaponState(EWeaponState::Unequipping);


	K2_Unequip();
	TimerManager.ClearTimer(TimerHandle_Unequip);
	TimerManager.SetTimer(TimerHandle_Unequip, this, &UEXInventory::UnequipComplete, EquipRate * Modifiers[EWeaponState::Unequipping]);

	return true;
}

void UEXInventory::UnequipComplete()
{
	bQuickUse = false;

	SetVisibility(false);
	SetWeaponState(EWeaponState::Down);

	InventoryOwner->UnequipComplete(this);
}

bool UEXInventory::CanFire(EWeaponInput Mode) const // #CHECKING_NEARLY_COMPLETE
{
	if (!IsReadyToPerformAction(EWeaponState::Firing))
	{
		return false;
	}
	if (IsHolding())
	{
		return true;
	}
	if (GetOwningCharacter()->IsDead() != bUsedWhenDead)
	{
		return false;
	}
	return InventoryOwner->IsEquippedInventory(this);
}

bool UEXInventory::Fire(EWeaponInput Mode, float WorldTimeOverride) // #Fire
{
	if (!CanFire(Mode))
	{
		return false;
	}
	if (Mode != EWeaponInput::Primary)
	{
		return true;
	}

	UE_LOG(LogEXWeapon, Log, TEXT("Fire: %s"), *this->GetName());


	if (bShootOnFireReleased)
	{
		SetWeaponState(EWeaponState::Holding);
	}
	else
	{
		UseAbility(WorldTimeOverride);
	}
	K2_Fire(Mode);

	return true;
}

bool UEXInventory::StopFire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (Mode != EWeaponInput::Primary)
	{
		return true;
	}

	UE_LOG(LogEXWeapon, Log, TEXT("FireReleased: %s"), *this->GetName());
	if (bShootOnFireReleased && IsHolding()) // IsHolding will be false if player canceled
	{
		if (!CanFire(Mode))
		{
			return false;
		}
		UseAbility(WorldTimeOverride);
	}
	K2_StopFire(Mode);
	return true;
}

void UEXInventory::UseAbility(float WorldTimeOverride)
{
	if (IsLocallyOwned())
	{
		Server_Reliable_UseAbility(WorldTimeOverride);
	}

	SetWeaponState(EWeaponState::Firing);
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(TimerHandle_Fire);
	TimerManager.SetTimer(TimerHandle_Fire, FTimerDelegate::CreateUObject(this, &UEXInventory::FireComplete, EWeaponInput::Primary), FireRate * Modifiers[EWeaponState::Firing], false);
	K2_UseAbility();
}

void UEXInventory::Server_Reliable_UseAbility_Implementation(float WorldTimeOverride)
{
	UseAbility(WorldTimeOverride);
}

void UEXInventory::FireComplete(EWeaponInput Mode) // #FireComplete
{
	//The default firemode code is only run for the primary firemode.
	if (Mode != EWeaponInput::Primary)
	{
		return;
	}

	// The ability was used and is not in players hands anymore
	UnequipComplete();
	K2_FireComplete(Mode);

	if (IsLocallyOwned())
	{
		const bool bReequip = !bQuickUse && !IsPendingUnequip() && !(bUnequipAbilityWhenEmpty && !CanUse());
		if (bReequip)
		{
			InventoryOwner->SetPendingInventory(this);
		}
	}
}

void UEXInventory::SetWeaponState(EWeaponState InWeaponState)
{
	if (InWeaponState == WeaponState)
	{
		return;
	}
	const EWeaponState OldWeaponState = WeaponState;
	WeaponState = InWeaponState;
	if (GetActionTimeRemaining(WeaponState) > 0.f)
	{
		FinishCurrentState(OldWeaponState);
	}
	
	GetOwningCharacter()->WeaponStateChanged(InWeaponState, Modifiers[WeaponState]);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const int32 bLogDebug = CVarLogWeaponState.GetValueOnGameThread();
	if (bLogDebug)
	{
		if (IsAuthority())
		{
			if (GetClass()->IsChildOf(UEXWeapon::StaticClass()))
			{
				UE_LOG(LogEXWeapon, Error, TEXT("%s SetWeaponState: %s"), *GetName(), *UEnum::GetValueAsString(InWeaponState));
			}
			else
			{
				UE_LOG(LogEXInventory, Error, TEXT("%s SetWeaponState: %s"), *GetName(), *UEnum::GetValueAsString(InWeaponState));
			}
		}
		else
		{
			if (GetClass()->IsChildOf(UEXWeapon::StaticClass()))
			{
				UE_LOG(LogEXWeapon, Warning, TEXT("%s SetWeaponState: %s"), *GetName(), *UEnum::GetValueAsString(InWeaponState));
			}
			else
			{
				UE_LOG(LogEXInventory, Warning, TEXT("%s SetWeaponState: %s"), *GetName(), *UEnum::GetValueAsString(InWeaponState));
			}
		}
	}
#endif

	switch (WeaponState)
	{
	case EWeaponState::Ready:
	{
		if (IsLocallyOwned())
		{
			if (IsPendingUnequip())
			{
				InventoryOwner->Unequip(this, false);
			}
			else
			{
				InventoryOwner->CheckPendingFire();
			}
		}
		break;
	}
	case EWeaponState::Equipping:
	{
		if (!EXCharacterOwner->IsDead())
		{
			SetVisibility(true);
		}
		break;
	}
	case EWeaponState::Firing:
	{
		if (bHideOnUse)
		{
			SetVisibility(false);
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

void UEXInventory::QuickUse(EWeaponInput Mode)
{
	if (bQuickUse)
	{
		return;
	}
	check(QuickUseType != EQuickUseType::None);
	bQuickUse = (QuickUseType == EQuickUseType::Use);
	QuickUseMode = Mode;
	InventoryOwner->RequestEquip(this);
}

void UEXInventory::Cancel()
{
	SetWeaponState(EWeaponState::Ready);
}

bool UEXInventory::CanUse(float Amount /*= 1.f*/) const
{
	return (Uses >= Amount);
}

void UEXInventory::Use(float Amount /*= 1.f*/)
{
	AddUses(-Amount);
}

void UEXInventory::AddUses(float Amount /*= 1.f*/)
{
	Uses = FMath::Max(0.f, FMath::Min(MaxUses, Uses + Amount));
	if (IsLocallyOwned())
	{
		GetOwningCharacter()->GetEXController()->UpdateInventory(this);
	}
}

void UEXInventory::HitOnClient(AActor* Actor, bool bHeadshot)
{
	AEXCharacter* Player = Cast<AEXCharacter>(Actor);
	if (Player)
	{
		// Display healthbar
		Player->ShowHitOnHUD();
	}
	if (EXController)
	{
		// Show hit marker
		EXController->HitOnClient(bHeadshot);
	}
}

void UEXInventory::OnRep_Visibility()
{
	SetVisibility(bVisible);
}

bool UEXInventory::IsReadyToPerformAction(EWeaponState Action) const
{
	if (IsSimulatedProxy())
	{
		return true;
	}

	if (IsPendingUnequip())
	{
		return (Action == EWeaponState::Unequipping);
	}
	if (!IsEquipped())
	{
		return false;
	}

	if (IsFiring() || IsEquipping() || IsReloading())
	{
		if (IsNonOwningAuthority())
		{
			// Check if client performed an illegal action
			if (IsFiring() && IsActionNearlyComplete(EWeaponState::Firing))
			{
				return true;
			}
			else if (IsEquipping() && IsActionNearlyComplete(EWeaponState::Equipping))
			{
				return true;
			}
			else if (IsReloading() && IsActionNearlyComplete(EWeaponState::Reloading))
			{
				return true;
			}
			else
			{
				ensure(0);
				// #CORRECTION
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

void UEXInventory::FinishCurrentState(EWeaponState State)
{
	if (!IsActionNearlyComplete(State))
	{
		UE_LOG(LogEXInventory, Error, TEXT("Ending state thats not nearly complete (%s)"),
			*UEnum::GetValueAsString(State));
	}
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	switch (State)
	{
	case EWeaponState::Equipping:
		TimerManager.ClearTimer(TimerHandle_Equip);
		EquipComplete();
		break;
	case EWeaponState::Unequipping:
		TimerManager.ClearTimer(TimerHandle_Unequip);
		UnequipComplete();
		break;
	case EWeaponState::Firing:
		TimerManager.ClearTimer(TimerHandle_Unequip);
		UnequipComplete();
		break;
	case EWeaponState::Holding:
	case EWeaponState::Ready:
	case EWeaponState::Down:
	case EWeaponState::Reloading:
	case EWeaponState::Max:
	default:
		break;
	}
}

void UEXInventory::RecoverAbility(float Multiplier)
{
	check(IsAuthority());
	AddUses(RecoverAmount * Multiplier);
}

TSoftObjectPtr<UTexture2D> UEXInventory::GetIcon(EIconSize Size) const
{
	switch (Size)
	{
	case EIconSize::KillFeed:
		return IconKillFeed;
		break;
	case EIconSize::Small:
		return IconSmall;
		break;
	case EIconSize::Medium:
		return IconMedium;
		break;
	case EIconSize::Large:
		return IconLarge;
		break;
	default:
		return nullptr;
		break;
	}
}

void UEXInventory::Client_Reliable_SetUses_Implementation(float Amount)
{
	Uses = Amount;
	GetOwningCharacter()->GetEXController()->UpdateInventory(this);
}

float UEXInventory::ReduceCooldown(float OldUses, float DeltaTime)
{
	return FMath::Min(OldUses + DeltaTime / CooldownTime, MaxUses);
}

bool UEXInventory::CanSprint() const
{
	if (!bPreventSprintWhenActive)
	{
		// We're not preventing so always true
		return true;
	}

	return IsEquipping() || IsUnequipping() || IsIdle();
}

