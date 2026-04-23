// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXCrosshair.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/PanelWidget.h"

int32 UEXCrosshair::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 Ret = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext Context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const UPanelWidget* Parent = GetParent();
	if (Parent)
	{
		FVector2D Center = GetWidgetCenterLocation(Parent);
		UWidgetBlueprintLibrary::DrawLine(Context, Center, Center * 2);
	}

	return Ret;
}

FVector2D UEXCrosshair::GetWidgetCenterLocation(const UWidget* ParentWidget) const
{
	FGeometry Geometry = ParentWidget->GetCachedGeometry();
	FVector2D Position = Geometry.AbsoluteToLocal(GetCachedGeometry().GetAbsolutePosition()) + GetCachedGeometry().GetLocalSize() / 2.0f;
	return Position;
}
