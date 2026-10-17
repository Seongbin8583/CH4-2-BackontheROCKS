// Copyright Epic Games, Inc. All Rights Reserved.

#include "BackontheROCKSWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UBackontheROCKSWheelRear::UBackontheROCKSWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}