/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
export default class OverrideUpgrader {
    constructor(reactRepo, ovrRepo) {
        this.reactRepo = reactRepo;
        this.ovrRepo = ovrRepo;
    }
    getUpgraded(override, newVersion) {
        return __awaiter(this, void 0, void 0, function* () {
            if (override.type === 'copy') {
                return {
                    overrideFile: override.file,
                    hasConflicts: false,
                    content: yield this.reactRepo.getFileContents(override.baseFile, newVersion),
                };
            }
            const ovrContent = yield this.ovrRepo.getFileContents(override.file);
            const ovrAsPatch = yield this.reactRepo.generatePatch(override.baseFile, override.baseVersion, ovrContent);
            const patched = (yield this.reactRepo.getPatchedFile(override.baseFile, newVersion, ovrAsPatch))
                .replace(/<<<<<<< ours/g, '<<<<<<< Upstream')
                .replace(/>>>>>>> theirs/g, '>>>>>>> Override');
            return {
                overrideFile: override.file,
                hasConflicts: patched.includes('<<<<<<<'),
                content: patched,
            };
        });
    }
}
//# sourceMappingURL=OverrideUpgrader.js.map