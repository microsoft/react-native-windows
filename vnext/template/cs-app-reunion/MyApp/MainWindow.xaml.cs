using Microsoft.ReactNative;
using Microsoft.UI.Xaml;
using System;
using System.Runtime.InteropServices;
using Windows.Win32.Foundation;
using Windows.Win32.Graphics.Gdi;
using Windows.Win32.UI.Controls;
using Windows.Win32.UI.WindowsAndMessaging;
using static Windows.Win32.Constants;
using static Windows.Win32.PInvoke;

namespace {{ namespace }}
{

    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();

            Title = "React Native for Windows - Windows App SDK";

            HWND hwnd = (HWND)WinRT.Interop.WindowNative.GetWindowHandle(this);
            SetWindowSize(hwnd, 1050, 800);
            PlacementCenterWindowInMonitorWin32(hwnd);
          }

        private void SetWindowSize(HWND hwnd, int width, int height)
        {
          // Win32 uses pixels and WinUI 3 uses effective pixels, so you should apply the DPI scale factor
          uint dpi = GetDpiForWindow(hwnd);
          float scalingFactor = (float)dpi / 96;
          width = (int)(width * scalingFactor);
          height = (int)(height * scalingFactor);

          SetWindowPos(hwnd, default, 0, 0, width, height, SET_WINDOW_POS_FLAGS.SWP_NOMOVE | SET_WINDOW_POS_FLAGS.SWP_NOZORDER);
        }

        private void PlacementCenterWindowInMonitorWin32(HWND hwnd)
        {
          RECT windowMonitorRectToAdjust;
          GetWindowRect(hwnd, out windowMonitorRectToAdjust);
          ClipOrCenterRectToMonitorWin32(ref windowMonitorRectToAdjust);
          SetWindowPos(hwnd, default, windowMonitorRectToAdjust.left,
              windowMonitorRectToAdjust.top, 0, 0,
              SET_WINDOW_POS_FLAGS.SWP_NOSIZE |
              SET_WINDOW_POS_FLAGS.SWP_NOZORDER |
              SET_WINDOW_POS_FLAGS.SWP_NOACTIVATE);
        }

        private void ClipOrCenterRectToMonitorWin32(ref RECT adjustedWindowRect)
        {
          MONITORINFO mi = new MONITORINFO();
          mi.cbSize = (uint)Marshal.SizeOf<MONITORINFO>();
          GetMonitorInfo(MonitorFromRect(adjustedWindowRect, MONITOR_FROM_FLAGS.MONITOR_DEFAULTTONEAREST), ref mi);

          RECT rcWork = mi.rcWork;
          int w = adjustedWindowRect.right - adjustedWindowRect.left;
          int h = adjustedWindowRect.bottom - adjustedWindowRect.top;

          adjustedWindowRect.left = rcWork.left + (rcWork.right - rcWork.left - w) / 2;
          adjustedWindowRect.top = rcWork.top + (rcWork.bottom - rcWork.top - h) / 2;
          adjustedWindowRect.right = adjustedWindowRect.left + w;
          adjustedWindowRect.bottom = adjustedWindowRect.top + h;
        }
      }
    }

