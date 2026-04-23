// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "EXGameEngine.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXGameEngine : public UGameEngine
{
	GENERATED_BODY()
	
public:
	virtual void Init(class IEngineLoop* InEngineLoop) override;


	virtual void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) override;



};
