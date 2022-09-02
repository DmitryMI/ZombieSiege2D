// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInfo.h"
#include "BlueprintedWeaponInfo.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UBlueprintedWeaponInfo : public UWeaponInfo
{
	GENERATED_BODY()

protected:	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool canAttackPointReturn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool canAttackTargetReturn;

public:
	
	virtual float GetBackswingDuration() override
	{
		return Super::GetBackswingDuration();
	}

	virtual float GetRelaxationDuration() override
	{
		return Super::GetRelaxationDuration();
	}

	virtual float GetCooldownDuration() override
	{
		return Super::GetCooldownDuration();
	}

	virtual FVector2D GetDamageMinMax() override
	{
		return Super::GetDamageMinMax();
	}

	virtual float GetRange() override
	{
		return Super::GetRange();
	}

	virtual bool CanThisWeaponEverAttackTarget() override
	{
		return Super::CanThisWeaponEverAttackTarget();
	}

	virtual bool CanThisWeaponEverAttackPoint() override
	{
		return Super::CanThisWeaponEverAttackPoint();
	}

	virtual bool CanAttackTarget(AUnitBase* attacker, AUnitBase* target) override
	{
		BlueprintedCanAttackTarget(attacker, target);
		return canAttackTargetReturn;
	}

	virtual bool CanAttackPoint(AUnitBase* attacker, const FVector& targetPoint) override
	{
		BlueprintedCanAttackPoint(attacker, targetPoint);
		return canAttackPointReturn;
	}

	virtual void AttackTarget(AUnitBase* attacker, AUnitBase* target) override
	{
		BlueprintedAttackTarget(attacker, target);
	}

	virtual void AttackPoint(AUnitBase* attacker, const FVector& targetPoint) override
	{
		BlueprintedAttackPoint(attacker, targetPoint);
	}

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedAttackPoint(AUnitBase* attacker, const FVector& targetPoint);

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedAttackTarget(AUnitBase* attacker, AUnitBase* target);

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedCanAttackTarget(AUnitBase* attacker, AUnitBase* target);

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedCanAttackPoint(AUnitBase* attacker, const FVector& targetPoint);
};
