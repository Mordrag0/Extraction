// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EXCharacterMovement.generated.h"


/** FSavedMove_Character represents a saved move on the client that has been sent to the server and might need to be played back. */
class FSavedMove_EXMovement : public FSavedMove_Character
{
public:
	using Super = FSavedMove_Character;

	// Resets all saved variables.
	virtual void Clear() override;

	// Store input commands in the compressed flags.
	virtual uint8 GetCompressedFlags() const override;

	// This is used to check whether or not two moves can be combined into one.
	// Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	virtual void CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation) override;

	// Sets up the move before sending it to the server. 
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	// Sets variables on character movement component before making a predictive correction.
	virtual void PrepMoveFor(class ACharacter* Character) override;

	virtual void SetInitialPosition(ACharacter* Character) override;

	uint32 bSavedSprinting : 1;
	uint32 bSavedWalking : 1;
	uint32 bSavedLongJumping : 1;

	virtual void PostUpdate(ACharacter* Character, EPostUpdateMode PostUpdateMode) override;

	float CrouchAmount;

};

// Network data representation on the client
class FNetworkPredictionData_Client_EXMovement : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_EXMovement(const UCharacterMovementComponent& ClientMovement);

	using Super = FNetworkPredictionData_Client_Character;

	// Allocates a new copy of our custom saved move

	virtual FSavedMovePtr AllocateNewMove() override
	{
		return FSavedMovePtr(new FSavedMove_EXMovement());
	}
};


/**
 * 
 */
UCLASS()
class EX_API UEXCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
	friend class FSavedMove_EXMovement;

public:
	//Set input flags on character from saved inputs
	virtual void UpdateFromCompressedFlags(uint8 Flags) override; // Client only

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateCrouch(float DeltaTime);
	void Crouch(bool bClientSimulation = false) override;
	void UnCrouch(bool bClientSimulation = false) override;
	bool IsCrouching() const override;

	void StartLongJumping();
	void StopLongJumping();

	void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	bool CanCrouchInCurrentState() const override;

	bool IsRunning() const;

	float GetWeaponModifier() const;

protected:

	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;

	virtual bool ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;

	virtual bool ServerExceedsAllowablePositionError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;

	virtual void SimulateMovement(float DeltaTime) override;

	virtual void PerformMovement(float DeltaTime) override;

	virtual bool CanDelaySendingMove(const FSavedMovePtr& NewMove) override;


	void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration) override;

public:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	//~ Begin UMovementComponent Interface
	virtual float GetMaxSpeed() const override;
	//~ End UMovementComponent Interface

	//~ Begin UCharacterMovementComponent Interface
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual float GetMaxAcceleration() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	//~ End UCharacterMovementComponent Interface

	void LongJump();

	float GetWalkModifier() const { return WalkModifier; }
	void SetWalkModifier(float InModifier) { WalkModifier = InModifier; }

	void SetWantsToSprint(bool bSprinting);
	void SetWantsToWalk(bool bWalking);

	bool IsSprinting() const;
	bool IsWalking() const;

	bool CrouchResize(float CrouchAmount, float DeltaCrouch, bool bClientSimulation = false);
	float GetCrouchAmount() const { return CrouchAmount; }

	float GetClientTimeStamp() const;
	bool IsClientTimeStampInWindow(float CurrentTimeStamp, float WindowStart, float WindowEnd) const;
protected:
	UPROPERTY()
	class AEXCharacter* EXCharacterOwner = nullptr;

	// Was the last jump from the floor a long jump?
	bool bLongJump = false;
	float LongJumpWindowStart = -1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Jumping / Falling")
	float LongJumpWindowDuration = .4f;

	bool IsOnJumpCooldown() const;
	bool IsAccelOnJumpCooldown() const;
	bool IsOnWallJumpCoolDown() const;
	bool IsMovingForward() const;

	bool CanPerformLongJump(bool bDedicatedButton) const;

	float GetMaxSprintSpeed(float WeaponModifier) const;
	void StartLongJumpWindow();
	FORCEINLINE bool IsInLongJumpWindow() const;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	uint8 bWantsToSprint : 1;
	uint8 bWantsToWalk : 1;
	uint8 bWantsToLongJump : 1;

	float DefaultCapsuleHalfHeight = 0.f;
	float DefaultMeshRelativeZ = 0.f;
	float DefaultCameraRelativeZ = 0.f;

	float JumpLandClientTimeStamp = -10.f;

	float WallJumpClientTimeStamp = -10.f;

	float CrouchAmount = 0.f;

private:

	/** Time to crouch on ground in seconds */
	UPROPERTY(Category = "Character Movement: Walking", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float CrouchTime = .25f;

	/** Time to crouch in air in seconds */
	UPROPERTY(Category = "Character Movement: Walking", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float CrouchJumpTime = .25f;

	float CrouchTimeInv = 1.f;
	float CrouchJumpTimeInv = 1.f;


	UPROPERTY(Category = "Character Movement: Walking", EditDefaultsOnly, meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float WalkModifier = .5f;
	UPROPERTY(Category = "Character Movement: Walking", EditDefaultsOnly, meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float SprintModifier = 1.5f;
	UPROPERTY(Category = "Character Movement: Walking", EditDefaultsOnly, meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float SprintAccelerationMultiplier = 2.f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float JumpCooldownAccelerationMultiplier = .5f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float LongJumpCooldownAccelerationMultiplier = .4f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float JumpCooldownModifier = 1.f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float LongJumpCooldownModifier = 1.f;

	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float LongJumpForwardVelocity = 1000.f;
	// How much time you have after crouching at max speed to jump in order to do a long jump
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float LongJumpWindow = .5f;

	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float WallDirectionBoostMin = 100.f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float WallDirectionBoost = 200.f;

	// This is to prevent walljumping from the same wall twice in quick succession
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float TimeBetweenWallJumps = 0.1f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float JumpLandingCooldown = 0.3f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float LongJumpLandingCooldown = 0.5f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float JumpLandingAccelCooldown = 1.f;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float LongJumpLandingAccelCooldown = 1.f;
};
