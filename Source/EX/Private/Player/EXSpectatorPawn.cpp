// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXSpectatorPawn.h"

void AEXSpectatorPawn::Test()
{
	UE_LOG(LogTemp, Warning, TEXT("Test %s"), *GetController()->GetName());
	
}

void AEXSpectatorPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	InInputComponent->BindAxis("MoveForward", this, &ADefaultPawn::MoveForward);
	InInputComponent->BindAxis("MoveRight", this, &ADefaultPawn::MoveRight);
	InInputComponent->BindAxis("MoveUp", this, &ADefaultPawn::MoveUp_World);
	InInputComponent->BindAxis("Turn", this, &ADefaultPawn::AddControllerYawInput);
	InInputComponent->BindAxis("LookUp", this, &ADefaultPawn::AddControllerPitchInput);

}
