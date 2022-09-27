// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitGraphicsData.h"
#include "ActorSingleton.h"
#include "UnitGraphicsManager.generated.h"

UCLASS()
class ZOMBIESIEGE_API AUnitGraphicsManager : public AActor
{
	GENERATED_BODY()

	ACTOR_SINGLETON(AUnitGraphicsManager)
	
public:	
	// Sets default values for this actor's properties
	AUnitGraphicsManager();

protected:

	UPROPERTY(EditAnywhere)
	TMap<FName, TSubclassOf<UUnitGraphicsData>> unitGraphicsDataClasses;

	UPROPERTY(EditAnywhere)
	TMap<FName, UUnitGraphicsData*> unitGraphicsDataMap;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	UUnitGraphicsData* GetUnitGraphicsData(const FName& name) const;

	UFUNCTION(BlueprintCallable)
	bool AddUnitGraphicsData(const FName& name, UUnitGraphicsData* unitGraphicsData);

	UFUNCTION(BlueprintCallable)
	bool HasUnitGraphicsData(const FName& name) const;
};
