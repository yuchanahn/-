#pragma once

class ANetPC;
class UFMODEvent;

namespace YC::FMod {
struct CheckServer {
	static ANetPC* PC;
};
void SendSFX(const FString& Key, UFMODEvent* Event, const FVector& InPos);
void SendSFX_Stop(const FString& Key, bool bRelease);


// @ - TODO : SendSFXTo(엑터에 붙여서 사운드 재생.) 함수 구현 해야함.
void SendSFXTo(UFMODEvent* Event, const AActor* InTarget);
}
