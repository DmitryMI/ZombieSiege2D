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
class ZOMBIESIEGE_API UUnitGraphicsComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	bool bAutoSetGraphicsData = true;

	UPROPERTY(EditAnywhere)
	bool bOverrideGraphicsDataName = false;

	UPROPERTY(EditAnywhere)
	FName graphicsDataName;

	UPROPERTY(EditAnywhere)
	bool bAutoUpdateFlipbook = true;

	UPROPERTY(VisibleAnywhere)
	UUnitGraphicsData* unitGraphicsData;
	
protected:

	UPROPERTY(Transient)
	UPaperFlipbookComponent* flipbookRenderer;

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
