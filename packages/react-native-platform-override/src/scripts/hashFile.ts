/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'nice-fs';
import {hashContent} from '../Hash';

(async function() {
  const content = await fs.readFile(process.argv[2]);
  console.log(hashContent(content));
})();
