// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "EXPrimaryObjProgress.generated.h"

class UEXProgress;
class UProgressBar;
class AEXInteract;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class EX_API UEXPrimaryObjProgress : public UEXHUDElement
{
	GENERATED_BODY()
	
public:
	void SetProgress(int32 ObjectiveIdx, int32 Repeat, float Percent);
	void SetRepeatsCompleted(int32 ObjectiveIdx, int32 Repeats);
	void SetSecondaryProgress(int32 Idx, float Percent);
	void SetPrimaryObjectiveInfo(int32 InCount, const TArray<bool>& CanBeDefused);
protected:
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* PrimaryProgressBar1 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* PrimaryProgressBar2 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* PrimaryProgressBar3 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* DefuseProgressBar1 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* DefuseProgressBar2 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* DefuseProgressBar3 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* Box1 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* Box2 = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* Box3 = nullptr;


	virtual void NativeConstruct() override;

	TArray<UProgressBar*> PrimaryBars;
	TArray<UProgressBar*> DefuseBars;
	TArray<UVerticalBox*> Boxes;

	int32 Count = 0;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnNumObjectivesChanged(int32 NewCount);
};
