//
// This file is generated with ITraceBuilder.Generated.tt
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Tracing
{
    /// <summary>
    /// An interface for adding properties to a trace operation.
    /// </summary>
    public partial interface ITraceBuilder
    {
        /// <summary>
        /// Adds a <see cref="Boolean"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Boolean"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, Boolean value);

        /// <summary>
        /// Adds a <see cref="Boolean"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Boolean"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Boolean[] value);


        /// <summary>
        /// Adds a <see cref="Double"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Double"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, Double value);

        /// <summary>
        /// Adds a <see cref="Double"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Double"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Double[] value);


        /// <summary>
        /// Adds a <see cref="Guid"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Guid"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, Guid value);

        /// <summary>
        /// Adds a <see cref="Guid"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Guid"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Guid[] value);


        /// <summary>
        /// Adds a <see cref="Int16"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Int16"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, Int16 value);

        /// <summary>
        /// Adds a <see cref="Int16"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Int16"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Int16[] value);


        /// <summary>
        /// Adds a <see cref="Int32"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Int32"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, Int32 value);

        /// <summary>
        /// Adds a <see cref="Int32"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Int32"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Int32[] value);


        /// <summary>
        /// Adds a <see cref="Int64"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Int64"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, Int64 value);

        /// <summary>
        /// Adds a <see cref="Int64"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Int64"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Int64[] value);


        /// <summary>
        /// Adds a <see cref="Single"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Single"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, Single value);

        /// <summary>
        /// Adds a <see cref="Single"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Single"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Single[] value);


        /// <summary>
        /// Adds a <see cref="String"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="String"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, String value);

        /// <summary>
        /// Adds a <see cref="String"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="String"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, String[] value);


        /// <summary>
        /// Adds a <see cref="TimeSpan"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="TimeSpan"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, TimeSpan value);

        /// <summary>
        /// Adds a <see cref="TimeSpan"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="TimeSpan"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, TimeSpan[] value);


        /// <summary>
        /// Adds a <see cref="UInt16"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="UInt16"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, UInt16 value);

        /// <summary>
        /// Adds a <see cref="UInt16"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="UInt16"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, UInt16[] value);


        /// <summary>
        /// Adds a <see cref="UInt32"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="UInt32"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, UInt32 value);

        /// <summary>
        /// Adds a <see cref="UInt32"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="UInt32"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, UInt32[] value);


        /// <summary>
        /// Adds a <see cref="UInt64"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="UInt64"/> value.</param>
        /// <returns>The trace builder instance.</returns>       
        ITraceBuilder With(string name, UInt64 value);

        /// <summary>
        /// Adds a <see cref="UInt64"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="UInt64"/> array value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, UInt64[] value);


        /// <summary>
        /// Adds a <see cref="Byte"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Byte"/> value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Byte value);

        /// <summary>
        /// Adds a <see cref="Byte"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Byte"/> array value.</param>
        /// <returns>The trace builder instance.</returns>     
        ITraceBuilder With(string name, Byte[] value);


        /// <summary>
        /// Adds a <see cref="Char"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Char"/> value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, Char value);

        /// <summary>
        /// Adds a <see cref="Char"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="Char"/> array value.</param>
        /// <returns>The trace builder instance.</returns>     
        ITraceBuilder With(string name, Char[] value);


        /// <summary>
        /// Adds a <see cref="DateTimeOffset"/> property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="DateTimeOffset"/> value.</param>
        /// <returns>The trace builder instance.</returns>      
        ITraceBuilder With(string name, DateTimeOffset value);

        /// <summary>
        /// Adds a <see cref="DateTimeOffset"/> array property to the trace.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The <see cref="DateTimeOffset"/> array value.</param>
        /// <returns>The trace builder instance.</returns>     
        ITraceBuilder With(string name, DateTimeOffset[] value);
    }
}
