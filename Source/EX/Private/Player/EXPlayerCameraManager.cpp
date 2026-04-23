// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXPlayerCameraManager.h"
#include "Player/EXCharacter.h"
#include "Player/EXPersistentUser.h"
#include "Player/EXLocalPlayer.h"

AEXPlayerCameraManager::AEXPlayerCameraManager()
{
	bAlwaysApplyModifiers = true;
}

void AEXPlayerCameraManager::SetDefaultFOV(float FOV)
{
	NormalFOV = FOV;
	DefaultFOV = FOV;
	NormalFOVTanInv = 1 / FMath::Tan(NormalFOV * HalfRad2Deg);
}

void AEXPlayerCameraManager::SetTargetFOV(float NewFOV)
{
	TargetingFOV = NewFOV;
	TargetSensAdjustment = FMath::Tan(NewFOV / 2) / FMath::Tan(NormalFOV / 2);
	bUpdating = true;
}

void AEXPlayerCameraManager::ResetFOV()
{
	TargetingFOV = NormalFOV;
	bUpdating = true;
}

void AEXPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	if (bUpdating)
	{
		if (PCOwner && PCOwner->IsLocalController())
		{
			if (FMath::IsNearlyEqual(DefaultFOV, TargetingFOV))
			{
				bUpdating = false;
				DefaultFOV = TargetingFOV;
			}
			else
			{
				DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetingFOV, DeltaTime, 20.0f);
			}
			SetFOV(DefaultFOV);
			UE_LOG(LogEXController, Log, TEXT("FOV %f"), DefaultFOV);
			// Here we adjust the sens with the FOV change
			// The equation to keep approximately the same sens on the new FOV:
			// tan(NewFov / 2) / tan(OldFov / 2)
			// And then lerp based on ScopedSensitivityScale [0-1]
			// 0 means we use what the equation gave us, 1 means no adjustment for FOV change and everything else is something in between
			float SensAdjustment = FMath::Lerp(FMath::Tan(DefaultFOV * HalfRad2Deg) * NormalFOVTanInv, 1.f, ScopedSensitivityScale);
			UE_LOG(LogEXController, VeryVerbose, TEXT("SensAdjustment %f"), SensAdjustment);
			AEXCharacter* MyPawn = PCOwner ? PCOwner->GetPawn<AEXCharacter>() : nullptr; 
			if(MyPawn)
			{
				MyPawn->SetFOVSensAdjustment(SensAdjustment);
			}
		}
	}

	Super::UpdateCamera(DeltaTime);
}

void AEXPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	TargetingFOV = NormalFOV;
	SetFOV(NormalFOV);
}

