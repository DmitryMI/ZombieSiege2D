// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PaperFlipbook.h"
#include "FaceDirection.h"
#include "UnitGraphicsData.h"
#include "PaperFlipbookComponent.h"
#include "UnitGraphicsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UUnitGraphicsComponent : public UPaperFlipbookComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	bool bAutoSetGraphicsData = true;

	UPROPERTY(EditDefaultsOnly)
	bool bOverrideGraphicsDataName = false;

	UPROPERTY(EditDefaultsOnly)
	FName graphicsDataName;

	UPROPERTY(EditDefaultsOnly)
	bool bAutoUpdateFlipbook = true;

	UPROPERTY(EditDefaultsOnly)
	float minimalFlipbookAutoUpdateInterval = 0.1f;

	UPROPERTY()
	UUnitGraphicsData* unitGraphicsData;
	
	UPROPERTY()
	float flipbookAutoUpdateIntervalCounter = 0.0f;

protected:

	virtual void BeginPlay() override;

public:	

	UUnitGraphicsComponent();

	UFUNCTION(BlueprintCallable)
	void UpdateFlipbook(EUnitState state, EFaceDirection direction);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		

	UFUNCTION(BlueprintCallable)
	void SetUnitGraphicsData(UUnitGraphicsData* graphicsData);

	UFUNCTION(BlueprintCallable)
	UUnitGraphicsData* GetUnitGraphicsData();
};
