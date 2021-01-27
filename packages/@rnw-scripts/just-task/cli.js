/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const path = require('path');

// Adding default config fallback to just just-task.js. If there is no build
// customization, there is no need to define a just configuraiotn file like
// just.config.ts or just-task.js etc. If users do define that file, that will
// superseded this default config file and users are encouraged to manually
// import this file and override only the tasks that need customization.
process.argv.push("--default-config", path.resolve(__dirname, "just-task.js"));

// Runs the main code of just-scripts
require("just-scripts/bin/just-scripts.js");