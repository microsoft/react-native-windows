import { RNTesterAction } from './RNTesterActions';
interface IRNTesterNavigationState {
    openExample?: string;
}
declare function rnTesterNavigationReducer(state: IRNTesterNavigationState | undefined, action: RNTesterAction): IRNTesterNavigationState;
export default rnTesterNavigationReducer;
