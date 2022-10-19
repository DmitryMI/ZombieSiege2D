// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitBase.h"
#include "TurretCarrier.h"
#include "UnitAiController.h"
#include "TurretCarrierControllerComponent.generated.h"

class AUnitAiController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UTurretCarrierControllerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	AUnitBase* controlledUnit;

public:	
	// Sets default values for this component's properties
	UTurretCarrierControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void OnOwnerPossessedPawnChangedHandler(const FPossessedPawnChangedEventArgs& args);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template<typename T>
	T* GetOwningController()
	{
		return Cast<T>(GetOwner());
	}
};
