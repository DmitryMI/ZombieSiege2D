// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintedWeaponInfo.h"

void UBlueprintedWeaponInfo::BlueprintedAttackPoint_Implementation(AUnitBase* attacker, const FVector& targetPoint)
{
	Super::AttackPoint(attacker, targetPoint);
}

void UBlueprintedWeaponInfo::BlueprintedAttackTarget_Implementation(AUnitBase* attacker, AActor* target)
{
	Super::AttackTarget(attacker, target);
}

void UBlueprintedWeaponInfo::BlueprintedCanAttackTarget_Implementation(AUnitBase* attacker, AActor* target)
{
	canAttackTargetReturn = Super::CanAttackTarget(attacker, target);
}

void UBlueprintedWeaponInfo::BlueprintedCanAttackPoint_Implementation(AUnitBase* attacker, const FVector& targetPoint)
{
	canAttackPointReturn = Super::CanAttackPoint(attacker, targetPoint);
}
