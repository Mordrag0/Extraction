// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXHUDElement.h"
#include "EXOptionsHUD.generated.h"

class FEXSettings;
class UComboBoxString;
class UEXHUDWidget;
class UCanvasPanel;
class UBorder;
class UEditableText;
class UTextBlock;
class UScaleBox;
class UEXHudEditWidget;



UENUM(BlueprintType)
enum class EHUDItem : uint8
{
	SpawnTimer,
	RoundTimer,
	Ammo,
	Abilities,
	Interact,
	ExpNotifications,
	KillFeedNotifications,
	PushToTalk,
	QuickChat,
	GameStatus,
	Chat,
	Announcements,
	PrimaryObjectiveProgress,
	Health,
	Spotted,
	InteractProgress,
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EHUDItem, EHUDItem::Count);

USTRUCT(BlueprintType)
struct FHUDEditableItem
{
	GENERATED_BODY()

	FHUDEditableItem() {};
	FHUDEditableItem(EHUDItem InType, const FString& InText, UEXHUDElement* InWidget)
	{
		Type = InType;
		Text = InText;
		Widget = InWidget;
	}

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	EHUDItem Type = EHUDItem::Count;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	FString Text;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UEXHUDElement* Widget = nullptr;
};

UENUM(BlueprintType)
enum class EAnchorType : uint8
{
	TopLeft,
	TopCenter,
	TopRight,
	Left,
	Center,
	Right,
	BottomLeft,
	BottomCenter,
	BottomRight,
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EAnchorType, EAnchorType::Count);

USTRUCT(BlueprintType)
struct FEXAnchor
{
	GENERATED_BODY()

	FEXAnchor() {};
	FEXAnchor(EAnchorType InType, const FString& InText)
	{
		Type = InType;
		Text = InText;
	}

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	FString Text;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	EAnchorType Type = EAnchorType::Count;
};

UENUM(BlueprintType)
enum class EAspect : uint8
{
	E4_3,
	E5_4,
	E16_9,
	E21_9,
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EAspect, EAspect::Count);

USTRUCT(BlueprintType)
struct FEXAspect
{
	GENERATED_BODY()

	FEXAspect() {};
	FEXAspect(EAspect InType, const FString& InText, FIntPoint InPoint)
	{
		Type = InType;
		Text = InText;
		Point = InPoint;
	}

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	FString Text;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	FIntPoint Point = FIntPoint(0, 0);
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	EAspect Type = EAspect::Count;
};

/**
 * 
 */
UCLASS()
class EX_API UEXOptionsHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(TSharedPtr<FEXSettings> InSettings, bool bReset);

protected:
	UFUNCTION()
	void Resize();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	FVector2D TransformCoord(FVector2D Coord);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	UEXHUDElement* GetHUDElement(FVector2D Coord);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SelectElement(UEXHUDElement* Element);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void MoveSelectedElement(FVector2D Delta);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ResizeSelectedElement(FVector2D Delta);

	UFUNCTION()
	void AnchorChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void AspectChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void ElementSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void PositionXChanged(const FText& Text);
	UFUNCTION()
	void PositionYChanged(const FText& Text);

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UComboBoxString* AspectOptions = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXHUDWidget* HUD = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UBorder* HUDBorder = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UBorder* HUDBackground = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UScaleBox* HUDScaleBox = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEditableText* PositionX = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEditableText* PositionY = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UComboBoxString* AnchorOptions = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UComboBoxString* ElementSelect = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UTextBlock* SelectedElement = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXHudEditWidget* EditWidget = nullptr;

	TSharedPtr<FEXSettings> Settings;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TMap<EHUDItem, FHUDEditableItem> HUDItems;
	UPROPERTY()
	TMap<EAspect, FEXAspect> Aspects;
	UPROPERTY()
	TMap<EAnchorType, FEXAnchor> Anchors;

	EHUDItem CurrentSelection;

	template <typename E, class T>
	E GetItemType(TMap<E, T> Container, const FString& Str)
	{
		for (E Val : TEnumRange<E>())
		{
			if (Container[Val].Text.Equals(Str))
			{
				return Val;
			}
		}
		check(0);
		return E::Count;
	}

	void NativeConstruct() override;

private:
	FVector2D NewSize;
};
