// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXBarricade.h"
#include "Misc/EXGenerator.h"
#include "EX.h"
#include "System/EXProgress.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

AEXBarricade::AEXBarricade()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	bReplicates = true;
}

void AEXBarricade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXBarricade, bActive);
}

void AEXBarricade::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (!Generator)
		{
			UE_LOG(LogEXObjective, Warning, TEXT("Barricade not connected to a generator"));
			return;
		}

		Generator->GetRepairComp()->OnCompleted.AddUObject(this, &AEXBarricade::OnGenRepaired);
		Generator->GetDestroyComp()->OnCompleted.AddUObject(this, &AEXBarricade::OnGenDestroyed);

		bActive = Generator->IsRepairedByDefault();
	}
	OnActiveChanged();
}

void AEXBarricade::OnRep_Active()
{
	OnActiveChanged();
}

void AEXBarricade::OnGenRepaired(AEXPlayerController* Player)
{
	bActive = true;
	OnActiveChanged();
}

void AEXBarricade::OnGenDestroyed(AEXPlayerController* Player)
{
	bActive = false;
	OnActiveChanged();
}

void AEXBarricade::OnActiveChanged()
{
	if (bActive)
	{
		OnGoUp();
	}
	else
	{
		OnGoDown();
	}
}

void AEXBarricade::BarricadePassed()
{
	if (HasAuthority())
	{
		Generator->Passed();
	}
}

void AEXBarricade::Reset()
{
	Super::Reset();

	if(Generator)
	{
		bActive = Generator->IsRepairedByDefault();
		OnActiveChanged();
	}
}

