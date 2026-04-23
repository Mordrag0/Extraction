// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXHudEditWidget.generated.h"

class UBorder;
class UCanvasPanelSlot;
class UEXHUDElement;

UENUM(BlueprintType)
enum class EResizeType : uint8
{
	TL, TC, TR, CL, CR, BL, BC, BR
};

/**
 * 
 */
UCLASS()
class EX_API UEXHudEditWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void CopyPositionFrom(const UEXHUDElement* Element, const FVector2D& CanvasSize);
	void CopyPositionTo(UCanvasPanelSlot* OtherSlot);
protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UBorder* MoveBorder = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* TL = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* TC = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* TR = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* CL = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* CR = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* BL = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* BC = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta=(BindWidget))
	UBorder* BR = nullptr;

	void NativeConstruct() override;

	UFUNCTION() FEventReply BeginResizeTL(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::TL); }
	UFUNCTION() FEventReply BeginResizeTC(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::TC); }
	UFUNCTION() FEventReply BeginResizeTR(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::TR); }
	UFUNCTION() FEventReply BeginResizeCL(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::CL); }
	UFUNCTION() FEventReply BeginResizeCR(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::CR); }
	UFUNCTION() FEventReply BeginResizeBL(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::BL); }
	UFUNCTION() FEventReply BeginResizeBC(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::BC); }
	UFUNCTION() FEventReply BeginResizeBR(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return BeginResize(MyGeometry, MouseEvent, EResizeType::BR); }
	FEventReply BeginResize(FGeometry MyGeometry, const FPointerEvent& MouseEvent, EResizeType Type);

	UFUNCTION() FEventReply EndResizeTL(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::TL); }
	UFUNCTION() FEventReply EndResizeTC(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::TC); }
	UFUNCTION() FEventReply EndResizeTR(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::TR); }
	UFUNCTION() FEventReply EndResizeCL(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::CL); }
	UFUNCTION() FEventReply EndResizeCR(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::CR); }
	UFUNCTION() FEventReply EndResizeBL(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::BL); }
	UFUNCTION() FEventReply EndResizeBC(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::BC); }
	UFUNCTION() FEventReply EndResizeBR(FGeometry MyGeometry, const FPointerEvent& MouseEvent) { return EndResize(MyGeometry, MouseEvent, EResizeType::BR); }
	FEventReply EndResize(FGeometry MyGeometry, const FPointerEvent& MouseEvent, EResizeType Type);


	UFUNCTION()
	FEventReply BeginMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION()
	FEventReply EndMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
};
