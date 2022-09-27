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

#include "CoreMinimal.h"
#include "EditorUtilityBlueprint.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EngineUtils.h"
#include "IRedTechArtToolsEditor.h"
#include "ISettingsModule.h"
#include "RedBPEnum.h"
#include "RedDeveloperSettings.h"
#include "RedEditorIconWidget.h"
#include "ToolMenus.h"
#include "Blutility/Classes/EditorUtilityToolMenu.h"
#include "Blutility/Classes/EditorUtilityWidget.h"
#include "Blutility/Public/EditorUtilitySubsystem.h"
#include "Customization/RedBPEnumCustomization.h"
#include "Customization/RedEditorIconPathCustomization.h"
#include "Interfaces/IMainFrameModule.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "RedTechArtTools"

class FRedTechArtToolsEditor final : public IRedTechArtToolsEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void MainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow) const;
	void HandleStartup() const;
};

void FRedTechArtToolsEditor::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FRedEditorIconPath::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRedEditorIconPathCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FRedBPEnum::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRedBPEnumCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	// In StartupModule
	IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
	if (MainFrameModule.IsWindowInitialized())
	{
		HandleStartup();
	}
	else
	{
		MainFrameModule.OnMainFrameCreationFinished().AddRaw(this, &FRedTechArtToolsEditor::MainFrameCreationFinished);
	}
}


void FRedTechArtToolsEditor::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
			"PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FRedEditorIconPath::StaticStruct()->GetFName());

		PropertyModule.NotifyCustomizationModuleChanged();
	}

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "RedTechArtTools");
	}
	
	IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
	MainFrameModule.OnMainFrameCreationFinished().RemoveAll(this);
}

void FRedTechArtToolsEditor::HandleStartup() const
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	TArray<UObject*> LoadedAssets;
	
	const URedDeveloperSettings* RedSettings = GetDefault<URedDeveloperSettings>();
	
	for( const FString& Path : RedSettings->AutoRegisterUtilityWidgetPaths)
	{
		EngineUtils::FindOrLoadAssetsByPath("/RedTechArtTools/EditorWidgets", LoadedAssets, EngineUtils::ATL_Regular);
	}
	
	for(UObject* Object : LoadedAssets)
	{
		if(UBlueprint* Blueprint = Cast<UBlueprint>(Object))
		{
			if(UClass* Class = Blueprint->GeneratedClass)
			{
				UObject* DefaultObject = Class->GetDefaultObject();
				if(UEditorUtilityToolMenuEntry* Entry = Cast<UEditorUtilityToolMenuEntry>(DefaultObject))
				{
					UToolMenus::Get()->AddMenuEntryObject(Entry);
				}
				else if(UEditorUtilityWidget* Widget = Cast<UEditorUtilityWidget>(DefaultObject))
				{
					if(Widget->ShouldAlwaysReregisterWithWindowsMenu())
					{
						if(UEditorUtilityWidgetBlueprint* WidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(Blueprint))
						{
							FName NewTabId;
							EditorUtilitySubsystem->RegisterTabAndGetID(WidgetBlueprint, NewTabId);
						}
					}
				}	
			}
		}
	}	
}

void FRedTechArtToolsEditor::MainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow) const
{
	HandleStartup();
	IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
	MainFrameModule.OnMainFrameCreationFinished().RemoveAll(this);
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRedTechArtToolsEditor, RedTechArtToolsEditor)