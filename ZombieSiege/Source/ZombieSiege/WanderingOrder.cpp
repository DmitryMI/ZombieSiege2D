// Fill out your copyright notice in the Description page of Project Settings.


#include "WanderingOrder.h"

UWanderingOrder::UWanderingOrder()
{
    orderType = EUnitOrderType::Wander;
}

void UWanderingOrder::Execute()
{
    Super::Execute();

    UBlackboardComponent* blackboard = GetBlackboard();

    blackboard->SetValueAsVector("StickToPoint", stickToPoint);
    blackboard->SetValueAsFloat("WanderingRadius", wanderingRadius);
    blackboard->SetValueAsFloat("StandingDuration", standingDuration);
}

void UWanderingOrder::SetParameters(const FVector& aroundPoint, float radius, float standForSeconds)
{
    stickToPoint = aroundPoint;
    wanderingRadius = radius;
    standingDuration = standForSeconds;
}
