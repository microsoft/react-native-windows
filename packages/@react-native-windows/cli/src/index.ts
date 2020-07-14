/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {autoLinkCommand} from './runWindows/utils/autolink';
import {runWindowsCommand} from './runWindows/runWindows';
import {dependencyConfigWindows} from './config/dependencyConfig';
import {projectConfigWindows} from './config/projectConfig';

export const commands = [autoLinkCommand, runWindowsCommand];
export const dependencyConfig = dependencyConfigWindows;
export const projectConfig = projectConfigWindows;
