// MIT License
//
// Copyright (c) 2022 Ryan DowlingSoka
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "CoreMinimal.h"
#include "DeveloperSettingsClasses.h"
#include "Engine/DeveloperSettings.h"
#include "RedDeveloperSettings.generated.h"

/**
 * Developer settings for the RED Tech Art Tools Plugin.
 */
UCLASS(config=RedTechArtTools, defaultconfig, meta=(DisplayName="RED Tech Art Tools"))
class REDTECHARTTOOLSEDITOR_API URedDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Editor Icon Widget")
	TArray<FString> EditorIconWidgetSearchPaths;

	virtual FName GetCategoryName() const override { return FName("Plugins"); }

	URedDeveloperSettings();
};
