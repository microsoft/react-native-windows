/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {NativeEventEmitter} from 'react-native';

class AlertModule extends NativeEventEmitter {}

export const Alert = new AlertModule();
export default Alert;
