// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.Text.RegularExpressions;
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
            StorageFolder storageFolder = ApplicationData.Current.LocalFolder;
            string fileName = GetOutputFile(dumpID);
            Debug.WriteLine($"output file = {storageFolder.Path + "\\" + fileName}");

            StorageFile outFile = await storageFolder.CreateFileAsync(fileName, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(outFile, outputJson);

            StorageFile masterFile = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync($@"Assets\{GetMasterFile(dumpID)}");

            string masterJson = await FileIO.ReadTextAsync(masterFile);

            if (!DumpsAreEqual(masterJson, outputJson))
            {
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
            return JsonComparesEqual(expected, actual, "root");
        }

        static double epsilon = 1.0;

        private static bool JsonComparesEqual(IJsonValue expected, IJsonValue actual, string keyName)
        {
            //const string anything = "‎<ANYTHING>";
            const string Anything = "<ANYTHING>";
            //System.Diagnostics.Debug.Assert(anything == Anything);
            if (expected.ValueType == JsonValueType.String && Regex.Replace(expected.GetString(), @"\p{C}", "") == Anything)
            {
                Debug.WriteLine($"Skipping ignored value: {actual.ValueType} {actual}");
                return true;
            }
            //Debug.WriteLine($"keyname: {keyName} {expected.ValueType} {actual.ValueType}");
            if (expected.ValueType != actual.ValueType)
            {
                Debug.WriteLine($"Expected {expected} got {actual}");
                return false;
            }
            switch (expected.ValueType)
            {
                case JsonValueType.String:
                    if (expected.GetString() != actual.GetString())
                    {
                        Debug.WriteLine($"string:Expected {expected.GetString()} got {actual.GetString()}");
                        return false;
                    }
                    return true;
                case JsonValueType.Number:
                    if (Math.Abs(expected.GetNumber() - actual.GetNumber()) > epsilon)
                    {
                        Debug.WriteLine($"number: {keyName} {expected.GetNumber()} {actual.GetNumber()}");
                        return false;
                    }
                    return true;
                case JsonValueType.Boolean:
                    return expected.GetBoolean() == actual.GetBoolean();
                case JsonValueType.Null:
                    return true;
                case JsonValueType.Array:
                    {
                        var ea = expected.GetArray();
                        var aa = actual.GetArray();
                        if (!JsonCompareArray(ea, aa))
                        {
                            Debug.WriteLine("in key " + keyName);
                            return false;
                        }
                        return true;
                    }
                case JsonValueType.Object:
                    {
                        if (!JsonCompareObject(expected.GetObject(), actual.GetObject()))
                        {
                            Debug.WriteLine("in key " + keyName);
                            return false;
                        }
                        return true;
                    }
                default:
                    throw new ArgumentException();
            }
        }

        private static bool JsonCompareArray(JsonArray ea, JsonArray aa)
        {
            if (ea.Count != aa.Count)
            {
                Debug.WriteLine($"Array count expected {ea.Count} got {aa.Count}");
                return false;
            }
            for (int i = 0; i < ea.Count; i++)
            {
                var _e = ea[i];
                var _a = aa[i];
                if (!JsonComparesEqual(_e, _a, "array element"))
                {
                    Debug.WriteLine($"Array element {i} expected {_e.ValueType} got {_a.ValueType}");
                    return false;
                }
            }
            return true;
        }

        private static bool JsonCompareObject(JsonObject eo, JsonObject ao)
        {
            var evisible = true;
            const string visibilityProperty = "Visibility";
            if (eo.Keys.Contains(visibilityProperty))
            {
                evisible = eo[visibilityProperty].GetString() == "Visible";
                eo.Remove("Visibility");
            }
            var avisible = true;
            if (ao.Keys.Contains(visibilityProperty))
            {
                avisible = ao[visibilityProperty].GetString() == "Visible";
                ao.Remove(visibilityProperty);
            }
            if (avisible != evisible) { return false; }
            if (avisible == false)
            {
                // both are collapsed (or nonexistent) so don't compare children
                ao.Remove("children");
                eo.Remove("children");
            }

            if (eo.Keys.Count != ao.Keys.Count)
            {
                Debug.WriteLine($"Expected {eo.Keys.Count} but got {ao.Keys.Count}");
                Debug.WriteLine(string.Join(", ", eo.Keys));
                Debug.WriteLine(string.Join(", ", ao.Keys));
                return false;
            }
            foreach (var key in eo.Keys)
            {
                JsonValue evalue = eo.GetNamedValue(key);
                JsonValue avalue = ao.GetNamedValue(key);
                if (!JsonComparesEqual(evalue, avalue, key))
                {
                    Debug.WriteLine($"Property {key} compared differently {evalue.ValueType} {avalue.ValueType}: expected {evalue} got {avalue}");
                    return false;
                }
            }
            return true;
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
