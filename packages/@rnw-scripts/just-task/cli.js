/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const path = require('path');

// Adding just.config.js here is a temporary workaround to not requiring a just.config file in each project.
// This will not allow users to specify a custom override, but I'm sending a PR to the just libary to 
// add an option for a defaultConfig option that is a fallback in case it is not specified in individual projects.
process.argv.push("--default-config", path.resolve(__dirname, "just-task.js"));

// Runs the main code of just-scripts
require("just-scripts/bin/just-scripts.js");