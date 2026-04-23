// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "EXSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:
	UFUNCTION(exec)
	void Test();
private:
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

};
