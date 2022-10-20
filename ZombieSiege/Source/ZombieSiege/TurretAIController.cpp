// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretAIController.h"
#include "Turret.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h" 

void ATurretAIController::BeginPlay()
{
	Super::BeginPlay();

	UBlackboardComponent* blackboard = GetBlackboardComponent();
	blackboard->SetValueAsBool("AutoAimEnabled", bAutoAimEnabled);

	if (PerceptionComponent)
	{
		UAISenseConfig_Sight* sightConfig = GetSightSenseConfig();
		check(sightConfig);
		sightConfig->PeripheralVisionAngleDegrees = 360;

		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ATurretAIController::OnTargetPerceptionUpdatedHandler);
	}
}

UAISenseConfig_Sight* ATurretAIController::GetSightSenseConfig()
{
	FAISenseID senseId = UAISense::GetSenseID(PerceptionComponent->GetDominantSense());
	UAISenseConfig* senseConfig = PerceptionComponent->GetSenseConfig(senseId);
	UAISenseConfig_Sight* sightConfig = Cast<UAISenseConfig_Sight>(senseConfig);
	return sightConfig;
}

void ATurretAIController::SetPerceptionSightRadius(float radius)
{
	UAISenseConfig_Sight* sightConfig = GetSightSenseConfig();
	if (sightConfig)
	{
		sightConfig->SightRadius = radius;
		sightConfig->LoseSightRadius = radius;
	}
}

void ATurretAIController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	if (PerceptionComponent)
	{
		UAIPerceptionSystem* perceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
		perceptionSystem->UpdateListener(*PerceptionComponent);
	}
}

void ATurretAIController::OnTargetPerceptionUpdatedHandler(AActor* Actor, FAIStimulus Stimulus)
{
	if (!bAutoAimEnabled)
	{
		return;
	}

	if (!Stimulus.WasSuccessfullySensed())
	{
		return;
	}

	AUnitBase* perceivedUnit = Cast<AUnitBase>(Actor);
	if (!perceivedUnit)
	{
		return;
	}
	
	FAttackTestParameters attackTestParams(false, true, true, false);

	ATurret* turret = Cast<ATurret>(GetPawn());
	AUnitBase* currentTarget = GetTargetUnit();
	if (!currentTarget || !turret->CanAttackTarget(currentTarget, attackTestParams))
	{
		if (turret->CanAttackTarget(perceivedUnit, attackTestParams))
		{
			SetTargetUnit(perceivedUnit);
		}
	}
}

FGenericTeamId ATurretAIController::GetGenericTeamId() const
{
	ATurret* controlledTurret = Cast<ATurret>(GetPawn());
	if (!controlledTurret)
	{
		return FGenericTeamId();
	}

	AUnitBase* turretOwner = controlledTurret->GetOwningUnit();
	if (!turretOwner)
	{
		return FGenericTeamId();
	}

	return turretOwner->GetGenericTeamId();
}

bool ATurretAIController::GetPerceivedAttackableEnemies(TArray<AUnitBase*>& outEnemies) const
{
	if (!PerceptionComponent)
	{
		return false;
	}

	ATurret* controlledTurret = Cast<ATurret>(GetPawn());
	if (!controlledTurret)
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

		if (!controlledTurret->CanAttackTarget(unit, FAttackTestParameters(false, false, true, false)))
		{
			continue;
		}

		outEnemies.Add(unit);
	}

	return !outEnemies.IsEmpty();
}

bool ATurretAIController::IsAutoAimEnabled()
{
	return bAutoAimEnabled;
}

void ATurretAIController::SetAutoAimEnabled(bool bEnabled)
{
	if (bAutoAimEnabled != bEnabled)
	{
		bAutoAimEnabled = bEnabled;

		UBlackboardComponent* blackboard = GetBlackboardComponent();
		blackboard->SetValueAsBool("AutoAimEnabled", bAutoAimEnabled);
	}
}

AUnitBase* ATurretAIController::GetTargetUnit()
{
	UBlackboardComponent* blackboard = GetBlackboardComponent();
	AUnitBase* target = Cast<AUnitBase>(blackboard->GetValueAsObject("TargetUnit"));
	return target;
}

void ATurretAIController::SetTargetUnit(AUnitBase* target)
{
	UBlackboardComponent* blackboard = GetBlackboardComponent();
	blackboard->SetValueAsObject("TargetUnit", target);
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