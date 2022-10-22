// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackParameters.h"
#include "AttackDispatcher.generated.h"

class AUnitBase;
class UWeaponInfo;

USTRUCT(BlueprintType)
struct FAttackDispatcherStateChangedEventArgs
{
	GENERATED_BODY();
public:
	UAttackDispatcher* dispatcher;
	EAttackState stateOld;
	EAttackState stateNew;

	FAttackDispatcherStateChangedEventArgs(UAttackDispatcher* dispatcherArg, EAttackState stateOldArg, EAttackState stateNewArg);
	FAttackDispatcherStateChangedEventArgs();
};

UENUM(BlueprintType)
enum class EAttackMode : uint8
{
	Target,
	Point
};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	None,
	Backswing,
	Relaxation,
	Cooldown
};

UCLASS()
class ZOMBIESIEGE_API UAttackDispatcher : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	AUnitBase* owningUnit;

	UPROPERTY(VisibleAnywhere)
	EAttackMode attackMode;

	UPROPERTY(VisibleAnywhere)
	EAttackState attackState;

	UPROPERTY(VisibleAnywhere)
	AActor* targetActor;

	UPROPERTY(VisibleAnywhere)
	FVector targetPoint;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* weaponInfo;

	UPROPERTY(VisibleAnywhere)
	FAttackParameters attackParameters;

	FTimerHandle attackTimerHandle;
	FTimerDelegate attackTimerDelegate;

	UFUNCTION()
	void OnAttackTimerElapsed();

	void TickStateMachine();

	bool BeginAttack();

	bool CommitAttack();

	void BroadcastStateChangedEvent(EAttackState stateOld, EAttackState stateNew);

	void SetTimer(float timeout);

	void ClearTimer();

public:
	UAttackDispatcher();
	~UAttackDispatcher();

	EAttackMode GetAttackMode();
	EAttackState GetAttackState();
	UWeaponInfo* GetWeaponInfo();
	AUnitBase* GetOwningUnit();
	AActor* GetTargetActor();
	FVector GetTargetPoint();
	bool IsOnCooldown();

	bool CanBeginAttackTarget(AUnitBase* owner, UWeaponInfo* weapon, AActor* unit);

	bool CanBeginAttackPoint(AUnitBase* owner, UWeaponInfo* weapon, const FVector& point);

	bool BeginAttackTarget(AUnitBase* owner, UWeaponInfo* weapon, AActor* unit, const FAttackParameters& params = FAttackParameters());

	bool BeginAttackPoint(AUnitBase* owner, UWeaponInfo* weapon, const FVector& point, const FAttackParameters& params = FAttackParameters());

	void CancelAttack();

	DECLARE_EVENT_OneParam(UAttackDispatcher, FAttackDispatcherStateChangedEvent, const FAttackDispatcherStateChangedEventArgs&);
	FAttackDispatcherStateChangedEvent& OnAttackDispatcherStateChanged() { return onAttackDispatcherStateChangedEvent; }
private:
	FAttackDispatcherStateChangedEvent onAttackDispatcherStateChangedEvent;
};
