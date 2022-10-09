// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageInstance.h"
#include "WeaponInfo.h"

FDamageInstance::FDamageInstance()
{
	this->source = nullptr;
	this->amount = 0;
	this->weaponInfo = nullptr;
}

FDamageInstance::FDamageInstance(AUnitBase* source, float amount, UWeaponInfo* weaponInfoArg)
{
	this->source = source;
	this->amount = amount;
	this->weaponInfo = weaponInfoArg;

	check(this->amount >= 0);
}

FDamageInstance::~FDamageInstance()
{
}
