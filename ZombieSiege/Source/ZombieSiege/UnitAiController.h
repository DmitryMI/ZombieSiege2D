// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UnitBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UnitAiController.generated.h"

class UUnitOrder;


USTRUCT(BlueprintType)
struct FPossessedPawnChangedEventArgs
{
	GENERATED_BODY()

	AUnitAiController* controller;
	AUnitBase* possessedUnitOld;
	AUnitBase* possessedUnitNew;
};

USTRUCT(BlueprintType)
struct FOrderExecutionStartedEventArgs
{
	GENERATED_BODY()

	AUnitAiController* controller;
	UUnitOrder* order;
};

UCLASS()
class ZOMBIESIEGE_API AUnitAiController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> attackUnitOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> attackOnMoveOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> holdPositionOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> wandererOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> enterPassengerCarrierOrderClass;

	UPROPERTY(EditAnywhere)
	TArray<UUnitOrder*> orderQueue;

	UPROPERTY(EditAnywhere)
	UUnitOrder* executingOrder;

	template<typename T>
	T* CreateOrder(TSubclassOf<UUnitOrder> clazz)
	{
		check(clazz);
		T* order = NewObject<T>(this, clazz);
		order->SetController(this);
		return order;
	}

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* pawn) override;

	virtual void OnUnPossess() override;

	virtual void UnitEnteredPassengerCarrierEventHandler(const FUnitEnteredPassengerCarrierEventArgs& args);

	void ExecuteOrder(UUnitOrder* order);
	
public:
	UFUNCTION(BlueprintCallable)
	void IssueOrder(UUnitOrder* order);

	UFUNCTION(BlueprintCallable)
	void QueueOrder(UUnitOrder* order);

	UFUNCTION(BlueprintCallable)
	void IssueMoveOrder(const FVector& moveToLocation);

	UFUNCTION(BlueprintCallable)
	void IssueAttackUnitOrder(AUnitBase* attackTarget);

	UFUNCTION(BlueprintCallable)
	void IssueAttackOnMoveOrder(const FVector& location);

	UFUNCTION(BlueprintCallable)
	virtual void IssueWanderingOrder(FVector aroundLocation, float radius = 500.0f, float standingDuration = 2.0f);

	UFUNCTION(BlueprintCallable)
	virtual void IssueHoldPositionOrder();

	UFUNCTION(BlueprintCallable)
	virtual void IssueEnterPassengerCarrierOrder(AUnitBase* carrier);

	UFUNCTION(BlueprintCallable)
	virtual void CancelAllOrders();

	UFUNCTION(BlueprintCallable)
	virtual void OnOrderFinished(UUnitOrder* order);

	DECLARE_EVENT_OneParam(AUnitAiController, FOnOrderExecutionStartedEvent, const FOrderExecutionStartedEventArgs&);
	FOnOrderExecutionStartedEvent& OnOrderExecutionStarted() { return onOrderExecutionStartedEvent; }

	DECLARE_EVENT_OneParam(AUnitAiController, FOnPossessedPawnChangedEvent, const FPossessedPawnChangedEventArgs&);
	FOnPossessedPawnChangedEvent& OnPossessedPawnChanged() { return onPossessedPawnChangedEvent; }
private:
	FOnPossessedPawnChangedEvent onPossessedPawnChangedEvent;
	FOnOrderExecutionStartedEvent onOrderExecutionStartedEvent;
};


