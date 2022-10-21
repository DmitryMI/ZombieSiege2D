// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAiController.h"
#include "UnitOrder.h"
#include "HoldPositionOrder.h"
#include "AttackUnitOrder.h"
#include "AttackOnMoveOrder.h"
#include "WanderingOrder.h"
#include "ZombieSiegeUtils.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h" 


void AUnitAiController::BeginPlay()
{
    Super::BeginPlay();

    if (PerceptionComponent)
    {
        UAISenseConfig_Sight* sightConfig = GetSightSenseConfig();
        check(sightConfig);
        sightConfig->PeripheralVisionAngleDegrees = 360;

        PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AUnitAiController::OnTargetPerceptionUpdatedHandler);
    }

    AUnitBase* unit = Cast<AUnitBase>(GetPawn());
    if (unit)
    {
        unitDiedEventDelegateHandle = unit->OnUnitDied().AddUObject(this, &AUnitAiController::UnitDiedEventHandler);
        unitDamageReceivedEventDelegateHandle = unit->OnDamageReceived().AddUObject(this, &AUnitAiController::UnitDamageReceivedEventHandler);
    }
}

void AUnitAiController::OnPossess(APawn* pawn)
{
    AUnitBase* oldUnit = Cast<AUnitBase>(GetPawn());
    AUnitBase* unit = Cast<AUnitBase>(pawn);
   
    Super::OnPossess(unit);

    if (oldUnit && oldUnit != unit)
    {
        oldUnit->OnUnitDied().Remove(unitDiedEventDelegateHandle);   
        oldUnit->OnDamageReceived().Remove(unitDamageReceivedEventDelegateHandle);
    }

    if (unit)
    {
        unitDiedEventDelegateHandle = unit->OnUnitDied().AddUObject(this, &AUnitAiController::UnitDiedEventHandler);
        unitDamageReceivedEventDelegateHandle = unit->OnDamageReceived().AddUObject(this, &AUnitAiController::UnitDamageReceivedEventHandler);
    }

    if (unit && unit->IsAlive())
    {
        IssueHoldPositionOrder();
    }

    if (PerceptionComponent)
    {
        float sightRadius = unit->GetVisionRadius();
        SetPerceptionSightRadius(sightRadius);
        UAIPerceptionSystem* perceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
        perceptionSystem->UpdateListener(*GetPerceptionComponent());
    }

    FPossessedPawnChangedEventArgs args;
    args.controller = this;
    args.possessedUnitOld = oldUnit;
    args.possessedUnitNew = unit;
    if (oldUnit != unit)
    {
        onPossessedPawnChangedEvent.Broadcast(args);
    }
}

void AUnitAiController::OnUnPossess()
{
    FPossessedPawnChangedEventArgs args;
    args.controller = this;
    args.possessedUnitOld = Cast<AUnitBase>(GetPawn());
    Super::OnUnPossess();
   
    args.possessedUnitNew = nullptr;

    onPossessedPawnChangedEvent.Broadcast(args);
}

UAISenseConfig_Sight* AUnitAiController::GetSightSenseConfig()
{
    FAISenseID senseId = UAISense::GetSenseID(PerceptionComponent->GetDominantSense());
    UAISenseConfig* senseConfig = PerceptionComponent->GetSenseConfig(senseId);
    UAISenseConfig_Sight* sightConfig = Cast<UAISenseConfig_Sight>(senseConfig);
    return sightConfig;
}

void AUnitAiController::SetPerceptionSightRadius(float radius)
{
    UAISenseConfig_Sight* sightConfig = GetSightSenseConfig();
    if (sightConfig)
    {
        sightConfig->SightRadius = radius;
        sightConfig->LoseSightRadius = radius;
    }
}

void AUnitAiController::UnitEnteredPassengerCarrierEventHandler(const FUnitEnteredPassengerCarrierEventArgs& args)
{
    if (args.enteringUnit == GetPawn())
    {
        CancelAllOrders();
    }
}

void AUnitAiController::ExecuteOrder(UUnitOrder* order)
{
    order->Execute();
    FOrderExecutionStartedEventArgs args;
    args.controller = this;
    args.order = order;
    onOrderExecutionStartedEvent.Broadcast(args);
}

void AUnitAiController::OnTargetPerceptionUpdatedHandler(AActor* Actor, FAIStimulus Stimulus)
{
    AUnitBase* unit = Cast<AUnitBase>(Actor);
    if (!unit)
    {
        return;
    }

    if (executingOrder)
    {
        executingOrder->UnitPerceptionUpdated(unit, Stimulus);
    }
}

FGenericTeamId AUnitAiController::GetGenericTeamId() const
{
    AUnitBase* controlledUnit = Cast<AUnitBase>(GetPawn());
    if (!controlledUnit)
    {
        return FGenericTeamId();
    }

    return controlledUnit->GetGenericTeamId();
}

bool AUnitAiController::GetPerceivedUnits(TArray<AUnitBase*>& outUnits) const
{
    if (!PerceptionComponent)
    {
        return false;
    }

    TArray<AActor*> arrTemp;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), arrTemp);

    for (AActor* actor : arrTemp)
    {
        AUnitBase* unit = Cast<AUnitBase>(actor);
        if (!unit)
        {
            continue;
        }

        outUnits.Add(unit);
    }
    return !outUnits.IsEmpty();
}

bool AUnitAiController::GetPerceivedAttackableEnemies(TArray<AUnitBase*>& outEnemies) const
{
    if (!PerceptionComponent)
    {
        return false;
    }

    AUnitBase* controlledUnit = Cast<AUnitBase>(GetPawn());
    if (!controlledUnit)
    {
        return false;
    }

    TArray<AActor*> arrTemp;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), arrTemp);

    for (AActor* actor : arrTemp)
    {
        AUnitBase* unit = Cast<AUnitBase>(actor);
        if (!unit)
        {
            continue;
        }

        if (!unit->IsAlive())
        {
            continue;
        }

        if (!controlledUnit->CanAttackTarget(unit, FAttackTestParameters(false, false, true, false)))
        {
            continue;
        }

        outEnemies.Add(unit);
    }

    return !outEnemies.IsEmpty();
}

void AUnitAiController::IssueOrder(UUnitOrder* order)
{
    orderQueue.Empty();
    executingOrder = order;
    ExecuteOrder(executingOrder);
}

void AUnitAiController::QueueOrder(UUnitOrder* order)
{
    if (executingOrder == nullptr || executingOrder->GetOrderType() == EUnitOrderType::HoldPosition)
    {
        IssueOrder(order);
    }
    else
    {
        orderQueue.Add(order);
    }
}

void AUnitAiController::IssueMoveOrder(const FVector& moveToLocation)
{
}

void AUnitAiController::IssueAttackUnitOrder(AUnitBase* attackTarget)
{
    UAttackUnitOrder* order = CreateOrder<UAttackUnitOrder>(attackUnitOrderClass);
    order->SetTargetUnit(attackTarget);
    IssueOrder(order);
}

void AUnitAiController::IssueAttackOnMoveOrder(const FVector& location)
{
    UAttackOnMoveOrder* order = CreateOrder<UAttackOnMoveOrder>(attackOnMoveOrderClass);
    order->SetTargetLocation(location);
    IssueOrder(order);
}

void AUnitAiController::IssueWanderingOrder(FVector aroundLocation, float radius, float standingDuration)
{
    UWanderingOrder* order = CreateOrder<UWanderingOrder>(wandererOrderClass);
    order->SetParameters(aroundLocation, radius, standingDuration);
    IssueOrder(order);
}

void AUnitAiController::UnitDamageReceivedEventHandler(const FDamageReceivedEventArgs& args)
{
    AUnitBase* attackedUnit = args.target;
    AUnitBase* attacker = args.source;

    if (attackedUnit != GetPawn())
    {
        return;
    }

    if (executingOrder)
    {
        executingOrder->ControlledUnitAttacked(args);
    }
}

void AUnitAiController::UnitDiedEventHandler(const FUnitDiedEventArgs& args)
{
    AUnitBase* diedUnit = args.deadUnit;
    if (diedUnit != GetPawn())
    {
        return;
    }

    if (executingOrder)
    {
        CancelAllOrders();
    }
}

void AUnitAiController::IssueHoldPositionOrder()
{
    UHoldPositionOrder* order = CreateOrder<UHoldPositionOrder>(holdPositionOrderClass);
    order->SetHoldLocation(GetPawn()->GetActorLocation());
    IssueOrder(order);
}

void AUnitAiController::IssueEnterPassengerCarrierOrder(AUnitBase* carrier)
{
    
}

void AUnitAiController::CancelAllOrders()
{
    if (executingOrder)
    {
        executingOrder->CancelOrder();
    }
    orderQueue.Empty();

    AUnitBase* unit = Cast<AUnitBase>(GetPawn());
    if (unit && unit->IsAlive())
    {
        IssueHoldPositionOrder();
    }
}

void AUnitAiController::OnOrderFinished(UUnitOrder* order)
{
    check(executingOrder == order);

    UUnitOrder* nextOrder = nullptr;
    
    if (orderQueue.IsEmpty())
    {
        AUnitBase* unit = Cast<AUnitBase>(GetPawn());
        if (unit && unit->IsAlive())
        {
            IssueHoldPositionOrder();
        }
    }
    else
    {
        nextOrder = orderQueue[0];
        orderQueue.Remove(0);
        check(nextOrder);
        executingOrder = nextOrder;

        ExecuteOrder(executingOrder);
    }
}
