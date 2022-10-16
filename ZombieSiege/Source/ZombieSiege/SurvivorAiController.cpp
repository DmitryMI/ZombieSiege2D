// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorAiController.h"
#include "UnitBase.h"
#include "Building.h"


void ASurvivorAiController::BeginPlay()
{
    Super::BeginPlay();

    ensure(holdPositionBehaviorTree);
    ensure(wandererBehaviorTree);
    ensure(gathererBehaviorTree);

    if (GetPawn() != nullptr)
    {
        //IssueWanderingOrder(GetPawn()->GetActorLocation());
        IssueHoldPositionOrder();
    }
}

void ASurvivorAiController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
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

void ASurvivorAiController::IssueGatherOrder(AUnitBase* gatherableUnit)
{
    check(gatherableUnit);    

    bool ok = RunBehaviorTree(gathererBehaviorTree);
    check(ok);

    UBlackboardComponent* blackboard = GetBlackboardComponent();
    check(blackboard);

    blackboard->SetValueAsObject("GatheringTarget", gatherableUnit);
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
