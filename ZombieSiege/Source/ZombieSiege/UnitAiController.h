// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UnitBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UnitAiController.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AUnitAiController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* attackUnitBehaviorTree;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* attackOnMoveBehaviorTree;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* holdPositionBehaviorTree;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* wandererBehaviorTree;
	
public:
	UFUNCTION(BlueprintCallable)
	void IssueMoveOrder(const FVector& moveToLocation);

	UFUNCTION(BlueprintCallable)
	void IssueAttackUnitOrder(AUnitBase* attackTarget);

	UFUNCTION(BlueprintCallable)
	void IssueAttackOnMoveOrder(const FVector& location);

	UFUNCTION(BlueprintCallable)
	virtual void IssueWanderingOrder(FVector aroundLocation, float radius = 500.0f, float standingDuration = 2.0f);

	UFUNCTION(BlueprintCallable)
	virtual void CancelOrder();
};
