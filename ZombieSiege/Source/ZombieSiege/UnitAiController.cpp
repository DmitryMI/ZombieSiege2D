// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAiController.h"

void AUnitAiController::BeginPlay()
{
    Super::BeginPlay();
}

void AUnitAiController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    IssueHoldPositionOrder();
}

void AUnitAiController::UnitEnteredPassengerCarrierEventHandler(const FUnitEnteredPassengerCarrierEventArgs& args)
{
    if (args.enteringUnit == GetPawn())
    {
        CancelOrder();
    }
}

void AUnitAiController::IssueMoveOrder(const FVector& moveToLocation)
{
}

void AUnitAiController::IssueAttackUnitOrder(AUnitBase* attackTarget)
{
    check(attackTarget);

    bool ok = RunBehaviorTree(attackUnitBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsObject("Target", attackTarget);
}

void AUnitAiController::IssueAttackOnMoveOrder(const FVector& location)
{
    bool ok = RunBehaviorTree(attackOnMoveBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsVector("TargetLocation", location);
}

void AUnitAiController::IssueWanderingOrder(FVector aroundLocation, float radius, float standingDuration)
{
    bool ok = RunBehaviorTree(wandererBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsVector("StickToPoint", aroundLocation);
    blackboard->SetValueAsFloat("WanderingRadius", radius);
    blackboard->SetValueAsFloat("StandingDuration", standingDuration);
}

void AUnitAiController::IssueHoldPositionOrder()
{
    bool ok = RunBehaviorTree(holdPositionBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    APawn* pawn = GetPawn();
    check(pawn);

    blackboard->SetValueAsVector("TargetLocation", pawn->GetActorLocation());
}

void AUnitAiController::IssueEnterPassengerCarrierOrder(AUnitBase* carrier)
{
    if (!carrier->HasClassifications(EUnitClassification::PassengerCarrier))
    {
        return;
    }

    if (carrier->GetFreePassengerSeats() == 0)
    {
        return;
    }

    bool ok = RunBehaviorTree(enterPassengerCarrierBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsObject("PassengerCarrier", carrier);
}

void AUnitAiController::CancelOrder()
{
    bool ok = RunBehaviorTree(holdPositionBehaviorTree);
    check(ok);
}