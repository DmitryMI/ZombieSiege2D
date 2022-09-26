// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitBase.h"
#include "Components/WidgetComponent.h"
#include "BuildingProgressChangedEventArgs.h"
#include "UnitHudComponent.generated.h"

class UUnitHudWidget;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UUnitHudComponent : public UWidgetComponent
{
	GENERATED_BODY()

private:
	/// <summary>
	/// Widget instance created by the parent class. Must NOT be UPROPERTY
	/// </summary>
	UUnitHudWidget* unitHudWidgetInstance;

	UPROPERTY(EditAnywhere)
	bool bHideHealthIfFull;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnOwnerHealthChangedHandler(const FHealthChangedEventArgs& args);
	virtual void OnOwnerBuildingProgressChangedHandler(const FBuildingProgressChangedEventArgs& args);

	virtual void UpdateUnitHudWidget();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AUnitBase* GetOwnerUnit();
};
