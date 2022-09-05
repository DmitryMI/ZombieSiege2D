// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "Doodad.generated.h"

UCLASS()
class ZOMBIESIEGE_API ADoodad : public AUnitBase
{
	GENERATED_BODY()
		
private:	
	
public:	
	ADoodad();

protected:
	UPROPERTY(EditDefaultsOnly)
	bool bCanEverBeGathered;

	virtual void BeginPlay() override;

public:	
	virtual bool CanEverBeGathered();
};
