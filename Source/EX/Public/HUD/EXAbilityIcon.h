// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXAbilityIcon.generated.h"

class UEXInventory;
class UTextBlock;
class UImage;
class UMaterialInstanceDynamic;

/**
 * 
 */
UCLASS()
class EX_API UEXAbilityIcon : public UUserWidget
{
	GENERATED_BODY()

public:

	void UpdateInventory();

	void InitIcon(const UEXInventory* InInventory, UTexture* AbilityTexture);

	void ShowIcon();
	void HideIcon();
protected:

	UPROPERTY(BlueprintReadOnly, Category = "AbilityIcon", Meta = (BindWidget))
	UImage* AbilityIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "AbilityIcon", Meta = (BindWidget))
	UTextBlock* CooldownNumber = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "AbilityIcon", Meta = (BindWidget))
	UImage* UsesIcon = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "AbilityIcon", Meta = (BindWidget))
	UTextBlock* UsesNumber = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* AbilityIconMaterial = nullptr;
	
	UPROPERTY()
	UMaterialInstanceDynamic* UsesIconMaterial = nullptr;

	UPROPERTY()
	const UEXInventory* Inventory = nullptr;
};
