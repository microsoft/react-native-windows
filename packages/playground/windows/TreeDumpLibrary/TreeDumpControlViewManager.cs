// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace TreeDumpLibrary
{
    public static class TreeDumpHelper
    {

        private static DependencyObject FindChildWithMatchingUIAID(DependencyObject element, string uiaId)
        {
            string automationId = (string)element.GetValue(Windows.UI.Xaml.Automation.AutomationProperties.AutomationIdProperty);
            if (automationId == uiaId)
            {
                return element;
            }
            int childrenCount = VisualTreeHelper.GetChildrenCount(element);
            for (int i = 0; i < childrenCount; i++)
            {
                var result = FindChildWithMatchingUIAID(VisualTreeHelper.GetChild(element, i), uiaId);
                if (result != null)
                {
                    return result;
                }
            }

            return null;
        }


        internal static async Task<bool> MatchDump(string outputJson, string dumpID)
        {
            Debug.WriteLine($"master file = {Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\Assets\\{GetMasterFile(dumpID)}");
            StorageFile masterFile = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync($@"Assets\{GetMasterFile(dumpID)}");

            string masterJson = await FileIO.ReadTextAsync(masterFile);

            if (!DumpsAreEqual(masterJson, outputJson))
            {
                StorageFolder storageFolder = ApplicationData.Current.LocalFolder;
                string fileName = GetOutputFile(dumpID);
                Debug.WriteLine($"output file = {storageFolder.Path + "\\" + fileName}");
                StorageFile outFile = await storageFolder.CreateFileAsync(fileName, CreationCollisionOption.ReplaceExisting);
                await FileIO.WriteTextAsync(outFile, outputJson);
                return false;
            }
            else
            {
                return true;
            }
        }

        public static bool DumpsAreEqual(string dumpExpectedText, string dumpText)
        {
            JsonValue expected = JsonValue.Parse(dumpExpectedText);
            JsonValue actual = JsonValue.Parse(dumpText);
            return JsonComparesEqual(expected, actual);
        }

        private static bool JsonComparesEqual(IJsonValue expected, IJsonValue actual)
        {
            if (expected.ValueType != actual.ValueType)
            {
                return false;
            }
            switch (expected.ValueType)
            {
                case JsonValueType.String:
                    if (expected.GetString() == actual.GetString())
                    {
                        return true;
                    }
                    else { Debug.WriteLine($"Expected {expected.GetString()} got {actual.GetString()}"); return false; }
                case JsonValueType.Number:
                    return expected.GetNumber() == actual.GetNumber();
                case JsonValueType.Boolean:
                    return expected.GetBoolean() == actual.GetBoolean();
                case JsonValueType.Null:
                    return true;
                case JsonValueType.Array:
                    {
                        var ea = expected.GetArray();
                        var aa = actual.GetArray();
                        if (ea.Count != aa.Count) { return false; }
                        for (uint i = 0; i < ea.Count; i++)
                        {
                            if (!JsonComparesEqual(ea[(int)i], aa[(int)i]))
                            {
                                return false;
                            }
                        }
                        return true;
                    }
                case JsonValueType.Object:
                    {
                        var eo = expected.GetObject();
                        var ao = actual.GetObject();
                        if (eo.Keys.Count != ao.Keys.Count) { return false; }
                        foreach (var key in eo.Keys)
                        {
                            if (!JsonComparesEqual(eo.GetNamedValue(key), ao.GetNamedValue(key)))
                            {
                                return false;
                            }
                        }
                        return true;
                    }
                default:
                    throw new ArgumentException();
            }
        }

        private static string GetOutputFile(string dumpID)
        {
            return "TreeDump\\" + dumpID + ".json";
        }

        private static string GetMasterFile(string dumpID)
        {
            return "TreeDump\\masters\\" + dumpID + ".json";
        }
    }
}
