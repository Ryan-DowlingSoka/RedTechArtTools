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

#include "RedTechArtToolsBlueprintLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameStateBase.h"

/** Output entire contents of a DataTable as a string */
FString URedTechArtToolsBlueprintLibrary::GetTableAsString(const UDataTable* DataTable, const bool bUseJsonObjectsForStructs, const bool bUseSimpleText)
{
	if(IsValid(DataTable))
	{
		EDataTableExportFlags Flags = EDataTableExportFlags::None;
		if(bUseJsonObjectsForStructs)
			Flags = EDataTableExportFlags::UseJsonObjectsForStructs;

		if(bUseSimpleText)
			Flags = Flags | EDataTableExportFlags::UseSimpleText;
		
		return DataTable->GetTableAsString(Flags);
	}
	return "";
}


/** Output entire contents of DataTable as CSV */
FString URedTechArtToolsBlueprintLibrary::GetTableAsCSV(const UDataTable* DataTable, const bool bUseJsonObjectsForStructs, const bool bUseSimpleText)
{
	if(IsValid(DataTable))
	{
		EDataTableExportFlags Flags = EDataTableExportFlags::None;
		if(bUseJsonObjectsForStructs)
			Flags = EDataTableExportFlags::UseJsonObjectsForStructs;

		if(bUseSimpleText)
			Flags = Flags | EDataTableExportFlags::UseSimpleText;
		
		return DataTable->GetTableAsString(Flags);
	}
	return "";
}


/** Output entire contents of DataTable as JSON */
FString URedTechArtToolsBlueprintLibrary::GetTableAsJSON(const UDataTable* DataTable, const bool bUseJsonObjectsForStructs, const bool bUseSimpleText)
{
	if(IsValid(DataTable))
	{
		EDataTableExportFlags Flags = EDataTableExportFlags::None;
		if(bUseJsonObjectsForStructs)
			Flags = EDataTableExportFlags::UseJsonObjectsForStructs;

		if(bUseSimpleText)
			Flags = Flags | EDataTableExportFlags::UseSimpleText;
		
		return DataTable->GetTableAsString(Flags);
	}
	return "";

	AInfo a;
	a.GetActorLocation();
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericLessThan(FString& A, FString& B)
{
	return A < B;
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericLessThanOrEqual(FString& A, FString& B)
{
	return A <= B;
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericGreaterThan(FString& A, FString& B)
{
	return A > B;
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericGreaterThanOrEqual(FString& A, FString& B)
{
	return A >= B;
}