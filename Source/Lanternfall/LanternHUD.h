#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LanternHUD.generated.h"
UCLASS()
class LANTERNFALL_API ALanternHUD : public AHUD
{
 GENERATED_BODY()
public: virtual void DrawHUD() override;
};

