// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbook.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "ProjectileBase.generated.h"

UCLASS()
class ZOMBIESIEGE_API AProjectileBase : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* movingFlipbook;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* deathFlipbook;

	UPROPERTY(EditDefaultsOnly)
	FVector2D impactDamageMinMax;

	UPROPERTY(EditDefaultsOnly)
	bool bDiesOnCollision = true;

	UPROPERTY(EditDefaultsOnly)
	float maxSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	float collisionRadius = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float reachibilityTestRadius = 5.0f;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* weaponInfo;

	UPROPERTY(VisibleAnywhere)
	bool bIsProjectileAlive = true;

	UPROPERTY(EditAnywhere)
	FVector targetPoint;

public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* movementComponent;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* collisionComponent;

	UPROPERTY(VisibleAnywhere)
	UPaperFlipbookComponent* flipbookRenderer;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool IsTargetLocationReached();

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
	void MoveTowards(const FVector& targetPointArg);

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
