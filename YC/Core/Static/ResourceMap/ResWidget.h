#pragma once
#include "ResCharacter.h"
#include "Blueprint/UserWidget.h"

#include "ResWidget.generated.h"

#define RES_WRAPPER(STRUCT, VAR, VAL) \
	STRUCT.VAR = VAL; \
	VAR = VAL;

USTRUCT()
struct FResWidgetStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_MainMenu;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_InGameWidget;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_Loading;

	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_Inv;
	UPROPERTY()
	TArray<TSubclassOf<UUserWidget>> WBP_Items;
	UPROPERTY()
	TArray<UTexture2D*> Tex_ItemShapeShared;
	UPROPERTY()
	TArray<UTexture2D*> Tex_ItemShapePadma;
	UPROPERTY()
	TArray<UTexture2D*> Tex_ItemShapeFaema;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_ItemTT;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_LvUp;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_LvUpItem;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_ChrMenuMsg;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_FadeInOut;

	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_FaemaUniqueAttack;
	UPROPERTY()
	TSubclassOf<UUserWidget> WBP_FaemaChargeHUD;
};

namespace Res
{

inline FResWidgetStruct ResWidget;

inline TSubclassOf<UUserWidget> WBP_MainMenu;
inline TSubclassOf<UUserWidget> WBP_InGameWidget;

inline TSubclassOf<UUserWidget> WBP_Inv;
inline TArray<TSubclassOf<UUserWidget>> WBP_Items;
inline TArray<UTexture2D*> Tex_ItemShapeShared;
inline TArray<UTexture2D*> Tex_ItemShapePadma;
inline TArray<UTexture2D*> Tex_ItemShapeFaema;
inline TSubclassOf<UUserWidget> WBP_ItemTT;
inline TSubclassOf<UUserWidget> WBP_LvUp;
inline TSubclassOf<UUserWidget> WBP_LvUpItem;
inline TSubclassOf<UUserWidget> WBP_ChrMenuMsg;
inline TSubclassOf<UUserWidget> WBP_FadeInOut;
inline TSubclassOf<UUserWidget> WBP_Loading;

inline TSubclassOf<UUserWidget> WBP_FaemaUniqueAttack;
inline TSubclassOf<UUserWidget> WBP_FaemaChargeHUD;

inline void LoadWidget()
{
	using Find_WBP = ConstructorHelpers::FClassFinder<UUserWidget>;
	using Find_Tex2D = ConstructorHelpers::FObjectFinder<UTexture2D>;

	RES_WRAPPER(ResWidget, WBP_MainMenu, Find_WBP(L"/Game/01_Source/Yuchan/UI/WBP_CharacterSelect").Class);
	RES_WRAPPER(ResWidget, WBP_InGameWidget, Find_WBP(L"/Game/01_Source/Dongyun/UI/WBP_HUD").Class);

	RES_WRAPPER(ResWidget, WBP_Inv, Find_WBP(L"/Game/01_Source/Dongjun/UI/Inventory/WBP_Inventory").Class);
	ResWidget.WBP_Items.Init(nullptr, 53);
	WBP_Items.Init(nullptr, 53);
	ResWidget.Tex_ItemShapeShared.Init(nullptr, 53);
	Tex_ItemShapeShared.Init(nullptr, 53);
	ResWidget.Tex_ItemShapePadma.Init(nullptr, 53);
	Tex_ItemShapePadma.Init(nullptr, 53);
	ResWidget.Tex_ItemShapeFaema.Init(nullptr, 53);
	Tex_ItemShapeFaema.Init(nullptr, 53);
	for (int i = 0; i < 21; ++i)
	{
		RES_WRAPPER(ResWidget, WBP_Items[i],
			Find_WBP(*(FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/ItemWidget/WBP_Item_")) +
				FString::FromInt(i))).Class);

		if (TArray<int>{7, 9, 10, 11, 16, 17, 19, 20, 21, 51}.Contains(i + 1))
			continue;
		
		FString TexNumStr;
		if (i + 1 < 10) TexNumStr += FString(TEXT("0"));
		TexNumStr += FString::FromInt(i + 1);

		const auto Tex_PathShared =
			FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/Texture/Shared/UI_Item_Shape")) + TexNumStr + TEXT(
				"_Inventory_Public");
		const auto Tex_PathPadma =
			FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/Texture/Padma/UI_Item_Shape")) + TexNumStr + TEXT(
				"_Inventory_Padma");
		const auto Tex_PathFaema =
			FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/Texture/Faema/UI_Item_Shape")) + TexNumStr + TEXT(
				"_Inventory_Faema");

		Find_Tex2D Find_Tex_ItemShapeShared(*Tex_PathShared);
		Find_Tex2D Find_Tex_ItemShapePadma(*Tex_PathPadma);
		Find_Tex2D Find_Tex_ItemShapeFaema(*Tex_PathFaema);

		if (Find_Tex_ItemShapeShared.Succeeded())
			RES_WRAPPER(ResWidget, Tex_ItemShapeShared[i], Find_Tex_ItemShapeShared.Object);

		if (Find_Tex_ItemShapePadma.Succeeded())
			RES_WRAPPER(ResWidget, Tex_ItemShapePadma[i], Find_Tex_ItemShapePadma.Object);

		if (Find_Tex_ItemShapeFaema.Succeeded())
			RES_WRAPPER(ResWidget, Tex_ItemShapeFaema[i], Find_Tex_ItemShapeFaema.Object);
	}
	for (int i = 49; i < 52; ++i)
	{
		RES_WRAPPER(ResWidget, WBP_Items[i],
			Find_WBP(*(FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/ItemWidget/WBP_Item_")) +
				FString::FromInt(i))).Class);

		if (TArray<int>{7, 9, 10, 11, 16, 17, 19, 20, 21, 51}.Contains(i + 1))
			continue;
		
		FString TexNumStr;
		if (i + 1 < 10) TexNumStr += FString(TEXT("0"));
		TexNumStr += FString::FromInt(i + 1);

		const auto Tex_PathShared =
			FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/Texture/Shared/UI_Item_Shape")) + TexNumStr + TEXT(
				"_Inventory_Public");
		const auto Tex_PathPadma =
			FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/Texture/Padma/UI_Item_Shape")) + TexNumStr + TEXT(
				"_Inventory_Padma");
		const auto Tex_PathFaema =
			FString(TEXT("/Game/01_Source/Dongjun/UI/Inventory/Texture/Faema/UI_Item_Shape")) + TexNumStr + TEXT(
				"_Inventory_Faema");

		Find_Tex2D Find_Tex_ItemShapeShared(*Tex_PathShared);
		Find_Tex2D Find_Tex_ItemShapePadma(*Tex_PathPadma);
		Find_Tex2D Find_Tex_ItemShapeFaema(*Tex_PathFaema);

		if (Find_Tex_ItemShapeShared.Succeeded())
			RES_WRAPPER(ResWidget, Tex_ItemShapeShared[i], Find_Tex_ItemShapeShared.Object);

		if (Find_Tex_ItemShapePadma.Succeeded())
			RES_WRAPPER(ResWidget, Tex_ItemShapePadma[i], Find_Tex_ItemShapePadma.Object);

		if (Find_Tex_ItemShapeFaema.Succeeded())
			RES_WRAPPER(ResWidget, Tex_ItemShapeFaema[i], Find_Tex_ItemShapeFaema.Object);
	}
	RES_WRAPPER(ResWidget, WBP_ItemTT, Find_WBP(L"/Game/01_Source/Dongjun/UI/Inventory/WBP_ItemTooltip").Class);

	RES_WRAPPER(ResWidget, WBP_LvUp, Find_WBP(L"/Game/01_Source/Dongjun/LevelUp/WBP_LevelUp").Class);
	RES_WRAPPER(ResWidget, WBP_LvUpItem, Find_WBP(L"/Game/01_Source/Dongjun/LevelUp/WBP_LevelUpItem").Class);
	RES_WRAPPER(ResWidget, WBP_ChrMenuMsg, Find_WBP(L"/Game/01_Source/Yuchan/UI/WBP_ChrMenuMsg").Class);
	RES_WRAPPER(ResWidget, WBP_FadeInOut, Find_WBP(L"/Game/01_Source/Yuchan/UI/WBP_FadeInOut").Class);
	RES_WRAPPER(ResWidget, WBP_Loading, Find_WBP(L"/Game/01_Source/Yuchan/UI/WBP_Loading").Class);

	RES_WRAPPER(ResWidget, WBP_FaemaUniqueAttack, Find_WBP(L"/Game/01_Source/Dongyun/UI/WBP_FaemaUniqueAttack").Class);
	RES_WRAPPER(ResWidget, WBP_FaemaChargeHUD, Find_WBP(L"/Game/01_Source/Dongyun/UI/WBP_FaemaChargeHUD").Class);

	
}

#undef RES_WRAPPER

inline std::pair<std::vector<UClass*>, std::unordered_map<UClass*, int>> Load_WBP() {
	static std::vector WBPList = {
		WBP_MainMenu.Get(),
		WBP_InGameWidget.Get(),
		WBP_Inv.Get(),
		WBP_ItemTT.Get(),
		WBP_LvUp.Get(),
		WBP_LvUpItem.Get(),
		WBP_ChrMenuMsg.Get(),
		WBP_FadeInOut.Get(),
		WBP_Loading.Get(),
		WBP_FaemaUniqueAttack.Get(),
		WBP_FaemaChargeHUD.Get()
	};
	static std::unordered_map<UClass*, int> WBPMap;
	if(WBPMap.empty()) {
		for (int i = 0; i < WBPList.size(); ++i) {
			WBPMap[WBPList[i]] = i;
		}
	}
	return { WBPList, WBPMap };
}

namespace Widget
{
inline int GetIdxOf(UUserWidget* WBP) {
	return Load_WBP().second[Cast<UClass>(WBP)];
}

inline UUserWidget* Load(const int Id) {
	return Cast<UUserWidget>(Load_WBP().first[Id]);
}
}


}
