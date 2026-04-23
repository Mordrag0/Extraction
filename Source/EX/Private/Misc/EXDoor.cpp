// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXDoor.h"
#include "Misc/EXGenerator.h"
#include "System/EXProgress.h"
#include "Player/EXPlayerController.h"
#include "Net/UnrealNetwork.h"

AEXDoor::AEXDoor()
{
	Root = CreateDefaultSubobject<USceneComponent>("RootScene");
	RootComponent = Root;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	DoorMesh->SetupAttachment(Root);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
}

void AEXDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		if (Gen)
		{
			Gen->GetRepairComp()->OnCompleted.AddUObject(this, &AEXDoor::Close);
			Gen->GetDestroyComp()->OnCompleted.AddUObject(this, &AEXDoor::Open);
		}
	}
	Reset();
}

void AEXDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSliding)
	{
		if (FMath::IsNearlyEqual(Current, Target, 0.01f))
		{
			bSliding = false;
			Current = Target;
			PrimaryActorTick.SetTickFunctionEnable(false);
		}
		else
		{
			Current = FMath::FInterpTo(Current,Target,DeltaTime,Speed);
		}
		FVector NewLoc = DoorMesh->GetRelativeLocation();
		NewLoc.X = Current;
		DoorMesh->SetRelativeLocation(NewLoc);
	}
}

void AEXDoor::Change()
{
	Target = bOpen ? OpenedX : ClosedX;
	bSliding = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
}

void AEXDoor::Reset()
{
	Super::Reset();
	if (!Gen)
	{
		return;
	}

	const bool bOpenAtStart = !Gen->IsRepairedByDefault();
	if (bOpenAtStart)
	{
		Open(nullptr);
	}
	else
	{
		Close(nullptr);
	}
}

void AEXDoor::Open(AEXPlayerController* Player)
{
	bOpen = true;
	Change();
}

void AEXDoor::Close(AEXPlayerController* Player)
{
	bOpen = false;
	Change();
}

void AEXDoor::OnRep_Open()
{
	Change();
}

void AEXDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXDoor, bOpen);
}

