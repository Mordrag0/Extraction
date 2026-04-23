// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXPlayerContextMenu.generated.h"

class AEXPlayerState;
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class EX_API UEXPlayerContextMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void Show(AEXPlayerState* InPS, const FVector2D AbsolutePosition);
	UFUNCTION()
	void Hide();

protected:

	UFUNCTION(BlueprintCallable, Category = "UI Player")
	void ViewProfile();
	UFUNCTION(BlueprintCallable, Category = "UI Player")
	void Report();
	UFUNCTION(BlueprintCallable, Category = "UI Player")
	void Kick();
	UFUNCTION(BlueprintCallable, Category = "UI Player")
	void Mute();

	UPROPERTY()
	AEXPlayerState* EXPS = nullptr;

};
