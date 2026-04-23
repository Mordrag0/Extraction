// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXInputInfo.h"

UEXInputInfo::UEXInputInfo()
{
	Inputs.Add(FMyInputId(FName("MoveForward")), FInputBindValue(FName("Move Forward")));
	Inputs.Add(FMyInputId(FName("MoveForward"), true), FInputBindValue(FName("Move Back")));
	Inputs.Add(FMyInputId(FName("MoveRight")), FInputBindValue(FName("Move Right")));
	Inputs.Add(FMyInputId(FName("MoveRight"), true), FInputBindValue(FName("Move Left")));
	Inputs.Add(FMyInputId(FName("Interact")), FInputBindValue(FName("Interact")));
	Inputs.Add(FMyInputId(FName("PrimaryWeapon")), FInputBindValue(FName("Primary Weapon")));
	Inputs.Add(FMyInputId(FName("SecondaryWeapon")), FInputBindValue(FName("Secondary Weapon")));
	Inputs.Add(FMyInputId(FName("TertiaryWeapon")), FInputBindValue(FName("Melee Weapon")));
	Inputs.Add(FMyInputId(FName("QuaternaryWeapon")), FInputBindValue(FName("Primary Ability")));
	Inputs.Add(FMyInputId(FName("QuinaryWeapon")), FInputBindValue(FName("Secondary Ability")));
	Inputs.Add(FMyInputId(FName("PassiveAbility")), FInputBindValue(FName("Context Ability")));
	Inputs.Add(FMyInputId(FName("PrimaryAbility")), FInputBindValue(FName("Quick use primary")));
	Inputs.Add(FMyInputId(FName("SecondaryAbility")), FInputBindValue(FName("Quick use secondary")));
	Inputs.Add(FMyInputId(FName("Jump")), FInputBindValue(FName("Jump")));
	Inputs.Add(FMyInputId(FName("Fire")), FInputBindValue(FName("Fire")));
	Inputs.Add(FMyInputId(FName("FireAlt")), FInputBindValue(FName("Alternative fire")));
	Inputs.Add(FMyInputId(FName("Reload")), FInputBindValue(FName("Reload")));
	Inputs.Add(FMyInputId(FName("Sprint")), FInputBindValue(FName("Sprint")));
	Inputs.Add(FMyInputId(FName("Crouch")), FInputBindValue(FName("Crouch")));
	Inputs.Add(FMyInputId(FName("SelfKill")), FInputBindValue(FName("Self Kill")));
	Inputs.Add(FMyInputId(FName("Respawn")), FInputBindValue(FName("Respawn")));
	Inputs.Add(FMyInputId(FName("ShowScoreboard")), FInputBindValue(FName("Show Scoreboard")));
	Inputs.Add(FMyInputId(FName("LongJump")), FInputBindValue(FName("Long Jump")));
	Inputs.Add(FMyInputId(FName("PrimaryMerc")), FInputBindValue(FName("Primary Merc")));
	Inputs.Add(FMyInputId(FName("SecondaryMerc")), FInputBindValue(FName("Secondary Merc")));
	Inputs.Add(FMyInputId(FName("TertiaryMerc")), FInputBindValue(FName("Tertiary merc")));
	Inputs.Add(FMyInputId(FName("OpenChat")), FInputBindValue(FName("Chat")));
	Inputs.Add(FMyInputId(FName("OpenTeamChat")), FInputBindValue(FName("Team chat")));
	Inputs.Add(FMyInputId(FName("PushToTalk")), FInputBindValue(FName("Push to talk")));
	Inputs.Add(FMyInputId(FName("Cancel")), FInputBindValue(FName("Cancel")));
	Inputs.Add(FMyInputId(FName("QuickChat")), FInputBindValue(FName("Quick chat")));
	Inputs.Add(FMyInputId(FName("ChangeFireMode")), FInputBindValue(FName("Change fire mode")));
	Inputs.Add(FMyInputId(FName("VoteYes")), FInputBindValue(FName("Vote yes")));
	Inputs.Add(FMyInputId(FName("VoteNo")), FInputBindValue(FName("Vote no")));
	Inputs.Add(FMyInputId(FName("ShowObjectives")), FInputBindValue(FName("Show objectives")));
	Inputs.Add(FMyInputId(FName("OpenAdminPanel")), FInputBindValue(FName("Open admin panel")));

	AltInputs.Add(FMyInputId(FName("MoveForward")), FKey("w"));
	AltInputs.Add(FMyInputId(FName("MoveForward"), true), FKey("s"));
	AltInputs.Add(FMyInputId(FName("MoveRight")), FKey("d"));
	AltInputs.Add(FMyInputId(FName("MoveRight"), true), FKey("a"));
	AltInputs.Add(FMyInputId(FName("Interact")), FKey("f"));
	AltInputs.Add(FMyInputId(FName("PassiveAbility")), FKey("q"));
	AltInputs.Add(FMyInputId(FName("PrimaryAbility")), FKey("e"));
	AltInputs.Add(FMyInputId(FName("SecondaryAbility")), FKey("f"));
	AltInputs.Add(FMyInputId(FName("Reload")), FKey("r"));
	AltInputs.Add(FMyInputId(FName("SelfKill")), FKey("k"));
	AltInputs.Add(FMyInputId(FName("OpenChat")), FKey("z"));
	AltInputs.Add(FMyInputId(FName("OpenTeamChat")), FKey("t"));
	AltInputs.Add(FMyInputId(FName("PushToTalk")), FKey("b"));
	AltInputs.Add(FMyInputId(FName("QuickChat")), FKey("v"));
	AltInputs.Add(FMyInputId(FName("ChangeFireMode")), FKey("x"));
	AltInputs.Add(FMyInputId(FName("ShowObjectives")), FKey("c"));
}

FString UEXInputInfo::GetBindText(const FName& Bind, bool bInverted /*= false*/) const
{
	if (Inputs.Contains(FMyInputId(Bind, bInverted)))
	{
		return Inputs[FMyInputId(Bind, bInverted)].Key.ToString();
	}
	return FString();
}

