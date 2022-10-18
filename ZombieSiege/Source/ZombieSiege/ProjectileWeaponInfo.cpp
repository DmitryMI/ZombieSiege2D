// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeaponInfo.h"

AProjectileBase* UProjectileWeaponInfo::SpawnProjectile(AUnitBase* instigator, const FAttackParameters& params)
{
	UWorld* world = GetWorld();
	
	FVector location = instigator->GetActorLocation() + params.projectileSpawnRelativeLocation;

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = instigator;

	AProjectileBase* projectile = world->SpawnActor<AProjectileBase>(projectileClass, location, FRotator::ZeroRotator, spawnParameters);

	projectile->SetImpactDamageRange(damageMinMax);
	projectile->SetWeaponInfo(this);

	return projectile;
}

UProjectileWeaponInfo::UProjectileWeaponInfo()
{
	bCanEverAttackPoint = true;
}

void UProjectileWeaponInfo::AttackTarget(AUnitBase* attacker, AUnitBase* target, const FAttackParameters& params)
{
	AttackPoint(attacker, target->GetActorLocation(), params);
}

void UProjectileWeaponInfo::AttackPoint(AUnitBase* attacker, const FVector& targetPoint, const FAttackParameters& params)
{
	if (!CanAttackPoint(attacker, targetPoint))
	{
		return;
	}

	AProjectileBase* projectile = SpawnProjectile(attacker, params);
	projectile->MoveTowards(targetPoint);
}

bool UProjectileWeaponInfo::CanAttackTarget(AUnitBase* attacker, AUnitBase* target)
{
	return CanAttackPoint(attacker, target->GetActorLocation());
}

bool UProjectileWeaponInfo::CanAttackPoint(AUnitBase* attacker, const FVector& targetPoint)
{
	if (!CanThisWeaponEverAttackPoint())
	{
		return false;
	}

	float distance = (attacker->GetActorLocation() - targetPoint).Size2D();

	if (distance > range)
	{
		return false;
	}

	return true;
}
