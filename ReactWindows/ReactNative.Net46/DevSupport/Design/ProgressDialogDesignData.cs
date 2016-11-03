using ReactNative.DevSupport;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReactNative.DevSupport.Design
{
    internal sealed class ProgressDialogDesignData
    {
        public ProgressDialogDesignData()
        {
            Heading = @"Please wait...";

            Message = @"Fetching JavaScript bundle";
        }

        public string Heading { get; }

        public string Message { get; }
    }
}
