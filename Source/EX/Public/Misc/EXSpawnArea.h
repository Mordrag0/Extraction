// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXTypes.h"
#include "EXSpawnArea.generated.h"

UCLASS()
class EX_API AEXSpawnArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AEXSpawnArea();

	UFUNCTION(BlueprintCallable, Category = "SpawnArea")
	APlayerStart* GetPlayerStart(int32 Idx);

protected:
	virtual void BeginPlay() override;

	TArray<class APlayerStart*> PlayerStarts;
	
	UPROPERTY(EditInstanceOnly, Category = "SpawnArea")
	ETeam FirstSpawnFor = ETeam::Invalid;
};
