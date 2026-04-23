// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/EXShield.h"

AEXShield::AEXShield()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
}
