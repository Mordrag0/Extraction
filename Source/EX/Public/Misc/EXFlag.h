// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/EXInteract.h"
#include "EXFlag.generated.h"

class AEXCharacter;
class AEXPlayerController;
class AEXSpawnArea;

/**
 * 
 */
UCLASS()
class EX_API AEXFlag : public AEXInteract // #Interactable
{
	GENERATED_BODY()

public:

	AEXFlag();

	virtual void Reset() override;

	void OnActivated() override;

	virtual void StartProgress(AEXCharacter* Player) override {};

	bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;

	AEXSpawnArea* GetAttackerSpawn() const { return AttackerSpawn; }
	AEXSpawnArea* GetDefenderSpawn() const { return DefenderSpawn; }

protected:
	void BeginPlay() override;

	void Captured(AEXPlayerController* Player);

	UFUNCTION()
	void PlayerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void PlayerExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	FColor CaptureColor = FColor::Yellow;

	TArray<AEXCharacter*> Attackers;
	TArray<AEXCharacter*> Defenders;

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	ETeam DefaultCapturingTeam = ETeam::Attack; 
	ETeam CapturingTeam; 

	UPROPERTY(EditInstanceOnly, Category = "Objective")
	class AEXSpawnArea* AttackerSpawn = nullptr;
	UPROPERTY(EditInstanceOnly, Category = "Objective")
	class AEXSpawnArea* DefenderSpawn = nullptr;

	float OldProgress = 0.f;

	void RecalculateProgress();
	void AwardExp();
	void AwardExpToTeam(TArray<AEXCharacter*>& Team, float DeltaProgress);

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	float TotalCapureExp = 500.f;

private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Flag", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootScene = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Flag", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Flag", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* CaptureWidget = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Flag", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* CaptureWidgetComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Flag", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* CaptureComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Flag", Meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CaptureArea = nullptr;
};
