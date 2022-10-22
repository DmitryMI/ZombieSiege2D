// Fill out your copyright notice in the Description page of Project Settings.


#include "Attackable.h"

// Add default functionality here for any IAttackable functions that are not pure virtual.

bool IAttackable::IsAlive() const
{
	return false;
}

bool IAttackable::HasAnyClassification(EUnitClassification classifications) const
{
	return false;
}

float IAttackable::ReceiveDamage(const FDamageInstance& damageInstance)
{
	return 0.0f;
}
