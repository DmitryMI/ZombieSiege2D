// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "DamageInstance.h"
#include "Attackable.h"
#include "Doodad.generated.h"

UCLASS()
class ZOMBIESIEGE_API ADoodad : public AActor, public IAttackable
{
	GENERATED_BODY()
		
private:	
	
	UPROPERTY(EditAnywhere)
	float health;

	UPROPERTY(EditDefaultsOnly)
	float maxHealth;

	UPROPERTY(EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = EUnitClassification))
	uint8 classificationFlags = (int)EUnitClassification::NONE;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlive = true;
	
public:	
	ADoodad();

protected:
	UPROPERTY(EditDefaultsOnly)
	bool bCanEverBeGathered;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHarvestingHit();

protected:


public:	
	virtual bool CanEverBeGathered();

	UFUNCTION(BlueprintCallable)
	virtual float ReceiveDamage(const FDamageInstance& damage) override;

	virtual void FinishDying(const FDamageInstance& killingDamageInstance);

	virtual bool HasAnyClassification(EUnitClassification classifications) const override;

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const override;
};
