// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXAugmentIcon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAugmentIconClicked, UEXAugmentIcon*, Icon, EAugment, Augment);


/**
 * 
 */
UCLASS()
class EX_API UEXAugmentIcon : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Augment")
	void OnDataSet();

	void Init(EAugment InType, int32 InIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void SetHighlighted(bool bHighlight);

	void ChangeValue(EAugment InType);

	// Broadcasts when the icon is clicked, sends its index and augment type
	FOnAugmentIconClicked OnIconClicked;

	EAugment GetSelectedValue() const { return Type; }

protected:
	void OnValueChanged();

	UPROPERTY(Meta = (BindWidget))
	class UButton* AugmentButton = nullptr;

	UFUNCTION()
	void OnClicked();

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Augment")
	EAugment Type;
	int32 Index = -1; // #EXTODO

	/*UPROPERTY(Meta = (BindWidget)) // #EXTODO
	class UImage* AugmentIcon = nullptr;*/
};
