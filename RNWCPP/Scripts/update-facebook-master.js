// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

'use strict';

const execSync = require('child_process').execSync;

console.log(execSync("git remote add fb-github https://github.com/facebook/react-native.git"));
console.log(execSync("git fetch fb-github"));
console.log(execSync("git checkout -b facebook/master --track origin/facebook/master"));
console.log(execSync("git pull fb-github master"));
console.log(execSync("git push origin facebook/master"));
