// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXCrosshair.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXCrosshair : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	FVector2D GetWidgetCenterLocation(const UWidget* ParentWidget) const;
};
