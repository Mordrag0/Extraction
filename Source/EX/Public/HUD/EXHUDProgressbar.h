// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "EXHUDProgressbar.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXHUDProgressbar : public UEXHUDElement
{
	GENERATED_BODY()
	
public:
	void SetPercent(float Ratio);

protected:
	UPROPERTY(BlueprintReadonly, Category="HUD", Meta=(BindWidget))
	class UProgressBar* Progress = nullptr;
};
