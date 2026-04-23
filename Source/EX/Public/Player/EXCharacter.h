// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EX.h"
#include "GameFramework/Character.h"
#include "EXTypes.h"
#include "System/EXInteractable.h"
#include "WorldCollision.h"
#include "EXCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChangedSignature, float, Health, float, HealthDelta, struct FDamageEvent const&, DamageEvent, class AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractCompletedSignature, AEXCharacter*, Player);

class UInputComponent;
class UStaticMeshComponent;
class AEXPlayerController;
class UTexture2D;
class UEXMercStatusIcon;
class AEXTeam;
class UEXAnimInstance;
class AEXGameModeBase;
class AEXAssaultCourseGame;
class AEXGameStateBase;


USTRUCT()
struct FDamageTaken
{
	GENERATED_BODY()

	AEXPlayerState* Player = nullptr;
	float Damage = 0.f;

	FDamageTaken() : Player(nullptr), Damage(0.f) { }
	FDamageTaken(AEXPlayerState* InPlayer, float InDamage) : Player(InPlayer), Damage(InDamage) {}
};

USTRUCT()
struct FWeaponState
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponState WeaponState = EWeaponState::Down;

	UPROPERTY()
	float Modifier = 1.f;

	FWeaponState() : WeaponState(EWeaponState::Down), Modifier(1.f) {}
	FWeaponState(EWeaponState InWeaponState, float InModifier) : WeaponState(InWeaponState), Modifier(InModifier) {}
};


UCLASS(config=Game)
class AEXCharacter : public ACharacter, public IEXInteractable
{
	GENERATED_BODY()
		
public:
	AEXCharacter();

	TArray<FSoftObjectPath> GetAssets();
	TArray<FSoftObjectPath> GetMenuAssets();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FORCEINLINE EMerc GetType() const { return MercType; }

	void SetMaterial();

	virtual void AddControllerPitchInput(float Val) override;

	virtual void AddControllerYawInput(float Val) override;

	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UFUNCTION(BlueprintCallable, Category = "Character")
	USkeletalMeshComponent* GetWeaponMeshComponent() const { return Gun; }
	UFUNCTION(BlueprintCallable, Category = "Character")
	USkeletalMeshComponent* GetWeaponMeshComponentSecondary() const { return GunSecondary; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE class UEXInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetHealth() const { return Health; }
	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetMaxHealth() const { return MaxHealth; }

	void OnStartSprint();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual bool CanPerformWallJump(FVector& Normal) const;

	virtual void SetPlayerDefaults() override;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Interact")
	FOnInteractCompletedSignature OnInteractCompleted;

	// The actor that the player is currently interacting with (milkjug, ev, plantsite)
	UPROPERTY()
	AActor* InteractableActor = nullptr;


	virtual void OnRep_Controller() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnPossessed();

	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE AEXPlayerController* GetEXController() const { return EXController; }
	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE AEXPlayerState* GetEXPlayerState() const { return EXPS; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Character", Meta = (DisplayName = "On Equip", ScriptName = "OnEquip"))
	void K2_OnEquip(USkeletalMeshComponent* WeaponMesh, USkeletalMeshComponent* CharacterMesh, FName SocketName, const FVector& Scale);


	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	float GetMaxTraceRange() const { return MaxTraceRange; }

	void SetControlsEnabled(bool bEnabled);

	void OnRep_IsCrouched() override;

	FORCEINLINE EMerc GetMercType() const { return MercType; }



	virtual void PreInitializeComponents() override;

	void LoadPersistentUserSettings();

	virtual void Landed(const FHitResult& Hit) override;


	virtual void TornOff() override;

	void WeaponEquipped(EWeapon Type);
	void WeaponUnequipped();
	void AbilityEquipped(EAbility Type);
	void AbilityUnequipped();

	//~ Begin AEXInteract Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
	virtual float GetMovementMultiplier() const override;
	//~ End AEXInteract Interface

protected:
	UFUNCTION(BlueprintCallable, Category = "Interact")
	AActor* GetLookAtInteractableActor() const;
	UFUNCTION(BlueprintCallable, Category = "Interact")
	AActor* GetInteractableActor() const;

public:

	virtual void OnRep_PlayerState() override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	void SetFOVSensAdjustment(float Val);

	virtual void Reset() override;

	virtual void UnPossessed() override;

	virtual void OnPlayerStateInitialized();

	virtual void CheckJumpInput(float DeltaTime) override;


	virtual void ClearJumpInput(float DeltaTime) override;


	/// Simulated proxy
public:
	void SetHelpIconsToShow(bool bAmmo, bool bHeal, bool bRevive);
	void SetShowAmmoIcon(bool bShowIcon) { bShowAmmoIcon = bShowIcon; }
protected:
	bool bShowAmmoIcon = false;
	bool bShowHealIcon = false;
	bool bShowReviveIcon = false;
	///
	/// Autonomous proxy
public:
	void SetHelpActions(bool bAmmo, bool bHeal, bool bRevive);
protected:
	bool bCanHelpAmmo = false;
	bool bCanHelpHeal = false;
	bool bCanHelpRevive = false;
	void ShowMyIcons(AEXCharacter* Teammate);
	///

	void SetHealIconVisibility(bool bVisible);
	void SetReviveIconVisibility(bool bVisible);
	void SetAmmoIconVisibility(bool bVisible);

	bool LowOnHealth() const;

	UPROPERTY(EditDefaultsOnly, Category = "UI", Meta=(Tooltip = "How low must health be for heal icon to show to medic players"))
	float HealthRatioForHealIcon = 0.4f;


protected:
	float FOVSensAdjustment = 1.f;

	UPROPERTY()
	TArray<AEXCharacter*> InteractingPlayers;

	bool bStatTracking = false;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FColor GibbedMaterialColor = FColor::Silver;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FColor FriendlyMaterialColor = FColor::Blue;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FColor EnemyMaterialColor = FColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FColor AttackerMaterialColor = FColor::Yellow;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FColor DefenderMaterialColor = FColor::Purple;

	// If input is set to hold to interact or toggle
	bool bHoldObjective = true;
	bool bToggleSprint = false;
	bool bToggleWalk = false;

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float MaxTraceRange = 1000.f;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	void MoveForward(float Val);

	void MoveRight(float Val);

	bool bForward = false;

	UFUNCTION()
	virtual void StartCrouch();
	UFUNCTION()
	virtual void StopCrouch();

	virtual bool CanJumpInternal_Implementation() const override;


	UFUNCTION()
	virtual void LongJump();
	UFUNCTION()
	virtual void StopLongJumping();

public:
	FORCEINLINE bool IsMovingForward() const { return bForward; }

	virtual void Jump() override;
	virtual void StopJumping() override;

	UFUNCTION()
	virtual void StartRunning();
	UFUNCTION()
	virtual void StopRunning();

	UFUNCTION()
	virtual void StartWalking();
	UFUNCTION()
	virtual void StopWalking();

protected:
	virtual void StartRunning_Internal();
	virtual void StopRunning_Internal();
	virtual void StartWalking_Internal();
	virtual void StopWalking_Internal();

public:
	virtual void CancelReloadAnim();
protected:
	/** When true, player wants to sprint. */
	bool bPressedSprint = false;
	/** When true, player wants to walk. */
	bool bPressedWalk = false;

protected:
	UFUNCTION()
	virtual void InteractPressed();
	UFUNCTION()
	virtual void InteractReleased();
public:
	virtual void StartInteract();
	virtual void StartInteract(AActor* InInteractableActor);
	virtual void StopInteract(bool bOnlyLocal = false);
	// bOnlyLocal means that we don't do RPC
	virtual void StopInteract(AActor* InInteractableActor, bool bInitiatedByServer, bool bOnlyLocal = false);
	UFUNCTION(Client, Reliable)
	virtual void Client_Reliable_ForceStopInteracting(AActor* SInteractableActor);

	void OnCrouchUpdate(float RelativeLocZ);

	FORCEINLINE bool IsInteracting() const { return !!InteractableActor; }
	float GetInteractMovementMultiplier() const { return InteractMovementMultiplier; }

protected:
	void LookTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& TraceData) const;
	FTraceDelegate TraceDelegate;
	float InteractMovementMultiplier = 1.f;

public:
	virtual void SelfKill();

	bool IsADSing() const { return bADSing; }

protected:

	bool bADSing = false;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_Reliable_StartInteracting(AActor* SInteractableActor, bool bLookAt);
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_Reliable_StopInteracting(AActor* SInteractableActor);

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float FallDamageVelocity = -2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float FallDamage = 20.f;

protected:
	void SetHealth(float InHealth);
	virtual void HealthChanged(float OldHealth);

	UFUNCTION()
	void StartSlowHeal();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual	void Server_Reliable_SelfKill();

	UFUNCTION()
	void Die(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Health", Meta = (DisplayName = "On Death", ScriptName = "OnDeath"))
	void K2_OnDeath();
	UFUNCTION(BlueprintImplementableEvent, Category = "Health", Meta = (DisplayName = "On Hit", ScriptName = "OnHit"))
	void OnHit(float Damage);



	// #EXTODORELIABLE
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reliable_PlayerDied(AEXPlayerState* PS, UTexture2D* KillIcon, AEXPlayerState* Killer);


	UFUNCTION()
	virtual void DoDeathEffect();

	UFUNCTION(BlueprintImplementableEvent, Category = "Health", Meta = (DisplayName = "On Revive", ScriptName = "OnRevive"))
	void K2_Revive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Health", Meta = (DisplayName = "On Insta Heal", ScriptName = "OnInstaHeal"))
	void K2_OnInstaHeal();
	UFUNCTION(BlueprintImplementableEvent, Category = "Health", Meta = (DisplayName = "On Slow Heal", ScriptName = "OnSlowHeal"))
	void K2_OnSlowHeal();

public:

	FRotator GetAimOffsets() const;

	UFUNCTION()
	void Revive(float ReviveHealth, AController* EventInstigator, EMerc OtherMercType);

	void OnRevive();
	void OnDeath();

	void StartTracking();
	void StopTracking();

	UFUNCTION(BlueprintCallable, Category = "Health")
	FORCEINLINE bool IsDead() const { return Health <= 0.f; }
	UFUNCTION(BlueprintCallable, Category = "Health")
	FORCEINLINE bool IsGibbed() const { return bGibbed; }

	UFUNCTION()
	void InstaHeal(float AddedHealth, AController* EventInstigator);

	UFUNCTION()
	void SetHealingRate(float NewHealingRate, AController* EventInstigator);


	UFUNCTION(BlueprintImplementableEvent, Category = "Health", Meta = (DisplayName = "On Gib", ScriptName = "OnGib"))
	void K2_OnGib();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void StartProgress(float Goal, float ProgressModifier = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void StopProgress();

	UFUNCTION(BlueprintPure, Category = "Interact")
	float GetProgress() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void OnLongJump();

protected:
	void SetCollisionEnabled(bool bEnabled);

private:
	// Do not call this directly, gib should only be called from TakeDamage()
	void Gib(struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Spotted)
	bool bSpotted = false;

	FTimerHandle TimerHandle_Spotted;

	UFUNCTION()
	void OnRep_Spotted();

	UFUNCTION()
	void SpottedOver();

	FTimerHandle TimerHandle_ShowHit;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float HitShowDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> CharacterIconKillFeed = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> CharacterIconSmall = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> CharacterIconMedium = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> CharacterIconLarge = nullptr;

public:
	UFUNCTION()
	void Spotted(float Duration);

	void ShowHitOnHUD();

	void StartScoping();
	void StopScoping();
	void StartADSing();
	void StopADSing();

	void WeaponStateChanged(EWeaponState InWeaponState, float Modifier);
	void WeaponTypeChanged(EWeaponType InWeaponType);

	bool BetterEngiThan(AEXCharacter* OtherPlayer, EEngiTool Tool) const;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponType)
	EWeaponType WeaponType;
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
	FWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponType();
	UFUNCTION()
	void OnRep_WeaponState();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponStateChanged(EWeaponState InWeaponState, float Modifier);

	UFUNCTION()
	void InteractionCompleted(AEXPlayerController* Player);
	
	UPROPERTY()
	float Health = 1;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Health)
	int16 RepHealth;
	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxDownHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float SlowHealingDelay = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float SlowHealingRate = 0.f;

	UPROPERTY(Transient)
	float HealingRate;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float SpawnProtectionLength = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float SpawnProtectionDamageModifier = .5f;

	UPROPERTY(Transient)
	bool bSpawnProtectionEnabled = false;

	FTimerHandle TimerHandle_SlowHeal;
	FTimerHandle TimerHandle_SpawnProtection;

	UPROPERTY()
	AEXPlayerController* HealingInstigator = nullptr;

	float HealingRateStartHealth = 0.f;

	void StopHealing();

	UPROPERTY(Transient)
	bool bGibbed;

	TArray<FDamageTaken> DamageTaken;

	void AddScoreForKill(float MaxScore, bool bKill);

	float SelfKillDamage = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UDamageType> SelfKillDamageType;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UDamageType> FallDamageType;

public:

	float GetMovementSpreadModifier() { return MovementSpreadModifier; }

	void SetStatusIcon(UEXMercStatusIcon* Icon);
	UEXMercStatusIcon* GetStatusIcon() const { return StatusIcon; }

protected:
	void StartSlowRes();
	void StopSlowRes();
	UFUNCTION()
	void OnSlowRes(AEXCharacter* Player);

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float TimeToSlowRevive = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float SlowResHealthGiven = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float PostGibLifeSpan = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WallJumpSphereOffset = 32.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WallJumpMinNormalZ = -0.05f;


	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float JumpingSpreadModifier = 3.f;

	float MovementSpreadModifier = 1.f;

	void SetMovementSpreadModifier(float Val);

	UPROPERTY()
	UEXMercStatusIcon* StatusIcon = nullptr;

#pragma region Augments
// #Augments
private:

	using AugmentFunction = void (AEXCharacter::*)();

	AugmentFunction AugmentFunctions[EAugment::AugmentCount] = 
	{ 
		&AEXCharacter::AugmentEmpty,
		&AEXCharacter::AugmentDrilled,
		&AEXCharacter::AugmentQuickDraw,
		&AEXCharacter::AugmentLooter,
		&AEXCharacter::AugmentRecycle,
		&AEXCharacter::AugmentMechanic,
		&AEXCharacter::AugmentQuickEye,
		&AEXCharacter::AugmentWallJumper,

	};

	bool bAugmentsApplied = false;
public:
	void ApplyAugments();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Augments")
	TArray<EAugment> GetAvailableAugments() const { return AvailableAugments; }
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Augments")
	TArray<EAugment> AvailableAugments;

	void AugmentEmpty();
	UPROPERTY(EditDefaultsOnly, Category = "Augments")
	float DrilledModifier = .5f;
	void AugmentDrilled();
	UPROPERTY(EditDefaultsOnly, Category = "Augments")
	float QuickDrawEquipModifier = .5f;
	UPROPERTY(EditDefaultsOnly, Category = "Augments")
	float QuickDrawUnequipModifier = .5f;
	void AugmentQuickDraw();
	void AugmentLooter();
	void AugmentRecycle();
	UPROPERTY(EditDefaultsOnly, Category = "Augments")
	float MechanicModifier = 1.2f;
	void AugmentMechanic();
	UPROPERTY(EditDefaultsOnly, Category = "Augments")
	float QuickEyeMultiplier = 1.33f;
	void AugmentQuickEye();
	void AugmentWallJumper();

public:
	bool CanRecycle() const { return bCanRecycle; }
protected:
	bool bLooter = false;
	bool bCanRecycle = false;
#pragma endregion Augments

public:
	class UCameraComponent* GetFPSCamera() const { return FirstPersonCameraComponent; }
	
	bool IsMerc(EMerc InMerc) const { return InMerc == MercType; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	TSoftObjectPtr<UTexture2D> GetIcon(EIconSize Size) const;

	UEXAnimInstance* GetAnimInstance() const { return AnimInst; }

	FORCEINLINE EMercStatus GetStatus() const { return IsGibbed() ? EMercStatus::Gibbed : IsDead() ? EMercStatus::Downed : EMercStatus::Alive; }

	class UEXCharacterMovement* GetEXCharacterMovement() const { return EXCharacterMovement; }

protected:
	void InitGameStatusWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Merc")
	EMerc MercType = EMerc::Default;

	void SetMaterialColor(FColor MaterialColor);

	UPROPERTY()
	AEXGameModeBase* GM = nullptr;
	UPROPERTY()
	AEXGameStateBase* GS = nullptr;
	UPROPERTY()
	AEXAssaultCourseGame* GMAC = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	class AEXPlayerState* EXPS = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	class AEXPlayerController* EXController = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	class UEXAnimInstance* AnimInst = nullptr;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character",  Meta = (AllowPrivateAccess = "true"))
	class UEXCharacterMovement* EXCharacterMovement = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Gun = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* GunSecondary = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
	class UEXInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Interact", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* ProgressComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* SpottedComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarComp = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXHealthBar* HealthBarWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealIconComp = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UUserWidget* HealWidget = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ReviveIconComp = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UUserWidget* ReviveWidget = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* AmmoIconComp = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UUserWidget* AmmoWidget = nullptr;

};
