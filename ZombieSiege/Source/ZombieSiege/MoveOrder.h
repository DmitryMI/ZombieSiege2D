// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "MoveOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UMoveOrder : public UUnitOrder
{
	GENERATED_BODY()
	
private:
	FVector targetLocation;

public:

	virtual void ExecuteOrder() override;

	UFUNCTION(BlueprintCallable)
	FVector GetTargetLocation();

	UFUNCTION(BlueprintCallable)
	virtual void SetTargetLocation(const FVector& location);

	virtual FString ToString() override;
};
