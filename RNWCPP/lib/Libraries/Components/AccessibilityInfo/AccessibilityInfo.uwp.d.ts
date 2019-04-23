declare const AccessibilityInfo: {
    fetch: () => Promise<boolean>;
    addEventListener: (eventName: "change", handler: (enabled: boolean) => void) => void;
    removeEventListener: (eventName: "change", handler: Function) => void;
};
export = AccessibilityInfo;
