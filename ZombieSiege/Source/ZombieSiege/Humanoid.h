// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "FaceDirection.h"
#include "UnitState.h"
#include "UnitGraphicsComponent.h"
#include "WeaponInfo.h"
#include "WeaponManager.h"
#include "AttackTestParameters.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Humanoid.generated.h"

UCLASS()
class ZOMBIESIEGE_API AHumanoid : public AUnitBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	FName weaponDefaultName = "SurvivorFists";	

	UPROPERTY(VisibleAnywhere)
	AWeaponManager* weaponManager;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* weaponDefault;

public:
	AHumanoid();	

protected:

	UFUNCTION(BlueprintCallable)
	AWeaponManager* GetWeaponManager();
	
	virtual void BeginPlay() override;	

	virtual void OnAttackStateChanged(const FAttackDispatcherStateChangedEventArgs& args) override;

public:		

	UFUNCTION(BlueprintCallable)
	static EFaceDirection GetDirectionFromVector(const FVector& vec);

	virtual void Tick(float DeltaTime) override;	

	virtual bool CanMove() const override;

	virtual float GetAttackRange() override;

	virtual bool CanAttackPoint(const FVector& targetPoint, FAttackTestParameters testParams) override;

	virtual bool CanAttackTarget(AActor* targetActor, FAttackTestParameters testParams) override;

	virtual bool BeginAttackTarget(AActor* targetActor) override;

	virtual bool BeginAttackPoint(const FVector& point) override;
};
