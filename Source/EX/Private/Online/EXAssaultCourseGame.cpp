// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXAssaultCourseGame.h"
#include "EX.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/EXDummy.h"
#include "Inventory/EXInventoryComponent.h"
#include "Player/EXCharacter.h"
#include "Player/EXPlayerController.h"
#include "Online/EXGameInstance.h"
#include "EXTypes.h"
#include "HUD/EXHUDWidget.h"
#include "HUD/EXAssaultCourseScores.h"
#include "System/EXGameplayStatics.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2D.h"
#include "System/EXItemDataTable.h"

AEXAssaultCourseGame::AEXAssaultCourseGame()
{
}

void AEXAssaultCourseGame::CalculateScore(float RunTime)
{
	const int32 TotalScore = static_cast<int32>(FMath::RoundToPositiveInfinity(FMath::Max(DefaultScore - RunTime * PenaltyPerSecond + Score, 0.f)));
	bool bPersonalBest = TotalScore > PBScore;
	OnScoreChanged.Broadcast(TotalScore);
	if (bPersonalBest)
	{
		PBScore = TotalScore;
		SubmitScore(PBScore);
	}
	uint32 MedalId = 0;
	for (const TTuple<uint32, uint32>& Medal : MedalInfo)
	{
		if (PBScore > (int32)Medal.Value)
		{
			MedalId = Medal.Key;
		}
	}

	UTexture2D* MedalIcon = (MedalIcons.Contains(MedalId)) ? MedalIcons[MedalId] : nullptr;
	UE_LOG(LogTemp, Warning, TEXT("medal aquired = '%d'"), (int32)MedalId);
	AEXPlayerController* PC = GetWorld()->GetFirstPlayerController<AEXPlayerController>();
	if (PC->GetHUDWidget()) 
	{
		PC->GetHUDWidget()->GetAssaultCourseScores()->ShowResult(TotalScore, bPersonalBest, MedalIcon);
	}
}

void AEXAssaultCourseGame::ResetCourse()
{
	TArray<AActor*> Dummies;
	UGameplayStatics::GetAllActorsOfClass(this, AEXDummy::StaticClass(), Dummies);
	for (AActor* DummyActor : Dummies)
	{
		AEXDummy* Dummy = Cast<AEXDummy>(DummyActor);
		Dummy->Reset();
	}

	bActive = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RunTimer);
	AEXPlayerController* PC = GetWorld()->GetFirstPlayerController<AEXPlayerController>();
	AEXCharacter* Character = PC ? Cast<AEXCharacter>(PC->GetCharacter()) : nullptr;
	if (Character)
	{
		UEXInventoryComponent* InventoryComp = Character->GetInventoryComponent();
		for (UEXInventory* Inventory : InventoryComp->GetInventoryList())
		{
			if (Inventory->UsesAbility())
			{
				Inventory->AddUses(Inventory->GetMaxUses());
			}
		}
	}
	Score = 0;

	OnReset.Broadcast();
}

void AEXAssaultCourseGame::Start()
{
	Score = 0;
	OnScoreChanged.Broadcast(Score);
	bActive = true;
	StartTime = GetWorld()->GetRealTimeSeconds();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RunTimer, this, &AEXAssaultCourseGame::UpdateTimer, 0.1, true);
}

void AEXAssaultCourseGame::Finish()
{
	bActive = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RunTimer);
	const float RunTime = GetWorld()->GetRealTimeSeconds() - StartTime;
	CalculateScore(RunTime);
	ResetCourse();
}

void AEXAssaultCourseGame::AddScore(int32 InScore)
{
	Score += InScore;
	UE_LOG(LogEXAssaultCourse, Warning, TEXT("Scored %d"), Score);
	OnScoreChanged.Broadcast(Score);
}

float AEXAssaultCourseGame::GetTimeElapsed() const
{
	if (!bActive)
	{
		return 0.f;
	}
	const float RunTime = GetWorld()->GetRealTimeSeconds() - StartTime;
	return RunTime;
}

void AEXAssaultCourseGame::SubmitScore(int32 InScore)
{
	FAssaultCourseResult Result;
	Result.Score = InScore;
	Result.Map = GetWorld()->GetMapName();

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->SubmitAssaultCourseResult(Result);
}

void AEXAssaultCourseGame::ReceiveGlobalStats(const FAssaultCourseStats& InGlobalStats)
{
	GlobalStats = InGlobalStats;

	AEXPlayerController* PC = GetWorld()->GetFirstPlayerController<AEXPlayerController>();
	UEXHUDWidget* HUD = PC->GetHUDWidget();
	UEXAssaultCourseScores* ACScores = HUD->GetAssaultCourseScores();
	ACScores->LoadScore(InGlobalStats);
}

UClass* AEXAssaultCourseGame::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	return AssaultMercClass.Get();
}

void AEXAssaultCourseGame::StartToLeaveMap()
{
	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->MSDisconnect();
	Super::StartToLeaveMap();

	AEXPlayerController* PC = GetWorld()->GetFirstPlayerController<AEXPlayerController>();
	TArray<TSoftClassPtr<AEXCharacter>> ClassesToUnload;
	ClassesToUnload.Add(AssaultMercClass);
	PC->UnloadAssets(ClassesToUnload);
}

void AEXAssaultCourseGame::LoadMedals(const TMap<uint32, uint32>& InMedalInfo)
{
	MedalInfo = InMedalInfo;
	for (const TTuple<uint32, uint32>& Medal : MedalInfo)
	{
		FEXItemData* ItemData = ItemTable->FindRow<FEXItemData>(ItemTable->GetRowNames()[Medal.Key - 1], FString("AEXAssaultCourseGame::LoadMedals"));
		check(ItemData);
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		UTexture2D* ItemIcon = StreamableManager.LoadSynchronous(ItemData->Icon);
		check(ItemIcon);
		MedalIcons.Add(Medal.Key, ItemIcon);
		UE_LOG(LogTemp, Warning, TEXT("Medal loaded id = '%d'"), (int32)Medal.Key);
	}

}

void AEXAssaultCourseGame::UpdateTimer()
{
	OnTimeUpdated.Broadcast(FMath::FloorToInt(GetTimeElapsed() * 10));
}

void AEXAssaultCourseGame::BeginPlay()
{
	check(ItemTable);
	Super::BeginPlay();

	AEXPlayerController* PC = GetWorld()->GetFirstPlayerController<AEXPlayerController>();
	TArray<TSoftClassPtr<AEXCharacter>> ClassesToLoad;
	ClassesToLoad.Add(AssaultMercClass);
	PC->LoadAssets(ClassesToLoad);

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->MSConnect();
	GI->RequestACInfo(GetWorld()->GetMapName());
	GI->RequestAssaultCourseGlobalStats(GetWorld()->GetMapName());

	GI->HideTravelWidget();
}

