// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXScoreboardBase.generated.h"

class UEXScoreboardRow;
class UGridPanel;
class AEXTeam;
class UEXPlayerContextMenu;
class UPanelWidget;
class UTextBlock;
class AEXPlayerState;

/**
 * 
 */
UCLASS()
class EX_API UEXScoreboardBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void Show();

	void RowClicked(UEXScoreboardRow* Row, const FVector2D AbsolutePosition);

	bool IsOpen() const { return GetVisibility() != ESlateVisibility::Hidden; }

	//void SetContextMenu(UEXPlayerContextMenu* InContextMenu) { ContextMenu = InContextMenu; }

	virtual void Close(bool bExitUIMode = false);

	void BeginUIMode();
protected:

	virtual void PopulateRows(const TArray<UEXScoreboardRow*>& TeamRows, const TArray<AEXPlayerState*>& TeamMembers) const;

	UPROPERTY(Meta = (BindWidget))
	UEXPlayerContextMenu* ContextMenu = nullptr;


	UPROPERTY(Meta = (BindWidgetOptional))
	UTextBlock* Spectators = nullptr;

	bool bUIMode = false;

	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
