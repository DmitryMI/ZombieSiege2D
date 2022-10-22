// Fill out your copyright notice in the Description page of Project Settings.


#include "Doodad.h"

ADoodad::ADoodad()
{
 	// Doodads never tick.
	PrimaryActorTick.bCanEverTick = false;

}

void ADoodad::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ADoodad::CanEverBeGathered()
{
	return bCanEverBeGathered;
}

float ADoodad::ReceiveDamage(const FDamageInstance& damage)
{
	if (!bIsAlive)
	{
		return 0.0f;
	}

	float currentHealth = health;
	float healthOld = currentHealth;

	currentHealth -= damage.amount;

	if (currentHealth < 0)
	{
		health = 0;
	}
	else
	{
		health = currentHealth;
	}

	if (FMath::IsNearlyZero(health))
	{
		FinishDying(damage);
	}

	return healthOld - health;
}

void ADoodad::FinishDying(const FDamageInstance& killingDamageInstance)
{
	bIsAlive = false;
	Destroy();
}

bool ADoodad::HasAnyClassification(EUnitClassification classifications) const
{
	return (classificationFlags & static_cast<int>(classifications));
}

float ADoodad::GetMaxHealth()
{
	return maxHealth;
}

bool ADoodad::IsAlive() const
{
	return bIsAlive;
}


