#pragma once
#include <YC/ErrorOr/ErrorOr.hpp>

#include "EnhancedPlayerInput.h"
#include "InputActionValue.h"

class UInputAction;

namespace YC {
struct InputAction {
	//Not Null!!
	ErrorOr<UInputAction*> IA = Err { std::string("InputAction::IA is not valid") };
	FInputActionValue Value;
};
}

namespace YC::Input{
static ErrorOr<FInputActionValue> Check_(UInputAction* InInputAct, UEnhancedPlayerInput* InputComp) {
	if(!IsValid(InInputAct)) return Err { std::string("InputCheck_ : UInputAction* is not valid") };
	if(!IsValid(InputComp)) return Err { std::string("InputCheck_ : UEnhancedPlayerInput* is not valid") };
	
	return InputComp->GetActionValue(InInputAct);
}

//InputCheck_(UInputAction*, UEnhancedPlayerInput*) -> ErrorOr<FInputActionValue>
static auto Check = Curry(Check_);

static ErrorOr<FInputActionValue> Triggered_(const InputAction& InInputAct, UEnhancedPlayerInput* InputComp) {
	if(InInputAct.IA.IsErr()) return Err { InInputAct.IA.GetError() };

	const auto R = Check_(InInputAct.IA.Unwrap(), InputComp);
	if(R.IsErr()) return Err { R.GetError() };
	auto Value = R.Unwrap();
	bool bIsTriggered = false;
	switch (R.Unwrap().GetValueType()) {
		case EInputActionValueType::Boolean:
			bIsTriggered = Value.Get<bool>();
			break;
		case EInputActionValueType::Axis1D:
			bIsTriggered = Value.Get<float>() > 0.0f;
			break;
		case EInputActionValueType::Axis2D:
			bIsTriggered = Value.Get<FVector2D>().SizeSquared() > 0.0f;
			break;
		case EInputActionValueType::Axis3D:
			bIsTriggered = Value.Get<FVector>().SizeSquared() > 0.0f;
			break;
	default: ;
	}
	if(bIsTriggered) return Value;
	return Err { std::string("Input_Triggered : Input is not triggered") };
}
//Input_Triggered_(YC::InputAction, UEnhancedPlayerInput*) -> ErrorOr<FInputActionValue>
static auto Triggered = Curry(Triggered_);

static ErrorOr<FInputActionValue> Started_(const InputAction& InInputAct, UEnhancedPlayerInput* InputComp) {
	if(InInputAct.IA.IsErr()) return Err { InInputAct.IA.GetError() };

	const auto R = Check_(InInputAct.IA.Unwrap(), InputComp);
	if(R.IsErr()) return Err { R.GetError() };
	const auto Value = R.Unwrap();
	bool bIsStarted = false;
	switch (R.Unwrap().GetValueType()) {
	case EInputActionValueType::Boolean:
		bIsStarted = Value.Get<bool>() && !InInputAct.Value.Get<bool>();
		break;
	case EInputActionValueType::Axis1D:
		bIsStarted = Value.Get<float>() > 0.0f && InInputAct.Value.Get<float>() <= 0.0f;
		break;
	case EInputActionValueType::Axis2D:
		bIsStarted = Value.Get<FVector2D>().SizeSquared() > 0.0f && InInputAct.Value.Get<FVector2D>().SizeSquared() <= 0.0f;
		break;
	case EInputActionValueType::Axis3D:
		bIsStarted = Value.Get<FVector>().SizeSquared() > 0.0f && InInputAct.Value.Get<FVector>().SizeSquared() <= 0.0f;
		break;
	default: ;
	}
	if(bIsStarted) return Value;
	return Err { std::string("Input_Started : Input is not started") };
}
//Input_Started_(YC::InputAction, UEnhancedPlayerInput*) -> ErrorOr<FInputActionValue>
static auto Started = Curry(Started_);

static ErrorOr<FInputActionValue> Completed_(const InputAction& InInputAct, UEnhancedPlayerInput* InputComp) {
	if(InInputAct.IA.IsErr()) return Err { InInputAct.IA.GetError() };

	const auto R = Check_(InInputAct.IA.Unwrap(), InputComp);
	if(R.IsErr()) return Err { R.GetError() };
	const auto Value = R.Unwrap();
	bool bIsCompleted = false;
	switch (R.Unwrap().GetValueType()) {
	case EInputActionValueType::Boolean:
		bIsCompleted = !Value.Get<bool>() && InInputAct.Value.Get<bool>();
		break;
	case EInputActionValueType::Axis1D:
		bIsCompleted = Value.Get<float>() <= 0.0f && InInputAct.Value.Get<float>() > 0.0f;
		break;
	case EInputActionValueType::Axis2D:
		bIsCompleted = Value.Get<FVector2D>().SizeSquared() <= 0.0f && InInputAct.Value.Get<FVector2D>().SizeSquared() > 0.0f;
		break;
	case EInputActionValueType::Axis3D:
		bIsCompleted = Value.Get<FVector>().SizeSquared() <= 0.0f && InInputAct.Value.Get<FVector>().SizeSquared() > 0.0f;
		break;
	default: ;
	}
	if(bIsCompleted) return Value;
	return Err { std::string("Input_Completed : Input is not completed") };
}
//Input_Completed_(YC::InputAction, UEnhancedPlayerInput*) -> ErrorOr<FInputActionValue>
static auto Completed = Curry(Completed_);


static ErrorOr<InputAction> Update_(const YC::InputAction& InInputAct, UEnhancedPlayerInput* InputComp) {
	if(InInputAct.IA.IsErr()) return Err { InInputAct.IA.GetError() };

	const auto R = Check_(InInputAct.IA.Unwrap(), InputComp);
	if(R.IsErr()) return Err { R.GetError() };
	const auto Value = R.Unwrap();
	
	return { YC::InputAction { InInputAct.IA.Unwrap(), Value } };
}
//NewInputState_(YC::InputAction, UEnhancedPlayerInput*) -> ErrorOr<YC::InputAction>
static auto Update = Curry(Update_);
}
