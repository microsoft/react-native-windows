/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {execSync} from 'child_process';
import path from 'path';
import {powershell} from '../runWindows/utils/commandWithProgress';
import {HealthCheckList} from '../healthCheckList';

test('Verify list of health checks aligns with rnw-dependencies', async () => {
  const rnwDepScriptPath = path.join(
    path.dirname(
      require.resolve('react-native-windows/package.json', {
        paths: [process.cwd()],
      }),
    ),
    'Scripts/rnw-dependencies.ps1',
  );

  const rnwDeps = execSync(
    `${powershell} -ExecutionPolicy Unrestricted -NoProfile "${rnwDepScriptPath}" -NoPrompt -ListChecks`,
    {stdio: 'pipe'},
  );
  const deps = rnwDeps.toString().trim().split('\n');
  const rnwHelathChecks = deps.map(dep => {
    const match = /([^:]+): ([^:]+): (.*)/.exec(dep);
    if (!match) {
      throw new Error(`Unexpected output from ${rnwDepScriptPath}`);
    }
    const [, optional, id, name] = match;
    return [optional.trim() === 'Required', id, name];
  });

  expect(HealthCheckList).toEqual(rnwHelathChecks);
});
