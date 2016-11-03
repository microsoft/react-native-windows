using ReactNative.DevSupport;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReactNative.DevSupport.Design
{
    internal sealed class StackFrameDesignData : IStackFrame
    {
        public StackFrameDesignData()
        {
        }

        public StackFrameDesignData(string method, string sourceInfo)
        {
            this.Method = method;
            this.SourceInfo = sourceInfo;
        }

        public StackFrameDesignData(string method, int line, int column, string filename, string sourceInfo)
        {
            this.Method = method;
            this.Line = line;
            this.Column = column;
            this.FileName = filename;
            this.SourceInfo = sourceInfo;
        }

        public string Method { get; }
        public int Line { get; }
        public int Column { get; }
        public string FileName { get; }
        public string SourceInfo { get; }
    }

    internal sealed class RedBoxDialogDesignData
    {
        public RedBoxDialogDesignData()
        {
            Message = @"Unable to download JS bundle. Did you forget to start the development server or connect your device?";

            StackTrace = new List<IStackFrame>
        {
            new StackFrameDesignData("MoveNext", "<unkown file>:0,0"),
            new StackFrameDesignData("ThrowForNoSuccess", "<unkown file>:0,0"),
            new StackFrameDesignData("MoveNext", "<unkown file>:0,0"),
            new StackFrameDesignData("ThrowForNoSuccess", "<unkown file>:0,0"),
            new StackFrameDesignData("MoveNext", "<unkown file>:0,0"),
            new StackFrameDesignData("ThrowForNoSuccess", "<unkown file>:0,0"),
            new StackFrameDesignData("ThrowForNoSuccess", "C:\\Program Files (x86)\\Windows Kits\\10\\DesignTime\\CommonConfiguration\\Neutral\\UAP\\10.0.10586.0\\Generic")
        };
        }

        public string Message { get; set; }

        public IList<IStackFrame> StackTrace { get; set; }
    }
}
