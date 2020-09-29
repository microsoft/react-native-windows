/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {AppRegistry} from 'react-native';
import TestInstructions from './TestInstructions';

require('react-native-windows/IntegrationTests/IntegrationTestsApp');
require('./tests');

AppRegistry.registerComponent('TestInstructions', () => TestInstructions);
