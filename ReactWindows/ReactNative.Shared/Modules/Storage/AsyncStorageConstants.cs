using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Newtonsoft.Json.Linq;

namespace ReactNative.Modules.Storage
{
    class AsyncStorageConstants
    {
        public const string DirectoryName = "AsyncStorage";
        public const string FileExtension = ".data";

        public static readonly IDictionary<char, string> s_charToString = new Dictionary<char, string>
        {
            { '\\', "{bsl}" },
            { '/', "{fsl}" },
            { ':', "{col}" },
            { '*', "{asx}" },
            { '?', "{q}" },
            { '<', "{gt}" },
            { '>', "{lt}" },
            { '|', "{bar}" },
            { '\"', "{quo}" },
            { '.', "{dot}" },
            { '{', "{ocb}" },
            { '}', "{ccb}" },
        };

        public static readonly IDictionary<string, char> s_stringToChar = s_charToString.ToDictionary(kv => kv.Value, kv => kv.Key);

        public static readonly int s_maxReplace = s_charToString.Values.Select(s => s.Length).Max();

        public static string GetFileName(string key)
        {
            var sb = default(StringBuilder);
            for (var i = 0; i < key.Length; ++i)
            {
                var ch = key[i];
                var replacement = default(string);
                if (s_charToString.TryGetValue(ch, out replacement))
                {
                    if (sb == null)
                    {
                        sb = new StringBuilder();
                        sb.Append(key, 0, i);
                    }

                    sb.Append(replacement);
                }
                else if (sb != null)
                {
                    sb.Append(ch);
                }
            }

            if (sb == null)
            {
                return string.Concat(key, FileExtension);
            }
            else
            {
                sb.Append(AsyncStorageConstants.FileExtension);
                return sb.ToString();
            }
        }

        public static string GetKeyName(string fileName)
        {
            var length = fileName.Length - FileExtension.Length;
            var sb = default(StringBuilder);
            for (var i = 0; i < length; ++i)
            {
                var start = fileName[i];
                if (start == '{')
                {
                    var j = i;
                    while (j < length && (j - i) < s_maxReplace)
                    {
                        var end = fileName[++j];
                        if (end == '}')
                        {
                            break;
                        }
                    }

                    if (j < length && (j - i) < s_maxReplace)
                    {
                        var substring = fileName.Substring(i, j - i + 1);
                        var replacement = default(char);
                        if (s_stringToChar.TryGetValue(substring, out replacement))
                        {
                            if (sb == null)
                            {
                                sb = new StringBuilder();
                                sb.Append(fileName, 0, i);
                            }

                            sb.Append(replacement);
                            i = j;
                        }
                    }
                }
                else if (sb != null)
                {
                    sb.Append(start);
                }
            }

            return sb == null
                ? fileName.Substring(0, length)
                : sb.ToString();
        }

        public static void DeepMergeInto(JObject oldJson, JObject newJson)
        {
            foreach (var property in newJson)
            {
                var key = property.Key;
                var value = property.Value;
                var newInner = value as JObject;
                var oldInner = oldJson[key] as JObject;
                if (newInner != null && oldInner != null)
                {
                    DeepMergeInto(oldInner, newInner);
                }
                else
                {
                    oldJson[key] = value;
                }
            }
        }
    }
}
