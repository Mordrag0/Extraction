// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptionsHUD.h"
#include "System/EXSettings.h"
#include "Components/ComboBoxString.h"
#include "HUD/EXHUDWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/ScaleBox.h"
#include "Player/EXPlayerController.h"
#include "Online/EXMenuController.h"
#include "HUD/EXHudEditWidget.h"


#define LOCTEXT_NAMESPACE "HUD"


void UEXOptionsHUD::Init(TSharedPtr<FEXSettings> InSettings, bool bReset)
{
	Settings = InSettings;
	if (!bReset)
	{
		{
			HUDItems.Add(EHUDItem::SpawnTimer, FHUDEditableItem(EHUDItem::SpawnTimer, FString("Spawn timer"), (UEXHUDElement*)HUD->SpawnTimer));
			HUDItems.Add(EHUDItem::RoundTimer, FHUDEditableItem(EHUDItem::RoundTimer, FString("Round timer"), (UEXHUDElement*)HUD->RoundTimer));
			HUDItems.Add(EHUDItem::Ammo, FHUDEditableItem(EHUDItem::Ammo, FString("Ammo count"), (UEXHUDElement*)HUD->Ammo));
			HUDItems.Add(EHUDItem::Abilities, FHUDEditableItem(EHUDItem::Abilities, FString("Ability status"), (UEXHUDElement*)HUD->Abilities));
			HUDItems.Add(EHUDItem::Interact, FHUDEditableItem(EHUDItem::Interact, FString("Interact icon"), (UEXHUDElement*)HUD->InteractIcon));
			HUDItems.Add(EHUDItem::ExpNotifications, FHUDEditableItem(EHUDItem::ExpNotifications, FString("Exp notifications"), (UEXHUDElement*)HUD->ExpNotifications));
			HUDItems.Add(EHUDItem::KillFeedNotifications, FHUDEditableItem(EHUDItem::KillFeedNotifications, FString("Killfeed"), (UEXHUDElement*)HUD->KillFeedNotifications));
			HUDItems.Add(EHUDItem::PushToTalk, FHUDEditableItem(EHUDItem::PushToTalk, FString("Push to talk icon"), (UEXHUDElement*)HUD->PushToTalkIcon));
			HUDItems.Add(EHUDItem::QuickChat, FHUDEditableItem(EHUDItem::QuickChat, FString("Quick chat"), (UEXHUDElement*)HUD->QuickChatWindow));
			HUDItems.Add(EHUDItem::GameStatus, FHUDEditableItem(EHUDItem::GameStatus, FString("Game status"), (UEXHUDElement*)HUD->GameStatus));
			HUDItems.Add(EHUDItem::Chat, FHUDEditableItem(EHUDItem::Chat, FString("Chat"), (UEXHUDElement*)HUD->ChatWidget));
			HUDItems.Add(EHUDItem::Announcements, FHUDEditableItem(EHUDItem::Announcements, FString("Announcements"), (UEXHUDElement*)HUD->Announcements));
			HUDItems.Add(EHUDItem::PrimaryObjectiveProgress, FHUDEditableItem(EHUDItem::PrimaryObjectiveProgress, FString("Primary objective status"), (UEXHUDElement*)HUD->PrimaryObjProgress));
			HUDItems.Add(EHUDItem::Health, FHUDEditableItem(EHUDItem::Health, FString("Healthbar"), (UEXHUDElement*)HUD->HUDHealthBar));
			HUDItems.Add(EHUDItem::Spotted, FHUDEditableItem(EHUDItem::Spotted, FString("Spotted notification"), (UEXHUDElement*)HUD->SpottedNotification));
			HUDItems.Add(EHUDItem::InteractProgress, FHUDEditableItem(EHUDItem::InteractProgress, FString("Interaction progress"), (UEXHUDElement*)HUD->InteractProgressBar));

			for (EHUDItem Val : TEnumRange<EHUDItem>())
			{
				FHUDEditableItem Item = HUDItems[Val];
				ElementSelect->AddOption(Item.Text);
			}
		}
		{
			Anchors.Add(EAnchorType::TopLeft, FEXAnchor(EAnchorType::TopLeft, FString("Top left")));
			Anchors.Add(EAnchorType::TopCenter, FEXAnchor(EAnchorType::TopCenter, FString("Top center")));
			Anchors.Add(EAnchorType::TopRight, FEXAnchor(EAnchorType::TopRight, FString("Top right")));
			Anchors.Add(EAnchorType::Left, FEXAnchor(EAnchorType::Left, FString("Center left")));
			Anchors.Add(EAnchorType::Center, FEXAnchor(EAnchorType::Center, FString("Center")));
			Anchors.Add(EAnchorType::Right, FEXAnchor(EAnchorType::Right, FString("Center right")));
			Anchors.Add(EAnchorType::BottomLeft, FEXAnchor(EAnchorType::BottomLeft, FString("Bottom left")));
			Anchors.Add(EAnchorType::BottomCenter, FEXAnchor(EAnchorType::BottomCenter, FString("Bottom center")));
			Anchors.Add(EAnchorType::BottomRight, FEXAnchor(EAnchorType::BottomRight, FString("Bottom right")));

			for (EAnchorType Val : TEnumRange<EAnchorType>())
			{
				FEXAnchor Item = Anchors[Val];
				AnchorOptions->AddOption(Item.Text);
			}
		}

		{
			Aspects.Add(EAspect::E4_3, FEXAspect(EAspect::E4_3, FString("4:3"), FIntPoint(4, 3)));
			Aspects.Add(EAspect::E5_4, FEXAspect(EAspect::E5_4, FString("5:4"), FIntPoint(5, 4)));
			Aspects.Add(EAspect::E16_9, FEXAspect(EAspect::E16_9, FString("16:9"), FIntPoint(16, 9)));
			Aspects.Add(EAspect::E21_9, FEXAspect(EAspect::E21_9, FString("21:9"), FIntPoint(21, 9)));

			for (EAspect Val : TEnumRange<EAspect>())
			{
				FEXAspect Item = Aspects[Val];
				AspectOptions->AddOption(Item.Text);
			}
		}

		AnchorOptions->OnSelectionChanged.AddDynamic(this, &UEXOptionsHUD::AnchorChanged);
		AspectOptions->OnSelectionChanged.AddDynamic(this, &UEXOptionsHUD::AspectChanged);
		ElementSelect->OnSelectionChanged.AddDynamic(this, &UEXOptionsHUD::ElementSelectionChanged);
		AspectOptions->SetSelectedIndex(2);
		ElementSelect->SetSelectedIndex(0);

		PositionX->OnTextChanged.AddDynamic(this, &UEXOptionsHUD::PositionXChanged);
		PositionY->OnTextChanged.AddDynamic(this, &UEXOptionsHUD::PositionYChanged);

		HUD->SetCrosshairVisibility(false);
	}
	Resize();
}

void UEXOptionsHUD::AnchorChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Direct)
	{
		return;
	}
	FEXAnchor Item = Anchors[GetItemType(Anchors, SelectedItem)];
}

void UEXOptionsHUD::Resize()
{
	ForceLayoutPrepass();
	float GeomScaleY = GetCachedGeometry().GetAccumulatedRenderTransform().GetMatrix().GetScale().GetVector().Y;
	const FGeometry& BGGeometry = HUDBackground->GetCachedGeometry();
	const FVector2D HUDCanvasSize = BGGeometry.GetAbsoluteSize();
	if (HUDCanvasSize.SizeSquared() == 0.f)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEXOptionsHUD::Resize);
		return;
	}
	FEXAspect Item = Aspects[GetItemType<EAspect, FEXAspect>(Aspects, AspectOptions->GetSelectedOption())];
	const FIntPoint SelectedAspect = Item.Point;
	const float CanvasRatio = HUDCanvasSize.X / HUDCanvasSize.Y;
	const float HUDRatio = ((float)SelectedAspect.X) / SelectedAspect.Y;
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HUDBorder->Slot);
	int32 WindowX, WindowY;
	GetOwningPlayer()->GetViewportSize(WindowX, WindowY);

	if (HUDRatio > CanvasRatio)
	{
		NewSize = FVector2D(HUDCanvasSize.X, HUDCanvasSize.X / HUDRatio);
		CanvasSlot->SetSize(NewSize / GeomScaleY);
		CanvasSlot->SetPosition(FVector2D(0.f, (BGGeometry.GetAbsoluteSize().Y - NewSize.Y) / GeomScaleY / 2));
		HUDScaleBox->SetUserSpecifiedScale(NewSize.Y / WindowY);
	}
	else
	{
		NewSize = FVector2D(HUDCanvasSize.Y * HUDRatio, HUDCanvasSize.Y );
		CanvasSlot->SetSize(NewSize / GeomScaleY);
		CanvasSlot->SetPosition(FVector2D((BGGeometry.GetAbsoluteSize().X - NewSize.X) / GeomScaleY / 2, 0.f));
		HUDScaleBox->SetUserSpecifiedScale(NewSize.Y / WindowY);
	}
}

FVector2D UEXOptionsHUD::TransformCoord(FVector2D Coord)
{
	const FGeometry& CanvasGeometry = HUDBorder->GetCachedGeometry();
	return (Coord - CanvasGeometry.GetAbsolutePosition()) / CanvasGeometry.GetAbsoluteSize();
}

UEXHUDElement* UEXOptionsHUD::GetHUDElement(FVector2D Coord)
{
	for (const TTuple<EHUDItem, FHUDEditableItem>& KVP : HUDItems)
	{
		if (KVP.Value.Widget->IsAt(Coord))
		{
			return KVP.Value.Widget;
		}
	}
	return nullptr;
}

void UEXOptionsHUD::SelectElement(UEXHUDElement* Element)
{
	EditWidget->CopyPositionFrom(Element, NewSize);
}

void UEXOptionsHUD::MoveSelectedElement(FVector2D Delta)
{

}

void UEXOptionsHUD::ResizeSelectedElement(FVector2D Delta)
{

}

void UEXOptionsHUD::AspectChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	Resize();
}

void UEXOptionsHUD::ElementSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	FHUDEditableItem Item = HUDItems[GetItemType(HUDItems, SelectedItem)];
	CurrentSelection = Item.Type;

	SelectedElement->SetText(FText::FromString(Item.Text));
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Item.Widget->Slot);
	PositionX->SetText(FText::FromString(FString::FromInt(CanvasSlot->GetPosition().X)));
	PositionY->SetText(FText::FromString(FString::FromInt(CanvasSlot->GetPosition().Y)));
}

void UEXOptionsHUD::PositionXChanged(const FText& Text)
{
	float X = FCString::Atof(*Text.ToString());
	FHUDEditableItem Item = HUDItems[CurrentSelection];
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Item.Widget->Slot);
	float Y = CanvasSlot->GetPosition().Y;
	CanvasSlot->SetPosition(FVector2D(X, Y));
}

void UEXOptionsHUD::PositionYChanged(const FText& Text)
{
	float Y = FCString::Atof(*Text.ToString());
	FHUDEditableItem Item = HUDItems[CurrentSelection];
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Item.Widget->Slot);
	float X = CanvasSlot->GetPosition().X;
	CanvasSlot->SetPosition(FVector2D(X, Y));
}

void UEXOptionsHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

#undef LOCTEXT_NAMESPACE
