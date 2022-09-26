// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitHudComponent.h"
#include "ZombieSiegeUtils.h"
#include "UnitHudWidget.h"
#include "Blueprint/UserWidget.h"


// Called when the game starts
void UUnitHudComponent::BeginPlay()
{
	Super::BeginPlay();

	AUnitBase* owner = GetOwnerUnit();

	owner->OnHealthChanged().AddUObject(this, &UUnitHudComponent::OnOwnerHealthChangedHandler);
}

void UUnitHudComponent::OnOwnerHealthChangedHandler(const FHealthChangedEventArgs& args)
{
	UpdateUnitHudWidget();
}

void UUnitHudComponent::UpdateUnitHudWidget()
{
	if (bHideHealthIfFull)
	{
		AUnitBase* owner = GetOwnerUnit();
		float health = owner->GetHealth();
		float maxHealth = owner->GetMaxHealth();

		if (FMath::IsNearlyEqual(health, maxHealth))
		{
			unitHudWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			unitHudWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (IsWidgetVisible())
	{
		unitHudWidgetInstance->UpdateUnitHud();
	}
}

// Called every frame
void UUnitHudComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AUnitBase* owner = GetOwnerUnit();

	if (!unitHudWidgetInstance)
	{
		unitHudWidgetInstance = Cast<UUnitHudWidget>(GetWidget());
		if (unitHudWidgetInstance)
		{
			unitHudWidgetInstance->SetUnit(owner);
			UpdateUnitHudWidget();
		}
	}
	else
	{
		
	}
}

AUnitBase* UUnitHudComponent::GetOwnerUnit()
{
	AActor* ownerActor = GetOwner();
	check(ownerActor);

	AUnitBase* ownerUnit = Cast<AUnitBase>(ownerActor);
	check(ownerUnit);

	return ownerUnit;
}

