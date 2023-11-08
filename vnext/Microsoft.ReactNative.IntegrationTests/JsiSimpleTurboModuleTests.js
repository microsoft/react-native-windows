/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

// Make sure that we import something from react-native so that we build a complete bundle
// eslint-disable-next-line no-unused-vars
import {View} from 'react-native';

import {default as myTrivialTurboModule} from './NativeMyTrivialTurboModule';

myTrivialTurboModule.startFromJS();
