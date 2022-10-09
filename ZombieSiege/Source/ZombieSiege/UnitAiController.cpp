// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAiController.h"

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
    bool ok = RunBehaviorTree(wandererBehaviorTree);
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

void AUnitAiController::CancelOrder()
{
    bool ok = RunBehaviorTree(holdPositionBehaviorTree);
    check(ok);
}