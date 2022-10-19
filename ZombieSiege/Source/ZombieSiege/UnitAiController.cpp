// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAiController.h"
#include "UnitOrder.h"
#include "HoldPositionOrder.h"
#include "AttackUnitOrder.h"
#include "AttackOnMoveOrder.h"
#include "WanderingOrder.h"

void AUnitAiController::BeginPlay()
{
    Super::BeginPlay();
}

void AUnitAiController::OnPossess(APawn* pawn)
{
    FPossessedPawnChangedEventArgs args;
    args.controller = this;
    args.possessedUnitOld = Cast<AUnitBase>(GetPawn());

    Super::OnPossess(pawn);

    IssueHoldPositionOrder();

    args.possessedUnitNew = Cast<AUnitBase>(pawn);
    
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
