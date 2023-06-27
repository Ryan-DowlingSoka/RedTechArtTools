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
#include "BlueprintEditorLibrary.h"
#include "Blueprint/UserWidget.h"
#include "RedTechArtToolsBlueprintLibrary.generated.h"

class UDataTable;

#define LOCTEXT_NAMESPACE "RedTechArtToolsBlueprintLibrary"

/** Array of referencing actors as soft object ptrs.*/
USTRUCT(BlueprintType)
struct REDTECHARTTOOLSEDITOR_API FReferencingActors
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ReferencingActors)
	TSoftObjectPtr<AActor> ReferencedActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ReferencingActors)
	TArray<TSoftObjectPtr<AActor>> Actors;

	
	FReferencingActors(){}
	FReferencingActors(AActor* InReferencedActor, TArray<AActor*>& InActors)
	{
		ReferencedActor = InReferencedActor;
		for(TSoftObjectPtr<AActor> Actor : InActors)
		{
			Actors.Add(Actor);
		}
	}
};

/** Window options for the ShowWidgetDialog blueprint function.*/
USTRUCT(BlueprintType)
struct REDTECHARTTOOLSEDITOR_API FShowWidgetDialogOptions
{
	GENERATED_BODY()

	/** Size to the widget contents. When false, minimum size is used instead and the user can resize the widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShowWidgetDialogOptions)
	bool bAutoSized = true;

	/** Minimum size for the widget, necessary for when bAutoSized is false, but also will restrict the user from shrinking
	 *  the widget too small.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShowWidgetDialogOptions)
	FVector2D MinimumSize = FVector2D::ZeroVector;

	/** Creates default OK and CANCEL buttons. Without these the ShowDialog command will always return false.
	 *  If bUseDefaultButtons is false you will need to implement your own buttons, you can use CloseWidgetParentWindow
	 *  to close the window. Useful when you need more user button options.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShowWidgetDialogOptions)
	bool bUseDefaultButtons = true;
	
	/** When using bUseDefaultButtons the text to show in the "Ok" field. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShowWidgetDialogOptions)
	FText OkayButtonText = LOCTEXT("ShowWidgetDialogOptions_Ok", "Ok");

	/** When using bUseDefaultButtons the text to show in the "Cancel" field. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShowWidgetDialogOptions)
	FText CancelButtonText = LOCTEXT("ShowWidgetDialogOptions_Cancel", "Cancel");
	
	/** Sets the window to have a top-bar close button. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShowWidgetDialogOptions)
	bool bHasCloseButton = true;

};

/**
 *  Blueprint Library for useful misc Tech Art Tools.
 */
UCLASS()
class REDTECHARTTOOLSEDITOR_API URedTechArtToolsBlueprintLibrary : public UBlueprintEditorLibrary
{
	GENERATED_BODY()
public:
	/** Output entire contents of a DataTable as a string */
	UFUNCTION(BlueprintCallable, Category=DataTable)
	static FString GetTableAsString(const UDataTable* DataTable, bool bUseJsonObjectsForStructs = false,
	                                bool bUseSimpleText = false);

	/** Output entire contents of DataTable as CSV */
	UFUNCTION(BlueprintCallable, Category=DataTable)
	static FString GetTableAsCSV(const UDataTable* DataTable, bool bUseJsonObjectsForStructs = false,
	                             bool bUseSimpleText = false);

	/** Output entire contents of DataTable as JSON */
	UFUNCTION(BlueprintCallable, Category=DataTable)
	static FString GetTableAsJSON(const UDataTable* DataTable, bool bUseJsonObjectsForStructs = false,
	                              bool bUseSimpleText = false);

	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable, Category=String)
	static bool AlphaNumericLessThan(UPARAM(ref) FString& A, UPARAM(ref) FString& B);

	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable, Category=String)
	static bool AlphaNumericLessThanOrEqual(UPARAM(ref) FString& A, UPARAM(ref) FString& B);

	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable, Category=String)
	static bool AlphaNumericGreaterThan(UPARAM(ref) FString& A, UPARAM(ref) FString& B);

	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable, Category=String)
	static bool AlphaNumericGreaterThanOrEqual(UPARAM(ref) FString& A, UPARAM(ref) FString& B);

	/** Opens a modal content picker for a content directory. */
	UFUNCTION(BlueprintCallable, Category=EditorScripting)
	static FString PickContentPath(bool& bWasPathPicked, const FString DialogTitle = "Pick Path",
	                               const FString DefaultPath = "/Game");

	/** Converts a game relative (or long) package path to a local filesystem path. */
	UFUNCTION(BlueprintCallable, Category=EditorScripting)
	static void ConvertPackagePathToLocalPath(const FString PackagePath, bool& bOutSuccess, FString& OutLocalPath);

	/** Get array values from loaded config Array Values */
	UFUNCTION(BlueprintCallable, Category=EditorScripting)
	static void GetConfigArrayValue(const FString ConfigName, const FString SectionName, const FString KeyName,
	                                TArray<FString>& OutArrayValues);

	/** Set array values from loaded config Array Values. Flushes config file after setting values. */
	UFUNCTION(BlueprintCallable, Category=EditorScripting)
	static void SetConfigArrayValue(const FString ConfigName, const FString SectionName, const FString KeyName,
	                                UPARAM(ref) const TArray<FString>& InArrayValues);

	UFUNCTION(BlueprintCallable, Category=EditorScripting)
	static TMap<FString, FString> GetEditorSettableVariables(UObject* Object);

	/**
	 * Open a modal message box dialog containing a details view for inspecting / modifying a UObject. 
	 * @param Title 			The title of the created dialog window
	 * @param InOutWidget 		Widget instance to be viewed. Construct before hand with ConstructObject.
	 * @param Options			Additional options for presentation of the dialog.
	 * @return The result of the users decision, true=Ok, false=Cancel, or false if running in unattended mode.
	*/
	UFUNCTION(BlueprintCallable, Category = EditorScripting)
	static bool ShowWidgetDialog(const FText& Title, UUserWidget* InOutWidget, UPARAM(ref) const FShowWidgetDialogOptions& Options);

	/** Close the parent window for a given widget. */
	UFUNCTION(BlueprintCallable, Category=EditorScripting, meta=(DefaultToSelf="Widget"))
	static bool CloseWidgetParentWindow(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = EditorScripting)
	static UObject* GetDefaultObjectFromBlueprint(const UObject* Blueprint);

	/** A blueprint compatible wrapper for FBlueprintEditorUtils::GetActorReferenceMap(World, InClassesToIgnore, ReferencingActors); If using C++, use that instead.*/
	UFUNCTION(BlueprintCallable, Category = EditorScripting, meta=(WorldContext=WorldContextObject, AutoCreateRefTerm=InClassesToIgnore))
	static TMap<TSoftObjectPtr<AActor>, FReferencingActors> GetActorReferenceMap(UObject* WorldContextObject, UPARAM(ref) TArray<UClass*>& InClassesToIgnore);
	
};

#undef LOCTEXT_NAMESPACE
