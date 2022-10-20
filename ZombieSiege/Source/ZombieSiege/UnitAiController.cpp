// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAiController.h"
#include "UnitOrder.h"
#include "HoldPositionOrder.h"
#include "AttackUnitOrder.h"
#include "AttackOnMoveOrder.h"
#include "WanderingOrder.h"
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
}

void AUnitAiController::OnPossess(APawn* pawn)
{
    FPossessedPawnChangedEventArgs args;
    args.controller = this;
    args.possessedUnitOld = Cast<AUnitBase>(GetPawn());

    Super::OnPossess(pawn);

    IssueHoldPositionOrder();
    args.possessedUnitNew = Cast<AUnitBase>(pawn);

    if (PerceptionComponent)
    {
        AUnitBase* unit = Cast<AUnitBase>(GetPawn());
        float sightRadius = unit->GetVisionRadius();

        SetPerceptionSightRadius(sightRadius);

        UAIPerceptionSystem* perceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
        perceptionSystem->UpdateListener(*GetPerceptionComponent());
    }

    if (args.possessedUnitOld != args.possessedUnitNew)
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

    AUnitBase* controlledUnit = Cast<AUnitBase>(GetPawn());

    if (executingOrder)
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            executingOrder->TargetPerceptionStarted(unit);
        }
        else
        {
            executingOrder->TargetPerceptionEnded(unit);
        }
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
    executingOrder->CancelOrder();
    orderQueue.Empty();

    IssueHoldPositionOrder();
}

void AUnitAiController::OnOrderFinished(UUnitOrder* order)
{
    check(executingOrder == order);

    UUnitOrder* nextOrder = nullptr;
    
    if (orderQueue.IsEmpty())
    {
        IssueHoldPositionOrder();
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
