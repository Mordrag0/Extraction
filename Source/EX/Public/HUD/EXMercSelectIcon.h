// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXMercSelectIcon.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXMercSelectIcon : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "MercSelection")
	void Highlight();
	UFUNCTION(BlueprintImplementableEvent, Category = "MercSelection")
	void Unhighlight();

	void SetIcon(const TSoftObjectPtr<UTexture2D>& MercIcon);

protected:
	UPROPERTY(Meta = (BindWidget))
	class UImage* MercImage = nullptr;

};
