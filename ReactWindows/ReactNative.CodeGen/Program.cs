// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.IO;
using System.Windows.Forms;

namespace ReactNative.CodeGen
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            var fileDialog = new OpenFileDialog();
            fileDialog.Multiselect = true;
            var result = fileDialog.ShowDialog();
            if (result != DialogResult.OK)
            {
                return;
            }

            foreach (var fileName in fileDialog.FileNames)
            {
                var json = JToken.Parse(File.ReadAllText(fileName));
                var generator = new ReactPackageGenerator(json);
                var packageFullName = json.Value<string>("name");
                var packageName = packageFullName.Substring(packageFullName.LastIndexOf(".") + 1);
                var wrapperPackageName = $"{packageName}Wrapper.cs";
                var outputFileName = Path.Combine(Path.GetDirectoryName(fileName), wrapperPackageName);
                File.WriteAllText(outputFileName, generator.TransformText());
            }
        }
    }
}
