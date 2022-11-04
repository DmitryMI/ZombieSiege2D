// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbook.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "ProjectileBase.generated.h"

UCLASS()
class ZOMBIESIEGE_API AProjectileBase : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	FVector2D impactDamageMinMax;

	UPROPERTY(EditDefaultsOnly)
	bool bDiesOnCollision = true;

	UPROPERTY(EditDefaultsOnly)
	bool bDiesOnTargetPointReached = false;

	UPROPERTY(EditDefaultsOnly)
	float maxRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly)
	bool bOverrideMovement = false;

	UPROPERTY(EditDefaultsOnly)
	bool bOverrideCollision = false;

	UPROPERTY(EditDefaultsOnly)
	float maxSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	float collisionRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly)
	float reachibilityTestRadius = 5.0f;

	UPROPERTY()
	UWeaponInfo* weaponInfo;

	UPROPERTY()
	bool bIsProjectileAlive = true;

	UPROPERTY()
	FVector launchLocation;

	UPROPERTY()
	FVector targetPoint;

public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* movementComponent;

	UPROPERTY(VisibleAnywhere)
	UShapeComponent* collisionComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool IsTargetLocationReached();

	bool IsRangeExceeded();

	virtual void BeginProjectileDeath();

	virtual void FinishProjectileDeath();

	void KillProjectile();

	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void MoveTowards(const FVector& targetPointArg, const FVector& velocity);

	UFUNCTION(BlueprintCallable)
	UWeaponInfo* GetWeaponInfo();

	UFUNCTION(BlueprintCallable)
	void SetWeaponInfo(UWeaponInfo* weapon);

	UFUNCTION(BlueprintCallable)
	void SetImpactDamageRange(const FVector2D& damageRange);

	UFUNCTION(BlueprintCallable)
	float GetMaxSpeed();

	UFUNCTION(BlueprintCallable)
	virtual void SetMaxSpeed(float speed);

};
