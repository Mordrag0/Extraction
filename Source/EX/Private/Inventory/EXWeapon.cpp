// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXWeapon.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/EXWeaponAction.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXCharacter.h"
#include "Sound/SoundCue.h"
#include "Player/EXPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Player/EXPersistentUser.h"
#include "Misc/EXImpactEffect.h"
#include "Inventory/EXADSAction.h"
#include "System/EXGameplayStatics.h"
#include "Curves/CurveFloat.h"
#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXFireMode.h"
#include "Inventory/EXReloadAction.h"
#include "EXNetDefines.h"


UEXWeapon::UEXWeapon()
{
}

void UEXWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(UEXWeapon, ReplicatedActions, PushReplicationParams::OwnerOnly);
}

bool UEXWeapon::IsInitialized() const
{
	int32 NumReplicatedActions = 0;
	for (const TPair<EWeaponInput, TSubclassOf<UEXWeaponAction>>& ActionClassPair : ActionClassList)
	{
		if (ActionClassPair.Value->GetDefaultObject<UEXWeaponAction>()->GetReplicates())
		{
			NumReplicatedActions++;
		}
	}
	if (ReplicatedActions.Num() < NumReplicatedActions)
	{
		return false;
	}
	for (UEXWeaponAction* Action : ReplicatedActions)
	{
		if (!Action)
		{
			return false;
		}
	}
	return true;
}

void UEXWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (bUsesAbility)
	{
		SetComponentTickEnabled(true);
	}

	float MinTime;
	float MaxTime;
	if (RecoilX && RecoilY)
	{
		RecoilX->GetTimeRange(MinTime, MaxTime);
		RecoilTime = MaxTime - MinTime;
		RecoilY->GetTimeRange(MinTime, MaxTime);
		RecoilTime = FMath::Max(RecoilTime, MaxTime - MinTime);
	}
}

void UEXWeapon::PreInitialize()
{
	if (bPreInit)
	{
		return;
	}
	Super::PreInitialize();

	int32 Idx = 0;
	for (const TTuple<EWeaponInput, TSubclassOf<UEXWeaponAction>>& KVP : ActionClassList)
	{
		if (KVP.Value && KVP.Value->GetDefaultObject<UEXWeaponAction>())
		{
			const EWeaponInput ActionType = KVP.Key;
			if (ensure(!Actions.Contains(ActionType)))
			{
				// Replicated Actions should only ever be spawned on the server.
				const bool bActionReplicates = KVP.Value->GetDefaultObject<UEXWeaponAction>()->GetReplicates();
				if (IsAuthority() || (IsOwnedByLocalRemote() && !bActionReplicates))
				{
					UEXWeaponAction* NewAction = NewObject<UEXWeaponAction>(GetOwningActor(), KVP.Value);
					Actions.Add(ActionType, NewAction);
					if (IsAuthority() && bActionReplicates)
					{
						ReplicatedActions.Add(NewAction);
						MARK_PROPERTY_DIRTY_FROM_NAME(UEXWeapon, ReplicatedActions, this);
					}
					NewAction->Initialize(this, ActionType);
				}
			}
		}
		Idx++;
	}
}

bool UEXWeapon::CanFire(EWeaponInput Mode) const
{
	if (!Actions.Contains(Mode))
	{
		return false;
	}
	if (!Actions[Mode]->CanFire())
	{
		return false;
	}
	return Super::CanFire(Mode);
}

bool UEXWeapon::Fire(EWeaponInput Mode, float WorldTimeOverride) // #Fire
{
	if (!Actions.Contains(Mode))
	{
		UE_LOG(LogEXWeapon, Warning, TEXT("Invalid fire mode index"));
		return false;
	}

	if (!CanFire(Mode))
	{
		if (!HasEnoughAmmo(Actions[Mode]->GetAmmoPerShot()))
		{
			OnOutOfAmmo();
		}
		return false;
	}

	Actions[Mode]->Fire(WorldTimeOverride);

	K2_Fire(Mode);

	return true;
}

void UEXWeapon::FireComplete(EWeaponInput Mode) // #FireComplete
{
	if (UEXFireMode* FireMode = Cast<UEXFireMode>(Actions[Mode]))
	{
		if (IsLocallyOwned())
		{
			if (bQuickUse && !IsPendingUnequip())
			{
				Unequip();
			}
			else if (!HasEnoughAmmo(FireMode->GetAmmoPerShot()))
			{
				OnOutOfAmmo();
			}
		}
	}
}

bool UEXWeapon::StopFire(EWeaponInput Mode, float WorldTimeOverride) // #FireReleased
{
	if (!Actions.Contains(Mode))
	{
		return false;
	}
	UE_LOG(LogEXWeapon, Log, TEXT("FireReleased: %s"), *this->GetName());

	Actions[Mode]->StopFire(WorldTimeOverride);

	K2_StopFire(Mode);
	return true;
}

bool UEXWeapon::ToggleOff(EWeaponInput Mode, float WorldTimeOverride) // #FireReleased
{
	if (!Actions.Contains(Mode))
	{
		return false;
	}
	UE_LOG(LogEXWeapon, Log, TEXT("ToggleOff: %s"), *this->GetName());

	Actions[Mode]->ToggleOff(WorldTimeOverride);

	K2_StopFire(Mode);
	return true;
}

void UEXWeapon::PerformShot()
{
	if (IsLocallyOwned())
	{
		AddRecoil();
		PlayFireEffects();
	}
	if (IsAuthority())
	{
		Multicast_Unreliable_PlayFireEffects();
	}
}

bool UEXWeapon::FirePressed(EWeaponInput Mode)
{
	if (!Actions.Contains(Mode))
	{
		return false;
	}
	UEXWeaponAction* Action = Actions[Mode];
	const float WorldTime = GetWorld()->GetTimeSeconds();
	if (Action->IsToggled() && Action->IsHeldDown()) // We're doing this here because each action can have different settings
	{
		return ToggleOff(Mode, WorldTime);
	}
	else
	{
		return Fire(Mode, WorldTime);
	}

	return false;
}

bool UEXWeapon::FireReleased(EWeaponInput Mode)
{
	if (!Actions.Contains(Mode))
	{
		return false;
	}
	UEXWeaponAction* Action = Actions[Mode];
	if (!Action->IsToggled())
	{
		const float WorldTime = GetWorld()->GetTimeSeconds();
		return StopFire(Mode, WorldTime);
	}

	return false;
}

bool UEXWeapon::Equip()
{
	if (!Super::Equip())
	{
		return false;
	}

	if (IsLocallyOwned())
	{
		UpdateHUD();
	}
	// Recalculate spread here to update the values in the PC
	SetMovementSpreadModifier(GetOwningCharacter()->GetMovementSpreadModifier());
	return true;
}

bool UEXWeapon::Unequip()
{
	if (!Super::Unequip())
	{
		return false;
	}

	StopFireEffects();

	if (IsLocallyOwned())
	{
		for (const TTuple<EWeaponInput, UEXWeaponAction*>& WeaponAction : Actions)
		{
			if (WeaponAction.Value->IsHeldDown())
			{
				const float WorldTime = GetWorld()->GetTimeSeconds();
				WeaponAction.Value->StopFire(WorldTime);
			}
		}
	}
	return true;
}

TArray<FSoftObjectPath> UEXWeapon::GetAssets() const
{
	TArray<FSoftObjectPath> Assets = Super::GetAssets();
	return Assets;
}

void UEXWeapon::EquipComplete()
{
	Super::EquipComplete();

	if (!bUsesAbility)
	{
		SetComponentTickEnabled(true);
	}
}

void UEXWeapon::UnequipComplete()
{
	Super::UnequipComplete();

	if (!bUsesAbility)
	{
		SetComponentTickEnabled(false);
	}
	if (IsLocallyOwned())
	{
		EXController->UpdateAmmo(nullptr);
	}
}

void UEXWeapon::AddRecoil()
{
	if (!RecoilX || !RecoilY)
	{
		return;
	}
	bRecoil = true;
	TimeSinceLastShot = 0.f;
	TotalShotRecoilX = 0;
	TotalShotRecoilY = 0;
	bRecoilXDirection = FMath::RandBool();
}

bool UEXWeapon::IsLowOnAmmo() const
{
	if (UEXReloadAction* ReloadAction = GetReloadAction())
	{
		return ReloadAction->IsLowOnAmmo();
	}
	return false;
}

bool UEXWeapon::IsAmmoFull() const
{
	if (UEXReloadAction* ReloadAction = GetReloadAction())
	{
		ReloadAction->IsAmmoFull();
	}
	return true;
}

bool UEXWeapon::Tick(float DeltaTime)
{
	bool bAnythingChanged = Super::Tick(DeltaTime);

	if (bRecoil)
	{
		TimeSinceLastShot += DeltaTime;
		// Recoil before
		const float PreviousX = TotalShotRecoilX;
		const float PreviousY = TotalShotRecoilY;
		// Recoil now
		TotalShotRecoilX = RecoilX->GetFloatValue(TimeSinceLastShot);
		TotalShotRecoilY = RecoilX->GetFloatValue(TimeSinceLastShot);
		// Difference
		const float DeltaRecoilX = (TotalShotRecoilX - PreviousX) * RecoilMultiplierX;
		const float DeltaRecoilY = (TotalShotRecoilY - PreviousY) * RecoilMultiplierY;
		// Add the difference
		GetOwningCharacter()->AddControllerYawInput(bRecoilXDirection ? DeltaRecoilX : -DeltaRecoilX);
		GetOwningCharacter()->AddControllerPitchInput(-DeltaRecoilY);

		if (TimeSinceLastShot > RecoilTime)
		{
			bRecoil = false;
			TotalShotRecoilX = 0;
			TotalShotRecoilY = 0;
		}
		bAnythingChanged = true;
	}
	return bAnythingChanged;
}

void UEXWeapon::LoadSettings()
{
	for (const TTuple<EWeaponInput, UEXWeaponAction*>& WeaponAction : Actions)
	{
		if (UEXADSAction* ADSAction = Cast<UEXADSAction>(WeaponAction.Value))
		{
			ADSAction->SetToggled(InventoryOwner->GetToggleADS());
		}
		else if (UEXReloadAction* ReloadAction = Cast<UEXReloadAction>(WeaponAction.Value))
		{
			ReloadAction->SetToggled(InventoryOwner->GetReloadCancelsReload());
		}
	}
}

float UEXWeapon::GetActionTimeRemaining(EWeaponState State) const
{
	switch (State)
	{
	case EWeaponState::Firing: // Override for firing, because the firing mechanism for weapons is entirely different from inventory
	{
		ensure(GetActiveFireMode());
		if (UEXFireMode* ActiveFireMode = GetActiveFireMode())
		{
			if (ActiveFireMode->IsFiring())
			{
				return ActiveFireMode->GetTimeRemaining();
			}
		}
		return 0.f;
	}
	case EWeaponState::Reloading: // Override for reloading, because the parent class does not have reloading
	{
		ensure(Actions.Contains(EWeaponInput::Reload));
		UEXReloadAction* ReloadAction = GetReloadAction();
		if (ReloadAction && ReloadAction->IsFiring())
		{
			return ReloadAction->GetTimeRemaining();
		}
		return 0.f;
	}
	default: 
		return Super::GetActionTimeRemaining(State);
	}
}

void UEXWeapon::FinishCurrentState(EWeaponState State)
{
	switch (State)
	{
	case EWeaponState::Firing:
		if (UEXFireMode* ActiveFireMode = GetActiveFireMode())
		{
			ActiveFireMode->FinishEarly();
		}
		break;
	case EWeaponState::Reloading:
		if (UEXReloadAction* ReloadAction = GetReloadAction())
		{
			ReloadAction->FinishEarly();
		}
		break;
	case EWeaponState::Ready:
	case EWeaponState::Down:
	case EWeaponState::Equipping:
	case EWeaponState::Unequipping:
	case EWeaponState::Holding:
	case EWeaponState::Max:
	default:
		Super::FinishCurrentState(State);
		break;
	}
}

int32 UEXWeapon::AddAmmo(uint8 MagAmount)
{
	if (UEXReloadAction* ReloadAction = GetReloadAction())
	{
		ReloadAction->AddAmmo(MagAmount);
	}
	/*const int32 MagsGiven = FMath::CeilToInt(((float)(MaxTotalAmmo - TotalAmmo)) / MaxMagAmmo);
	const uint8 AddedAmount = MaxMagAmmo * MagAmount;
	TotalAmmo += AddedAmount;
	TotalAmmo = FMath::Min(MaxTotalAmmo, TotalAmmo);
	Client_Reliable_AddAmmo(AddedAmount);
	return MagsGiven;*/
	return 0.f;
}

bool UEXWeapon::HasEnoughAmmo(uint8 AmmoCount) const
{
	if (AmmoCount == 0)
	{
		return true;
	}
	if (UEXReloadAction* ReloadAction = GetReloadAction())
	{
		return (ReloadAction->GetMagAmmo() >= AmmoCount);
	}
	return true;
}

void UEXWeapon::ConsumeAmmo(uint8 Amount)
{
	UEXReloadAction* ReloadAction = GetReloadAction();
	if (ReloadAction)
	{
		ReloadAction->ConsumeAmmo(Amount);
	}
}

void UEXWeapon::PlayFireEffects()
{
	const float VolumeMultiplier = UEXGameplayStatics::GetMasterVolume();

	//StopFireEffects();
	USkeletalMeshComponent* MeshComp = GetWeaponMeshComponent();
	if (MeshComp)
	{
		if (MuzzleEffect)
		{
			MuzzleParticleComp = UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}
		if (FireSound)
		{
			/*if (GetOwningCharacter()->IsLocallyControlled())
			{
				FireSoundComp = UGameplayStatics::SpawnSound2D(
					this,
					FireSoundComp,
					VolumeMultiplier,
					1.f,
					0.f,
					SoundConcurrencySettings,
				);
			}
			else*/
			{
				/*FireSoundComp = UGameplayStatics::SpawnSoundAttached(
					FireSound,
					MeshComp,
					MuzzleSocketName,
					FVector(ForceInit),
					FRotator::ZeroRotator,
					EAttachLocation::KeepRelativeOffset,
					false,
					VolumeMultiplier,
					1.f,
					0.f,
					AttenuationSettings,
					ConcurrencySettings
				);*/
				const bool bLocal = IsLocallyOwned();
				if (bLocal)
				{
					FireSoundComp = UGameplayStatics::SpawnSound2D(
						this,
						FireSound,
						VolumeMultiplier,
						1.f,
						0.f,
						ConcurrencySettings
					);
				}
				else
				{
					FVector SocketLoc;
					FRotator SocketRot;
					MeshComp->GetSocketWorldLocationAndRotation(MuzzleSocketName, SocketLoc, SocketRot);
					FireSoundComp = UGameplayStatics::SpawnSoundAtLocation(
						this,
						FireSound,
						SocketLoc,
						SocketRot,
						VolumeMultiplier,
						1.f,
						0.f,
						bLocal ? AttenuationSettings : AttenuationSettings3P,
						bLocal ? ConcurrencySettings : ConcurrencySettings3P
					);
				}
			}
			check(MeshComp->GetSocketByName(MuzzleSocketName));
		}
	}
}

void UEXWeapon::StopFireEffects()
{
	if (MuzzleParticleComp)
	{
		MuzzleParticleComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		MuzzleParticleComp->Deactivate();
	}
	if (FireSoundComp)
	{
		//FireSoundComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		//FireSoundComp->Deactivate();
	}
}

void UEXWeapon::UpdateHUD()
{
	if (IsLocallyOwned() && IsEquipped())
	{
		EXController->UpdateAmmo(this);
	}
}

void UEXWeapon::SetFireModeSpreadModifier(float Val)
{
	FireModeSpreadModifier = Val;
	if (IsLocallyOwned())
	{
		// Only for HUD purposes
		float WorldTime = GetWorld()->GetTimeSeconds();
		RecalculateSpread(WorldTime);
	}
}

void UEXWeapon::SetMovementSpreadModifier(float Val)
{
	MovementSpreadModifier = Val;
	if (IsLocallyOwned())
	{
		// Only for HUD purposes
		float WorldTime = GetWorld()->GetTimeSeconds();
		RecalculateSpread(WorldTime);
	}
}

void UEXWeapon::RecalculateSpread(float WorldTimeOverride)
{
	UEXFireMode* ActiveFireMode = GetActiveFireMode();
	if (!ActiveFireMode || !ActiveFireMode->IsInitialized())
	{
		return;
	}
	const float FireModeSpread = ActiveFireMode->GetSpread(WorldTimeOverride);
	UE_LOG(LogEXWeapon, Verbose, TEXT("FireModeSpread = %f, MovementSpreadModifier = %f, FireModeSpreadModifier = %f"),
		FireModeSpread, MovementSpreadModifier, FireModeSpreadModifier);
	Spread = FireModeSpread * MovementSpreadModifier * FireModeSpreadModifier * 0.01f;
	if (IsLocallyOwned() && EXController)
	{
		EXController->SetSpread(Spread);
	}
}

void UEXWeapon::Multicast_Unreliable_PlayFireEffects_Implementation()
{
	if (IsSimulatedProxy())
	{
		PlayFireEffects();
	}
}

void UEXWeapon::OnRep_Actions()
{
	for (UEXWeaponAction* Action : ReplicatedActions)
	{
		if (Action && !Action->IsInitialized())
		{
			for (const TTuple<EWeaponInput, TSubclassOf<UEXWeaponAction>>& KVP : ActionClassList)
			{
				if (Action->GetClass() == KVP.Value)
				{
					const EWeaponInput ActionType = KVP.Key;
					if (ensure(!Actions.Contains(ActionType)))
					{
						Actions.Add(ActionType, Action);
						Action->Initialize(this, ActionType);
					}
					break;
				}
			}
		}
	}
	LoadSettings();
}

void UEXWeapon::SpawnImpactEffect(const FHitResult& Impact)
{
	if (ImpactEffect && Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;

		// trace again to find component lost during replication
		if (!Impact.Component.IsValid())
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = PerformFXTrace(StartTrace, EndTrace);
			UseImpact = Hit;
		}

		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
		AEXImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AEXImpactEffect>(ImpactEffect, SpawnTransform);
		if (EffectActor)
		{
			EffectActor->SurfaceHit = UseImpact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}
}

FHitResult UEXWeapon::PerformFXTrace(FVector StartTrace, FVector EndTrace) const
{
	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.bReturnPhysicalMaterial = true;
	CQP.bTraceComplex = true;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_WeaponTrace, CQP) || !HitResult.GetActor();
	return HitResult;
}

UEXFireMode* UEXWeapon::GetActiveFireMode() const
{
	if (!Actions.Contains(EWeaponInput::Primary))
	{
		return nullptr;
	}
	return Cast<UEXFireMode>(Actions[EWeaponInput::Primary]);
}

UEXReloadAction* UEXWeapon::GetReloadAction() const
{
	if (!Actions.Contains(EWeaponInput::Reload))
	{
		return nullptr;
	}
	return Cast<UEXReloadAction>(Actions[EWeaponInput::Reload]);
}

UEXADSAction* UEXWeapon::GetADSAction() const
{
	if (!Actions.Contains(EWeaponInput::Secondary))
	{
		return nullptr;
	}
	return Cast<UEXADSAction>(Actions[EWeaponInput::Secondary]);
}

bool UEXWeapon::CanSprint() const
{
	for (const TTuple<EWeaponInput, UEXWeaponAction*>& KVP : Actions)
	{
		UEXWeaponAction* WeaponAction = KVP.Value;
		if (WeaponAction->GetPreventsSprint() && WeaponAction->IsFiring())
		{
			return false;
		}
	}
	return Super::CanSprint();
}

void UEXWeapon::CancelReload()
{
	if (UEXReloadAction* ReloadAction = GetReloadAction())
	{
		const float WorldTime = GetWorld()->GetTimeSeconds();
		ReloadAction->Cancel(WorldTime);
	}
}

void UEXWeapon::OnOutOfAmmo()
{
	if (bReloadOnFireNoAmmo)
	{
		if (UEXReloadAction* ReloadAction = GetReloadAction())
		{
			const float WorldTime = GetWorld()->GetTimeSeconds();
			ReloadAction->Fire(WorldTime);
			ReloadAction->StopFire(WorldTime);
		}
	}
}

