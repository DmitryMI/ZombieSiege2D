// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAiController.h"
#include "UnitOrder.h"
#include "EnterPassengerCarrierOrder.h"
#include "AttackUnitOrder.h"
#include "WanderingOrder.h"
#include "AreaScanningAttackOrder.h"
#include "ZombieSiegeUtils.h"
#include "BrainComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h" 
#include "Doodad.h"
#include "MoveOrder.h"


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
    UE_LOG(LogTemp, Display, TEXT("Unit %s (%s) executes order %s"), *GetPawn()->GetName(), *GetName(), *order->GetName());

    if (BrainComponent)
    {
        BrainComponent->ResumeLogic("ExecureOrder");
    }
    order->ExecuteOrder();
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

bool AUnitAiController::GetPerceivedAttackableEnemies(TArray<AUnitBase*>& outEnemies, FAttackTestParameters attackTestParams) const
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

        if (!controlledUnit->CanAttackTarget(unit, attackTestParams))
        {
            continue;
        }

        outEnemies.Add(unit);
    }

    return !outEnemies.IsEmpty();
}

AUnitAiController::AUnitAiController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}

void AUnitAiController::PostSetManualModeEnabled(bool bEnabled)
{

}

void AUnitAiController::SetManualModeEnabled(bool bEnabled)
{
    if (bIsManualModeEnabled == bEnabled)
    {
        return;
    }

    bIsManualModeEnabled = bEnabled;

    PostSetManualModeEnabled(bEnabled);
}

bool AUnitAiController::IsManualModeEnabled()
{
    return bIsManualModeEnabled;
}

bool AUnitAiController::HandleTargetActorCommandAction(AActor* targetActor)
{
    if (bIsManualModeEnabled)
    {
        SetManualModeEnabled(false);
    }

    AUnitBase* targetUnit = Cast<AUnitBase>(targetActor);
    if (targetUnit)
    {
        ETeamAttitude::Type attitude = FGenericTeamId::GetAttitude(this, targetActor);
        switch (attitude)
        {
        case ETeamAttitude::Friendly:
        case ETeamAttitude::Neutral:
            IssueMoveOrder(targetActor->GetActorLocation());
            return true;
            break;
        case ETeamAttitude::Hostile:
            IssueAttackUnitOrder(targetUnit);
            return true;
            break;
        }
    }    

    return false;
}

bool AUnitAiController::HandleTargetPointCommandAction(const FVector targetPoint)
{
    if (bIsManualModeEnabled)
    {
        SetManualModeEnabled(false);
    }

    IssueMoveOrder(targetPoint);
    return true;
}

void AUnitAiController::IssueOrder(UUnitOrder* order, bool bQueue)
{
    if (bQueue)
    {
        QueueOrder(order);
    }
    else
    {
        if (executingOrder)
        {
            executingOrder->CancelOrder();
        }

        orderQueue.Empty();
        executingOrder = order;

        ExecuteOrder(executingOrder);
    }
}

void AUnitAiController::QueueOrder(UUnitOrder* order)
{
    if (executingOrder == nullptr || executingOrder->GetOrderType() == EUnitOrderType::HoldPosition)
    {
        IssueOrder(order, false);
    }
    else
    {
        orderQueue.Add(order);
    }
}

void AUnitAiController::IssueMoveOrder(const FVector& moveToLocation, bool bQueue)
{
    if (!moveOrderClass)
    {
        return;
    }

    UMoveOrder* order = CreateOrder<UMoveOrder>(moveOrderClass);
    order->SetTargetLocation(moveToLocation);
    IssueOrder(order, bQueue);
}

void AUnitAiController::IssueAttackUnitOrder(AUnitBase* attackTarget, bool bQueue)
{
    if (!attackUnitOrderClass)
    {
        return;
    }

    UAttackUnitOrder* order = CreateOrder<UAttackUnitOrder>(attackUnitOrderClass);
    order->SetTargetUnit(attackTarget);
    IssueOrder(order, bQueue);
}

void AUnitAiController::IssueAttackOnMoveOrder(const FVector& location, bool bQueue)
{
    if (!attackOnMoveOrderClass)
    {
        return;
    }

    UAreaScanningAttackOrder* order = CreateOrder<UAreaScanningAttackOrder>(attackOnMoveOrderClass);
    order->SetTargetLocation(location);
    IssueOrder(order, bQueue);
}

void AUnitAiController::IssueWanderingOrder(FVector aroundLocation, float radius, float standingDuration)
{
    if (!wandererOrderClass)
    {
        return;
    }

    UWanderingOrder* order = CreateOrder<UWanderingOrder>(wandererOrderClass);
    order->SetParameters(aroundLocation, radius, standingDuration);
    IssueOrder(order, false);
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
    UAreaScanningAttackOrder* order = CreateOrder<UAreaScanningAttackOrder>(holdPositionOrderClass);
    order->SetTargetLocation(GetPawn()->GetActorLocation());
    IssueOrder(order, false);
}

void AUnitAiController::IssueEnterPassengerCarrierOrder(AUnitBase* carrier, bool bQueue)
{
    UEnterPassengerCarrierOrder* order = CreateOrder<UEnterPassengerCarrierOrder>(enterPassengerCarrierOrderClass);
    order->SetTargetUnit(carrier);
    IssueOrder(order, false);
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
    else
    {
        if (BrainComponent)
        {
            BrainComponent->PauseLogic("Unit is dead");
        }
    }
}

void AUnitAiController::ReportOrderFinished(UUnitOrder* order, EOrderResult result)
{
    check(executingOrder == order);

    UE_LOG(LogTemp, Display, TEXT("Unit %s (%s) finished order %s with result %d"), *GetPawn()->GetName(), *GetName(), *order->GetName(), (int)result);

    switch (result)
    {
    case EOrderResult::Abort:
        order->CancelOrder();
        break;
    case EOrderResult::Success:
        order->FinishOrder(true);
        break;
    case EOrderResult::Fail:
        order->FinishOrder(false);
        break;
    }

    UUnitOrder* nextOrder = nullptr;
    
    if (orderQueue.IsEmpty())
    {
        AUnitBase* unit = Cast<AUnitBase>(GetPawn());
        if (unit && unit->IsAlive())
        {
            IssueHoldPositionOrder();
        }
        else
        {
            if (BrainComponent)
            {
                BrainComponent->PauseLogic("Unit is dead");
            }
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
