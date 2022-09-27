// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorAiController.h"
#include "UnitBase.h"
#include "Building.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASurvivorAiController::BeginPlay()
{
    Super::BeginPlay();

    ensure(holdPositionBehaviorTree);
    ensure(wandererBehaviorTree);
    ensure(gathererBehaviorTree);

    if (GetPawn() != nullptr)
    {
        IssueWanderingOrder(GetPawn()->GetActorLocation());
    }
}

void ASurvivorAiController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if (gatheringTarget == nullptr || !gatheringTarget->IsAlive())
    {
        gatheringTarget = nullptr;

    }
}

AJobBase* ASurvivorAiController::GetAssignedToJob()
{
    return assignedToJob;
}

void ASurvivorAiController::SetAssignedToJob(AJobBase* job)
{
    assignedToJob = job;

    if (job == nullptr)
    {
        IssueWanderingOrder(GetPawn()->GetActorLocation());
    }
}

void ASurvivorAiController::IssueAttackUnitOrder(AUnitBase* target)
{
    check(target);

    attackTarget = target;

    bool ok = RunBehaviorTree(attackUnitBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsObject("Target", target);
}

void ASurvivorAiController::IssueGatherOrder(AUnitBase* gatherableUnit)
{
    check(gatherableUnit);    

    gatheringTarget = gatherableUnit;

    bool ok = RunBehaviorTree(gathererBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsObject("GatheringTarget", gatheringTarget);
}

void ASurvivorAiController::IssueWanderingOrder(FVector aroundLocation, float radius, float standingDuration)
{
    bool ok = RunBehaviorTree(wandererBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsVector("StickToPoint", aroundLocation);
    blackboard->SetValueAsFloat("WanderingRadius", radius);
    blackboard->SetValueAsFloat("StandingDuration", standingDuration);
}

void ASurvivorAiController::IssueBuildOrder(TSubclassOf<ABuilding> buildingClass, const FVector& location)
{
    bool ok = RunBehaviorTree(buildBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsClass("BuildingClass", buildingClass);
    blackboard->SetValueAsVector("Location", location);
}

void ASurvivorAiController::IssueRepairOrder(ABuilding* building)
{
    bool ok = RunBehaviorTree(repairBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsObject("Building", building);
}

void ASurvivorAiController::CancelOrder()
{
    gatheringTarget = nullptr;
    attackTarget = nullptr;

    bool ok = RunBehaviorTree(holdPositionBehaviorTree);
    check(ok);
}
