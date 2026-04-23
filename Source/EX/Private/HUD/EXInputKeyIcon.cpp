// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXInputKeyIcon.h"
#include "Misc/EXInputInfo.h"
#include "System/EXGameplayStatics.h"
#include "Components/TextBlock.h"

void UEXInputKeyIcon::NativeConstruct()
{
	Super::NativeConstruct();

	Update();
	UEXInputInfo* InputInfo = UEXGameplayStatics::GetInputInfoCDO();
	InputInfo->OnInputsUpdated.AddUObject(this, &UEXInputKeyIcon::Update);
}

void UEXInputKeyIcon::Update()
{
	UEXInputInfo* InputInfo = UEXGameplayStatics::GetInputInfoCDO();
	FString BindText = InputInfo->GetBindText(Bind);
	TextIcon->SetText(FText::FromString(BindText));
}
