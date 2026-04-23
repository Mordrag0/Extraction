// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EXTypes.h"
#include "EXItemDataTable.generated.h"

USTRUCT(BlueprintType)
struct FEXItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 IdItem = 0;

	UPROPERTY(EditAnywhere, Category = "Item")
	FString Name = FString("Item Name");
	UPROPERTY(EditAnywhere, Category = "Item")
	FString Description = FString("Item Description");
	UPROPERTY(EditAnywhere, Category = "Item")
	EItemType Type = EItemType::Weapon;
	UPROPERTY(EditAnywhere, Category = "Item")
	TSoftObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditAnywhere, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;
};

///**
// * 
// */
//UCLASS()
//class EX_API UEXItemDataTable : public UDataTable
//{
//	GENERATED_BODY()
//	
//};
