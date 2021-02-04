// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative.Managed;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace InteropTestModuleCS
{
    class Point
    {
        public Point(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public Point()
        {
        }

        public int x = 0;
        public int y = 0;
    };

    // Sample ReactModule

    [ReactModule]
    internal sealed class TestModuleCS
    {
        private string m_value;

        #region Constants

        [ReactConstant]
        public double NumberConstant => Math.PI;

        [ReactConstant]
        public string StringConstant => "Hello World";

        [ReactConstant]
        public Point PointConstant => new Point(10,20);

        [ReactConstant]
        public string[] ArrayConstant => new [] { "one", "two", "three" };

        [ReactConstant]
        public Point NullConstant => null;

        [ReactMethod]
        public int Add(int x, int y)
        {
            return x + y;
        }

        [ReactMethod]
        public async Task<int> AddAsync(int x, int y)
        {
            await Sleep();
            return x + y;
        }

        [ReactMethod]
        public string Concat(string x, string y)
        {
            return x + y;
        }

        [ReactMethod]
        public async Task<string> ConcatAsync(string x, string y)
        {
            await Sleep();
            return x + y;
        }

        [ReactMethod]
        public Point DeltaPoint(Point p1, Point p2)
        {
            return new Point(Math.Abs(p1.x - p2.x), Math.Abs(p1.y - p2.y));
        }
        
        [ReactMethod]
        public async Task<Point> DeltaPointAsync(Point p1, Point p2)
        {
            await Sleep();
            return DeltaPoint(p1, p2);
        }

        [ReactMethod]
        public bool IsNull(Point p1)
        {
            return p1 == null;
        }

        [ReactMethod]
        public async Task<bool> IsNullAsync(Point p1)
        {
            await Sleep();
            return IsNull(p1);
        }

        [ReactMethod]
        public void SetValue(string value)
        {
            m_value = value;
        }

        [ReactMethod]
        public async Task SetValueAsync(string value)
        {
            await Sleep();
            m_value = value;
        }

        [ReactMethod]
        public string GetValue()
        {
            return m_value;
        }
        
        [ReactMethod]
        public async Task<string> GetValueAsync()
        {
            await Sleep();
            return m_value;
        }

        [ReactMethod]
        public void Failure(Action<bool> _, Action<Dictionary<string, JSValue>> reject)
        {
            reject(new Dictionary<string, JSValue>()
            {
                { "code", "EF001"},
                { "message", "ExpectedFailure"},
            });
        }

        [ReactMethod]
        public async Task<bool> FailureAsync()
        {
            await Sleep();
            throw new InvalidOperationException("ExpectedFailure");
        }


        #endregion

        #region Events
        [ReactMethod]
        public void RaiseTestEvent(int x)
        {
            TestEvent?.Invoke(x);
        }

        [ReactEvent]
        public Action<int> TestEvent { get; set; }

        #endregion

        private Task Sleep()
        {
            return Task.Delay(TimeSpan.FromMilliseconds(100));
        }
    }
}
