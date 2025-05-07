/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {AppRegistry} from 'react-native';
import TestInstructions from './TestInstructions';

// Ideally we'd import this file, rather than the local IntegrationTestsApp override
// It appears the upstream tests are broken
//import 'react-native-windows/IntegrationTests/IntegrationTestsApp';
import './IntegrationTestsApp';
import './tests';

AppRegistry.registerComponent('TestInstructions', () => TestInstructions);
