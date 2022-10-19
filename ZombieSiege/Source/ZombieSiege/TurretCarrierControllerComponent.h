// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitBase.h"
#include "TurretCarrier.h"
#include "UnitAiController.h"
#include "TurretCarrierControllerComponent.generated.h"

class AUnitAiController;
class ATurret;

UENUM(BlueprintType)
enum class ETurretAimStrategy : uint8
{
	Auto,
	FocusOneTargetAlways,
	SpreadWhenNoOrders,
	SpreadAlways
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UTurretCarrierControllerComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere)
	ETurretAimStrategy turretAimStrategy = ETurretAimStrategy::Auto;

	UPROPERTY(Transient)
	AUnitBase* focusTarget;

public:	
	// Sets default values for this component's properties
	UTurretCarrierControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	const TArray<ATurret*> GetTurrets() const;

	void UpdateTurretControllers();

	void OnOwnerPossessedPawnChangedHandler(const FPossessedPawnChangedEventArgs& args);

	void OnCarrierOrderExecutionStartedHandler(const FOrderExecutionStartedEventArgs& args);

	void SetTurretAutoAimEnabled(ATurret* turret, bool bEnabled);

	void SetAllTurretsAutoAimEnabled(bool bEnabled);

	AUnitBase* GetTurretTargetUnit(ATurret* turret);

	void SetTurretTargetUnit(ATurret* turret, AUnitBase* unit);

	AUnitBase* GetClosestAliveAttackableEnemy();

	void SpreadTargetsAcrossTurrets();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template<typename T>
	T* GetOwningController() const
	{
		return Cast<T>(GetOwner());
	}

	template<typename T>
	T* GetOwningUnit() const
	{
		AAIController* controller = GetOwningController<AAIController>();
		if (!controller)
		{
			return nullptr;
		}

		return Cast<T>(controller->GetPawn());
	}
};
