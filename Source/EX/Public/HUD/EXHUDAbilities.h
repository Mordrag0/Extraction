// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "EXHUDAbilities.generated.h"

class UEXInventoryComponent;
class UEXInventory;
class UEXAbilityIcon;

/**
 * 
 */
UCLASS()
class EX_API UEXHUDAbilities : public UEXHUDElement
{
	GENERATED_BODY()

public:
	void Init(UEXInventoryComponent* InventoryComponent);
	void UpdateInventory(const UEXInventory* Inventory);
protected:
	UPROPERTY()
	TMap<const UEXInventory*, UEXAbilityIcon*> AbilityIcons;
	UPROPERTY()
	TArray<UEXAbilityIcon*> IconArray;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXAbilityIcon* AbilityIcon1 = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXAbilityIcon* AbilityIcon2 = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXAbilityIcon* AbilityIcon3 = nullptr;

	void NativeConstruct() override;

};
