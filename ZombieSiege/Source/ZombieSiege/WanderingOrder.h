// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "WanderingOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UWanderingOrder : public UUnitOrder
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FVector stickToPoint;
	
	UPROPERTY(EditAnywhere)
	float wanderingRadius;

	UPROPERTY(EditAnywhere)
	float standingDuration;

public:

	UWanderingOrder();

	virtual void Execute() override;

	UFUNCTION(BlueprintCallable)
	void SetParameters(const FVector& aroundPoint, float radius, float standForSeconds);
};
