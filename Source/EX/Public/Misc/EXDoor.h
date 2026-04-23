// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXDoor.generated.h"

class AEXGenerator;
class AEXPlayerController;

UCLASS()
class EX_API AEXDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEXDoor();

	//~ Begin AActor Interface
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Reset() override;
	//~ End AActor Interface

	UFUNCTION(BlueprintCallable, Category = "Door")
	void Open(AEXPlayerController* Player);
	UFUNCTION(BlueprintCallable, Category = "Door")
	void Close(AEXPlayerController* Player);

protected:

	UFUNCTION()
	void OnRep_Open();

	void Change();

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float OpenedX = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float ClosedX = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float Speed = 2.f;

	UPROPERTY(ReplicatedUsing = OnRep_Open)
	bool bOpen = false;


	float Target = 0.f;
	bool bSliding = false;
	float Current = 0.f;

	UPROPERTY(EditInstanceOnly, Category = "Door")
	AEXGenerator* Gen = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DoorMesh = nullptr;


};
