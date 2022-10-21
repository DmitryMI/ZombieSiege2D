// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "HoldPositionOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UHoldPositionOrder : public UUnitOrder
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector holdLocation;

public:

	UHoldPositionOrder();

	virtual void ExecuteOrder() override;

	UFUNCTION(BlueprintCallable)
	AUnitBase* GetTargetUnit();

	UFUNCTION(BlueprintCallable)
	void SetTargetUnit(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	void SetHoldLocation(const FVector& location);

	virtual void UnitPerceptionUpdated(AUnitBase* unit, FAIStimulus Stimulus) override;

	virtual void ControlledUnitAttacked(const FDamageReceivedEventArgs& damageEventArgs) override;
};
