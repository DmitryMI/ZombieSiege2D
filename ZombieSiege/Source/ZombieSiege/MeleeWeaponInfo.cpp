// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeaponInfo.h"

void UMeleeWeaponInfo::AttackTarget(AUnitBase* attacker, AActor* target, const FAttackParameters& params)
{
	if (!CanAttackTarget(attacker, target))
	{
		return;
	}

	float damageAmount = FMath::RandRange(damageMinMax.X, damageMinMax.Y);

	FDamageInstance damageInstance(attacker, damageAmount, this);
	IAttackable* attackable = Cast<IAttackable>(target);
	check(attackable);
	attackable->ReceiveDamage(damageInstance);
}
