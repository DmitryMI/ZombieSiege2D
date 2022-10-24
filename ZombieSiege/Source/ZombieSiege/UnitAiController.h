// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UnitBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionSystem.h"
#include "Navigation/CrowdManager.h"
#include "AttackTestParameters.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "UnitAiController.generated.h"

class UUnitOrder;
class UAISenseConfig_Sight;
class UAreaScanningAttackOrder;
class UAttackUnitOrder;
class UWanderingOrder;
class UEnterPassengerCarrierOrder;
class UMoveOrder;

UENUM(BlueprintType)
enum class EOrderResult : uint8
{
	Success,
	Fail,
	Abort
};

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

private:
	//UPROPERTY()
	FDelegateHandle unitDiedEventDelegateHandle;
	FDelegateHandle unitDamageReceivedEventDelegateHandle;

	UPROPERTY(EditAnywhere)
	bool bIsManualModeEnabled = false;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttackUnitOrder> attackUnitOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMoveOrder> moveOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAreaScanningAttackOrder> attackOnMoveOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAreaScanningAttackOrder> holdPositionOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWanderingOrder> wandererOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEnterPassengerCarrierOrder> enterPassengerCarrierOrderClass;

	UPROPERTY(EditAnywhere)
	TArray<UUnitOrder*> orderQueue;

	UPROPERTY(EditAnywhere)
	UUnitOrder* executingOrder;

	template<typename T>
	T* CreateOrder(TSubclassOf<T> clazz)
	{
		check(clazz);
		T* order = NewObject<T>(this, clazz);
		order->SetController(this);
		return order;
	}

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

	UAISenseConfig_Sight* GetSightSenseConfig();

	void SetPerceptionSightRadius(float radius);

	virtual void UnitDiedEventHandler(const FUnitDiedEventArgs& args);
	virtual void UnitDamageReceivedEventHandler(const FDamageReceivedEventArgs& args);

	virtual void UnitEnteredPassengerCarrierEventHandler(const FUnitEnteredPassengerCarrierEventArgs& args);

	void ExecuteOrder(UUnitOrder* order);

	UFUNCTION()
	virtual void OnTargetPerceptionUpdatedHandler(AActor* Actor, FAIStimulus Stimulus);

	virtual void PostSetManualModeEnabled(bool bIsEnabled);
	
public:

	AUnitAiController(const FObjectInitializer& ObjectInitializer);

	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	bool GetPerceivedUnits(TArray<AUnitBase*>& outUnits) const;

	UFUNCTION(BlueprintCallable)
	bool GetPerceivedAttackableEnemies(TArray<AUnitBase*>& outEnemies, FAttackTestParameters attackTestParams) const;

	UFUNCTION(BlueprintCallable)
	void IssueOrder(UUnitOrder* order, bool bQueue);

	UFUNCTION(BlueprintCallable)
	void QueueOrder(UUnitOrder* order);

	UFUNCTION(BlueprintCallable)
	void IssueMoveOrder(const FVector& moveToLocation, bool bQueue = false);

	UFUNCTION(BlueprintCallable)
	void IssueAttackUnitOrder(AUnitBase* attackTarget, bool bQueue = false);

	UFUNCTION(BlueprintCallable)
	void IssueAttackOnMoveOrder(const FVector& location, bool bQueue = false);

	UFUNCTION(BlueprintCallable)
	virtual void IssueWanderingOrder(FVector aroundLocation, float radius = 500.0f, float standingDuration = 2.0f);

	UFUNCTION(BlueprintCallable)
	virtual void IssueHoldPositionOrder();

	UFUNCTION(BlueprintCallable)
	virtual void IssueEnterPassengerCarrierOrder(AUnitBase* carrier, bool bQueue = false);

	UFUNCTION(BlueprintCallable)
	virtual void CancelAllOrders();

	UFUNCTION(BlueprintCallable)
	virtual void ReportOrderFinished(UUnitOrder* order, EOrderResult result);

	UFUNCTION(BlueprintCallable)
	bool IsManualModeEnabled();

	UFUNCTION(BlueprintCallable)
	void SetManualModeEnabled(bool bEnabled);

	virtual bool HandleTargetActorCommandAction(AActor* targetActor);

	virtual bool HandleTargetPointCommandAction(const FVector targetPoint);

	DECLARE_EVENT_OneParam(AUnitAiController, FOnOrderExecutionStartedEvent, const FOrderExecutionStartedEventArgs&);
	FOnOrderExecutionStartedEvent& OnOrderExecutionStarted() { return onOrderExecutionStartedEvent; }

	DECLARE_EVENT_OneParam(AUnitAiController, FOnPossessedPawnChangedEvent, const FPossessedPawnChangedEventArgs&);
	FOnPossessedPawnChangedEvent& OnPossessedPawnChanged() { return onPossessedPawnChangedEvent; }
private:
	FOnPossessedPawnChangedEvent onPossessedPawnChangedEvent;
	FOnOrderExecutionStartedEvent onOrderExecutionStartedEvent;
};


