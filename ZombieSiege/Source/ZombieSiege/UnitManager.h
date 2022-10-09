// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSingleton.h"
#include "UnitBase.h"
#include "UnitManager.generated.h"

UCLASS()
class ZOMBIESIEGE_API AUnitManager : public AActor
{
	GENERATED_BODY()

	ACTOR_SINGLETON(AUnitManager);

private:
	UPROPERTY(EditAnywhere)
	TMap<FName, TSubclassOf<AUnitBase>> unitClassMap;

public:	
	// Sets default values for this actor's properties
	AUnitManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage = "This method exposes the internal map for read-write."))

	TMap<FName, TSubclassOf<AUnitBase>>& GetUnitClassMapMutable();

	UFUNCTION(BlueprintCallable)

	const TMap<FName, TSubclassOf<AUnitBase>>& GetUnitClassMap() const;

	UFUNCTION(BlueprintCallable)
		
	TSubclassOf<AUnitBase> GetUnitClass(const FName& name) const;

	UFUNCTION(BlueprintCallable)
	bool AddUnitClass(const FName& name, TSubclassOf<AUnitBase> clazz);

	UFUNCTION(BlueprintCallable)
	bool HasUnitClass(const FName& name) const;

};
