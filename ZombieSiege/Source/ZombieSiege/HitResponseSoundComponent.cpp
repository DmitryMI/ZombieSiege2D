// Fill out your copyright notice in the Description page of Project Settings.


#include "HitResponseSoundComponent.h"
#include "UnitBase.h"
#include "WeaponInfo.h"

void UHitResponseSoundComponent::BeginPlay()
{
	Super::BeginPlay();

	soundManager = ASoundManager::GetInstance(GetWorld());
	check(soundManager);

	AUnitBase* ownerUnit = Cast<AUnitBase>(GetOwner());
	check(ownerUnit);

	ownerUnit->OnDamageReceived().AddUObject(this, &UHitResponseSoundComponent::OnOwnerDamageReceived);
}

void UHitResponseSoundComponent::OnOwnerDamageReceived(const FDamageReceivedEventArgs& args)
{
	check(args.target == GetOwner());

	if (args.weaponInfo == nullptr)
	{
		return;
	}

	if (!soundManager)
	{
		soundManager = ASoundManager::GetInstance(GetWorld());
		check(soundManager);
	}

	EArmorNature armorNature = args.target->GetArmorNature();
	EWeaponNature weaponNature = EWeaponNature::Unknown;

	if (args.weaponInfo)
	{
		weaponNature = args.weaponInfo->GetWeaponNature();
	}

	USoundBase* response = soundManager->GetHitResponseSound(armorNature, weaponNature);

	SetSound(response);
	Play();
}
