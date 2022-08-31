// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInfo.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInfo : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIESIEGE_API IWeaponInfo
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	
	virtual float GetBackswingDuration()
	{
		checkNoEntry();
		return 0;
	}

	virtual float GetRelaxationDuration()
	{
		checkNoEntry();
		return 0;
	}

	virtual float GetCooldownDuration()
	{
		checkNoEntry();
		return 0;
	}

	virtual float GetRandomDamage()
	{
		checkNoEntry();
		return 0;
	}

	virtual float GetRange()
	{
		checkNoEntry();
		return 0;
	}
};
