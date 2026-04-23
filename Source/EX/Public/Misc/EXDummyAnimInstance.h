// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EXDummyAnimInstance.generated.h"



/**
 * 
 */
UCLASS()
class EX_API UEXDummyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Dummy")
	void OnHit();
	UFUNCTION(BlueprintNativeEvent, Category = "Dummy")
	void OnDead();

	void Reset();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dummy")
	bool bDead = false;
};
