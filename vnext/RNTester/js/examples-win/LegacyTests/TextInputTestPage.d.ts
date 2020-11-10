/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
interface ITextInputTestPageState {
    log: string;
}
export declare class TextInputTestPage extends React.Component<{}, ITextInputTestPageState> {
    state: {
        log: string;
    };
    updateText: (text: string) => void;
    render(): JSX.Element;
}
export declare const displayName: (_undefined?: string | undefined) => void;
export declare const title = "<LegacyTextInputTest>";
export declare const description = "Legacy e2e test for TextInput";
export declare const examples: {
    render: () => JSX.Element;
}[];
export {};
//# sourceMappingURL=TextInputTestPage.d.ts.map