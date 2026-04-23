// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXInventory.h"
#include "Player/EXCharacter.h"
#include "Components/InputComponent.h"
#include "EX.h"
#include "Inventory/EXWeapon.h"
#include "Player/EXPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Inventory/EXMilkJug.h"
#include "Inventory/EXInventoryInteract.h"
#include "Misc/EXObjectiveTool.h"
#include "Components/SkeletalMeshComponent.h"
#include "Player/EXAnimInstance.h"
#include "Online/EXPlayerState.h"
#include "System/EXInteract.h"
#include "Online/EXGameStateSW.h"
#include "System/EXGameplayStatics.h"
#include "HUD/EXHUDWidget.h"
#include "Player/EXCharacterMovement.h"
#include "Player/EXLocalPlayer.h"
#include "Player/EXPersistentUser.h"
#include "Inventory/EXADSAction.h"
#include "Inventory/EXWeaponAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Inventory/EXWeaponAction.h"
#include "HUD/EXHUDAbilities.h"
#include "EXNetDefines.h"
#include "Inventory/EXAbilityTracker.h"

UEXInventoryComponent::UEXInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

	constexpr int32 ToolFrom = (int32)EEngiTool::None + 1;
	constexpr int32 ToolTo = (int32)EEngiTool::Max;
	for (int32 ToolIdx = ToolFrom; ToolIdx < ToolTo; ToolIdx++)
	{
		SpecialInventoryList.Add((EEngiTool)ToolIdx, nullptr);
	}
}

bool UEXInventoryComponent::IsInitialized() const
{
	TArray<UEXInventory*> Inventories = GetInventoryList();
	if (Inventories.Contains(nullptr))
	{
		return false;
	}

	for (const TTuple<EEngiTool, UEXInventory*>& KVP : SpecialInventoryList)
	{
		if (!KVP.Value)
		{
			return false;
		}
	}
	return true;
}

void UEXInventoryComponent::CheckInitialized()
{
	if (IsInitialized())
	{
		OnInitialized();
	}
}

void UEXInventoryComponent::OnInitialized()
{
	if (bClientInitialized)
	{
		return;
	}
	bClientInitialized = true;

	bool bAmmo = false;
	bool bHeal = false;
	bool bRevive = false;
	for (UEXInventory* Inventory : InventoryList)
	{
		if (Inventory->HasAmmo())
		{
			bAmmo = true;
		}
		if (Inventory->CanHeal())
		{
			bHeal = true;
		}
		if (Inventory->CanRevive())
		{
			bRevive = true;
		}
	}
	TArray<AEXCharacter*> Teammates = UEXGameplayStatics::GetTeammates(EXCharacterOwner);
	for (AEXCharacter* Teammate : Teammates)
	{
		Teammate->SetHelpIconsToShow(bAmmo, bHeal, bRevive);
	}
	EXCharacterOwner->SetHelpActions(bAmmo, bHeal, bRevive);

	EXCharacterOwner->ApplyAugments();

	InitializeHUD();
}

void UEXInventoryComponent::InitializeHUD()
{
	if (!EXCharacterOwner->GetEXController()->GetHUDWidget())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEXInventoryComponent::InitializeHUD);
		return;
	}
	check(EXCharacterOwner->GetEXController()->IsLocalController());
	EXCharacterOwner->GetEXController()->GetHUDWidget()->SetAbilities(this);
}

void UEXInventoryComponent::CheckPendingFire()
{
	bool bAction = false;

	uint8 Mode = 0;
	uint8 CurrentPendingFire = PendingFire;
	PendingFire = 0;
	while (CurrentPendingFire > 0)
	{
		if ((CurrentPendingFire & 1) != 0)
		{
			FirePressed((EWeaponInput)Mode);
		}
		CurrentPendingFire >>= 1;
		++Mode;
	}
}

void UEXInventoryComponent::OnPossessedLocal()
{
	OnRep_Inventory();
}

void UEXInventoryComponent::SetPendingInventory(UEXInventory* Inventory)
{
	// Pending inventory should be null if calling from FireComplete, because we checked !IsPendingUnEquip()
	PendingInventory = Inventory;
}

UEXWeapon* UEXInventoryComponent::GetPrimaryWeapon() const
{
	return InventoryList.IsValidIndex(0) ? Cast<UEXWeapon>(InventoryList[0]) : nullptr;
}

void UEXInventoryComponent::AssetsLoaded()
{
	OnRep_SimulatedEquippedInventory();
}

void UEXInventoryComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bLocal = EXController && EXController->IsLocalController();
	for (UEXInventory* Inv : InventoryList)
	{
		if (!Inv || !Inv->GetComponentTickEnabled())
		{
			continue;
		}
		const bool bUpdated = Inv->Tick(DeltaTime);
		if (bLocal && bUpdated)
		{
			EXController->UpdateInventory(Inv);
		}
	}
}

void UEXInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InventoryList, PushReplicationParams::Default);
	DOREPLIFETIME_CONDITION(ThisClass, DeliverTool, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, RepairTool, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, DefuseTool, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, PlantTool, COND_InitialOnly);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, EquippedInventory, PushReplicationParams::SimulatedOnly);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsLowOnAmmo, PushReplicationParams::SimulatedOnly);
}

void UEXInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (UEXInventory* Inventory : InventoryList)
	{
		if (Inventory)
		{
			Inventory->CleanUp();
		}
	}
}

void UEXInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (UEXInventory* Inventory : InventoryList)
		{
			if (IsValid(Inventory))
			{
				AddReplicatedSubObject(Inventory);
				UEXWeapon* Weapon = Cast<UEXWeapon>(Inventory);
				if (Weapon)
				{
					for (UEXWeaponAction* Action : Weapon->GetReplicatedActions())
					{
						if (Action && Action->GetReplicates())
						{
							AddReplicatedSubObject(Action);
						}
					}
				}
			}
		}
		for (const TPair<EEngiTool, UEXInventory*> Pair : SpecialInventoryList) 
		{
			if (IsValid(Pair.Value)) 
			{
				AddReplicatedSubObject(Pair.Value);
			}
		}
	}
}

void UEXInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsAuthority())
	{
		for (const TSubclassOf<UEXInventory>& InventoryClass : DefaultInventory)
		{
			AddInventory(InventoryClass);
		}

		DeliverTool = AddEngiTool<UEXMilkJug>(MilkjugClass);
		RepairTool = AddEngiTool<UEXInventoryInteract>(RepairToolClass);
		DefuseTool = AddEngiTool<UEXInventoryInteract>(DefuseToolClass);
		PlantTool = AddEngiTool<UEXInventoryInteract>(PlantToolClass);

		EXCharacterOwner->ApplyAugments();
		GetOwner()->ForceNetUpdate();
	}
}

template <class T>
T* UEXInventoryComponent::AddEngiTool(const TSubclassOf<T>& ToolClass)
{
	if (!ensure(ToolClass))
	{
		return nullptr;
	}
	T* Tool = NewObject<T>(GetOwner(), ToolClass);
	if (Tool)
	{
		SpecialInventoryList.Add(Tool->GetEngiType(), Tool);
		if (IsUsingRegisteredSubObjectList())
		{
			if (IsReadyForReplication())
			{
				AddReplicatedSubObject(Tool);
			}
		}
	}
	return Tool;
}

void UEXInventoryComponent::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &UEXInventoryComponent::SelectInventory<0>);
	PlayerInputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &UEXInventoryComponent::SelectInventory<1>);
	PlayerInputComponent->BindAction("TertiaryWeapon", IE_Pressed, this, &UEXInventoryComponent::SelectInventory<2>);
	PlayerInputComponent->BindAction("QuaternaryWeapon", IE_Pressed, this, &UEXInventoryComponent::SelectInventory<3>);
	PlayerInputComponent->BindAction("QuinaryWeapon", IE_Pressed, this, &UEXInventoryComponent::SelectInventory<4>);
	PlayerInputComponent->BindAction("SenaryWeapon", IE_Pressed, this, &UEXInventoryComponent::SelectInventory<5>);
	PlayerInputComponent->BindAction("SeptenaryWeapon", IE_Pressed, this, &UEXInventoryComponent::SelectInventory<6>);
	PlayerInputComponent->BindAction("PrimaryAbility", IE_Pressed, this, &UEXInventoryComponent::QuickUsePressed<0>);
	PlayerInputComponent->BindAction("PrimaryAbility", IE_Released, this, &UEXInventoryComponent::QuickUseReleased<0>);
	PlayerInputComponent->BindAction("SecondaryAbility", IE_Pressed, this, &UEXInventoryComponent::QuickUsePressed<1>);
	PlayerInputComponent->BindAction("SecondaryAbility", IE_Released, this, &UEXInventoryComponent::QuickUseReleased<1>);
	PlayerInputComponent->BindAction("PassiveAbility", IE_Pressed, this, &UEXInventoryComponent::QuickUsePressed<2>);
	PlayerInputComponent->BindAction("PassiveAbility", IE_Released, this, &UEXInventoryComponent::QuickUseReleased<2>);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &UEXInventoryComponent::FirePressed<EWeaponInput::Primary>);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &UEXInventoryComponent::FireReleased<EWeaponInput::Primary>);
	PlayerInputComponent->BindAction("FireAlt", IE_Pressed, this, &UEXInventoryComponent::FirePressed<EWeaponInput::Secondary>);
	PlayerInputComponent->BindAction("FireAlt", IE_Released, this, &UEXInventoryComponent::FireReleased<EWeaponInput::Secondary>);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &UEXInventoryComponent::FirePressed<EWeaponInput::Reload>);
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &UEXInventoryComponent::FireReleased<EWeaponInput::Reload>);

	PlayerInputComponent->BindAction("QuickMelee", IE_Pressed, this, &UEXInventoryComponent::QuickUseMelee<EWeaponInput::Primary>);
	PlayerInputComponent->BindAction("QuickMeleeSecondary", IE_Pressed, this, &UEXInventoryComponent::QuickUseMelee<EWeaponInput::Secondary>);
}

void UEXInventoryComponent::SetOwner(AEXCharacter* Player)
{
	EXCharacterOwner = Player;
	MinTimeBetweenTimeStampResets = EXCharacterOwner->GetEXCharacterMovement()->MinTimeBetweenTimeStampResets;

	OnRep_SimulatedEquippedInventory();
}

UEXInventory* UEXInventoryComponent::AddInventory(const TSubclassOf<UEXInventory> InventoryClass, int32 Idx /*= -1*/)
{
	ensure(InventoryClass);

	UEXInventory* Inventory = NewObject<UEXInventory>(GetOwner(), InventoryClass);
	if (Inventory)
	{
		if (InventoryList.IsValidIndex(Idx))
		{
			InventoryList[Idx] = Inventory;
		}
		else
		{
			InventoryList.Add(Inventory);
		}
		OnInventoryChanged();
		if (IsUsingRegisteredSubObjectList())
		{
			if (IsReadyForReplication())
			{
				AddReplicatedSubObject(Inventory);
				UEXWeapon* Weapon = Cast<UEXWeapon>(Inventory);
				if (Weapon)
				{
					for (UEXWeaponAction* Action : Weapon->GetReplicatedActions())
					{
						if (Action && Action->GetReplicates())
						{
							AddReplicatedSubObject(Action);
						}
					}
				}
			}
		}
		else
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(UEXInventoryComponent, InventoryList, this);
		}
		return Inventory;
	}

	return nullptr;
}

void UEXInventoryComponent::RemoveInventory(int32 Idx)
{
	check(InventoryList.IsValidIndex(Idx));
	check(EXCharacterOwner->HasAuthority());

	UEXInventory* Inventory = InventoryList[Idx];
	InventoryList[Idx] = nullptr;
	Inventory->Destroy();

	QuickUseInventoryList.Remove(Inventory);
	OnInventoryChanged();
}

void UEXInventoryComponent::OnInventoryChanged()
{
	for (UEXInventory* Inv : InventoryList)
	{
		// Find the melee weapon and store it separately
		UEXMeleeWeapon* Melee = Cast<UEXMeleeWeapon>(Inv);
		if (Melee)
		{
			MeleeWeapon = Melee;
			return;
		}
	}
}

void UEXInventoryComponent::EquipComplete(UEXInventory* Inventory)
{
}

void UEXInventoryComponent::ChangePerspective(bool bFirstToThird)
{
	SetMesh();
}

void UEXInventoryComponent::SetMesh()
{
	if (!EXCharacterOwner || !EXCharacterOwner->GetAnimInstance() || !EquippedInventory)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UEXInventoryComponent::SetMesh));
		return;
	}

	USkeletalMeshComponent* CharacterMesh = IsLocallyOwned() ? EXCharacterOwner->GetMesh1P() : EXCharacterOwner->GetMesh();

	USkeletalMeshComponent* WeaponMesh = EXCharacterOwner->GetWeaponMeshComponent();
	USkeletalMeshComponent* WeaponMeshSecondary = EXCharacterOwner->GetWeaponMeshComponentSecondary();

	WeaponMesh->SetSkeletalMesh(EquippedInventory->GetWeaponMesh().Get());
	EXCharacterOwner->K2_OnEquip(WeaponMesh, CharacterMesh, EquippedInventory->GetPrimaryAttachPoint(), FVector(EquippedInventory->GetScale()));
	WeaponMesh->SetAnimInstanceClass(EquippedInventory->GetAnimClass());

	UEXWeaponAnimInstance* AnimInst = Cast<UEXWeaponAnimInstance>(WeaponMesh->GetAnimInstance());
	UEXWeaponAnimInstance* AnimInstSecondary = nullptr;
	if (EquippedInventory->IsDualWield())
	{
		WeaponMeshSecondary->SetSkeletalMesh(EquippedInventory->GetWeaponMesh().Get());
		EXCharacterOwner->K2_OnEquip(WeaponMeshSecondary, CharacterMesh, EquippedInventory->GetSecondaryAttachPoint(), FVector(EquippedInventory->GetScale()));

		WeaponMeshSecondary->SetAnimInstanceClass(EquippedInventory->GetAnimClass());
		AnimInstSecondary = Cast<UEXWeaponAnimInstance>(WeaponMeshSecondary->GetAnimInstance());
	}
	else
	{
		WeaponMeshSecondary->SetSkeletalMesh(nullptr);
		AnimInstSecondary = nullptr;
	}

	EXCharacterOwner->GetAnimInstance()->SetWeaponAnimInst(AnimInst, AnimInstSecondary);
}

void UEXInventoryComponent::UnequipComplete(UEXInventory* Inventory)
{
	if (Inventory == EquippedInventory)
	{
		EquippedInventory = nullptr;
	}
	if (!PendingInventory && IsLocallyOwned())
	{
		if (PreviousInventory && PreviousInventory->CanEquip())
		{
			PendingInventory = PreviousInventory;
		}
		else
		{
			PendingInventory = GetPrimaryWeapon();
		}
	}

	if (Inventory->IsAnAbility())
	{
		EXCharacterOwner->AbilityUnequipped();
	}
	else
	{
		EXCharacterOwner->WeaponUnequipped();
	}
	if (IsLocallyOwned() && !EXCharacterOwner->IsDead())
	{
		RequestEquip(PendingInventory, !IsLocallyOwnedRemote());
	}
	PreviousInventory = Inventory;
}

bool UEXInventoryComponent::IsLocallyOwned() const
{
	return IsLocallyOwnedRemote() || (EXCharacterOwner && EXCharacterOwner->IsLocallyControlled());
}

int32 UEXInventoryComponent::AddAmmo(float MagAmount)
{
	int32 MagsGiven = 0.f;
	for (UEXInventory* Inventory : InventoryList)
	{
		if (UEXWeapon* Weapon = Cast<UEXWeapon>(Inventory))
		{
			const int32 WeaponMagsGiven = Weapon->AddAmmo(MagAmount);
			MagsGiven = FMath::Max(WeaponMagsGiven, MagsGiven);
		}
	}
	return MagsGiven;
}

void UEXInventoryComponent::OnRevive()
{
	bOwnerDead = false;
	if (IsLocallyOwned())
	{
		if (EquippedInventory)
		{
			UE_LOG(LogTemp, Error, TEXT("Inventory equipped on revive"));
			return;
		}
		UEXInventory* InventoryToEquip = PendingInventory ? PendingInventory : GetPrimaryWeapon();
		RequestEquip(InventoryToEquip);
	}
}

void UEXInventoryComponent::OnDeath()
{
	bOwnerDead = true;
	if (IsLocallyOwned() || IsAuthority())
	{
		if (IsLocallyOwned())
		{
			// If we have nothing pending then make the current weapon pending
			if (!PendingInventory || IsSpecial(PendingInventory))
			{
				if (!IsSpecial(EquippedInventory))
				{
					PendingInventory = EquippedInventory;
				}
			}
		}
		if (EquippedInventory)
		{
			EquippedInventory->Unequip();
			GetWorld()->GetTimerManager().ClearAllTimersForObject(EquippedInventory);
			EquippedInventory->UnequipComplete();
		}
	}
}

void UEXInventoryComponent::EquipTool(EEngiTool Tool, AEXInteract* Objective, bool bLocalOnly)
{
	if (IsAuthority())
	{
		InteractObjective = Objective;
	}
	RequestEquip(GetTool(Tool), bLocalOnly);
}

UEXInventory* UEXInventoryComponent::GetTool(EEngiTool Tool)
{
	check(Tool != EEngiTool::None);
	return SpecialInventoryList[Tool];
}

bool UEXInventoryComponent::IsSpecial(UEXInventory* Inventory) const
{
	if (!Inventory)
	{
		return false;
	}
	for (const TTuple<EEngiTool, UEXInventory*>& KVP : SpecialInventoryList)
	{
		if (Inventory == KVP.Value)
		{
			return true;
		}
	}
	return false;
}

void UEXInventoryComponent::SelectInventory(int32 Index)
{
	if (!InventoryList.IsValidIndex(Index) || !InventoryList[Index])
	{
		return;
	}

	RequestEquip(InventoryList[Index]);
}

void UEXInventoryComponent::QuickUsePressed(int32 Index)
{
	if (!QuickUseInventoryList.IsValidIndex(Index) || !QuickUseInventoryList[Index])
	{
		return;
	}

	UEXInventory* AbilityComp = QuickUseInventoryList[Index];
	if (AbilityComp->CanUse())
	{
		if (AbilityComp->IsEquipped())
		{
			const float WorldTime = GetWorld()->GetTimeSeconds();
			bool bFired = AbilityComp->Fire(EWeaponInput::Primary, WorldTime);
		}
		else
		{
			AbilityComp->QuickUse(EWeaponInput::Primary);
		}
	}
}

void UEXInventoryComponent::QuickUseReleased(int32 Index)
{
	if (!QuickUseInventoryList.IsValidIndex(Index) || !QuickUseInventoryList[Index])
	{
		return;
	}

	UEXInventory* Ability = QuickUseInventoryList[Index];
	if (Ability->IsUnequipping())
	{
		return; // Nothing to do
	}
	if (Ability->IsEquipped() && !Ability->IsEquipping())
	{
		const float WorldTime = GetWorld()->GetTimeSeconds();
		Ability->StopFire(EWeaponInput::Primary, WorldTime);
		Unequip(Ability, false);
	}
	else
	{
		Ability->ReleaseImmediately();
	}
}

template<EWeaponInput Mode>
void UEXInventoryComponent::QuickUseMelee()
{
	if (MeleeWeapon)
	{
		const float WorldTime = GetWorld()->GetTimeSeconds();
		if (MeleeWeapon->IsEquipped())
		{
			MeleeWeapon->Fire(Mode, WorldTime);
		}
		else
		{
			MeleeWeapon->QuickUse(Mode);
		}
	}
}

void UEXInventoryComponent::RequestEquip(UEXInventory* InventoryToEquip, bool bLocalOnly)
{
	if (EquippedInventory == InventoryToEquip)
	{
		return; // Nothing to do
	}
	if (bOwnerDead)
	{
		PendingInventory = InventoryToEquip;
		return;
	}
	if (!InventoryToEquip || !InventoryToEquip->CanEquip())
	{
		return;
	}

	if (!EquippedInventory)
	{
		PendingFire = HoldingFire;
		Equip(InventoryToEquip, bLocalOnly || !IsLocallyOwnedRemote());
	}
	else
	{
		PendingInventory = InventoryToEquip;
		Unequip(EquippedInventory, bLocalOnly || !IsLocallyOwnedRemote());
	}
}

void UEXInventoryComponent::Equip(UEXInventory* Inventory, bool bLocalOnly, float ClientTimeStamp) // #CHECKING_NEARLY_COMPLETE
{
	ensure(!(IsLocallyOwned() && EquippedInventory) && "Can't equip because something is already equipped");

	if (EquippedInventory && EquippedInventory->IsNonOwningAuthority()) // Do we still have something else equipped?
	{
		// Is EquippedInventory almost down?
		if (EquippedInventory->IsUnequipping() && EquippedInventory->IsActionNearlyComplete(EWeaponState::Unequipping))
		{
			EquippedInventory->FinishCurrentState(EWeaponState::Unequipping);
		}
		else if (EquippedInventory->IsAnAbility() && EquippedInventory->IsFiring()
			&& EquippedInventory->IsActionNearlyComplete(EWeaponState::Firing))
		{
			// Abilities are different, because they don't unequip after firing, but go straight to unequipped
			EquippedInventory->FinishCurrentState(EWeaponState::Firing);
		}
		else
		{
			// #CORRECTION
			GetWorld()->GetTimerManager().ClearAllTimersForObject(EquippedInventory);
			EquippedInventory->UnequipComplete();
		}
	}

	if (!Inventory->Equip())
	{
		return;
	}
	EquippedInventory = Inventory;
	MARK_PROPERTY_DIRTY_FROM_NAME(UEXInventoryComponent, EquippedInventory, this);

	if (IsAuthority())
	{
		if (Inventory->IsAnAbility())
		{
			EXCharacterOwner->AbilityEquipped(Inventory->GetAbilityType());
		}
		else
		{
			EXCharacterOwner->WeaponEquipped(Inventory->GetWeaponType());
		}
		LatestEquipIdx = WeaponSpeedModifiers.GetNextIndex(LatestEquipIdx);
		WeaponSpeedModifiers[LatestEquipIdx] = TTuple<float, float>(ClientTimeStamp, EquippedInventory->GetMovementModifier());
	}
	if (IsLocallyOwned())
	{
		SetMesh();
		EXCharacterOwner->GetEXController()->UpdateInventory(Inventory);

		if (!bLocalOnly && IsLocallyOwnedRemote())
		{
			Server_Reliable_Equip(EquippedInventory, EXCharacterOwner->GetEXCharacterMovement()->GetClientTimeStamp());
		}
	}
	ClearPendingInventory();
}

void UEXInventoryComponent::Unequip(UEXInventory* InventoryToUnequip, bool bLocalOnly)
{
	if (IsEquippedInventory(InventoryToUnequip)) // If it's equipped try to unequip it
	{
		if (!InventoryToUnequip->Unequip())
		{
			return;
		}
	}
	else if (PendingInventory == InventoryToUnequip) // If it's pending remove it from pending
	{
		ClearPendingInventory();
	}
	if (!bLocalOnly)
	{
		if (IsLocallyOwnedRemote())
		{
			Server_Reliable_Unequip(InventoryToUnequip);
		}
		else if (IsNetMode(NM_DedicatedServer))
		{
			Client_Reliable_RequestUnequip(InventoryToUnequip);
		}
	}
}

void UEXInventoryComponent::Server_Reliable_Equip_Implementation(UEXInventory* InventoryToEquip, float ClientTimeStamp)
{
	Equip(InventoryToEquip, true, ClientTimeStamp);
}

bool UEXInventoryComponent::Server_Reliable_Equip_Validate(UEXInventory* InventoryToEquip, float ClientTimeStamp)
{
	return true;
}

void UEXInventoryComponent::Server_Reliable_Unequip_Implementation(UEXInventory* InventoryToUnequip)
{
	Unequip(InventoryToUnequip, true);
}

bool UEXInventoryComponent::Server_Reliable_Unequip_Validate(UEXInventory* InventoryToUnequip)
{
	return true;
}

void UEXInventoryComponent::FirePressed(EWeaponInput Mode)
{
	HoldingFire |= (1 << (uint8)Mode);
	if (EquippedInventory)
	{
		if (!EquippedInventory->FirePressed(Mode))
		{
			PendingFire |= (1 << (uint8)Mode);
		}
	}
}

void UEXInventoryComponent::FireReleased(EWeaponInput Mode)
{
	HoldingFire &= ~(1 << (uint8)Mode);
	if (EquippedInventory)
	{
		EquippedInventory->FireReleased(Mode);
	}
	PendingFire &= ~(1 << (uint8)Mode);
}

void UEXInventoryComponent::ApplyDeferredFireInputs()
{
	for (FDeferredFireInput& Input : DeferredFireInputs)
	{
		if (Input.bStartFire)
		{
			if (!EXCharacterOwner->IsMoveInputIgnored())
			{
				FirePressed(Input.Mode);
			}
		}
		else
		{
			FireReleased(Input.Mode);
		}
	}
	DeferredFireInputs.Empty();
}

void UEXInventoryComponent::EngiToolEquipped()
{
	if (InteractObjective)
	{
		InteractObjective->StartProgress(EXCharacterOwner);
		InteractObjective = nullptr;
	}
}

void UEXInventoryComponent::OnRep_Inventory()
{
	QuickUseInventoryList.Empty();
	AEXPlayerController* EXPC = EXCharacterOwner->GetEXController();

	for (UEXInventory* Inventory : InventoryList)
	{
		if (Inventory)
		{
			Inventory->PreInitialize();

			if (Inventory->GetQuickUseType() != EQuickUseType::None)
			{
				if (Cast<UEXMeleeWeapon>(Inventory))
				{
					continue; // Don't add melee to the list, because it's using its own dedicated binds
				}
				QuickUseInventoryList.Add(Inventory);
				if (EXPC)
				{
					EXPC->UpdateInventory(Inventory);
				}
			}
		}
	}
	OnInventoryChanged();
	if (IsLocallyOwnedRemote() && !EquippedInventory && GetPrimaryWeapon())
	{
		FirstEquip();

		CheckInitialized();
	}
	OnRep_SimulatedEquippedInventory();
}

void UEXInventoryComponent::FirstEquip()
{
	UEXInventory* PrimaryWeapon = GetPrimaryWeapon();
	if (IsAuthority())
	{
		Equip(PrimaryWeapon, true);
	}
	else
	{
		if (PrimaryWeapon && PrimaryWeapon->IsInitialized())
		{
			RequestEquip(PrimaryWeapon, false);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEXInventoryComponent::FirstEquip);
		}
	}
}

void UEXInventoryComponent::OnRep_RepairTool()
{
	SpecialInventoryList[EEngiTool::Repair] = RepairTool;
	if (IsLocallyOwnedRemote())
	{
		CheckInitialized();
	}
}

void UEXInventoryComponent::OnRep_PlantTool()
{
	SpecialInventoryList[EEngiTool::Plant] = PlantTool;
	if (IsLocallyOwnedRemote())
	{
		CheckInitialized();
	}
}

void UEXInventoryComponent::OnRep_DefuseTool()
{
	SpecialInventoryList[EEngiTool::Defuse] = DefuseTool;
	if (IsLocallyOwnedRemote())
	{
		CheckInitialized();
	}
}

void UEXInventoryComponent::OnRep_DeliverTool()
{
	SpecialInventoryList[EEngiTool::Deliver] = DeliverTool;
	if (IsLocallyOwnedRemote())
	{
		CheckInitialized();
	}
}

void UEXInventoryComponent::OnRep_LowOnAmmo()
{
	EXCharacterOwner->SetShowAmmoIcon(bIsLowOnAmmo);
}

void UEXInventoryComponent::SetWantsToSprint(bool bSprinting)
{
	if (IsLocallyOwned() && bSprinting)
	{
		if (EquippedInventory && EquippedInventory->IsReloading())
		{
			GetEquippedInventory()->CancelReload();
		}
	}
}

void UEXInventoryComponent::RecheckLowAmmo()
{
	bool bAnyWeaponLowOnAmmo = false;
	for (UEXInventory* Inventory : InventoryList)
	{
		if (UEXWeapon* Weapon = Cast<UEXWeapon>(Inventory))
		{
			if (Weapon->IsLowOnAmmo())
			{
				bAnyWeaponLowOnAmmo = true;
				break;
			}
		}
	}
	if (bAnyWeaponLowOnAmmo != bIsLowOnAmmo)
	{
		bIsLowOnAmmo = bAnyWeaponLowOnAmmo;
		MARK_PROPERTY_DIRTY_FROM_NAME(UEXInventoryComponent, bIsLowOnAmmo, this);
	}
}

bool UEXInventoryComponent::IsDualWielding() const
{
	return EquippedInventory && EquippedInventory->IsDualWield();
}

void UEXInventoryComponent::ReinitializeInventorySettings()
{
	for (UEXInventory* Inventory : InventoryList)
	{
		if (Inventory->IsInitialized()) // Else it will do it itself in the initialization
		{
			Inventory->LoadSettings();
		}
	}
}

float UEXInventoryComponent::GetSpeedModifier(float ClientTimeStamp)
{
	if (IsNetMode(NM_DedicatedServer))
	{
		int32 WindowEndIdx = LatestEquipIdx;
		int32 WindowStartIdx = WeaponSpeedModifiers.GetPreviousIndex(WindowEndIdx);
		for (int32 Idx = WEAPON_TIMESTAMP_BUFFER_SIZE - 1; Idx > 0; --Idx)
		{
			const float WindowStart = WeaponSpeedModifiers[WindowStartIdx].Key;
			const float WindowEnd = WeaponSpeedModifiers[WindowEndIdx].Key;
			if ((WindowStart < ClientTimeStamp) && (ClientTimeStamp <= WindowEnd))
			{
				return WeaponSpeedModifiers[WindowStartIdx].Value;
			}
			if (WindowEnd < WindowStart) // Client time stamp was reset
			{
				if ((WindowStart - MinTimeBetweenTimeStampResets <= ClientTimeStamp) && (ClientTimeStamp < WindowEnd))
				{
					return WeaponSpeedModifiers[WindowStartIdx].Value;
				}
				if ((WindowStart <= ClientTimeStamp) && (ClientTimeStamp < WindowEnd + MinTimeBetweenTimeStampResets))
				{
					return WeaponSpeedModifiers[WindowStartIdx].Value;
				}
			}
			WindowEndIdx = WindowStartIdx;
			WindowStartIdx = WeaponSpeedModifiers.GetPreviousIndex(WindowEndIdx);
		}
	}
	// Client and server fallback result
	const UEXInventory* EquippedInv = GetEquippedInventory();
	return EquippedInv ? EquippedInv->GetMovementModifier() : 1.f;
}

bool UEXInventoryComponent::CanSprint() const
{
	return EquippedInventory ? EquippedInventory->CanSprint() : true;
}

void UEXInventoryComponent::OnRep_SimulatedEquippedInventory()
{
	if (!IsSimulatedProxy())
	{
		return;
	}

	if (EquippedInventory && EquippedInventory->GetOwningCharacter() && EXCharacterOwner)
	{
		Equip(EquippedInventory, true);
		SetMesh();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEXInventoryComponent::OnRep_SimulatedEquippedInventory);
	}
}

TArray<FSoftObjectPath> UEXInventoryComponent::GetAssets() const
{
	TArray<FSoftObjectPath> Assets;
	TArray<TSubclassOf<UEXInventory>> InventoryClasses = DefaultInventory;
	InventoryClasses.Add(RepairToolClass);
	InventoryClasses.Add(DefuseToolClass);
	InventoryClasses.Add(PlantToolClass);
	InventoryClasses.Add(MilkjugClass);
	for (const TSubclassOf<UEXInventory>& Inv : InventoryClasses)
	{
		if (Inv)
		{
			const UEXInventory* InvCDO = Inv->GetDefaultObject<UEXInventory>();
			TArray<FSoftObjectPath> InvAssets = InvCDO->GetAssets();
			for (const FSoftObjectPath& AssetRef : InvAssets)
			{
				if (!AssetRef.IsNull())
				{
					Assets.AddUnique(AssetRef);
					//UE_LOG(LogTemp, Warning, TEXT("Loaded asset: %s"), *AssetRef.GetAssetName());
				}
			}
		}
	}
	return Assets;
}

bool UEXInventoryComponent::IsAmmoFull() const
{
	for (UEXInventory* Inventory : InventoryList)
	{
		UEXWeapon* Weapon = Cast<UEXWeapon>(Inventory);
		if (Weapon && !Weapon->IsAmmoFull())
		{
			return false;
		}
	}
	return true;
}

void UEXInventoryComponent::SetToggleADS(bool bInToggleADS)
{
	bToggleADS = bInToggleADS;
	ReinitializeInventorySettings();
}

void UEXInventoryComponent::SetReloadCancelsReload(bool bInReloadCancelsReload)
{
	bReloadCancelsReload = bInReloadCancelsReload;
	ReinitializeInventorySettings();
}

void UEXInventoryComponent::Client_Reliable_RequestUnequip_Implementation(UEXInventory* InventoryToUnequip)
{
	if (EquippedInventory == InventoryToUnequip) // #EXTODO2
	{
		Unequip(InventoryToUnequip, false);
	}
}

