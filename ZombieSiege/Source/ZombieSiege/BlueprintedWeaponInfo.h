// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeWeaponInfo.h"
#include "Attackable.h"
#include "BlueprintedWeaponInfo.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UBlueprintedWeaponInfo : public UMeleeWeaponInfo
{
	GENERATED_BODY()

protected:	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool canAttackPointReturn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool canAttackTargetReturn;

public:
	
	virtual float GetBackswingDuration() const override
	{
		return Super::GetBackswingDuration();
	}

	virtual float GetRelaxationDuration() const override
	{
		return Super::GetRelaxationDuration();
	}

	virtual float GetCooldownDuration() const override
	{
		return Super::GetCooldownDuration();
	}

	virtual FVector2D GetDamageMinMax() const override
	{
		return Super::GetDamageMinMax();
	}

	virtual float GetRange() const override
	{
		return Super::GetRange();
	}

	virtual bool CanThisWeaponEverAttackTarget(AActor* unit) const override
	{
		return Super::CanThisWeaponEverAttackTarget(unit);
	}

	virtual bool CanThisWeaponEverAttackPoint() const override
	{
		return Super::CanThisWeaponEverAttackPoint();
	}

	virtual bool CanAttackTarget(AUnitBase* attacker, AActor* target) override
	{
		BlueprintedCanAttackTarget(attacker, target);
		return canAttackTargetReturn;
	}

	virtual bool CanAttackPoint(AUnitBase* attacker, const FVector& targetPoint) override
	{
		BlueprintedCanAttackPoint(attacker, targetPoint);
		return canAttackPointReturn;
	}

	virtual void AttackTarget(AUnitBase* attacker, AActor* target, const FAttackParameters& params = FAttackParameters()) override
	{
		BlueprintedAttackTarget(attacker, target);
	}

	virtual void AttackPoint(AUnitBase* attacker, const FVector& targetPoint, const FAttackParameters& params = FAttackParameters()) override
	{
		BlueprintedAttackPoint(attacker, targetPoint);
	}

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedAttackPoint(AUnitBase* attacker, const FVector& targetPoint);

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedAttackTarget(AUnitBase* attacker, AActor* target);

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedCanAttackTarget(AUnitBase* attacker, AActor* target);

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintedCanAttackPoint(AUnitBase* attacker, const FVector& targetPoint);
};
