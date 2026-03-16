// Fill out your copyright notice in the Description page of Project Settings.


#include "ThowWeaponAnimNotify.h"
#include "LegendsOfVanaraCharacter.h"

void UThowWeaponAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(!MeshComp || !Animation)
	{
		return;
	}

	ALegendsOfVanaraCharacter* Character = Cast<ALegendsOfVanaraCharacter>(MeshComp->GetOwner());

	if (Character && Character->GadaWeapon) {

		Character->GadaWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		FVector ThrowDirection = Character->GetControlRotation().Vector();

		Character->GadaWeapon->ThrowWeapon(ThrowDirection);
	}
}
