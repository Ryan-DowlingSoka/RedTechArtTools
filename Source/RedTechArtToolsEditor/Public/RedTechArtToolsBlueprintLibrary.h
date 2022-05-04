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
#include "RedTechArtToolsBlueprintLibrary.generated.h"

class UDataTable;
/**
 *  Blueprint Library for useful Tech Art Tools that are uncategorized.
 */
UCLASS()
class REDTECHARTTOOLSEDITOR_API URedTechArtToolsBlueprintLibrary : public UBlueprintEditorLibrary
{
	GENERATED_BODY()
public:

	/** Output entire contents of a DataTable as a string */
	static FString GetTableAsString(const UDataTable* DataTable, bool bUseJsonObjectsForStructs = false, bool bUseSimpleText = false);

	/** Output entire contents of DataTable as CSV */
	static FString GetTableAsCSV(const UDataTable* DataTable, bool bUseJsonObjectsForStructs = false, bool bUseSimpleText = false);

	/** Output entire contents of DataTable as JSON */
	static FString GetTableAsJSON(const UDataTable* DataTable, bool bUseJsonObjectsForStructs = false, bool bUseSimpleText = false);

	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable)
	static bool AlphaNumericLessThan(UPARAM(ref) FString& A, UPARAM(ref) FString& B);
	
	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable)
	static bool AlphaNumericLessThanOrEqual(UPARAM(ref) FString& A, UPARAM(ref) FString& B);

	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable)
	static bool AlphaNumericGreaterThan(UPARAM(ref) FString& A, UPARAM(ref) FString& B);

	/** Is A less than or equal to B alphanumerically. */
	UFUNCTION(BlueprintCallable)
	static bool AlphaNumericGreaterThanOrEqual(UPARAM(ref) FString& A, UPARAM(ref) FString& B);
};