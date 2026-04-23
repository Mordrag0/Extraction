// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/EXCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/EXCharacterMovement.h"
#include "Components/InputComponent.h"
#include "Inventory/EXInventoryComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/EXGameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EXPlayerController.h"
#include "Online/EXPlayerState.h"
#include "Online/EXLevelRules.h"
#include "System/EXProgress.h"
#include "Online/EXGameInstance.h"
#include "Inventory/EXWeapon.h"
#include "HUD/EXHealthBar.h"
#include "Components/WidgetComponent.h"
#include "Player/EXAnimInstance.h"
#include "Inventory/EXADSAction.h"
#include "Algo/Reverse.h"
#include "Inventory/EXDamageType.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Inventory/EXInventoryInteract.h"
#include "Inventory/EXInventory.h"
#include "Player/EXLocalPlayer.h"
#include "Player/EXPersistentUser.h"
#include "Player/EXPlayerCameraManager.h"
#include "DrawDebugHelpers.h"
#include "Misc/EXObjectiveTool.h"
#include "HUD/EXMercSelectIcon.h"
#include "HUD/EXGameStatus.h"
#include "HUD/EXHUDWidget.h"
#include "Engine/Texture2D.h"
#include "Online/EXTeam.h"
#include "Inventory/EXDefib.h"
#include "Online/EXAssaultCourseGame.h"
#include "Online/EXGameStateBase.h"
#include "Online/EXGameModeBase.h"
#include "Misc/EXCarryableObjective.h"
#include "Inventory/EXMilkJug.h"
#include "Inventory/EXAbilityTracker.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// #DEBUG
static TAutoConsoleVariable<int32> CVarShowMovementDebug(
	TEXT("x.ShowMovementDebug"),
	0,
	TEXT("Enabled rendering debug lines for character movement.\n")
	TEXT("  0: off\n")
	TEXT("  1: on\n"),
	ECVF_Cheat);
static TAutoConsoleVariable<int32> CVarShowMovementSpeed(
	TEXT("x.ShowMovementSpeed"),
	0,
	TEXT("Enabled the display of movement speed.\n")
	TEXT("  0: off\n")
	TEXT("  1: on\n"),
	ECVF_Cheat);
#endif


AEXCharacter::AEXCharacter() :
	Super(FObjectInitializer::Get().SetDefaultSubobjectClass<UEXCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	EXCharacterMovement = Cast<UEXCharacterMovement>(GetCharacterMovement());

	InventoryComponent = CreateDefaultSubobject<UEXInventoryComponent>(TEXT("InventoryComponent"));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	Gun->SetupAttachment(RootComponent);
	Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Gun->SetCollisionResponseToChannel(ECC_Melee, ECR_Overlap);
	Gun->SetCollisionObjectType(ECC_Melee);

	GunSecondary = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun2"));
	GunSecondary->SetupAttachment(RootComponent);
	GunSecondary->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GunSecondary->SetCollisionResponseToChannel(ECC_Melee, ECR_Overlap);
	GunSecondary->SetCollisionObjectType(ECC_Melee);

	ProgressComponent = CreateDefaultSubobject<UEXProgress>(TEXT("ObjectiveInteraction"));
	ProgressComponent->SetCharacterOwner(this);

	HealthBarComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Healthbar"));
	HealthBarComp->SetupAttachment(GetCapsuleComponent());
	HealthBarComp->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComp->SetOwnerNoSee(true);

	SpottedComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("SpottedIcon"));
	SpottedComp->SetupAttachment(GetCapsuleComponent());
	SpottedComp->SetWidgetSpace(EWidgetSpace::Screen);
	SpottedComp->SetOwnerNoSee(true);

	HealIconComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealIcon"));
	HealIconComp->SetupAttachment(GetCapsuleComponent());
	HealIconComp->SetWidgetSpace(EWidgetSpace::Screen);
	HealIconComp->SetOwnerNoSee(true);
	ReviveIconComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ReviveIcon"));
	ReviveIconComp->SetupAttachment(GetCapsuleComponent());
	ReviveIconComp->SetWidgetSpace(EWidgetSpace::Screen);
	ReviveIconComp->SetOwnerNoSee(true);
	AmmoIconComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("AmmoIcon"));
	AmmoIconComp->SetupAttachment(GetCapsuleComponent());
	AmmoIconComp->SetWidgetSpace(EWidgetSpace::Screen);
	AmmoIconComp->SetOwnerNoSee(true);

	PrimaryActorTick.bCanEverTick = true;

}

TArray<FSoftObjectPath> AEXCharacter::GetAssets()
{
	TArray<FSoftObjectPath> Assets;
	Assets.Append(InventoryComponent->GetAssets());
	Assets.AddUnique(GetIcon(EIconSize::Large).ToSoftObjectPath());
	Assets.AddUnique(GetIcon(EIconSize::Medium).ToSoftObjectPath());
	Assets.AddUnique(GetIcon(EIconSize::Small).ToSoftObjectPath());
	Assets.AddUnique(GetIcon(EIconSize::KillFeed).ToSoftObjectPath());
	return Assets;
}

TArray<FSoftObjectPath> AEXCharacter::GetMenuAssets()
{
	TArray<FSoftObjectPath> Assets;
	Assets.AddUnique(GetIcon(EIconSize::Large).ToSoftObjectPath());
	return Assets;
}

void AEXCharacter::SetMaterial()
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		const AEXPlayerController* LocalPC = GetWorld()->GetFirstPlayerController<AEXPlayerController>();;
		const AEXPlayerState* LocalPS = LocalPC ? LocalPC->GetPlayerState<AEXPlayerState>() : nullptr;
		if (LocalPS && EXPS)
		{
			const AEXTeam* LocalTeam = LocalPS->GetTeam();
			if (LocalTeam && LocalTeam->IsSpectating())
			{
				const bool bAttacker = EXPS->GetTeam() ? EXPS->GetTeam()->IsAttacking() : false;
				SetMaterialColor(bAttacker ? AttackerMaterialColor : DefenderMaterialColor);
			}
			else
			{
				const bool bSameTeam = LocalPS->OnSameTeam(EXPS);
				SetMaterialColor(bSameTeam ? FriendlyMaterialColor : EnemyMaterialColor);
			}
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEXCharacter::SetMaterial);
		}
	}
}

void AEXCharacter::SetMaterialColor(FColor MaterialColor)
{
	UMaterialInterface* Material = GetMesh()->GetMaterial(0);
	UMaterialInstanceDynamic* MatInstance = GetMesh()->CreateDynamicMaterialInstance(0, Material);

	if (MatInstance)
	{
		MatInstance->SetVectorParameterValue("BodyColor", MaterialColor);
	}
}

void AEXCharacter::AddControllerYawInput(float Val)
{
	if ((Val != 0.f) && EXController)
	{
		EXController->AddYawInput(Val * FOVSensAdjustment);
	}
}

void AEXCharacter::AddControllerPitchInput(float Val)
{
	if ((Val != 0.f) && EXController)
	{
		EXController->AddPitchInput(-Val * FOVSensAdjustment);
	}
}

void AEXCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXCharacter, bSpotted);
	DOREPLIFETIME_CONDITION(AEXCharacter, WeaponType, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AEXCharacter, WeaponState, COND_SimulatedOnly);

	DOREPLIFETIME(AEXCharacter, RepHealth);
}

void AEXCharacter::BeginPlay()
{
	if (IsLocallyControlled())
	{
		AnimInst = Cast<UEXAnimInstance>(GetMesh1P()->GetAnimInstance());
		HealthBarComp->SetVisibility(false, true);

		Gun->bCastDynamicShadow = false;
		Gun->CastShadow = false;
		GunSecondary->bCastDynamicShadow = false;
		GunSecondary->CastShadow = false;

		TraceDelegate.BindUObject(this, &AEXCharacter::LookTraceComplete);
	}
	else
	{
		if (!HasAuthority())
		{
			AnimInst = Cast<UEXAnimInstance>(GetMesh()->GetAnimInstance());
		}
		HealthBarComp->SetVisibility(true, true);
	}

	Super::BeginPlay();

	GMAC = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
	GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
	GS = GetWorld()->GetGameState<AEXGameStateBase>();

	// #SpawnProtection
	bSpawnProtectionEnabled = true;
	FTimerDelegate Timer_EndSpawnProtection;
	Timer_EndSpawnProtection.BindLambda([this]
		{
			bSpawnProtectionEnabled = false;
		});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnProtection, Timer_EndSpawnProtection, SpawnProtectionLength, false);

	if (HasAuthority())
	{
		ProgressComponent->OnCompleted.AddUObject(this, &AEXCharacter::InteractionCompleted);
		ProgressComponent->SetResetOnStop(true);
	}

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		HealthBarWidget = Cast<UEXHealthBar>(HealthBarComp->GetUserWidgetObject());
		HealthBarWidget->Hide();

		HealWidget = HealIconComp->GetUserWidgetObject();
		ReviveWidget = ReviveIconComp->GetUserWidgetObject();
		AmmoWidget = AmmoIconComp->GetUserWidgetObject();

		SetHealIconVisibility(false);
		SetReviveIconVisibility(false);
		SetAmmoIconVisibility(false);

	}
	else
	{
		HealIconComp->SetVisibility(false);
		ReviveIconComp->SetVisibility(false);
		AmmoIconComp->SetVisibility(false);

	}
	InitGameStatusWidget();

	SpottedComp->SetVisibility(false, true);

	SetMaterial();
	ForceNetUpdate();
}

void AEXCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AEXCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AEXCharacter::StopJumping);

	PlayerInputComponent->BindAction("LongJump", IE_Pressed, this, &AEXCharacter::LongJump);
	PlayerInputComponent->BindAction("LongJump", IE_Released, this, &AEXCharacter::StopLongJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AEXCharacter::StartRunning);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AEXCharacter::StopRunning);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AEXCharacter::StartWalking);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AEXCharacter::StopWalking);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AEXCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AEXCharacter::StopCrouch);

	PlayerInputComponent->BindAction("SelfKill", IE_Pressed, this, &AEXCharacter::SelfKill);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AEXCharacter::InteractPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AEXCharacter::InteractReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AEXCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEXCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AEXCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AEXCharacter::AddControllerPitchInput);

	GetInventoryComponent()->SetupPlayerInputComponent(PlayerInputComponent);
}

void AEXCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority())
	{
		if (HealingRate > 0)
		{
			float OldHealth = Health;
			Health += HealingRate * DeltaSeconds;

			if (Health >= MaxHealth)
			{
				StopHealing();
				Health = MaxHealth;
			}
			RepHealth = FMath::CeilToInt(Health);
			if (OnHealthChanged.IsBound())
			{
				FDamageEvent Event;
				OnHealthChanged.Broadcast(Health, Health - OldHealth, Event, nullptr, nullptr);
			}
		}
	}

	if (IsLocallyControlled())
	{
		if (EXController && !IsDead())
		{
			FVector StartLocation;
			FRotator TraceDirection;
			GetActorEyesViewPoint(StartLocation, TraceDirection);

			FVector EndLocation = StartLocation + (TraceDirection.Vector() * MaxTraceRange);

			FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
			CQP.AddIgnoredActor(this);
			GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Single, StartLocation, EndLocation, ECC_Interact, CQP, FCollisionResponseParams::DefaultResponseParam, &TraceDelegate);
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const int32 bShowDebug = CVarShowMovementSpeed.GetValueOnGameThread();
		if (bShowDebug)
		{
			FString SpeedStr = FString::SanitizeFloat(GetCharacterMovement()->GetLastUpdateVelocity().Size());
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, SpeedStr);
		}
#endif
	}
}

bool AEXCharacter::CanPerformWallJump(FVector& Normal) const
{
	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.AddIgnoredActor(this);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GetCapsuleComponent()->GetScaledCapsuleRadius() + WallJumpSphereOffset);

	TArray<FOverlapResult> Overlaps;
	const FVector JumpOrigin = GetActorLocation();// +FVector::UpVector * WallJumpSphereOffset;// +(GetActorForwardVector() * 8.f);
	const bool bHit = GetWorld()->OverlapMultiByChannel(Overlaps, JumpOrigin, FQuat::Identity, ECC_WorldOnly, Sphere, CQP);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const int32 bShowDebug = CVarShowMovementDebug.GetValueOnGameThread();
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), Sphere.Sphere.Radius, 32, FColor::Yellow, false, 5.f);
	}
#endif

	FVector ClosestPoint;
	float ClosestDistance = MAX_FLT;
	for (FOverlapResult& Overlap : Overlaps)
	{
		FVector Point;
		if (UEXGameplayStatics::GetClosestPointOnCollision(Overlap.GetActor(), JumpOrigin, ECC_WorldOnly, Point))
		{
			const float Distance = FVector::DistSquared(JumpOrigin, Point);

			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestPoint = Point;
			}
		}
	}
	if (ClosestDistance < MAX_FLT)
	{
		Normal = (JumpOrigin - ClosestPoint);
		Normal.Normalize();
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), JumpOrigin, ClosestPoint, FColor::Yellow, false, 3, 0, 5.f);
		}
#endif
		return true;
	}
	return false;
}

void AEXCharacter::SetPlayerDefaults()
{
	SetHealth(MaxHealth);
	HealingRate = 0;
	bGibbed = false;
	InteractMovementMultiplier = 1.f;
}

void AEXCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (!EXController) // Can be called multiple times initially and will be called again when unpossessing, but we want to keep the reference
	{
		EXController = GetController<AEXPlayerController>();
		OnPossessed();
	}
}

void AEXCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	EXController = Cast<AEXPlayerController>(NewController);
	EXPS = GetPlayerState<AEXPlayerState>();

	OnPossessed();
	if(EXPS)
	{
		OnPlayerStateInitialized();
	}

	if (IsNetMode(NM_DedicatedServer))
	{
		if (GS && GS->IsMatchInProgress())
		{
			StartTracking();
		}
	}
}

void AEXCharacter::OnPossessed()
{
	if (HasAuthority() || IsLocallyControlled())
	{
		if (HasAuthority())
		{
			GMAC = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
			GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
		}
		
		if (EXController)
		{
			InventoryComponent->SetController(EXController);
			EXController->GetAbilityTracker()->OnCharacterSpawned(this);
		}
		if (IsLocallyControlled())
		{
			InventoryComponent->OnPossessedLocal();
			if (EXController)
			{
				LoadPersistentUserSettings();
				EXController->GetHUDWidget()->SetCharacter(this);
			}
		}
		else
		{
			if (!EXController)
			{
				// First equip for AI
				InventoryComponent->FirstEquip();
			}
		}
	}
}

void AEXCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode /*= 0*/)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetLocalRole() > ROLE_SimulatedProxy)
	{
		switch (GetCharacterMovement()->MovementMode)
		{
			case EMovementMode::MOVE_Falling: 
				SetMovementSpreadModifier(JumpingSpreadModifier);
				break;

			case EMovementMode::MOVE_Walking: 
				SetMovementSpreadModifier((PrevMovementMode == MOVE_Falling) ? 1.f : 1.f);
				break;

			default: 
				break;
		}
	}

	if (AnimInst)
	{
		AnimInst->SetIsFalling(GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling);
	}
}

void AEXCharacter::SetControlsEnabled(bool bEnabled)
{
	bUseControllerRotationYaw = bEnabled;
}

void AEXCharacter::OnRep_IsCrouched()
{
	// We don't want the default behavior
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		EXCharacterMovement->bWantsToCrouch = bIsCrouched;
	}
}

void AEXCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	
	InventoryComponent->SetOwner(this);
}

void AEXCharacter::LoadPersistentUserSettings()
{
#if WITH_EDITOR // #EDITORDEFAULTS
	bToggleSprint = false;
	bToggleWalk = false;
	GetInventoryComponent()->SetToggleADS(false);
	GetInventoryComponent()->SetReloadCancelsReload(true);
#else
	UEXLocalPlayer* LP = Cast<UEXLocalPlayer>(EXController->GetLocalPlayer());
	UEXPersistentUser* PU = LP ? LP->GetPersistentUser() : nullptr;
	if (PU)
	{
		bHoldObjective = PU->GetHoldObjective();
		bToggleSprint = PU->GetToggleSprint();
		bToggleWalk = PU->GetToggleWalk();
		GetInventoryComponent()->SetToggleADS(PU->GetToggleADS());
		GetInventoryComponent()->SetToggleADS(PU->GetReloadCancelsReload());
	}
#endif
}

void AEXCharacter::Landed(const FHitResult& Hit)
{
	if (HasAuthority())
	{
		const float ZVelocity = EXCharacterMovement->Velocity.Z;
		if (ZVelocity < FallDamageVelocity)
		{
			FEXFallDamageEvent DamageEvent(ZVelocity, FallDamageType);
			TakeDamage(FallDamage, DamageEvent, EXController, this);
		}
	}
	Super::Landed(Hit);
}

void AEXCharacter::ShowHitOnHUD()
{
	if (HealthBarWidget && !IsDead())
	{
		HealthBarWidget->Show();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShowHit, HealthBarWidget, &UEXHealthBar::Hide, HitShowDuration);
	}
}

void AEXCharacter::StartScoping()
{
	EXController->SetScopeVisibility(true);
	Mesh1P->SetVisibility(false, true);
	// When scoping also call StartADSing()
	StartADSing();
}

void AEXCharacter::StopScoping()
{
	EXController->SetScopeVisibility(false);
	if (!IsDead()) // Don't make the mesh visible if the player is dead, will be handled in OnRevive()
	{
		Mesh1P->SetVisibility(true, true);
	}
	// When scoping also call StopADSing()
	StopADSing();
}

void AEXCharacter::StartADSing()
{
	if (AnimInst)
	{
		AnimInst->SetADS(true);
	}
	
	if (EXController) 
	{
		EXController->SetCrosshairVisibility(false);
	}
	bADSing = true;
}

void AEXCharacter::StopADSing()
{
	if (AnimInst)
	{
		AnimInst->SetADS(false);
	}

	if (EXController) 
	{
		EXController->SetCrosshairVisibility(true);
	}
	bADSing = false;
}

void AEXCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
	bForward = (Value > 0.f);
}

void AEXCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AEXCharacter::Jump()
{
	Super::Jump();
}

void AEXCharacter::StopJumping()
{
	Super::StopJumping();
}

void AEXCharacter::LongJump()
{
	if (IsDead())
	{
		return;
	}
	EXCharacterMovement->StartLongJumping();
	//StartCrouch();
	Jump();
}

void AEXCharacter::StopLongJumping()
{
	EXCharacterMovement->StopLongJumping();
	//StopCrouch();
	StopJumping();
}

void AEXCharacter::StartRunning()
{
	if (IsLocallyControlled() && bToggleSprint && bPressedSprint) // Sprinting toggled off
	{
		StopRunning_Internal();
	}
	else // Start sprinting
	{
		StartRunning_Internal();
	}
}

void AEXCharacter::StopRunning()
{
	// Stop sprinting
	if (!(IsLocallyControlled() && bToggleSprint))
	{
		StopRunning_Internal();
	}
}

void AEXCharacter::StartWalking()
{
	if (IsLocallyControlled() && bToggleWalk && bPressedWalk) // Walking toggled off
	{
		StopWalking_Internal();
	}
	else // Start walking
	{
		StartWalking_Internal();
	}
}

void AEXCharacter::StopWalking()
{
	// Stop walking
	if (!(IsLocallyControlled() && bToggleWalk))
	{
		StopWalking_Internal();
	}
}

void AEXCharacter::StartRunning_Internal()
{
	bPressedSprint = true;
	EXCharacterMovement->SetWantsToSprint(true);
}

void AEXCharacter::StopRunning_Internal()
{
	bPressedSprint = false;
	EXCharacterMovement->SetWantsToSprint(false);
	if (IsLocallyControlled())
	{
		GetInventoryComponent()->CheckPendingFire();
	}
}

void AEXCharacter::StartWalking_Internal()
{
	bPressedWalk = true;
	EXCharacterMovement->SetWantsToWalk(true);
}

void AEXCharacter::StopWalking_Internal()
{
	bPressedWalk = false;
	EXCharacterMovement->SetWantsToWalk(false);
}

void AEXCharacter::CancelReloadAnim()
{
	if (AnimInst) // Animation could still be reloading after the weapon state is set to something else (so this is animation canceling)
	{
		AnimInst->CancelReload();
	}
}

void AEXCharacter::StartCrouch()
{
	if (IsDead())
	{
		return;
	}
	
	EXCharacterMovement->Crouch();
}

void AEXCharacter::StopCrouch()
{
	EXCharacterMovement->UnCrouch();
}

bool AEXCharacter::CanJumpInternal_Implementation() const
{
	if (IsDead())
	{
		return false;
	}

	if (InteractableActor)
	{
		return false;
	}

	// Can only jump from the ground, or multi-jump if already falling.
	bool bCanJump = EXCharacterMovement->IsJumpAllowed() && (EXCharacterMovement->IsMovingOnGround() || EXCharacterMovement->IsFalling());

	if (bCanJump)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			return JumpCurrentCount < JumpMaxCount;
		}
		else
		{
			// Only consider JumpKeyHoldTime as long as:
			// A) The jump limit hasn't been met OR
			// B) The jump limit has been met AND we were already jumping
			const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
			return bJumpKeyHeld && ((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
		}
	}
	return false;
}

// #Interact Input
void AEXCharacter::InteractPressed()
{
	UEXInventory* EquippedInventory = GetInventoryComponent()->GetEquippedInventory();
	if (EquippedInventory && EquippedInventory->GetEngiType() != EEngiTool::None)
	{
		check(!InteractableActor);
		GetInventoryComponent()->Unequip(EquippedInventory, false);
		return;
	}
	if (bHoldObjective && InteractableActor)
	{
		StopInteract();
	}
	else
	{
		StartInteract();
	}
}

void AEXCharacter::InteractReleased()
{
	if (!bHoldObjective)
	{
		StopInteract();
	}
}

// #Interact
void AEXCharacter::StartInteract()
{
	if (IsDead())
	{
		return;
	}

	AActor* InteractableTarget = GetLookAtInteractableActor();
	bool bLookingAtInteractable = !!InteractableTarget;
	if (!InteractableTarget)
	{
		InteractableTarget = GetInteractableActor();
	}
	if (InteractableTarget)
	{
		StartInteract(InteractableTarget);
		Server_Reliable_StartInteracting(InteractableTarget, bLookingAtInteractable);
	}
}

void AEXCharacter::StartInteract(AActor* InInteractableActor)
{
	IEXInteractable::Execute_StartInteract(InInteractableActor, this);
	InteractableActor = InInteractableActor;
	IEXInteractable* Interactable = Cast<IEXInteractable>(InInteractableActor);
	InteractMovementMultiplier = Interactable->GetMovementMultiplier();
	if (Interactable->TapInteract())
	{
		// Tap interact means that we don't really do anything in Stop Interact but we must call it anyway, just to clear some values
		StopInteract(InInteractableActor, false, true);
	}
}

void AEXCharacter::StopInteract(bool bOnlyLocal)
{
	if (IsDead())
	{
		return;
	}

	if (InteractableActor)
	{
		StopInteract(InteractableActor, false, bOnlyLocal);
	}
}

void AEXCharacter::StopInteract(AActor* InInteractableActor, bool bInitiatedByServer, bool bOnlyLocal)
{
	InteractMovementMultiplier = 1.f;
	// InInteractableActor can be null if we pick up an ability that gets destroyed immediately
	if (InInteractableActor)
	{
		if (InteractableActor == InInteractableActor)
		{
			InteractableActor = nullptr; // Set it to null first so we don't get recursion
			IEXInteractable::Execute_StopInteract(InInteractableActor, this);
		}
	}
	else
	{
		UE_LOG(LogEXChar, Warning, TEXT("Interactable actor in StopInteract() was null"));
	}
	if (!bOnlyLocal)
	{
		if (bInitiatedByServer && HasAuthority())
		{
			if (!IsNetMode(NM_Standalone))
			{
				Client_Reliable_ForceStopInteracting(InInteractableActor);
			}
		}
		else if (!bInitiatedByServer && (IsLocallyControlled()))
		{
			Server_Reliable_StopInteracting(InInteractableActor);
		}
	}
}

void AEXCharacter::OnCrouchUpdate(float RelativeLocZ)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FVector CameraRelLoc = FirstPersonCameraComponent->GetRelativeLocation();
		CameraRelLoc.Z = RelativeLocZ;
		FirstPersonCameraComponent->SetRelativeLocation(CameraRelLoc);
	}
	else
	{
		BaseTranslationOffset.Z = RelativeLocZ;
	}
}

void AEXCharacter::Server_Reliable_StartInteracting_Implementation(AActor* SInteractableActor, bool bLookAt)
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		return;
	}
	if (IEXInteractable::Execute_CanInteract(SInteractableActor, this, bLookAt)) 
	{
		StartInteract(SInteractableActor);
	}
	else
	{
		Client_Reliable_ForceStopInteracting(SInteractableActor);
	}
}

bool AEXCharacter::Server_Reliable_StartInteracting_Validate(AActor* SInteractableActor, bool bLookAt)
{
	// #EXTODO2 bLookAt parameter
	return true;
}

void AEXCharacter::Server_Reliable_StopInteracting_Implementation(AActor* SInteractableActor)
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		return;
	}
	StopInteract(InteractableActor, false);
}

bool AEXCharacter::Server_Reliable_StopInteracting_Validate(AActor* SInteractableActor)
{
	return true;
}

void AEXCharacter::Client_Reliable_ForceStopInteracting_Implementation(AActor* SInteractableActor)
{
	if (InteractableActor == SInteractableActor)
	{
		StopInteract(InteractableActor, true);
	}
	// If whatever we want to stop interacting with requires a tool, unequip that tool
	if (AEXInteract* Interact = Cast<AEXInteract>(SInteractableActor))
	{
		const EEngiTool ToolType = Interact->GetRequiredTool();
		if (ToolType != EEngiTool::None) 
		{
			InventoryComponent->Unequip(InventoryComponent->GetTool(ToolType), true);
		}
	}
}

void AEXCharacter::SetHealth(float InHealth)
{
	float OldHealth = Health;
	Health = InHealth;
	if(HasAuthority())
	{
		RepHealth = FMath::CeilToInt(Health);
	}
	else
	{
		HealthChanged(OldHealth);
	}
}

void AEXCharacter::HealthChanged(float OldHealth)
{
	float HealthDelta = Health - OldHealth;
	if (OnHealthChanged.IsBound())
	{
		FDamageEvent DamageEvent;
		OnHealthChanged.Broadcast(Health, HealthDelta, DamageEvent, nullptr, nullptr);
	}
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealth(Health / MaxHealth);
	}

	if (bShowHealIcon)
	{
		SetHealIconVisibility(LowOnHealth());
	}
	
	if ((OldHealth > 0) && (Health <= 0))
	{
		OnDeath();
	}
	else if ((OldHealth <= 0) && (Health >= 0))
	{
		OnRevive();
	}
	if (Health <= -MaxDownHealth)
	{
		DoDeathEffect();
	}
}

void AEXCharacter::StartSlowHeal()
{
	if ((SlowHealingRate > 0) && (HealingRate == 0.f) && (!IsDead()))
	{
		SetHealingRate(SlowHealingRate, EXController);
	}
}

void AEXCharacter::LookTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& TraceData) const
{
	if (!EXController || IsDead())
	{
		EXController->SetInteractionIconVisibility(false);
		// No longer valid
		return;
	}
	AActor* Target;
	if (TraceData.OutHits.Num() > 0)
	{
		Target = TraceData.OutHits[0].GetActor();
		if (Target && !IEXInteractable::Execute_CanInteract(Target, this, true))
		{
			// We got a hit but we can't do anything with it
			Target = nullptr;
		}
	}
	else
	{
		Target = GetInteractableActor(); // Can be null
	}
	EXController->SetInteractionIconVisibility(!!Target);
}

void AEXCharacter::SelfKill()
{
	if (HasAuthority())
	{
		FEXSelfKillDamageEvent SelfKillDamageEvent(SelfKillDamage, SelfKillDamageType);
		TakeDamage(SelfKillDamage, SelfKillDamageEvent, EXController, nullptr);

		if (GMAC)
		{
			GMAC->ResetCourse();
		}

		ForceNetUpdate();
	}
	else
	{
		Server_Reliable_SelfKill();
	}
}

void AEXCharacter::Server_Reliable_SelfKill_Implementation()
{
	SelfKill();
}

bool AEXCharacter::Server_Reliable_SelfKill_Validate()
{
	return true;
}

void AEXCharacter::Die(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (EXPS)
	{
		EXPS->AddDeath(MercType);
		if (EventInstigator)
		{
			if (EXController == EventInstigator)
			{
				// #Selfkill
			}
			else
			{
				AEXPlayerState* OtherPS = Cast<AEXPlayerState>(EventInstigator->PlayerState);
				if (!EXPS->OnSameTeam(OtherPS))
				{
					if (DamageEvent.IsOfType(FEXPointDamageEvent::ClassID))
					{
						const FEXPointDamageEvent* EXDamageEvent = (FEXPointDamageEvent*)&DamageEvent;
						OtherPS->AddKill(EXDamageEvent->MercType);
					}
					else if (DamageEvent.IsOfType(FEXRadialDamageEvent::ClassID))
					{
						const FEXRadialDamageEvent* EXDamageEvent = (FEXRadialDamageEvent*)&DamageEvent;
						OtherPS->AddKill(EXDamageEvent->MercType);
					}
				}
			}
		}
		AddScoreForKill(MaxHealth, true);

		// Player died
		UEXDamageType* DamageType = Cast<UEXDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		UTexture2D* KillIcon = DamageType ? DamageType->KillIcon : nullptr;
		Multicast_Reliable_PlayerDied(Controller->GetPlayerState<AEXPlayerState>(), KillIcon, EventInstigator->GetPlayerState<AEXPlayerState>());
	}

	// Stop doing the objective
	StopInteract(InteractableActor, true);
	ForceNetUpdate();
	OnDeath();
}

void AEXCharacter::Multicast_Reliable_PlayerDied_Implementation(AEXPlayerState* PS, UTexture2D* KillIcon, AEXPlayerState* Killer)
{
	if (!HasAuthority())
	{
		const FString KillerName = Killer ? Killer->GetPlayerName() : FString();
		if (EXController)
		{
			EXController->AddToKillFeed(PS->GetPlayerName(), KillIcon, KillerName);
		}
	}
}

void AEXCharacter::TornOff()
{
	if (EXController && EXController->GetHUDWidget())
	{
		EXController->GetHUDWidget()->SetCharacter(nullptr);
	}
	if (!bGibbed)
	{
		DoDeathEffect();
	}
	EXPS = nullptr;
}

void AEXCharacter::WeaponEquipped(EWeapon Type)
{
	if (bStatTracking)
	{
		EXPS->WeaponEquipped(Type);
	}
}

void AEXCharacter::WeaponUnequipped()
{
	if (bStatTracking)
	{
		EXPS->WeaponUnequipped();
	}
}

void AEXCharacter::AbilityEquipped(EAbility Type)
{
	if (bStatTracking)
	{
		EXPS->AbilityEquipped(Type);
	}
}

void AEXCharacter::AbilityUnequipped()
{
	if (bStatTracking)
	{
		EXPS->AbilityUnequipped();
	}
}

bool AEXCharacter::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (IsValid(this))
	{
		return false;
	}
	if (EXPS && Player->GetEXPlayerState() && EXPS->OnSameTeam(Player->GetEXPlayerState()))
	{
		return IsDead();
	}
	return false;
}

bool AEXCharacter::StartInteract_Implementation(AEXCharacter* Player)
{
	int32 DefibIdx = Player->GetInventoryComponent()->GetInventoryList().IndexOfByPredicate(
		[&](const UEXInventory* Inv) { return Inv->StaticClass()->IsChildOf(UEXDefib::StaticClass()); }
	);
	if (DefibIdx != INDEX_NONE)
	{
		if (IsLocallyControlled())
		{
			Player->GetInventoryComponent()->QuickUsePressed(DefibIdx);
		}
	}
	else
	{
		if (HasAuthority())
		{
			InteractingPlayers.Add(Player);
			Player->StartProgress(TimeToSlowRevive);
			Player->OnInteractCompleted.AddDynamic(this, &AEXCharacter::OnSlowRes);
		}
		else
		{
			Player->StartSlowRes();
		}
	}
	return true;
}

bool AEXCharacter::StopInteract_Implementation(AEXCharacter* Player)
{
	int32 DefibIdx = Player->GetInventoryComponent()->GetInventoryList().IndexOfByPredicate([&](const UEXInventory* Inv) { return Inv->StaticClass()->IsChildOf(UEXDefib::StaticClass()); });
	if (DefibIdx != INDEX_NONE)
	{
		if (IsLocallyControlled())
		{
			Player->GetInventoryComponent()->QuickUseReleased(DefibIdx);
		}
	}
	else
	{
		if (HasAuthority())
		{
			InteractingPlayers.Remove(Player);
			Player->StopProgress();
			Player->OnInteractCompleted.RemoveDynamic(this, &AEXCharacter::OnSlowRes);
		}
		else
		{
			Player->StopSlowRes();

		}
	}
	return true;
}

void AEXCharacter::StartSlowRes()
{
	AnimInst->StartSlowRes();
}

void AEXCharacter::StopSlowRes()
{
	AnimInst->StopSlowRes();
}

void AEXCharacter::OnSlowRes(AEXCharacter* Player)
{
	Revive(SlowResHealthGiven, Player->GetEXController(), Player->GetMercType());
	UE_LOG(LogEXChar, Warning, TEXT("Slow revive"));
}

float AEXCharacter::GetMovementMultiplier() const
{
	return 1.f;
}

AActor* AEXCharacter::GetLookAtInteractableActor() const
{
	AActor* Target = UEXGameplayStatics::GetActorLookingAt(this, GetMaxTraceRange());
	if (Target)
	{
		if (IEXInteractable::Execute_CanInteract(Target, this, true))
		{
			return Target;
		}
	}
	return nullptr;
}

AActor* AEXCharacter::GetInteractableActor() const
{
	for (AActor* Actor : EXController->GetLevelRules()->GetInteractableActors())
	{
		if (Actor && IEXInteractable::Execute_CanInteract(Actor, this, false))
		{
			return Actor;
		}
	}
	return nullptr;
}

void AEXCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	EXPS = GetPlayerState<AEXPlayerState>();
	if (EXPS)
	{
		OnPlayerStateInitialized();
	}
}

void AEXCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (IsLocallyControlled())
	{
		OutLocation = FirstPersonCameraComponent->GetComponentLocation();
		OutRotation = FirstPersonCameraComponent->GetComponentRotation();
	}
	else
	{
		Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	}
}

void AEXCharacter::SetFOVSensAdjustment(float Val)
{
	FOVSensAdjustment = Val;
}

void AEXCharacter::Reset()
{
	if (InteractableActor)
	{
		StopInteract(InteractableActor, true, false);
	}
	Gib(FDamageEvent(), nullptr, nullptr);
	Super::Reset();
}

void AEXCharacter::UnPossessed()
{
	Super::UnPossessed();
	if (EXController && EXController->IsLocalController())
	{
		GetInventoryComponent()->ChangePerspective(true);
	}
}

void AEXCharacter::OnPlayerStateInitialized()
{
	if(GetLocalRole() == ROLE_SimulatedProxy)
	{
		AEXCharacter* LocalChar = GetWorld()->GetFirstPlayerController()->GetPawn<AEXCharacter>();
		if (LocalChar && EXPS->OnSameTeam(LocalChar->GetEXPlayerState()))
		{
			LocalChar->ShowMyIcons(this);
		}
	}
}

void AEXCharacter::CheckJumpInput(float DeltaTime)
{
	if (EXCharacterMovement)
	{
		if (bPressedJump)
		{
			// If this is the first jump and we're already falling,
			// then increment the JumpCount to compensate.
			const bool bFirstJump = JumpCurrentCount == 0;
			if (bFirstJump && EXCharacterMovement->IsFalling())
			{
				JumpCurrentCount++;
			}

			const bool bDidJump = CanJump() && EXCharacterMovement->DoJump(bClientUpdating);
			if (bDidJump)
			{
				// Transition from not (actively) jumping to jumping.
				if (!bWasJumping)
				{
					JumpCurrentCount++;
					JumpForceTimeRemaining = GetJumpMaxHoldTime();
					OnJumped();
				}
			}

			bWasJumping = bDidJump;
		}
	}
}

void AEXCharacter::ClearJumpInput(float DeltaTime)
{
	Super::ClearJumpInput(DeltaTime);
}

void AEXCharacter::SetHelpIconsToShow(bool bAmmo, bool bHeal, bool bRevive)
{
	check(GetLocalRole() == ROLE_SimulatedProxy);
	bShowAmmoIcon = bAmmo;
	bShowHealIcon = bHeal;
	bShowReviveIcon = bRevive;
}

void AEXCharacter::SetHelpActions(bool bAmmo, bool bHeal, bool bRevive)
{
	check(GetLocalRole() == ROLE_AutonomousProxy);
	bCanHelpAmmo = bAmmo;
	bCanHelpHeal = bHeal;
	bCanHelpRevive = bRevive;
}

void AEXCharacter::DoDeathEffect()
{
	bGibbed = true;
	// Character role is authority after tear off, so check controller
	if (EXController && EXController->GetLocalRole() == ROLE_AutonomousProxy)
	{
		GetMesh1P()->SetVisibility(false);
		GetMesh()->SetVisibility(true);

		TArray<AEXCharacter*> Teammates = UEXGameplayStatics::GetTeammates(this);
		for (AEXCharacter* Teammate : Teammates)
		{
			Teammate->SetHelpIconsToShow(false, false, false);
		}
	}

	if (AnimInst)
	{
		AnimInst->SetGibbed(true);
	}
	if (StatusIcon)
	{
		StatusIcon->SetStatus(EMercStatus::Gibbed);
	}

	// Disable collision
	FCollisionResponseContainer CRC;
	CRC.SetResponse(ECC_WeaponTrace, ECR_Ignore);
	CRC.SetResponse(ECC_Projectile, ECR_Ignore);
	CRC.SetResponse(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannels(CRC);

	SpottedComp->SetVisibility(false, true);

	SetOwner(nullptr);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);

	SetMaterialColor(GibbedMaterialColor);

	K2_OnGib();
	SetLifeSpan(PostGibLifeSpan);
}

FRotator AEXCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void AEXCharacter::Revive(float ReviveHealth, AController* EventInstigator, EMerc OtherMercType)
{
	if (!bGibbed && (ReviveHealth > 0))
	{
		SetHealth(FMath::Min(ReviveHealth, MaxHealth));

		AEXPlayerState* PS = EventInstigator->GetPlayerState<AEXPlayerState>();
		if (PS)
		{
			PS->AddScore(Health, EScoreType::Support, true);
			PS->AddRevive(MercType, Health);
		}

		// Stop any slow reviving
		const int32 NumInteracting = InteractingPlayers.Num();
		for (int32 PlayerIdx = NumInteracting - 1; PlayerIdx >= 0; --PlayerIdx)
		{
			InteractingPlayers[PlayerIdx]->StopProgress();
			// This will edit the InteractingPlayers array, thats why we iterate backwards
			InteractingPlayers[PlayerIdx]->StopInteract(this, true);
		}
		DamageTaken.Empty();

		OnRevive();
	}
}

void AEXCharacter::SetCollisionEnabled(bool bEnabled)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, bEnabled ? ECR_Block : ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, bEnabled ? ECR_Block : ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Ability, bEnabled ? ECR_Overlap : ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Vehicle, bEnabled ? ECR_Block : ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, bEnabled ? ECR_Block : ECR_Ignore);
}

void AEXCharacter::Gib(struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bGibbed || !HasAuthority())
	{ 
		return;
	}
	bGibbed = true;

	if (!DamageEvent.IsOfType(FEXSelfKillDamageEvent::ClassID))
	{
		if (GM)
		{
			AddScoreForKill(MaxDownHealth, false);
		}
	}
	// Stop any slow reviving
	{
		const int32 NumInteracting = InteractingPlayers.Num();
		for (int32 PlayerIdx = NumInteracting - 1; PlayerIdx >= 0; --PlayerIdx)
		{
			InteractingPlayers[PlayerIdx]->StopProgress();
			InteractingPlayers[PlayerIdx]->StopInteract(this, true); // This will edit the InteractingPlayers array
		}
	}

	// Put player to spec
	{
		if (EXPS)
		{
			EXPS->SetIsSpectator(true);
		}
		if (EXController)
		{
			EXController->ChangeState(NAME_Spectating);
			EXController->ClientGotoState(NAME_Spectating);
			EXController->UnPossess();
		}
	}
	if (UEXInventory* EquippedInventory = GetInventoryComponent()->GetEquippedInventory())
	{
		EquippedInventory->SetVisibility(false);
	}

	if (GS && GS->IsMatchInProgress())
	{
		StopTracking();
	}
	// #TearOff
	TearOff();
	ForceNetUpdate();
	SetLifeSpan(PostGibLifeSpan);
}

void AEXCharacter::StartProgress(float Goal, float ProgressModifier)
{
	ProgressComponent->SetGoal(Goal);
	ProgressComponent->StartProgress(GetEXController(), ProgressModifier);
	AEXPlayerController* Player = Cast<AEXPlayerController>(GetController());
	if (Player)
	{
		Player->Client_Reliable_SetInteractProgressVisibility(true);
	}
}

void AEXCharacter::StopProgress()
{
	if (HasAuthority())
	{
		ProgressComponent->StopProgress(EXController);
		EXController->Client_Reliable_SetInteractProgressVisibility(false);
	}
}

float AEXCharacter::GetProgress() const
{
	return ProgressComponent->GetRatio();
}

void AEXCharacter::OnRep_Spotted()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		EXController->SetSpotted(bSpotted);
	}
	else
	{
		SpottedComp->SetVisibility(bSpotted);
	}
}

void AEXCharacter::SpottedOver()
{
	bSpotted = false;
}

void AEXCharacter::Spotted(float Duration)
{
	if (HasAuthority())
	{
		bSpotted = true;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Spotted, this, &AEXCharacter::SpottedOver, Duration, false);
	}
}

void AEXCharacter::OnRep_WeaponType()
{
	WeaponTypeChanged(WeaponType);
}

void AEXCharacter::OnRep_WeaponState()
{
	WeaponStateChanged(WeaponState.WeaponState, WeaponState.Modifier);
}

void AEXCharacter::InteractionCompleted(AEXPlayerController* Player)
{
	OnInteractCompleted.Broadcast(this);
	ProgressComponent->ResetProgress();
}

void AEXCharacter::OnRep_Health()
{
	SetHealth((float)RepHealth);
}

void AEXCharacter::SetMovementSpreadModifier(float Val)
{
	MovementSpreadModifier = Val;
	UEXWeapon* CurrentWeapon = Cast<UEXWeapon>(InventoryComponent->GetEquippedInventory());
	if (CurrentWeapon)
	{
		CurrentWeapon->SetMovementSpreadModifier(MovementSpreadModifier);
	}
}

#pragma region Augments
void AEXCharacter::ApplyAugments()
{
	if (bAugmentsApplied)
	{
		return;
	}
	if (EXPS && InventoryComponent->IsInitialized())
	{
		const TArray<EAugment>& SelectedAugments = EXPS->GetAugments(GetType());
		UE_LOG(LogEXChar, Verbose, TEXT("Augments applied"));
		for (const EAugment& Augment : SelectedAugments)
		{
			(this->*AugmentFunctions[(uint8)Augment])();
		}
		bAugmentsApplied = true;
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEXCharacter::ApplyAugments);
	}
}

void AEXCharacter::AugmentEmpty()
{
	UE_LOG(LogTemp, Warning, TEXT("AugmentEmpty"));
}

void AEXCharacter::AugmentDrilled()
{
	TArray<UEXInventory*> Inventories = GetInventoryComponent()->GetInventoryList();
	for (UEXInventory* Inventory : Inventories)
	{
		if (Inventory)
		{
			Inventory->SetModifier(EWeaponState::Reloading, DrilledModifier);
		}
	}
}

void AEXCharacter::AugmentQuickDraw()
{
	TArray<UEXInventory*> Inventories = GetInventoryComponent()->GetInventoryList();
	for (UEXInventory* Inventory : Inventories)
	{
		if (Inventory)
		{
			Inventory->SetModifier(EWeaponState::Equipping, QuickDrawEquipModifier);
			Inventory->SetModifier(EWeaponState::Unequipping, QuickDrawUnequipModifier);
		}
	}
}

void AEXCharacter::AugmentLooter()
{
	bLooter = true;
}

void AEXCharacter::AugmentRecycle()
{
	bCanRecycle = true;
}

void AEXCharacter::AugmentMechanic()
{
	TMap<EEngiTool, UEXInventory*> SpecialInventory = GetInventoryComponent()->GetSpecialInventoryList();
	Cast<IEXObjectiveTool>(SpecialInventory[EEngiTool::Repair])->SetProgressModifier(MechanicModifier);
	Cast<IEXObjectiveTool>(SpecialInventory[EEngiTool::Defuse])->SetProgressModifier(MechanicModifier);
}

void AEXCharacter::AugmentQuickEye()
{
	EXCharacterMovement->SetWalkModifier(EXCharacterMovement->GetWalkModifier() * QuickEyeMultiplier);
}

void AEXCharacter::AugmentWallJumper()
{
	JumpMaxCount = 3;
}

#pragma endregion Augments

void AEXCharacter::ShowMyIcons(AEXCharacter* Teammate)
{
	Teammate->SetHelpIconsToShow(bCanHelpAmmo, bCanHelpHeal, bCanHelpRevive);
}

void AEXCharacter::SetHealIconVisibility(bool bVisible)
{
	if (HealWidget) 
	{
		HealWidget->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void AEXCharacter::SetReviveIconVisibility(bool bVisible)
{
	if (ReviveWidget)
	{
		ReviveWidget->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void AEXCharacter::SetAmmoIconVisibility(bool bVisible)
{
	if (AmmoWidget)
	{
		AmmoWidget->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

bool AEXCharacter::LowOnHealth() const
{
	return (Health < MaxHealth * HealthRatioForHealIcon) && (Health > 0.f);
}

TSoftObjectPtr<UTexture2D> AEXCharacter::GetIcon(EIconSize Size) const
{
	switch (Size)
	{
		case EIconSize::KillFeed:
			return CharacterIconKillFeed;
			break;
		case EIconSize::Small:
			return CharacterIconSmall;
			break;
		case EIconSize::Medium:
			return CharacterIconMedium;
			break;
		case EIconSize::Large:
			return CharacterIconLarge;
			break;
		default:
			return nullptr;
			break;
	}
}

void AEXCharacter::InitGameStatusWidget()
{
	if (HasAuthority())
	{
		return;
	}
	// Get the HUD of the local PC
	AEXPlayerController* LocalPC = Cast<AEXPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	UEXHUDWidget* HUD = LocalPC ? LocalPC->GetHUDWidget() : nullptr;
	UEXGameStatus* GameStatusWidget = HUD ? HUD->GetGameStatus() : nullptr;

	if (!(GameStatusWidget && EXPS && LocalPC->GetPlayerState<AEXPlayerState>()))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEXCharacter::InitGameStatusWidget);
		return;
	}
	// Get the icon assigned to the player
	if (EXPS)
	{
		SetStatusIcon(GameStatusWidget->GetIcon(EXPS));
	}
}

void AEXCharacter::SetStatusIcon(UEXMercStatusIcon* Icon)
{
	StatusIcon = Icon;
	if (!StatusIcon)
	{
		return;
	}
	
	TSoftObjectPtr<UTexture2D> SmallIcon = GetIcon(EIconSize::Small);
	if (!SmallIcon.IsValid())
	{
		UE_LOG(LogEXUI, Error, TEXT("Small icon isn't loaded"));
		return;
	}

	StatusIcon->SetTexture(SmallIcon);
	const EMercStatus CurrentStatus = GetStatus();
	StatusIcon->SetStatus(CurrentStatus);
}

bool AEXCharacter::BetterEngiThan(AEXCharacter* OtherPlayer, EEngiTool Tool) const
{
	if (!OtherPlayer)
	{
		return true;
	}
	if (Tool == EEngiTool::None)
	{
		return false;
	}

	const float PlayerRepairSpeed = Cast<IEXObjectiveTool>(GetInventoryComponent()->GetTool(Tool))->GetProgressModifier();
	const float OtherRepairSpeed = Cast<IEXObjectiveTool>(OtherPlayer->GetInventoryComponent()->GetTool(Tool))->GetProgressModifier();
	return PlayerRepairSpeed > OtherRepairSpeed;
}

void AEXCharacter::StopHealing()
{
	if (HealingRate == 0.f)
	{
		return;
	}
	HealingRate = 0.f;
	if (HasAuthority())
	{
		AEXPlayerState* PS = HealingInstigator ? HealingInstigator->GetPS() : nullptr;
		if (PS && PS->OnSameTeam(EXPS))
		{
			float HealthGiven = Health - HealingRateStartHealth;
			if (HealingInstigator != EXController)
			{
				PS->AddScore(HealthGiven, EScoreType::Support);
			}
		}
	}
}

void AEXCharacter::AddScoreForKill(float MaxScore, bool bKill)
{
	if (DamageTaken.Num() == 0)
	{
		return;
	}
	Algo::Reverse(DamageTaken);
	float Multiplier = bKill ? 1.0f : GM->GetGibbingXPModifier();

	TMap<AEXPlayerState*, float> Scores;
	float RemainingScore = MaxScore * Multiplier;
	for (const FDamageTaken& Damage : DamageTaken)
	{
		float Score = FMath::Min(Damage.Damage * Multiplier, (float)RemainingScore);

		if (Damage.Player)
		{
			float& PlayerScore = Scores.FindOrAdd(Damage.Player);
			PlayerScore += Score;
		}
		RemainingScore -= Score;
		if (RemainingScore <= 0.f)
		{
			break;
		}
	}
	for (const auto& KVP : Scores)
	{
		const bool bSameTeam = KVP.Key->OnSameTeam(EXPS);
		const int32 ScoreToAdd = FMath::RoundToInt(KVP.Value);
		KVP.Key->AddScore(bSameTeam ? -ScoreToAdd : ScoreToAdd, EScoreType::Combat, true);
	}

	if(DamageTaken[0].Player)
	{
		AEXPlayerState* Killer = DamageTaken[0].Player;
		for (const auto& KVP : Scores)
		{
			if ((KVP.Value > (MaxScore / 2)) && (KVP.Key != Killer))
			{
				const bool bSameTeam = KVP.Key->OnSameTeam(EXPS);
				if (!bSameTeam)
				{
					KVP.Key->AddAssist(KVP.Key->GetCurrentMerc());
				}
			}
		}
	}

	DamageTaken.Empty(10);
}

void AEXCharacter::WeaponStateChanged(EWeaponState InWeaponState, float Modifier)
{
	WeaponState = FWeaponState(InWeaponState, Modifier);
	if (AnimInst)
	{
		AnimInst->WeaponStateChanged(InWeaponState, Modifier);
	}

	OnWeaponStateChanged(InWeaponState, Modifier);
}

void AEXCharacter::WeaponTypeChanged(EWeaponType InWeaponType)
{
	WeaponType = InWeaponType;
	if (AnimInst)
	{
		AnimInst->WeaponTypeChanged(WeaponType);
	}
}

void AEXCharacter::OnRevive()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		SetControlsEnabled(true);
		Mesh1P->SetVisibility(true, true);
		GetMesh()->SetVisibility(false, true);
	}
	else if (bShowReviveIcon)
	{
		SetReviveIconVisibility(false);
	}
	K2_Revive();

	SetCollisionEnabled(true);
	InventoryComponent->OnRevive();

	if (AnimInst)
	{
		AnimInst->SetDead(false);
	}

	if (EXController)
	{
		EXController->CharacterRevived();
	}
	if (StatusIcon)
	{
		StatusIcon->SetStatus(EMercStatus::Alive);
	}
}

void AEXCharacter::OnDeath()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		SetControlsEnabled(false);
		Mesh1P->SetVisibility(false, true);
		GetMesh()->SetVisibility(true, true);
		EXController->OnDeath();
	}
	else if (bShowReviveIcon)
	{
		SetReviveIconVisibility(true);
	}
	K2_OnDeath();

	SetCollisionEnabled(false);
	InventoryComponent->OnDeath();

	if (AnimInst)
	{
		AnimInst->SetDead(true);
	}

	if (EXController)
	{
		EXController->CharacterDied();
	}
	if (StatusIcon)
	{
		StatusIcon->SetStatus(EMercStatus::Downed);
	}
}

void AEXCharacter::StartTracking()
{
	if (IsNetMode(NM_Standalone) || !EXPS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not tracking stats on %s because EXPS is null"), *GetName());
		return;
	}
	if (bStatTracking)
	{
		ensure(0);
		return;
	}
	bStatTracking = true;
	if (EXPS)
	{
		EXPS->MercStarted(GetType());
		UEXInventory* EquippedItem = InventoryComponent->GetEquippedInventory();
		if (EquippedItem)
		{
			if (EquippedItem->IsAnAbility())
			{
				EXPS->AbilityEquipped(EquippedItem->GetAbilityType());
			}
			else
			{
				EXPS->WeaponEquipped(EquippedItem->GetWeaponType());
			}
		}
	}
}

void AEXCharacter::StopTracking()
{
	if (IsNetMode(NM_Standalone))
	{
		return;
	}
	if (!bStatTracking)
	{
		// EXPS is null on AI characters, so it's fine if this is called when not tracking
		ensure(!EXPS && "StopTracking() was called when tracking was already stopped.");
		return;
	}
	bStatTracking = false;
	AEXCharacter* Character = EXPS ?  EXPS->GetPawn<AEXCharacter>() : nullptr;
	UEXInventory* Inventory = Character ? Character->GetInventoryComponent()->GetEquippedInventory() : nullptr;
	if (EXPS && Character && Inventory)
	{
		if (Inventory)
		{
			if (Inventory->IsAnAbility())
			{
				EXPS->AbilityUnequipped();
			}
			else
			{
				EXPS->WeaponUnequipped();
			}
			EXPS->MercStopped();
		}
	}
}

void AEXCharacter::InstaHeal(float AddedHealth, AController* EventInstigator)
{
	if (HasAuthority())
	{
		const float DeltaHealth = FMath::Min(AddedHealth, MaxHealth - Health);
		SetHealth(FMath::Min(Health + AddedHealth, MaxHealth));

		if(!IsNetMode(NM_Standalone))
		{
			AEXPlayerState* OtherPS = EventInstigator ? EventInstigator->GetPlayerState<AEXPlayerState>() : nullptr;
			if (OtherPS && OtherPS->OnSameTeam(EXPS))
			{
				OtherPS->AddScore(DeltaHealth, EScoreType::Support);
			}
		}

		K2_OnInstaHeal();
	}
}

void AEXCharacter::SetHealingRate(float NewHealingRate, AController* EventInstigator)
{
	if (!HasAuthority())
	{
		return;
	}
	if (FMath::IsNearlyZero(Health - MaxHealth)) // If full hp
	{
		return;
	}

	if (NewHealingRate <= HealingRate) // Only heal if new heal is better than previous
	{
		return;
	}
	StopHealing();
	HealingRate = NewHealingRate;
	HealingInstigator = Cast<AEXPlayerController>(EventInstigator);
	HealingRateStartHealth = Health;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SlowHeal);
	K2_OnSlowHeal();
}

void AEXCharacter::OnStartSprint()
{
	if(EXController && EXController->IsLocalController())
	{
		if (UEXInventory* EquippedInventory = InventoryComponent->GetEquippedInventory())
		{
			if (EquippedInventory->IsReloading())
			{
				EquippedInventory->CancelReload();
			}
		}
	}
}

float AEXCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bGibbed)
	{
		return 0.f;
	}
	// Modify damage based based on gamemode
	DamageAmount = GM ? GM->ModifyDamage(DamageAmount, this, DamageEvent, EventInstigator, DamageCauser) : DamageAmount;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bSpawnProtectionEnabled)
	{
		ActualDamage *= SpawnProtectionDamageModifier;
	}

	if (ActualDamage > 0)
	{
		StopHealing();

		bool bWasDead = IsDead();
		float OldHealth = Health;
		SetHealth(Health - ActualDamage);

		// Timer for slow healing needs to start over
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SlowHeal);
		if (!IsDead())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SlowHeal, this, &AEXCharacter::StartSlowHeal, SlowHealingDelay);
		}

		OnHealthChanged.Broadcast(Health, ActualDamage, DamageEvent, EventInstigator, DamageCauser);

		AEXPlayerState* InstigatorPS = EventInstigator ? EventInstigator->GetPlayerState<AEXPlayerState>() : nullptr;
		if (!IsDead()) // Taking damage while still up
		{
			DamageTaken.Add(FDamageTaken(InstigatorPS, ActualDamage));
			if (EXPS)
			{
				EXPS->DamageTaken(MercType, ActualDamage);
			}
		}
		else if (!bWasDead && IsDead()) // Going down
		{
			DamageTaken.Add(FDamageTaken(InstigatorPS, OldHealth));
			if (EXPS)
			{
				EXPS->DamageTaken(MercType, OldHealth);
			}
			Die(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			// Damage that carries over from kill to gibbing
			DamageTaken.Add(FDamageTaken(InstigatorPS, ActualDamage - OldHealth));
		}
		else // Taking damage while down
		{
			DamageTaken.Add(FDamageTaken(InstigatorPS, ActualDamage));
		}

		if (!bWasDead) 
		{
			OnHit(ActualDamage);
		}
		if (Health <= -MaxDownHealth)
		{
			Gib(DamageEvent, EventInstigator, DamageCauser);
		}
	}
	return ActualDamage;
}

