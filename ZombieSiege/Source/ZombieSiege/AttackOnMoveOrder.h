// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "AttackOnMoveOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UAttackOnMoveOrder : public UUnitOrder
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
	FVector targetLocation;

public:
	UAttackOnMoveOrder();

	virtual void Execute() override;

	UFUNCTION(BlueprintCallable)
	void SetTargetLocation(const FVector& location);
};
