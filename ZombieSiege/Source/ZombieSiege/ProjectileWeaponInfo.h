// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInfo.h"
#include "ProjectileBase.h"
#include "ProjectileWeaponInfo.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UProjectileWeaponInfo : public UWeaponInfo
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileBase> projectileClass;

	UPROPERTY(EditDefaultsOnly)
	float scatterAngleDeg = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float shootBehindTargetFactor = 0.0f;

	virtual AProjectileBase* SpawnProjectile(AUnitBase* instigator, const FAttackParameters& params = FAttackParameters());

public:
	UProjectileWeaponInfo();

	virtual void AttackTarget(AUnitBase* attacker, AUnitBase* target, const FAttackParameters& params = FAttackParameters()) override;
	virtual void AttackPoint(AUnitBase* attacker, const FVector& targetPoint, const FAttackParameters& params = FAttackParameters()) override;

	virtual bool CanAttackTarget(AUnitBase* attacker, AUnitBase* target) override;

	virtual bool CanAttackPoint(AUnitBase* attacker, const FVector& targetPoint) override;
};
