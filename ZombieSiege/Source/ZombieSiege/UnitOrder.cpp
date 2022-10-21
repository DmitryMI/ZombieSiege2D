// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitOrder.h"

UUnitOrder::UUnitOrder()
{
}

UUnitOrder::~UUnitOrder()
{
}

EUnitOrderType UUnitOrder::GetOrderType()
{
	return orderType;
}

AUnitAiController* UUnitOrder::GetController()
{
	return unitController;
}

void UUnitOrder::SetController(AUnitAiController* controller)
{
	unitController = controller;
}

UBehaviorTree* UUnitOrder::GetBehaviorTree()
{
	return behaviorTree;
}

void UUnitOrder::SetBehaviorTree(UBehaviorTree* behavior)
{
	behaviorTree = behavior;
}

UBlackboardComponent* UUnitOrder::GetBlackboard()
{
	if (!unitController)
	{
		return nullptr;
	}

	return unitController->GetBlackboardComponent();
}

void UUnitOrder::FinishOrder(bool success)
{
	unitController->OnOrderFinished(this);
}

void UUnitOrder::CancelOrder()
{
	unitController->OnOrderFinished(this);
}

void UUnitOrder::Execute()
{
	AUnitAiController* controller = GetController();
	check(controller);

	UBehaviorTree* tree = GetBehaviorTree();
	bool treeRunOk = controller->RunBehaviorTree(tree);
	check(treeRunOk);

	GetBlackboard()->SetValueAsObject("Order", this);
}

void UUnitOrder::UnitPerceptionUpdated(AUnitBase* unit, FAIStimulus Stimulus)
{
}

void UUnitOrder::ControlledUnitAttacked(const FDamageReceivedEventArgs& damageEventArgs)
{
}
