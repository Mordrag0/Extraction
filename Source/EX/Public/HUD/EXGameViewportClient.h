// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "EXGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:

	/** Allow the base World to be overridden, to support making the killcam the active world. */
	TWeakObjectPtr<UWorld> ActiveWorldOverride = nullptr;

	bool HasActiveWorldOverride() { return ActiveWorldOverride.IsValid(); }

};
