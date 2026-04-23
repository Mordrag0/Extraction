// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXHUDElement.generated.h"

UCLASS()
class UEXHUDElement : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	TArray<FText> GetEditableProperties() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void SetProperty(const FText& PropertyName, const FText& PropertyValue);

	//UFUNCTION(BlueprintCallable, Category = "HUD")
	void InitRatios(const FVector2D& HUDRes);
	//UFUNCTION(BlueprintCallable, Category = "HUD")
	void LoadRatios(const FIntPoint& HUDRes);
	//UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetRatios(const FVector2D& InPosition, const FVector2D& InSize);
	//UFUNCTION(BlueprintCallable, Category = "HUD")
	FVector2D GetPosition() const { return PositionRatio; }
	//UFUNCTION(BlueprintCallable, Category = "HUD")
	FVector2D GetSize() const { return SizeRatio; }
protected:
	FVector2D PositionRatio;
	FVector2D SizeRatio;

	void NativeConstruct() override;

public:
	bool IsAt(const FVector2D& Coord);
};

