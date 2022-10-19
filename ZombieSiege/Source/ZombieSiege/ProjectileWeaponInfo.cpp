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

	FVector vectorToTarget = (targetPoint - projectile->GetActorLocation());

	float scatter = FMath::RandRange(-scatterAngleDeg, scatterAngleDeg);
	vectorToTarget = vectorToTarget.RotateAngleAxis(scatter, FVector::UpVector);
	vectorToTarget *= 1 + shootBehindTargetFactor;

	FVector targetPointMod = projectile->GetActorLocation() + vectorToTarget;

	projectile->MoveTowards(targetPointMod);
}

bool UProjectileWeaponInfo::CanAttackTarget(AUnitBase* attacker, AUnitBase* target)
{
	if (!CanThisWeaponEverAttackTarget(target))
	{
		return false;
	}

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
