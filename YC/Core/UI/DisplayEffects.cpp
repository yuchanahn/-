#include "DisplayEffects.h"

#include <YC/Coroutine/Coro.hpp>

#include "Components/Image.h"
#include "YC/Core/Log/Log.hpp"

void UDisplayEffects::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

}

coroutine UDisplayEffects::CoFade(UImage* Img, float TargetOpacity, UUserWidget* Widget) {
	TargetOpacity = abs(TargetOpacity - 1);
	float Opacity = 0.f;
	while (true) {
		Opacity = FMath::Clamp(Opacity + 0.01f, 0.f, 1.f);
		Img->SetColorAndOpacity(FLinearColor(0, 0, 0, abs(TargetOpacity - Opacity)));
		YC::Log::PushFmt("Opacity: %f", abs(TargetOpacity - Opacity));
		if (Opacity >= 1.f) break;
		co_yield wait_time(0.01f);
	}
	Widget->RemoveFromParent();
	co_return;
}

void UDisplayEffects::FadeIn() {
	Coro::CoStart(CoFade(FadeImage, 1.f, this));
}

void UDisplayEffects::FadeOut() {
	Coro::CoStart(CoFade(FadeImage, 0.f, this));
}
