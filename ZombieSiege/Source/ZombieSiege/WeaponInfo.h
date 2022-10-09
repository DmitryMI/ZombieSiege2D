// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitClassification.h"
#include "UnitBase.h"
#include "ZombieSiegeUtils.h"
#include "WeaponNature.h"
#include "WeaponInfo.generated.h"

UCLASS(Blueprintable)
/// <summary>
/// Base class for all weapons
/// </summary>
class ZOMBIESIEGE_API UWeaponInfo : public UObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool canEverAttackPoint;

	UPROPERTY(EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = EUnitClassification))
	uint8 targetClassifications;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float backswingDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float relaxationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponNature weaponNature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D damageMinMax;

public:

	UFUNCTION(BlueprintCallable)
	EWeaponNature GetWeaponNature()
	{
		return weaponNature;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Helper method to instantiate a weapon from a class
	/// </summary>
	/// <param name="outer">The outer object that will hold the weapon's object. Should be the Weapon Manager.</param>
	/// <param name="weaponName">Name of the weapon instance</param>
	/// <param name="weaponClass">WeaponInfo subclass to be instantiated</param>
	/// <returns>WeaponInfo instance</returns>
	static UWeaponInfo* CreateWeapon(UObject* outer, FName weaponName, TSubclassOf<UWeaponInfo> weaponClass)
	{
		return NewObject<UWeaponInfo>(outer, weaponClass.Get(), weaponName);
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Duration in seconds of the animation preceding the hit or projectile launch. E.g. duration of a bowstring stretching
	/// </summary>
	/// <returns>Duration in seconds</returns>
	virtual float GetBackswingDuration() const
	{
		return backswingDuration;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Duration in seconds of the animation after the hit or projectile launch. E.g. time that a swordman needs to return his weapon after a swing
	/// </summary>
	/// <returns>Duration in seconds</returns>
	virtual float GetRelaxationDuration() const
	{
		return relaxationDuration;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Duration in seconds of a pause between two hits (excluding backswing and relaxation)
	/// </summary>
	/// <returns></returns>
	virtual float GetCooldownDuration() const
	{
		return cooldownDuration;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Approximate damage that this weapon can inflict to a single target. Only used in GUI and by AI decisions.
	/// </summary>
	/// <returns>Min and Max damage of this weapon</returns>
	virtual FVector2D GetDamageMinMax() const
	{
		return damageMinMax;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Approximate range of the weapon. Only used in GUI and by AI decisions.
	/// </summary>
	/// <returns>Range in Unreal units</returns>
	virtual float GetRange() const
	{
		return range;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Determines if this weapon can ever target a unit.
	/// </summary>
	/// <returns>True if weapon can target a unit</returns>
	virtual bool CanThisWeaponEverAttackTarget(AUnitBase* unit) const
	{
		check(unit);

		return unit->HasAnyClassification(static_cast<EUnitClassification>(targetClassifications));
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Determines if this weapon can ever target a point.
	/// </summary>
	/// <returns>True if weapon can target a point</returns>
	virtual bool CanThisWeaponEverAttackPoint() const
	{
		return canEverAttackPoint;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Determines if an exact unit using this weapon can immidiatly attack a target unit.
	/// This method must do all possible immediate checks and only return True if an attack is possible right away.
	/// </summary>
	/// <param name="attacker">Unit that is possibly using this weapon</param>
	/// <param name="target">Possible target unit of an attack</param>
	/// <returns>True if an attack is possible. Always false if CanThisWeaponEverAttackTarget() is false</returns>
	virtual bool CanAttackTarget(AUnitBase* attacker, AUnitBase* target)
	{
		if (!CanThisWeaponEverAttackTarget(target))
		{
			return false;
		}

		float distance = UZombieSiegeUtils::GetDistance2DBetweenSimpleCollisions(attacker, target);

		if (distance > range)
		{
			return false;
		}

		return true;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Determines if an exact unit using this weapon can immidiatly attack a target point.
	/// This method must do all possible immediate checks and only return True if an attack is possible right away.
	/// </summary>
	/// <param name="attacker">Unit that is possibly using this weapon</param>
	/// <param name="targetPoint">Possible target point of an attack</param>
	/// <returns>True if an attack is possible. Always false if CanThisWeaponEverAttackPoint() is false</returns>
	virtual bool CanAttackPoint(AUnitBase* attacker, const FVector& targetPoint)
	{
		return false;
	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Initiates an attack of a target unit. Here a projectile is spawned (if any), damage is dealt, etc.
	/// </summary>
	/// <param name="attacker">Weapon user</param>
	/// <param name="target">Attack target unit</param>
	virtual void AttackTarget(AUnitBase* attacker, AUnitBase* target)
	{

	}

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Initiates an attack to a target point. Here a projectile is spawned (if any), damage is dealt, etc. 
	/// </summary>
	/// <param name="attacker">Weapon user</param>
	/// <param name="targetPoint">Attack target unit</param>
	virtual void AttackPoint(AUnitBase* attacker, const FVector& targetPoint)
	{

	}
	
};
