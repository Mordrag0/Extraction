// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "EXBarricade.h"
#include "EXEVMovementComponent.generated.h"

class AEXCharacter;
class AEXPlayerController;

/**
 * 
 */
UCLASS()
class EX_API UEXEVMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	UEXEVMovementComponent();

	void Init();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Reset();

	FORCEINLINE bool IsMoving() const { return bMoving; }

protected:

	void StartMoving(AEXCharacter* Player);

	void StopMoving();

	void UpdatePosition(float Distance, AEXCharacter* Player = nullptr);

	UFUNCTION(NetMulticast, Reliable)// #EXTODORELIABLE
	void Multicast_Reliable_SetMoving(bool bInMoving, bool bInBlocked, float CurrentDistance, float TimeStamp);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reliable_Reset();// #EXTODORELIABLE

	// Returns the player that is moving the EV, but only if the EV can be moved
	UFUNCTION(BlueprintCallable, Category = "EV")
	AEXCharacter* GetMovingPlayer() const;
	AEXCharacter* CurrentMovingPlayer = nullptr;
	void ChangeMovingPlayer(AEXCharacter* Player);

	UPROPERTY()
	bool bMoving = false;

	UPROPERTY()
	class USplineComponent* Path = nullptr;
	UPROPERTY()
	class AEXEV* EV = nullptr;
	UPROPERTY()
	class AEXBarricade* ClosestBarricade = nullptr;
	UPROPERTY()
	class AEXGameStateBase* GS = nullptr;

	void SetClosestBarricade(AEXBarricade* Barricade);

	bool IsAuthority() const; 
	UFUNCTION()
	void ClosestBarricadeDestroyed(AEXPlayerController* Player);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float Speed = 10.f;


	UPROPERTY(BlueprintReadOnly, Category = "EV")
	float PathDistance = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "EV")
	float TraveledDistance = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "EV")
	bool bBlocked = false;

private:
	FDelegateHandle DelegateHandle_BaricadeDestroyed;
};
