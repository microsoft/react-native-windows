/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

// Make sure that we import something from react-native so that we build a complete bundle
import {View} from 'react-native';

import {default as myTrivialTurboModule} from './NativeMyTrivialTurboModule';

myTrivialTurboModule.startFromJS();
