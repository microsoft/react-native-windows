"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const Hash_1 = require("../Hash");
(async function () {
    const content = await fs.promises.readFile(process.argv[2]);
    console.log(Hash_1.hashContent(content));
})();
//# sourceMappingURL=hashFile.js.map