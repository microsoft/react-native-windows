"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.ButtonWin32 = void 0;
const react_1 = __importDefault(require("react"));
const ViewWin32_1 = require("../View/ViewWin32");
const TextWin32_1 = require("../Text/TextWin32");
var SelectState;
(function (SelectState) {
    SelectState[SelectState["NotSelected"] = 0] = "NotSelected";
    SelectState[SelectState["Selected"] = 1] = "Selected";
})(SelectState || (SelectState = {}));
/**
 * React-native <Button> control with additional Win32-specific functionality.
 */
class ButtonWin32 extends react_1.default.Component {
    constructor(props) {
        super(props);
        this._makeState = (select) => {
            return {
                accessibilityState: {
                    disabled: this.props.disabled,
                    selected: select === SelectState.Selected,
                },
            };
        };
        this._setState = (select) => {
            const state = this._makeState(select);
            this.setState(state);
        };
        this._onFocus = () => {
            this._setState(SelectState.Selected);
            if (this.props.onFocus) {
                this.props.onFocus();
            }
        };
        this._onBlur = () => {
            this._setState(SelectState.NotSelected);
            if (this.props.onBlur) {
                this.props.onBlur();
            }
        };
        this._onTouchEnd = (event) => {
            if (!this.props.disabled) {
                if (this.props.onPress) {
                    this.props.onPress(event);
                }
            }
            if (this.props.onTouchEnd) {
                this.props.onTouchEnd(event);
            }
        };
        this.state = this._makeState(SelectState.NotSelected);
    }
    render() {
        const viewProps = {
            accessible: true,
            accessibilityLabel: this.props.accessibilityLabel || this.props.title,
            accessibilityRole: 'button',
            accessibilityState: this.state.accessibilityState,
            focusable: true,
            onFocus: this._onFocus,
            onBlur: this._onBlur,
            onMouseEnter: this.props.onMouseEnter,
            onMouseLeave: this.props.onMouseLeave,
            onTouchStart: this.props.onTouchStart,
            onTouchEnd: this._onTouchEnd,
            testID: this.props.testID,
            style: this.props.style,
        };
        const textProps = {
            textStyle: 'None',
        };
        if (this.props.color) {
            textProps.style = { color: this.props.color };
        }
        return (react_1.default.createElement(ViewWin32_1.ViewWin32, Object.assign({}, viewProps),
            react_1.default.createElement(TextWin32_1.TextWin32, Object.assign({}, textProps), this.props.title)));
    }
}
exports.ButtonWin32 = ButtonWin32;
//# sourceMappingURL=ButtonWin32.js.map