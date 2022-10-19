// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void ATurretAIController::BeginPlay()
{
	Super::BeginPlay();

	UBlackboardComponent* blackboard = GetBlackboardComponent();
	blackboard->SetValueAsBool("AutoAimEnabled", bAutoAimEnabled);
}

bool ATurretAIController::IsAutoAimEnabled()
{
	return bAutoAimEnabled;
}

void ATurretAIController::SetAutoAimEnabled(bool bEnabled)
{
	bAutoAimEnabled = bEnabled;

	UBlackboardComponent* blackboard = GetBlackboardComponent();
	blackboard->SetValueAsBool("AutoAimEnabled", bAutoAimEnabled);
}

#if WITH_EDITOR  
void ATurretAIController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FString propName;
	PropertyChangedEvent.Property->GetName(propName);
	if (propName == "bAutoAimEnabled")
	{
		SetAutoAimEnabled(bAutoAimEnabled);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif