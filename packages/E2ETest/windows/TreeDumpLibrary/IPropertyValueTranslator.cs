// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace TreeDumpLibrary
{
    public interface IPropertyValueTranslator
    {
        string PropertyValueToString(string propertyName, object propertyObject);
    }
}