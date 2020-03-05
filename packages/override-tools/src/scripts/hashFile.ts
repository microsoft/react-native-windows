/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import Manifest from '../Manifest';

(async function() {
  const content = await fs.promises.readFile(process.argv[2]);
  console.log(Manifest.hashContent(content.toString()));
})();
