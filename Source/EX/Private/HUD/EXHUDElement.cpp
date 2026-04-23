// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHUDElement.h"
#include "Components/CanvasPanelSlot.h"

TArray<FText> UEXHUDElement::GetEditableProperties_Implementation() const
{
	return TArray<FText>();
}

void UEXHUDElement::SetProperty_Implementation(const FText& PropertyName, const FText& PropertyValue)
{

}

void UEXHUDElement::InitRatios(const FVector2D& HUDRes)
{
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	if(CanvasSlot)
	{
		FAnchors Anchors = CanvasSlot->GetAnchors();
		PositionRatio =
			FVector2D(Anchors.Minimum.X + CanvasSlot->GetPosition().X / HUDRes.X,
				Anchors.Minimum.Y + CanvasSlot->GetPosition().Y / HUDRes.Y);
		SizeRatio = FVector2D(CanvasSlot->GetSize().X / HUDRes.X, CanvasSlot->GetSize().Y / HUDRes.Y);
	}
}

void UEXHUDElement::LoadRatios(const FIntPoint& HUDRes)
{
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	FVector2D AbsolutePosition = FVector2D(PositionRatio.X * HUDRes.X, PositionRatio.Y * HUDRes.Y);
	FVector2D AbsoluteSize = FVector2D(SizeRatio.X * HUDRes.X, SizeRatio.Y * HUDRes.Y);
	CanvasSlot->SetPosition(AbsolutePosition);
	CanvasSlot->SetSize(AbsoluteSize);
}

void UEXHUDElement::SetRatios(const FVector2D& InPosition, const FVector2D& InSize)
{
	PositionRatio = InPosition;
	SizeRatio = InSize;
}

void UEXHUDElement::NativeConstruct()
{
	Super::NativeConstruct();
	
	InitRatios(FVector2D(1920, 1080));
}

bool UEXHUDElement::IsAt(const FVector2D& Coord)
{
	return (Coord.X > PositionRatio.X - SizeRatio.X / 2) && (Coord.X < PositionRatio.X + SizeRatio.X / 2)
		&& (Coord.Y > PositionRatio.Y - SizeRatio.X / 2) && (Coord.Y < PositionRatio.Y + SizeRatio.Y / 2);
}


