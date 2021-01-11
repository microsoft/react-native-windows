/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// TS 3.8 type-only imports lead to parse errors with versions of prettier
// earlier than 2.0, which React Native uses as of 0.63. Isolate the type
// import to just this file and blacklist the file to get around it. Note that
// we cannot import the real module because it may be in a different location
// for older versions of RNW.
import type {generateWindows, GenerateOptions} from '@react-native-windows/cli';

// Current version
export type GenerateWindows = typeof generateWindows;

// Older versions of GenerateWindows were synchronous
export type GenerateWindows62 = (projectDir: string, name: string, ns: string, options: GenerateOptions) => void;
