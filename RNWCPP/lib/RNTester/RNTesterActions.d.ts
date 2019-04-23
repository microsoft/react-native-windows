export interface IRNTesterBackAction {
    type: 'RNTesterBackAction';
}
export interface IRNTesterListAction {
    type: 'RNTesterListAction';
}
export interface IRNTesterExampleAction {
    type: 'RNTesterExampleAction';
    openExample: string;
}
export interface IRNTesterInitialAction {
    type: 'InitialAction';
}
export declare type RNTesterAction = IRNTesterBackAction | IRNTesterListAction | IRNTesterExampleAction | IRNTesterInitialAction;
