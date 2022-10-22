// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnitClassification.h"
#include "DamageInstance.h"
#include "Attackable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttackable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIESIEGE_API IAttackable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual bool IsAlive() const;

	virtual bool HasAnyClassification(EUnitClassification classifications) const;

	virtual float ReceiveDamage(const FDamageInstance& damageInstance);
};
