// Fill out your copyright notice in the Description page of Project Settings.

//
//#include "System/EditorHelper.h"
//#include <Kismet/GameplayStatics.h>
//#include <Editor/EditorEngine.h>
//#include <Engine/StaticMeshActor.h>
//#include <Editor.h>
//#include <Engine/PostProcessVolume.h>
//#include <Engine/Selection.h>
//#include <SCreateAssetFromObject.h>
//#include <Widgets/SWindow.h>
//#include <Templates/SharedPointer.h>
//#include <Delegates/DelegateSignatureImpl.inl>
//
//extern UNREALED_API UEditorEngine* GEditor;
//
//AEditorHelper::AEditorHelper()
//{
//}
//
//struct TConvertData
//{
//	const TArray<AActor*> ActorsToConvert;
//	UClass* ConvertToClass;
//	const TSet<FString> ComponentsToConsider;
//	bool bUseSpecialCases;
//
//	TConvertData(const TArray<AActor*>& InActorsToConvert, UClass* InConvertToClass, const TSet<FString>& InComponentsToConsider, bool bInUseSpecialCases)
//		: ActorsToConvert(InActorsToConvert)
//		, ConvertToClass(InConvertToClass)
//		, ComponentsToConsider(InComponentsToConsider)
//		, bUseSpecialCases(bInUseSpecialCases)
//	{
//
//	}
//};
//void OnBrushToStaticMeshNameCommitted(const FString& InSettingsPackageName, TConvertData InConvertData)
//{
//	GEditor->DoConvertActors(InConvertData.ActorsToConvert, InConvertData.ConvertToClass, InConvertData.ComponentsToConsider, InConvertData.bUseSpecialCases, InSettingsPackageName);
//}
//void AEditorHelper::Test()
//{
//	TArray<AActor*> Brushes;
//	UGameplayStatics::GetAllActorsOfClass(this, ABrush::StaticClass(), Brushes);
//	UE_LOG(LogTemp, Warning, TEXT("Hi %d"), Brushes.Num());
//	TSet<FString> ComponentsToConsider;
//
//	TArray<ABrush*> SubtractBrushes;
//	for (AActor* BrushActor : Brushes)
//	{
//		ABrush* Brush = Cast<ABrush>(BrushActor);
//		if (Brush->BrushType == EBrushType::Brush_Subtract)
//		{
//			SubtractBrushes.Add(Brush);
//		}
//	}
//	for (AActor* BrushActor : SubtractBrushes)
//	{
//		Brushes.Remove(BrushActor);
//	}
//	int32 Idx = 1;
//	for (AActor* BrushActor : Brushes)
//	{
//		ABrush* Brush = Cast<ABrush>(BrushActor);
//		if (Brush->BrushType == EBrushType::Brush_Add)
//		{
//			TArray<AActor*> BrushesToConvert;
//			BrushesToConvert.Add(Brush);
//
//			TArray<FStringFormatArg> StringArgs;
//			StringArgs.Add(FStringFormatArg(FString::FromInt(Idx++)));
//			FString ResolutionText = FString::Format(TEXT("/Game/BrushMesh_{0}"), StringArgs);
//
//			GEditor->DoConvertActors(BrushesToConvert, AStaticMeshActor::StaticClass(), ComponentsToConsider, false, ResolutionText);
//
//		}
//	}
//}
