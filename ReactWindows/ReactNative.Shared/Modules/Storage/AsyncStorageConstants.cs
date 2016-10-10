using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace ReactNative.Modules.Storage
{
    class AsyncStorageConstants
    {
        public const string DirectoryName = "AsyncStorage";
        public const string FileExtension = ".data";

        public static readonly IDictionary<char, string> CharToString = new Dictionary<char, string>
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

        public static readonly IDictionary<string, char> StringToChar = CharToString.ToDictionary(kv => kv.Value, kv => kv.Key);

        public static readonly int MaxReplace = CharToString.Values.Select(s => s.Length).Max();
    }
}
