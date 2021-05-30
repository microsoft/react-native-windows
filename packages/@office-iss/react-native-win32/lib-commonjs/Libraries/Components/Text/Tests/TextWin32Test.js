'use strict';
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.displayName = exports.title = void 0;
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const TextWin32_1 = require("../../Text/TextWin32");
const ViewWin32_1 = require("../../View/ViewWin32");
// Disabling no-jsx-lambda so functional components are more convenient to use
const TextRunsTest = () => {
    return (react_1.default.createElement(react_native_1.View, { focusable: true },
        react_1.default.createElement(TextWin32_1.TextWin32, null,
            react_1.default.createElement(TextWin32_1.TextWin32, null, "Text With "),
            react_1.default.createElement(TextWin32_1.TextWin32, null, "Multiple Text "),
            react_1.default.createElement(TextWin32_1.TextWin32, null, "Children [Runs]"))));
};
const FocusableTextTest = () => {
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, null,
        react_1.default.createElement(TextWin32_1.TextWin32, { focusable: true }, "This TextWin32 demonstrates focusable")));
};
const SelectableTextTest = () => {
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, null,
        react_1.default.createElement(TextWin32_1.TextWin32, { selectable: true }, "This TextWin32 demonstrates selectable")));
};
const TextStyleTest = () => {
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, null,
        react_1.default.createElement(TextWin32_1.TextWin32, { textStyle: 'MediumBold' }, "Normal Text Display")));
};
const TooltipTextTest = () => {
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, null,
        react_1.default.createElement(TextWin32_1.TextWin32, { tooltip: "Example Tooltip" }, "This TextWin32 demonstrates a tooltip")));
};
const TextPromotionTest = () => {
    let FocusabilityState;
    (function (FocusabilityState) {
        FocusabilityState[FocusabilityState["NoFocus"] = 0] = "NoFocus";
        FocusabilityState[FocusabilityState["Focusable"] = 1] = "Focusable";
        FocusabilityState[FocusabilityState["Selectable"] = 2] = "Selectable";
    })(FocusabilityState || (FocusabilityState = {}));
    const [focusabilityState, setFocusabilityState] = react_1.default.useState(FocusabilityState.NoFocus);
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, null,
        react_1.default.createElement(react_native_1.Button, { title: "Click to Promote Focusability", onPress: () => {
                if (focusabilityState === FocusabilityState.NoFocus) {
                    setFocusabilityState(FocusabilityState.Focusable);
                }
                else if (focusabilityState === FocusabilityState.Focusable) {
                    setFocusabilityState(FocusabilityState.Selectable);
                }
            } }),
        react_1.default.createElement(TextWin32_1.TextWin32, { focusable: focusabilityState === FocusabilityState.Focusable, selectable: focusabilityState === FocusabilityState.Selectable }, "This text is currently..."),
        react_1.default.createElement(TextWin32_1.TextWin32, null, (focusabilityState === FocusabilityState.NoFocus) ? 'No Focusability' :
            (focusabilityState === FocusabilityState.Focusable) ? 'Focusable' :
                'Selectable')));
};
const BlurringAndFocusingTextTest = () => {
    const [isFocusableFocused, setIsFocusableFocused] = react_1.default.useState(false);
    const [isSelectableFocused, setIsSelectableFocused] = react_1.default.useState(false);
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(TextWin32_1.TextWin32, { focusable: true, onBlur: () => setIsFocusableFocused(false), onFocus: () => setIsFocusableFocused(true) }, "Focusable text is focused:"),
        react_1.default.createElement(TextWin32_1.TextWin32, { textStyle: isFocusableFocused ? 'MediumBold' : 'MediumStandard' }, isFocusableFocused ? 'true' : 'false'),
        react_1.default.createElement(TextWin32_1.TextWin32, { selectable: true, onBlur: () => setIsSelectableFocused(false), onFocus: () => setIsSelectableFocused(true) }, "Selectable text is focused:"),
        react_1.default.createElement(TextWin32_1.TextWin32, { textStyle: isSelectableFocused ? 'MediumBold' : 'MediumStandard' }, isSelectableFocused ? 'true' : 'false')));
};
exports.title = 'TextWin32';
exports.displayName = 'TextWin32';
exports.description = 'TextWin32 Examples and Tests';
exports.examples = [
    {
        title: 'Text Runs Example',
        description: 'text runs in action',
        render() {
            return (react_1.default.createElement(TextRunsTest, null));
        },
    },
    {
        title: 'Focusable Example',
        description: 'focusable in action',
        render() {
            return (react_1.default.createElement(FocusableTextTest, null));
        },
    },
    {
        title: 'Selectable Example',
        description: 'selectable in action',
        render() {
            return (react_1.default.createElement(SelectableTextTest, null));
        },
    },
    {
        title: 'TextStyle Example',
        description: 'TextStyles in action',
        render() {
            return (react_1.default.createElement(TextStyleTest, null));
        },
    },
    {
        title: 'Tooltip Example',
        description: 'tooltips in action',
        render() {
            return (react_1.default.createElement(TooltipTextTest, null));
        },
    },
    {
        title: 'TextPromotion Example',
        description: 'dynamic increases in focusability in action',
        render() {
            return (react_1.default.createElement(TextPromotionTest, null));
        },
    },
    {
        title: 'Focus and Blur Example',
        description: 'onFocus/onBlur in action',
        render() {
            return (react_1.default.createElement(BlurringAndFocusingTextTest, null));
        },
    },
];
//# sourceMappingURL=TextWin32Test.js.map