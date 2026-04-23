// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXDeathScreen.generated.h"

class UEXMercSelectIcon;
class UTextBlock;
class AEXCharacter;

/**
 * 
 */
UCLASS()
class EX_API UEXDeathScreen : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void MercSelectionChanged(int32 MercIdx);

	void SetCanRespawn(bool bCanRespawn);

	void SetIcons(const TArray<TSoftClassPtr<AEXCharacter>>& PlayerClasses);

	void SetOwnerInSpectate(bool bInSpec) { bSpec = bInSpec; }

	void Show();
	void Hide();

protected:

	bool bSpec = false;

	UEXMercSelectIcon* GetMercIcon(int32 Idx) const;

	int32 SelectedMerc = 0;

	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UEXMercSelectIcon* PrimaryMerc = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UEXMercSelectIcon* SecondaryMerc = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UEXMercSelectIcon* TertiaryMerc = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UTextBlock* RespawnMessage = nullptr;

	virtual void NativeConstruct() override;

};
