/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
export default class Version {
    private major;
    private minor;
    private build;
    private qfe;
    constructor(major: number, minor?: number, build?: number, qfe?: number);
    eq(other: Version): boolean;
    gt(other: Version): boolean;
    gte(other: Version): boolean;
    toString(): string;
    static compare(x: Version, y: Version): number;
    static fromString(str: string): Version;
    static tryParse(str: string): Version | null;
}
