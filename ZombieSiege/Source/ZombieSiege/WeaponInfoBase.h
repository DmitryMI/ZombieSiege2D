// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInfo.h"
#include "WeaponInfoBase.generated.h"


UCLASS(Blueprintable)
class ZOMBIESIEGE_API UWeaponInfoBase : public UObject, public IWeaponInfo
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float backswingDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float relaxationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D damageUniformDistribution;

public:

	UFUNCTION(BlueprintCallable)
	static UWeaponInfoBase* CreateWeapon(UObject* outer, FName weaponName, TSubclassOf<UWeaponInfoBase> weaponClass)
	{
		return NewObject<UWeaponInfoBase>(outer, weaponClass.Get(), weaponName);
	}

	UFUNCTION(BlueprintCallable)
	virtual float GetBackswingDuration() override
	{
		return backswingDuration;
	}

	UFUNCTION(BlueprintCallable)
	virtual float GetRelaxationDuration() override
	{
		return relaxationDuration;
	}

	UFUNCTION(BlueprintCallable)
	virtual float GetCooldownDuration() override
	{
		return cooldownDuration;
	}

	UFUNCTION(BlueprintCallable)
	virtual float GetRandomDamage() override
	{
		return FMath::RandRange(damageUniformDistribution.X, damageUniformDistribution.Y);
	}

	UFUNCTION(BlueprintCallable)
	virtual float GetRange() override
	{
		return range;
	}

	
};
