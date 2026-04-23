// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/EXInteract.h"
#include "EXCarryableObjective.generated.h"

class USkeletalMeshComponent;
class USceneComponent;
class AEXPlayerState;

UCLASS()
class EX_API AEXCarryableObjective : public AEXInteract // #Interactable
{
	GENERATED_BODY()
	
public:	
	AEXCarryableObjective();

	void OnPickedUp(AEXPlayerController* Player);

	void OnDropped(const FVector Location, const FRotator Rotation);

public:
	//~ Begin AEXInteract Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
	//~ End AEXInteract Interface
	virtual void Reset() override;
	void SetVisible(bool bVisible);

	virtual void StartProgress(AEXCharacter* Player) override;

	void OnDelivered();


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	bool IsInSpawn() const;

	UFUNCTION()
	void ReturnCarryable(AEXPlayerController* Player);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Carryable")
	FLinearColor TimerProgressbarColor = FLinearColor::Yellow;

	UPROPERTY(EditDefaultsOnly, Category = "Carryable")
	FLinearColor ReturnProgressbarColor = FLinearColor::Blue;

	FVector StartingLocation;
	FRotator StartingRotation;

	bool bIsInSpawn = true;

	UPROPERTY()
	AEXCharacter* ReturningPlayer = nullptr;

	UFUNCTION()
	void OnRep_Enabled();

	UPROPERTY(ReplicatedUsing = OnRep_Enabled)
	bool bEnabled = true;

	UPROPERTY(ReplicatedUsing = OnRep_Loc)
	FVector Loc;
	UPROPERTY(ReplicatedUsing = OnRep_Rot)
	FRotator Rot;

	UFUNCTION()
	void OnRep_Loc();
	UFUNCTION()
	void OnRep_Rot();

	void SetLocAndRot(const FVector& InLoc, const FRotator& InRot);
	void ResetCarryable();

private:
	bool bDelivered = false;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* TimerWidgetComp = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* TimerWidget = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* TimerComp = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ReturnWidgetComp = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* ReturnWidget = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* ReturnComp = nullptr;

};
