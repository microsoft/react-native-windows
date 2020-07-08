/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import {hashContent} from '../Hash';

(async function() {
  const content = await fs.promises.readFile(process.argv[2]);
  console.log(hashContent(content));
})();
