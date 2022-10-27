// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "UnitBase.h"
#include "WaveManager.generated.h"

UCLASS()
class ZOMBIESIEGE_API AWaveManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float currentDifficulty = 0.0f;

	UPROPERTY(EditAnywhere)
	float difficultyGrowthFactor = 1.0f;

	UPROPERTY(EditAnywhere)
	float maxDifficulty = 600.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> spawnPositionMarkerClass;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> spawnPositionMarkers;

	UPROPERTY(EditAnywhere)
	float spawnPointSearchRadius = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, float> waveUnitCostMap;
	
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* waveIntervalCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* waveStrengthCurve;

	UPROPERTY(EditDefaultsOnly)

	FTimerHandle waveTimerHandle;

public:	
	// Sets default values for this actor's properties
	AWaveManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnWaveTimerExpiredHandler();

	void SpawnWaveInternal(float waveStrength);

	float CalculateWaveInterval();

	float CalculateWaveStrengh();

	TMap<FName, int> GetRandomUnitClassSet(float waveStrength);

	float GetNormalizedDifficulty();

	AUnitBase* SpawnUnit(TSubclassOf<AUnitBase> unitClass);

	void IssueOrders(AUnitBase* unit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetDifficulty(float difficulty);

	float GetDifficulty();

	UFUNCTION(BlueprintCallable)
	void SpawnWave(float waveStrength);
};
