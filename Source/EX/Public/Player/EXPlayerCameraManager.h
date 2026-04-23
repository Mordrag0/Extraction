// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "EXPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AEXPlayerCameraManager();

	void SetDefaultFOV(float FOV);
	void SetTargetFOV(float FOV);
	void ResetFOV();


	virtual void UpdateCamera(float DeltaTime) override;

	void SetScopedSensitivityScale(float InScopeSensitivityScale) { ScopedSensitivityScale = InScopeSensitivityScale; }

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Player Camera Manager")
	float NormalFOV = 90.f;
	float TargetingFOV = 0.f;
	float TargetSensAdjustment = 1.f;

	const float HalfRad2Deg = 0.0174533 / 2;
	float NormalFOVTanInv = 0.f;

	virtual void BeginPlay() override;

	bool bUpdating = false;
	float ScopedSensitivityScale = 0.f;
};
