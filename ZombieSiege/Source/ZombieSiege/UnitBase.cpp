// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"

AUnitBase::AUnitBase()
{

}

void AUnitBase::KillUnit(bool bForceDeath)
{
	health = 0;

	FDamageInstance emptyDamage;

	if (!bForceDeath)
	{
		BeginDyingInternal(emptyDamage);
	}
	else
	{
		FinishDyingInternal(emptyDamage);
	}
}

void AUnitBase::ReviveUnit()
{
	if (!bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reviving alive unit!"));
		return;
	}

	bIsAlive = false;
}

void AUnitBase::ReviveUnitWithHealth(float setHealthTo)
{
	if (!bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reviving alive unit! Health will not be updated"));
		return;
	}

	if (setHealthTo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reviving unit requires a non-zero positive health value"));
		setHealthTo = 1.0f;
	}

	SetHealth(setHealthTo);
	ReviveUnit();
}

bool AUnitBase::IsDying()
{
	return bIsDying;
}

bool AUnitBase::IsAlive()
{
	return bIsAlive;
}

void AUnitBase::PreventDeath(float healthValue)
{
	if (bIsDying)
	{
		bIsDying = false;

		if (healthValue <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Death preventer must set a non-zero positive amount of health"));
			healthValue = 1.0f;
		}

		SetHealth(healthValue);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PreventDeath called for a not dying Unit. Always check IsDying() before calling PreventDeath()"));
	}
}

void AUnitBase::SetHealth(float healthValue)
{
	healthValue = FMath::Clamp(healthValue, 0, maxHealth);

	health = healthValue;

	if (FMath::IsNearlyZero(health))
	{
		FDamageInstance emptyDamage;
		BeginDyingInternal(emptyDamage);
	}
}

float AUnitBase::GetHealth()
{
	return health;
}

void AUnitBase::SetMaxHealth(float maxHealthValue)
{
	if (maxHealthValue < 0)
	{
		maxHealthValue = 0;
	}

	if (health > maxHealthValue)
	{
		health = maxHealthValue;
	}

	if (FMath::IsNearlyZero(health))
	{
		FDamageInstance emptyDamage;
		BeginDyingInternal(emptyDamage);
	}
}

float AUnitBase::GetMaxHealth()
{
	return maxHealth;
}

void AUnitBase::FinishDyingInternal(const FDamageInstance& killingDamageInstance)
{
	// Should be overwritten by child classes (e.g. to start death animation).
	bIsAlive = false;
	Destroy();
}


bool AUnitBase::Attack(AUnitBase* target)
{
	// Should always be overriden in child classes
	checkNoEntry();

	return false;
}

void AUnitBase::BeginDyingInternal(const FDamageInstance& killingDamageInstance)
{
	bIsDying = true;

	FUnitIsDyingEventArgs args(this, killingDamageInstance.source);
	
	onUnitIsDyingEvent.Broadcast(args);

	// Check if unit is still dying (no one has prevented that)
	if (bIsDying)
	{
		FinishDyingInternal(killingDamageInstance);
	}
	else
	{
		// Alive units should never have 0 health. Usually, the death preventer must directly set health to non-zero value
		if (FMath::IsNearlyZero(health))
		{
			UE_LOG(LogTemp, Warning, TEXT("Death of %s was prevented, but it's health is still zero. Preventer should always directly set health to non-zero."));
			health = 1.0f;
		}
	}
}

void AUnitBase::ReceiveDamage(const FDamageInstance& damage)
{
	// Simple implementation, should be overriden in child classes

	float currentHealth = health;

	currentHealth -= damage.amount;

	if (currentHealth < 0)
	{
		health = 0;
	}
	else
	{
		health = currentHealth;
	}

	FDamageReceivedEventArgs damageEventArgs(this, damage.source, damage.amount);
	onDamageReceivedEvent.Broadcast(damageEventArgs);

	if (FMath::IsNearlyZero(health))
	{
		BeginDyingInternal(damage);
	}
}


