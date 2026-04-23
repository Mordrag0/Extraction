// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHudEditWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "HUD/EXHUDElement.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UEXHudEditWidget::CopyPositionFrom(const UEXHUDElement* Element, const FVector2D& CanvasSize)
{
	FVector2D Position = FVector2D((Element->GetPosition().X - 0.5f) * CanvasSize.X, (Element->GetPosition().Y - 0.5f) * CanvasSize.Y);
	FVector2D Size = FVector2D(Element->GetSize().X * CanvasSize.X, Element->GetSize().Y * CanvasSize.Y);
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	CanvasSlot->SetPosition(Position);
	CanvasSlot->SetSize(Size);
	UE_LOG(LogTemp, Warning, TEXT("%f, %f - %f, %f"), CanvasSlot->GetPosition().X, CanvasSlot->GetPosition().Y, CanvasSlot->GetSize().X, CanvasSlot->GetSize().Y);
}

void UEXHudEditWidget::CopyPositionTo(UCanvasPanelSlot* OtherSlot)
{
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	OtherSlot->SetPosition(CanvasSlot->GetPosition());
	OtherSlot->SetSize(CanvasSlot->GetSize());
}

void UEXHudEditWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MoveBorder->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginMove);
	MoveBorder->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndMove);

	TL->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeTL);
	TC->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeTC);
	TR->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeTR);
	CL->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeCL);
	CR->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeCR);
	BL->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeBL);
	BC->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeBC);
	BR->OnMouseButtonDownEvent.BindDynamic(this, &UEXHudEditWidget::BeginResizeBR);
	TL->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeTL);
	TC->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeTC);
	TR->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeTR);
	CL->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeCL);
	CR->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeCR);
	BL->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeBL);
	BC->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeBC);
	BR->OnMouseButtonUpEvent.BindDynamic(this, &UEXHudEditWidget::EndResizeBR);
}

FEventReply UEXHudEditWidget::BeginResize(FGeometry MyGeometry, const FPointerEvent& MouseEvent, EResizeType Type)
{
	return UWidgetBlueprintLibrary::Handled();
}
FEventReply UEXHudEditWidget::EndResize(FGeometry MyGeometry, const FPointerEvent& MouseEvent, EResizeType Type)
{
	return UWidgetBlueprintLibrary::Handled();
}

FEventReply UEXHudEditWidget::BeginMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	return UWidgetBlueprintLibrary::Handled();
}
FEventReply UEXHudEditWidget::EndMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	return UWidgetBlueprintLibrary::Handled();
}

