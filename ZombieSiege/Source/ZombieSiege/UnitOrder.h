// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "UnitAiController.h"
#include "UnitOrderType.h"
#include "UnitOrder.generated.h"

UCLASS(Blueprintable)
class ZOMBIESIEGE_API UUnitOrder : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	EUnitOrderType orderType = EUnitOrderType::None;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* behaviorTree;

	UPROPERTY(VisibleAnywhere)
	AUnitAiController* unitController;

public:
	UUnitOrder();
	~UUnitOrder();

	UFUNCTION(BlueprintCallable)
	EUnitOrderType GetOrderType();

	UFUNCTION(BlueprintCallable)
	AUnitAiController* GetController();

	UFUNCTION(BlueprintCallable)
	void SetController(AUnitAiController* controller);

	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree();

	UFUNCTION(BlueprintCallable)
	void SetBehaviorTree(UBehaviorTree* behavior);

	UBlackboardComponent* GetBlackboard();

	UFUNCTION(BlueprintCallable)
	virtual void FinishOrder(bool success);

	UFUNCTION(BlueprintCallable)
	virtual void CancelOrder();

	UFUNCTION(BlueprintCallable)
	virtual void Execute();

	UFUNCTION(BlueprintCallable)
	virtual void TargetPerceptionStarted(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	virtual void TargetPerceptionEnded(AUnitBase* unit);
};
