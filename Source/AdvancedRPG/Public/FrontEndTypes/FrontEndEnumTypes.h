#pragma once

UENUM(BlueprintType)
enum class EConfirmScreenType : uint8
{
	Ok,
	YesNo,
	OkCancel,
	Unknown UMETA(Hidden)

};

UENUM(BlueprintType)
enum class EConfirmScreenButtonType : uint8
{
	Confirmed,
	cancelled,
	Closed,
	Unknown UMETA(Hidden)

};
