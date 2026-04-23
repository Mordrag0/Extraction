// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "EXGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	/** Applies all current user settings to the game and saves to permanent storage (e.g. file), optionally checking for command line overrides. */
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	int32 GetGraphicsQuality() const
	{
		return (GraphicsQuality == 3) ? 0 : 1;
	}

	void SetGraphicsQuality(int32 InGraphicsQuality)
	{
		GraphicsQuality = (InGraphicsQuality == 0) ? 3 : 1;
	}

	bool IsDedicatedServer() const
	{
		return bIsDedicatedServer;
	}

	void SetDedicatedServer(bool InbIsDedicatedServer)
	{
		bIsDedicatedServer = InbIsDedicatedServer;
	}

	virtual void SetToDefaults() override;

private:
	/**
	 * Graphics Quality
	 *	0 = Low
	 *	1 = High
	 */
	UPROPERTY(config)
	int32 GraphicsQuality;

	/** Is dedicated server? */
	UPROPERTY(config)
	bool bIsDedicatedServer;
};
