// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintedWeaponInfo.h"

void UBlueprintedWeaponInfo::BlueprintedAttackPoint_Implementation(AUnitBase* attacker, const FVector& targetPoint)
{
	Super::AttackPoint(attacker, targetPoint);
}

void UBlueprintedWeaponInfo::BlueprintedAttackTarget_Implementation(AUnitBase* attacker, AUnitBase* target)
{
	Super::AttackTarget(attacker, target);
}

void UBlueprintedWeaponInfo::BlueprintedCanAttackTarget_Implementation(AUnitBase* attacker, AUnitBase* target)
{
	canAttackTargetReturn = false;
}

void UBlueprintedWeaponInfo::BlueprintedCanAttackPoint_Implementation(AUnitBase* attacker, const FVector& targetPoint)
{
	canAttackPointReturn = false;
}
