// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using Windows.System;

namespace PerfCompare
{
    public class PerfStats
    {
        public bool IsRunning
        {
            get
            {
                return Timer?.IsRunning ?? false;
            }
        }

        public long ElapsedTimeMs
        {
            get
            {
                return Timer?.ElapsedMilliseconds ?? 0;
            }
        }

        public string ElapsedTimeReadable
        {
            get
            {
                return GetTimeSpanReadable(Timer?.Elapsed ?? TimeSpan.Zero);
            }
        }

        public long TotalCommitInBytes
        {
            get
            {
                return EndCommitInBytes - StartCommitInBytes;
            }
        }

        public string TotalCommitReadable
        {
            get
            {
                return GetBytesReadable(TotalCommitInBytes);
            }
        }

        private long StartCommitInBytes = 0;
        private long EndCommitInBytes = 0;

        private Stopwatch Timer = null;

        public void Start()
        {
            GC.GetTotalMemory(true);
            EndCommitInBytes = StartCommitInBytes = (long)MemoryManager.AppMemoryUsage;
            Timer = Stopwatch.StartNew();
        }

        public void Stop(bool finalStop)
        {
            Timer.Stop();
            if (finalStop)
            {
                GC.GetTotalMemory(true);
                EndCommitInBytes = (long)MemoryManager.AppMemoryUsage;
            }
            else
            {
                Timer.Start();
            }
        }

        private static string GetTimeSpanReadable(TimeSpan timeSpan)
        {
            if (timeSpan.TotalMinutes >= 10.0)
            {
                return $"{timeSpan.TotalMinutes:0.00} m";
            }
            else if (timeSpan.TotalSeconds >= 10.0)
            {
                return $"{timeSpan.TotalSeconds:0.00} s";
            }

            return $"{timeSpan.TotalMilliseconds:0} ms";
        }

        // Returns the human-readable file size for an arbitrary, 64-bit file size 
        // The default format is "0.### XB", e.g. "4.2 KB" or "1.434 GB"
        // Adapted from https://www.somacon.com/p576.php
        private static string GetBytesReadable(long bytes)
        {
            // Get absolute value
            long absolute_bytes = (bytes < 0 ? -bytes : bytes);

            // Determine the suffix and readable value
            string suffix;
            double readable;
            if (absolute_bytes >= 0x1000000000000000) // Exabyte
            {
                suffix = "EB";
                readable = (bytes >> 50);
            }
            else if (absolute_bytes >= 0x4000000000000) // Petabyte
            {
                suffix = "PB";
                readable = (bytes >> 40);
            }
            else if (absolute_bytes >= 0x10000000000) // Terabyte
            {
                suffix = "TB";
                readable = (bytes >> 30);
            }
            else if (absolute_bytes >= 0x40000000) // Gigabyte
            {
                suffix = "GB";
                readable = (bytes >> 20);
            }
            else if (absolute_bytes >= 0x100000) // Megabyte
            {
                suffix = "MB";
                readable = (bytes >> 10);
            }
            else if (absolute_bytes >= 0x400) // Kilobyte
            {
                suffix = "KB";
                readable = bytes;
            }
            else
            {
                return bytes.ToString("0 B"); // Byte
            }

            // Divide by 1024 to get fractional value
            readable = (readable / 1024);

            // Return formatted number with suffix
            return readable.ToString("0.### ") + suffix;
        }
    }
}
