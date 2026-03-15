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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Throw Weapon Anim Notify Triggered"));

		Character->GadaWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		FVector ThrowDirection = Character->GetControlRotation().Vector();
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Throw Direction: %s"), *ThrowDirection.ToString()));

		Character->GadaWeapon->ThrowWeapon(ThrowDirection);
	}
}
