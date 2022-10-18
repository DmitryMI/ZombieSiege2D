// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitBase.h"
#include "TurretCarrier.h"
#include "TurretControllerComponent.generated.h"

class AUnitAiController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UTurretControllerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	AUnitBase* controlledUnit;

public:	
	// Sets default values for this component's properties
	UTurretControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void OnOwnerPossessedPawnChangedHandler(const FPossessedPawnChangedEventArgs& args);

	void OnHoldPositionOrderIssued();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template<typename T>
	T* GetOwningController()
	{
		return Cast<T>(GetOwner());
	}
};
