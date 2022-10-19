// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildOrder.h"

UBuildOrder::UBuildOrder()
{
	orderType = EUnitOrderType::Build;
}

void UBuildOrder::Execute()
{
	Super::Execute();

	GetBlackboard()->SetValueAsClass("BuildingClass", buildingClass);
	GetBlackboard()->SetValueAsVector("Location", targetLocation);
}

void UBuildOrder::SetParameters(TSubclassOf<ABuilding> clazz, const FVector& location)
{
	buildingClass = clazz;
	targetLocation = location;
}
