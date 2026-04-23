// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EXInventoryData.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXInventoryData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconKillFeed = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconSmall = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconMedium = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> IconLarge = nullptr;
};
