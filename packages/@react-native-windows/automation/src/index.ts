/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import AutomationEnvironment from './AutomationEnvironment';
import {app} from './AutomationClient';

export {app} from './AutomationClient';
module.exports = Object.assign(AutomationEnvironment, {app});
