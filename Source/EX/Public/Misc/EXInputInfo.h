// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Player/EXPersistentUser.h"
#include "EXInputInfo.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnInputsUpdated);

USTRUCT(BlueprintType)
struct FInputBindValue
{
	GENERATED_BODY()

	FInputBindValue() {}
	FInputBindValue(const FName& InDisplayName) : DisplayName(InDisplayName) {}
	FInputBindValue(const FName& InDisplayName, const FKey& InKey) : DisplayName(InDisplayName), Key(InKey) {}

	// Name shown in options menu
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FName DisplayName = "";
	// Selected key
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FKey Key;
};

/**
 * 
 */
UCLASS(Blueprintable)
class EX_API UEXInputInfo : public UObject
{
	GENERATED_BODY()
	
public:

	UEXInputInfo();

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TMap<FMyInputId, FInputBindValue> Inputs;
	TMap<FMyInputId, FKey> AltInputs;

	UFUNCTION(BlueprintCallable, Category  = "Input")
	FString GetBindText(const FName& Bind, bool bInverted = false)const;

	FOnInputsUpdated OnInputsUpdated;
};
