"use strict";
/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 */
var __rest = (this && this.__rest) || function (s, e) {
    var t = {};
    for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p) && e.indexOf(p) < 0)
        t[p] = s[p];
    if (s != null && typeof Object.getOwnPropertySymbols === "function")
        for (var i = 0, p = Object.getOwnPropertySymbols(s); i < p.length; i++) {
            if (e.indexOf(p[i]) < 0 && Object.prototype.propertyIsEnumerable.call(s, p[i]))
                t[p[i]] = s[p[i]];
        }
    return t;
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.ViewWin32 = void 0;
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const setAndForwardRef = require('../../Utilities/setAndForwardRef');
exports.ViewWin32 = react_1.default.forwardRef((props, ref) => {
    /**
     * Check for raw text in the DOM.
     */
    if (__DEV__) {
        react_1.default.Children.toArray(props.children).forEach(item => {
            if (typeof item === 'string') {
                console.error(`Unexpected text node: ${item}. A text node cannot be a child of a <View>.`);
            }
        });
    }
    /**
     * Process accessibility refs into node handles after initial DOM render, before sent across the bridge.
     * useLayoutEffect will invalidate the render to assess the ref-based accessibility properties.
     */
    const [labeledByTarget, setLabeledByTarget] = react_1.default.useState(null);
    const [describedByTarget, setDescribedByTarget] = react_1.default.useState(null);
    const { accessibilityLabeledBy, accessibilityDescribedBy } = props, rest = __rest(props, ["accessibilityLabeledBy", "accessibilityDescribedBy"]);
    react_1.default.useLayoutEffect(() => {
        if (accessibilityLabeledBy !== undefined && accessibilityLabeledBy.current !== null) {
            setLabeledByTarget(react_native_1.findNodeHandle(accessibilityLabeledBy.current));
        }
        if (accessibilityDescribedBy !== undefined && accessibilityDescribedBy.current !== null) {
            setDescribedByTarget(react_native_1.findNodeHandle(accessibilityDescribedBy.current));
        }
    }, [accessibilityLabeledBy, accessibilityDescribedBy]);
    /**
     * Set up the forwarding ref to enable adding the focus method.
     */
    const focusRef = react_1.default.useRef();
    const setNativeRef = setAndForwardRef({
        getForwardedRef: () => ref,
        setLocalRef: localRef => {
            focusRef.current = localRef;
            /**
             * Add focus() as a callable function to the forwarded reference.
             */
            if (localRef) {
                localRef.focus = () => {
                    react_native_1.NativeModules.UIManager.dispatchViewManagerCommand(react_native_1.findNodeHandle(localRef), react_native_1.NativeModules.UIManager.getViewManagerConfig('RCTView').Commands.focus, null);
                };
            }
        },
    });
    return react_1.default.createElement(react_native_1.View, Object.assign({ ref: setNativeRef }, rest, ((labeledByTarget !== null) ? { accessibilityLabeledBy: labeledByTarget } : {}), ((describedByTarget !== null) ? { accessibilityDescribedBy: describedByTarget } : {})));
});
//# sourceMappingURL=ViewWin32.js.map