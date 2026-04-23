// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/EXCharacterMovement.h"
#include "Player/EXCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Inventory/EXInventoryComponent.h"
#include "EX.h"
#include "GameFramework/GameNetworkManager.h"
#include "System/EXGameplayStatics.h"
#include "Player/EXAnimInstance.h"

#define FLAG_SPRINT FSavedMove_Character::FLAG_Custom_0
#define FLAG_WALK FSavedMove_Character::FLAG_Custom_1
#define FLAG_LONG_JUMP FSavedMove_Character::FLAG_Custom_2

void FSavedMove_EXMovement::Clear()
{
	Super::Clear();

	// Clear variables back to their default states
	bSavedSprinting = false;
	bSavedWalking = false;
	bSavedLongJumping = false;
	CrouchAmount = 0.f;
}

uint8 FSavedMove_EXMovement::GetCompressedFlags() const
{
	uint8 Result = 0;

	if (bPressedJump)
	{
		Result |= FLAG_JumpPressed;
	}
	if (bWantsToCrouch)
	{
		Result |= FLAG_WantsToCrouch;
	}
	if (bSavedSprinting)
	{
		Result |= FLAG_SPRINT;
	}
	if (bSavedWalking)
	{
		Result |= FLAG_WALK;
	}
	if (bSavedLongJumping)
	{
		Result |= FLAG_LONG_JUMP;
	}
	return Result;
}

bool FSavedMove_EXMovement::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_EXMovement* NewEXMove = (const FSavedMove_EXMovement*)NewMove.Get();
	if (bSavedSprinting != NewEXMove->bSavedSprinting)
	{
		return false;
	}
	if (bSavedWalking != NewEXMove->bSavedWalking)
	{
		return false;
	}
	if (bSavedLongJumping != NewEXMove->bSavedLongJumping)
	{
		return false;
	}
	if (bWantsToCrouch != NewEXMove->bWantsToCrouch)
	{
		return false;
	}
	if (FMath::IsNearlyZero(CrouchAmount) != FMath::IsNearlyZero(NewEXMove->CrouchAmount))
	{
		return false;
	}
	if (FMath::IsNearlyZero(CrouchAmount - 1) != FMath::IsNearlyZero(NewEXMove->CrouchAmount - 1))
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_EXMovement::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	AEXCharacter* EXCharacter = Cast<AEXCharacter>(Character);
	UEXCharacterMovement* EXCharacterMovement = EXCharacter ? EXCharacter->GetEXCharacterMovement() : nullptr;
	if (EXCharacter && EXCharacterMovement)
	{
		bSavedSprinting = EXCharacterMovement->IsSprinting();
		bSavedWalking = EXCharacterMovement->IsWalking();
		bSavedLongJumping = EXCharacterMovement->bWantsToLongJump;
		CrouchAmount = EXCharacterMovement->GetCrouchAmount();
	}
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
}

void FSavedMove_EXMovement::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UEXCharacterMovement* CharMov = Cast<UEXCharacterMovement>(Character->GetCharacterMovement());
	if (CharMov)
	{
	}
}

void FSavedMove_EXMovement::SetInitialPosition(ACharacter* Character)
{
	Super::SetInitialPosition(Character);

	AEXCharacter* EXCharacter = Cast<AEXCharacter>(Character);
	UEXCharacterMovement* EXCharacterMovement = EXCharacter ? EXCharacter->GetEXCharacterMovement() : nullptr;
	if (EXCharacter && EXCharacterMovement)
	{
	}
}

void FSavedMove_EXMovement::PostUpdate(ACharacter* Character, EPostUpdateMode PostUpdateMode)
{
	Super::PostUpdate(Character, PostUpdateMode);

	AEXCharacter* EXCharacter = Cast<AEXCharacter>(Character);
	UEXCharacterMovement* EXCharacterMovement = EXCharacter ? EXCharacter->GetEXCharacterMovement() : nullptr;
	if (EXCharacter && EXCharacterMovement)
	{
		UCapsuleComponent* Capsule = EXCharacter->GetCapsuleComponent();
		SavedLocation.Z -= Capsule->GetScaledCapsuleHalfHeight();
	}
}

void FSavedMove_EXMovement::CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation)
{
	Super::CombineWith(OldMove, InCharacter, PC, OldStartLocation);

	const FSavedMove_EXMovement* OldEXMove = (const FSavedMove_EXMovement*)OldMove;
}

FNetworkPredictionData_Client_EXMovement::FNetworkPredictionData_Client_EXMovement(const UCharacterMovementComponent& ClientMovement) 
	: FNetworkPredictionData_Client_Character(ClientMovement)
{
}


void UEXCharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	if (!EXCharacterOwner)
	{
		return;
	}

	// The Flags parameter contains the compressed input flags that are stored in the saved move.
	// UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	// It basically just resets the movement component to the state when the move was made so it can simulate from there.

	const bool bWasPressingJump = EXCharacterOwner->bPressedJump;
	CharacterOwner->bPressedJump = ((Flags & FSavedMove_Character::FLAG_JumpPressed) != 0);
	bWantsToCrouch = ((Flags & FSavedMove_Character::FLAG_WantsToCrouch) != 0);

	if (EXCharacterOwner->HasAuthority())
	{
		// bIsCrouched is only set to call OnReps
		EXCharacterOwner->bIsCrouched = bWantsToCrouch;

		bool bOldWantsToSprint = bWantsToSprint;
		// Detect change in sprint press on the server
		bWantsToSprint = ((Flags & FLAG_SPRINT) != 0);

		if (!CharacterOwner->bPressedJump && 
			((!EXCharacterOwner->bIsCrouched && bWantsToCrouch && IsRunning())
				|| (bOldWantsToSprint && !bWantsToSprint && IsRunning())))
		{
			StartLongJumpWindow();
		}

		bWantsToWalk = ((Flags & FLAG_WALK) != 0);
		bWantsToLongJump = ((Flags & FLAG_LONG_JUMP) != 0);

		// Detect change in jump press on the server
		{
			const bool bIsPressingJump = CharacterOwner->bPressedJump;
			if (bIsPressingJump && !bWasPressingJump)
			{
				EXCharacterOwner->Jump();
			}
			else if (!bIsPressingJump)
			{
				EXCharacterOwner->StopJumping();
			}
		}
	}
}

class FNetworkPredictionData_Client* UEXCharacterMovement::GetPredictionData_Client() const
{
	check(PawnOwner && !PawnOwner->HasAuthority());

	if (!ClientPredictionData)
	{
		UEXCharacterMovement* MutableThis = const_cast<UEXCharacterMovement*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_EXMovement(*this);
		/**
		 * Copied value from UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance.
		 * Maximum distance character is allowed to lag behind server location when interpolating between updates. 
		 */
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		/**
		 * Copied value from UCharacterMovementComponent::NetworkNoSmoothUpdateDistance.
		 * Maximum distance beyond which character is teleported to the new server location without any smoothing. 
		*/
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UEXCharacterMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if ((EXCharacterOwner->GetLocalRole() == ROLE_AutonomousProxy) || (IsNetMode(NM_Standalone)))
	{
		EXCharacterOwner->GetInventoryComponent()->ApplyDeferredFireInputs();
	}
}

void UEXCharacterMovement::UpdateCrouch(float DeltaTime)
{
	const float CrouchDelta = DeltaTime * ((MovementMode == EMovementMode::MOVE_Walking) ? CrouchTimeInv : CrouchJumpTimeInv);

	const bool bClientSimulation = (EXCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy);
	if (bWantsToCrouch && (CrouchAmount < 1.f) && (bClientSimulation || CanCrouchInCurrentState()))
	{
		const float NewCrouchAmount = FMath::Min(CrouchAmount + CrouchDelta, 1.f);
		if (CrouchResize(NewCrouchAmount, NewCrouchAmount - CrouchAmount, bClientSimulation))
		{
			CrouchAmount = NewCrouchAmount;
			if (!IsNetMode(NM_DedicatedServer))
			{
				EXCharacterOwner->GetAnimInstance()->SetCrouchAmount(CrouchAmount);
			}
		}
	}
	else if (!bWantsToCrouch && (CrouchAmount > 0.f))
	{
		const float NewCrouchAmount = FMath::Max(CrouchAmount - CrouchDelta, 0.f);
		if (CrouchResize(NewCrouchAmount, NewCrouchAmount - CrouchAmount, bClientSimulation))
		{
			CrouchAmount = NewCrouchAmount;
			if (!IsNetMode(NM_DedicatedServer))
			{
				EXCharacterOwner->GetAnimInstance()->SetCrouchAmount(CrouchAmount);
			}
		}
	}
}

void UEXCharacterMovement::Crouch(bool bClientSimulation /*= false*/)
{
	bWantsToCrouch = true;
	if (IsRunning())
	{
		StartLongJumpWindow();
	}
}

void UEXCharacterMovement::UnCrouch(bool bClientSimulation /*= false*/)
{
	bWantsToCrouch = false;
}

bool UEXCharacterMovement::IsCrouching() const
{
	return CrouchAmount > 0.f;
}

void UEXCharacterMovement::StartLongJumping()
{
	bWantsToLongJump = true;
}

void UEXCharacterMovement::StopLongJumping()
{
	bWantsToLongJump = false;
}

void UEXCharacterMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Super handles crouching changes but we don't need that here
}

void UEXCharacterMovement::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	// Super handles crouching changes but we don't need that here
}

bool UEXCharacterMovement::CanCrouchInCurrentState() const
{
	return ((EXCharacterOwner->GetVelocity().SizeSquared2D() < (FMath::Square(MaxWalkSpeed) * 1.1f) || IsFalling())
			&& !EXCharacterOwner->IsDead());
}

bool UEXCharacterMovement::IsRunning() const
{
	return (MovementMode == EMovementMode::MOVE_Walking)
		&& (EXCharacterOwner->GetVelocity().SizeSquared2D() > (FMath::Square(GetMaxSprintSpeed(GetWeaponModifier())) * .9f));
}

float UEXCharacterMovement::GetWeaponModifier() const
{
	if (!EXCharacterOwner)
	{
		return 1.f;
	}
	// We only need the time stamp on dedicated server
	const float TimeStamp = IsNetMode(NM_DedicatedServer) ? GetClientTimeStamp() : 0;
	return EXCharacterOwner->GetInventoryComponent()->GetSpeedModifier(TimeStamp);
}

void UEXCharacterMovement::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
	if (!HasValidData())
	{
		return;
	}
	UpdateFromCompressedFlags(CompressedFlags);
	CharacterOwner->CheckJumpInput(DeltaTime);

	Acceleration = ConstrainInputAcceleration(NewAccel);
	Acceleration = Acceleration.GetClampedToMaxSize(GetMaxAcceleration());
	AnalogInputModifier = ComputeAnalogInputModifier();

	PerformMovement(DeltaTime);

	// Check if data is valid as PerformMovement can mark character for pending kill
	if (!HasValidData())
	{
		return;
	}

	// If not playing root motion, tick animations after physics. We do this here to keep events, notifies, states and transitions in sync with client updates.
	if (CharacterOwner 
		&& !CharacterOwner->bClientUpdating 
		/*&& !CharacterOwner->IsPlayingRootMotion()*/
		&& CharacterOwner->GetMesh())
	{
		TickCharacterPose(DeltaTime);

		// Trigger Events right away, as we could be receiving multiple ServerMoves per frame.
		CharacterOwner->GetMesh()->ConditionallyDispatchQueuedAnimEvents();
	}

	// #LISTEN
	//if (CharacterOwner && UpdatedComponent)
	//{
	//	// Smooth local view of remote clients on listen servers
	//	if (CharacterMovementCVars::NetEnableListenServerSmoothing &&
	//		CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy &&
	//		IsNetMode(NM_ListenServer))
	//	{
	//		SmoothCorrection(OldLocation, OldRotation, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat());
	//	}
	//}
}

bool UEXCharacterMovement::ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	bool bError = Super::ServerCheckClientError(ClientTimeStamp, DeltaTime, Accel, ClientWorldLocation, RelativeClientLocation, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);

	if (bError)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerCheckClientError %d"), (int32)ClientMovementMode);
	}

	return bError;
}

bool UEXCharacterMovement::ServerExceedsAllowablePositionError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	// Check for disagreement in movement mode
	const uint8 CurrentPackedMovementMode = PackNetworkMovementMode();
	if (CurrentPackedMovementMode != ClientMovementMode)
	{
		// Consider this a major correction, see SendClientAdjustment()
		bNetworkLargeClientCorrection = true;
		UE_LOG(LogTemp, Warning, TEXT("Error CrouchAmount %f"), CrouchAmount);
		return true;
	}

	FVector LocDiff = UpdatedComponent->GetComponentLocation()  - ClientWorldLocation;
	if (!MovementBaseUtility::UseRelativeLocation(ClientMovementBase))
	{
		const UCapsuleComponent* const Capsule = Cast<UCapsuleComponent>(UpdatedComponent);
		LocDiff -= FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight());
	}
	const AGameNetworkManager* GameNetworkManager = (const AGameNetworkManager*)(AGameNetworkManager::StaticClass()->GetDefaultObject());
	if (GameNetworkManager->ExceedsAllowablePositionError(LocDiff))
	{
		bNetworkLargeClientCorrection |= (LocDiff.SizeSquared() > FMath::Square(NetworkLargeClientCorrectionDistance));

		if (!MovementBaseUtility::UseRelativeLocation(ClientMovementBase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Exceeded allowable position error: %s"), *LocDiff.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Exceeded allowable position error: %s"), *LocDiff.ToString());
		}

		return true;
	}
	return false;
}

void UEXCharacterMovement::SimulateMovement(float DeltaTime)
{
	UpdateCrouch(DeltaTime);
	Super::SimulateMovement(DeltaTime);
}

void UEXCharacterMovement::PerformMovement(float DeltaTime)
{
	UpdateCrouch(DeltaTime);
	Super::PerformMovement(DeltaTime);
}

bool UEXCharacterMovement::CanDelaySendingMove(const FSavedMovePtr& NewMove)
{
	if (!Super::CanDelaySendingMove(NewMove))
	{
		return false;
	}
	return true;
}

void UEXCharacterMovement::ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration)
{
	Super::ReplicateMoveToServer(DeltaTime, NewAcceleration);
}

void UEXCharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	EXCharacterOwner = Cast<AEXCharacter>(GetOwner());
	const AEXCharacter* DefaultCharacter = EXCharacterOwner->GetClass()->GetDefaultObject<AEXCharacter>();
	DefaultCapsuleHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	DefaultMeshRelativeZ = DefaultCharacter->GetMesh()->GetRelativeLocation().Z;
	DefaultCameraRelativeZ = DefaultCharacter->GetFPSCamera()->GetRelativeLocation().Z;

	CrouchTimeInv = 1.f / CrouchTime;
	CrouchJumpTimeInv = 1.f / CrouchJumpTime;
}

float UEXCharacterMovement::GetMaxSpeed() const
{
	if (!EXCharacterOwner || EXCharacterOwner->IsDead())
	{
		return 0.f;
	}

	const float WeaponModifier = GetWeaponModifier();
	if (IsCrouching())
	{
		return MaxWalkSpeedCrouched * WeaponModifier;
	}
	if (EXCharacterOwner->IsInteracting())
	{
		return MaxWalkSpeed * WeaponModifier * EXCharacterOwner->GetInteractMovementMultiplier();
	}
	if (IsOnJumpCooldown())
	{
		return MaxWalkSpeed * WeaponModifier * (bLongJump ? LongJumpCooldownModifier : JumpCooldownModifier);
	}
	if (IsSprinting())
	{
		return GetMaxSprintSpeed(WeaponModifier);
	}
	if (IsWalking())
	{
		return MaxWalkSpeed * WeaponModifier * WalkModifier;
	}
	return MaxWalkSpeed * WeaponModifier;
}

float UEXCharacterMovement::GetMaxAcceleration() const
{
	const float CurrentMaxAcceleration = Super::GetMaxAcceleration();

	if (IsAccelOnJumpCooldown())
	{
		return CurrentMaxAcceleration * (bLongJump ? LongJumpCooldownAccelerationMultiplier : JumpCooldownAccelerationMultiplier);
	}
	if (IsSprinting())
	{
		return CurrentMaxAcceleration * SprintAccelerationMultiplier;
	}

	return CurrentMaxAcceleration;
}

void UEXCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UEXCharacterMovement::LongJump()
{
	Velocity = Velocity.GetSafeNormal2D() * LongJumpForwardVelocity;
	Velocity.Z = JumpZVelocity;
	SetMovementMode(MOVE_Falling);
	bLongJump = true;
	EXCharacterOwner->OnLongJump();
}

bool UEXCharacterMovement::DoJump(bool bReplayingMoves)
{
	FVector Normal;
	if (!IsFalling())
	{
		if ((bWantsToLongJump || bWantsToCrouch) && CanPerformLongJump(bWantsToLongJump)) // #LongJump
		{
			LongJump();
			return true;
		}
		else
		{
			bLongJump = false;
			return Super::DoJump(bReplayingMoves);
		}
	}
	else if (!IsOnWallJumpCoolDown() && IsFalling() && EXCharacterOwner->CanPerformWallJump(Normal))
	{
		{ // Calculating velocity
			const FVector2D ForwardVelocity(Velocity);
			const FVector2D WallDirection(Normal);
			FVector2D NewForwardVelocity = ForwardVelocity + WallDirection * WallDirectionBoost;
			float WallProjectionVelocity = FVector2D::DotProduct(ForwardVelocity, WallDirection);
			if (WallProjectionVelocity < 0.f)
			{
				NewForwardVelocity -= WallProjectionVelocity * WallDirection;
			}
			if (NewForwardVelocity.SizeSquared() < FMath::Square(WallDirectionBoostMin))
			{
				const float Size = NewForwardVelocity.Size();
				if (!FMath::IsNearlyZero(Size))
				{
					const float Scale = WallDirectionBoostMin / Size;
					NewForwardVelocity *= Scale;
				}
			}

			const float NewUpwardVelocity = JumpZVelocity;
			Velocity = FVector(NewForwardVelocity, NewUpwardVelocity);
		}

		SetMovementMode(MOVE_Falling);
		WallJumpClientTimeStamp = GetClientTimeStamp();
		return true;
	}

	return false;
}

void UEXCharacterMovement::SetWantsToSprint(bool bSprinting)
{
	if (bWantsToSprint && !bSprinting && IsRunning())
	{
		StartLongJumpWindow();
	}
	EXCharacterOwner->GetInventoryComponent()->SetWantsToSprint(bSprinting);
	bWantsToSprint = bSprinting;
}

void UEXCharacterMovement::SetWantsToWalk(bool bWalking)
{
	bWantsToWalk = bWalking;
}

bool UEXCharacterMovement::IsOnJumpCooldown() const
{
	const float Cooldown = bLongJump ? LongJumpLandingCooldown : JumpLandingCooldown;
	const float CooldownOver = JumpLandClientTimeStamp + Cooldown;
	const float CurrentTimeStamp = GetClientTimeStamp();
	return IsClientTimeStampInWindow(CurrentTimeStamp, JumpLandClientTimeStamp, CooldownOver);
}

bool UEXCharacterMovement::IsAccelOnJumpCooldown() const
{
	const float Cooldown = (bLongJump ? LongJumpLandingAccelCooldown : JumpLandingAccelCooldown);
	const float CooldownOver = JumpLandClientTimeStamp + Cooldown;
	const float CurrentTimeStamp = GetClientTimeStamp();
	return IsClientTimeStampInWindow(CurrentTimeStamp, JumpLandClientTimeStamp, CooldownOver);
}

bool UEXCharacterMovement::IsOnWallJumpCoolDown() const
{
	const float CooldownOver = WallJumpClientTimeStamp + TimeBetweenWallJumps;
	const float CurrentTimeStamp = GetClientTimeStamp();
	return IsClientTimeStampInWindow(CurrentTimeStamp, WallJumpClientTimeStamp, CooldownOver);
}

bool UEXCharacterMovement::IsMovingForward() const
{
	return (FVector::DotProduct(CharacterOwner->GetVelocity().GetSafeNormal2D(), CharacterOwner->GetActorRotation().Vector()) > 0.25);
}

bool UEXCharacterMovement::CanPerformLongJump(bool bDedicatedButton) const
{
	// With the dedicated button we might not have started the window
	return (IsInLongJumpWindow() || (bDedicatedButton && IsRunning()))
		&& !IsOnJumpCooldown() && IsMovingForward();
}

float UEXCharacterMovement::GetMaxSprintSpeed(float WeaponModifier) const
{
	return MaxWalkSpeed * WeaponModifier * SprintModifier;
}

void UEXCharacterMovement::StartLongJumpWindow()
{
	LongJumpWindowStart = GetClientTimeStamp();
}

bool UEXCharacterMovement::IsInLongJumpWindow() const
{
	return IsClientTimeStampInWindow(GetClientTimeStamp(), LongJumpWindowDuration, LongJumpWindowStart + LongJumpWindowDuration);
}

bool UEXCharacterMovement::CrouchResize(float NewCrouchAmount, float DeltaCrouch, bool bClientSimulation)
{
	if (!HasValidData())
	{
		return false;
	}
	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float OldUnscaledHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = Capsule->GetUnscaledCapsuleRadius();

	const float DefaultFullCrouchDiff = DefaultCapsuleHalfHeight - GetCrouchedHalfHeight();

	const float HalfHeightAdjust = DefaultFullCrouchDiff * DeltaCrouch;
	// Height is not allowed to be smaller than radius.
	Capsule->SetCapsuleSize(OldUnscaledRadius, 
							FMath::Max(OldUnscaledRadius, 
									   DefaultCapsuleHalfHeight - NewCrouchAmount * DefaultFullCrouchDiff));
	if (!bClientSimulation)
	{
		if (bCrouchMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal 
			// plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -HalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, 
											EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		if (DeltaCrouch < 0) // Uncrouching
		{
			const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(EShrinkCapsuleExtent::SHRINK_None);
			const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
			const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, EXCharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
			if (bEncroached)
			{
				Capsule->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				if (bCrouchMaintainsBaseLocation)
				{
					UpdatedComponent->MoveComponent(FVector(0.f, 0.f, HalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr,
													EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				}
				return false;
			}
		}
	}

	bForceNextFloorCheck = true;
	bShrinkProxyCapsule = true;
	AdjustProxyCapsuleSize();

	if (bClientSimulation)
	{
		FVector& MeshRelativeLocation = CharacterOwner->GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultMeshRelativeZ + DefaultFullCrouchDiff * NewCrouchAmount;
		EXCharacterOwner->OnCrouchUpdate(MeshRelativeLocation.Z);
	}
	else if (EXCharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
	{
		FVector& CameraRelativeLocation = EXCharacterOwner->GetFPSCamera()->GetRelativeLocation_DirectMutable();
		CameraRelativeLocation.Z = DefaultCameraRelativeZ - DefaultFullCrouchDiff * NewCrouchAmount / 2;
		EXCharacterOwner->OnCrouchUpdate(CameraRelativeLocation.Z);
	}

	// Don't smooth this change in mesh position
	if (bClientSimulation && (EXCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData && (ClientData->MeshTranslationOffset.Z != 0.f))
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, HalfHeightAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
	return true;
}

bool UEXCharacterMovement::IsSprinting() const
{
	if (IsNetMode(NM_DedicatedServer))
	{
		return bWantsToSprint;
	}
	return bWantsToSprint
		// Is moving forward?
		&& EXCharacterOwner->IsMovingForward()
		// Is on the ground?
		&& (MovementMode == EMovementMode::MOVE_Walking)
		&& !bWantsToCrouch
		//&& ((EXCharacterOwner->HasAuthority() && !IsNetMode(NM_Standalone)) ? (CrouchAmount <= MAX_CROUCH_VARIANCE_ALLOWED) : !IsCrouching())
		// Is player performing an action that prevents sprinting?
		&& EXCharacterOwner->GetInventoryComponent()->CanSprint()
		&& !IsOnJumpCooldown();
}

bool UEXCharacterMovement::IsWalking() const
{
	if (IsNetMode(NM_DedicatedServer))
	{
		return bWantsToWalk;
	}
	return bWantsToWalk || EXCharacterOwner->IsADSing();
}

void UEXCharacterMovement::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if ((PreviousMovementMode == MOVE_Falling) && (MovementMode == MOVE_Walking))
	{
		if (EXCharacterOwner->GetLocalRole() > ROLE_SimulatedProxy)
		{
			JumpLandClientTimeStamp = GetClientTimeStamp();
		}
	}
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

float UEXCharacterMovement::GetClientTimeStamp() const
{
	if (!EXCharacterOwner)
	{
		return 0.f;
	}
	// #EXTODO client time resets, so we need to make a new function to compare them
	// or we could just add the time before reset to the return
	const ENetRole LocalRole = EXCharacterOwner->GetLocalRole();
	if (IsNetMode(NM_Standalone))
	{
		return GetWorld()->GetTimeSeconds();
	}
	if (LocalRole == ROLE_Authority)
	{
		return GetPredictionData_Server_Character()->CurrentClientTimeStamp;
	}
	if (LocalRole == ROLE_AutonomousProxy)
	{
		return GetPredictionData_Client_Character()->CurrentTimeStamp;
	}
	return 0.f;
}

bool UEXCharacterMovement::IsClientTimeStampInWindow(float CurrentTimeStamp, float WindowStart, float WindowEnd) const
{
	if ((WindowStart <= CurrentTimeStamp) && (CurrentTimeStamp < WindowEnd))
	{
		return true;
	}
	if (WindowEnd > MinTimeBetweenTimeStampResets)
	{
		return CurrentTimeStamp < WindowEnd - MinTimeBetweenTimeStampResets;
	}
	return false;
}

