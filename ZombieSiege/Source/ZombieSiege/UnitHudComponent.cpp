// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitHudComponent.h"
#include "ZombieSiegeUtils.h"
#include "UnitHudWidget.h"
#include "Building.h"
#include "Blueprint/UserWidget.h"


// Called when the game starts
void UUnitHudComponent::BeginPlay()
{
	Super::BeginPlay();

	AUnitBase* owner = GetOwnerUnit();
	check(owner);

	owner->OnHealthChanged().AddUObject(this, &UUnitHudComponent::OnOwnerHealthChangedHandler);

	ABuilding* building = Cast<ABuilding>(owner);
	if (building)
	{
		building->OnBuildingProgressChanged().AddUObject(this, &UUnitHudComponent::OnOwnerBuildingProgressChangedHandler);
	}

	UUnitHudWidget* unitHudWidgetInstance = Cast<UUnitHudWidget>(GetWidget());
	ensure(unitHudWidgetInstance);
	if (unitHudWidgetInstance)
	{
		unitHudWidgetInstance->SetUnit(owner);
	}
	UpdateUnitHudWidget();
}

void UUnitHudComponent::OnOwnerHealthChangedHandler(const FHealthChangedEventArgs& args)
{
	UpdateUnitHudWidget();
}

void UUnitHudComponent::OnOwnerBuildingProgressChangedHandler(const FBuildingProgressChangedEventArgs& args)
{
	UpdateUnitHudWidget();
}

void UUnitHudComponent::UpdateUnitHudWidget()
{
	UUnitHudWidget* unitHudWidgetInstance = Cast<UUnitHudWidget>(GetWidget());

	ensure(unitHudWidgetInstance);

	if (!unitHudWidgetInstance)
	{
		return;
	}

	if (bHideHealthIfFull)
	{
		AUnitBase* owner = GetOwnerUnit();
		float health = owner->GetHealth();
		float maxHealth = owner->GetMaxHealth();

		if (FMath::IsNearlyEqual(health, maxHealth))
		{
			unitHudWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			SetHiddenInGame(true, true);
		}
		else
		{
			unitHudWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			SetHiddenInGame(false, true);
		}
	}

	unitHudWidgetInstance->UpdateUnitHud();
}

void UUnitHudComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

AUnitBase* UUnitHudComponent::GetOwnerUnit()
{
	AActor* ownerActor = GetOwner();
	check(ownerActor);

	AUnitBase* ownerUnit = Cast<AUnitBase>(ownerActor);
	check(ownerUnit);

	return ownerUnit;
}

