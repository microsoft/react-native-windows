using System;
using System.Windows.Input;
using ReactNative.Bridge;

namespace ReactNative.Modules.Cursor
{
    class CursorModule : NativeModuleBase
    {
        public override string Name
        {
            get
            {
                return "Cursor";
            }
        }

        /// <summary>
        /// Set a mouse cursor
        /// </summary>
        /// <param name="cursor">The name of a system cursor (listed here: https://msdn.microsoft.com/en-us/library/system.windows.input.cursors(v=vs.110).aspx
        ///   ) or a file path to a custom cursor.</param>
        [ReactMethod]
        public void setCursor(string cursor)
        {
            var convertedCursor = Cursors.Arrow;
            switch (cursor.ToLowerInvariant())
            {
                case "appstarting":
                    convertedCursor = Cursors.AppStarting;
                    break;
                case "arrow":
                    convertedCursor = Cursors.Arrow;
                    break;
                case "arrowcd":
                    convertedCursor = Cursors.ArrowCD;
                    break;
                case "cross":
                    convertedCursor = Cursors.Cross;
                    break;
                case "hand":
                    convertedCursor = Cursors.Hand;
                    break;
                case "help":
                    convertedCursor = Cursors.Help;
                    break;
                case "ibeam":
                    convertedCursor = Cursors.IBeam;
                    break;
                case "no":
                    convertedCursor = Cursors.No;
                    break;
                case "none":
                    convertedCursor = Cursors.None;
                    break;
                case "pen":
                    convertedCursor = Cursors.Pen;
                    break;
                case "scrollall":
                    convertedCursor = Cursors.ScrollAll;
                    break;
                case "scrolle":
                    convertedCursor = Cursors.ScrollE;
                    break;
                case "scrolln":
                    convertedCursor = Cursors.ScrollN;
                    break;
                case "scrollne":
                    convertedCursor = Cursors.ScrollNE;
                    break;
                case "scrollns":
                    convertedCursor = Cursors.ScrollNS;
                    break;
                case "scrollnw":
                    convertedCursor = Cursors.ScrollNW;
                    break;
                case "scrolls":
                    convertedCursor = Cursors.ScrollS;
                    break;
                case "scrollse":
                    convertedCursor = Cursors.ScrollSE;
                    break;
                case "scrollsw":
                    convertedCursor = Cursors.ScrollSW;
                    break;
                case "scrollw":
                    convertedCursor = Cursors.ScrollW;
                    break;
                case "scrollwe":
                    convertedCursor = Cursors.ScrollWE;
                    break;
                case "sizeall":
                    convertedCursor = Cursors.SizeAll;
                    break;
                case "sizenesw":
                    convertedCursor = Cursors.SizeNESW;
                    break;
                case "sizens":
                    convertedCursor = Cursors.SizeNS;
                    break;
                case "sizenwse":
                    convertedCursor = Cursors.SizeNWSE;
                    break;
                case "sizewe":
                    convertedCursor = Cursors.SizeWE;
                    break;
                case "uparrow":
                    convertedCursor = Cursors.UpArrow;
                    break;
                case "wait":
                    convertedCursor = Cursors.Wait;
                    break;
                default:
                    convertedCursor = null;
                    break;
            }
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                if (convertedCursor != null)
                {
                    Mouse.OverrideCursor = convertedCursor;
                }
                else
                {
                    Mouse.OverrideCursor = new System.Windows.Input.Cursor(cursor, true);
                }
            });
        }
    }
}
