import MovingLight from './MovingLight';
import type {MovingLightHandle} from './MovingLight';

import DrawingIsland from './DrawingIsland';

import XamlHost from './XamlHostNativeComponent';

//import CalendarView from './FabricXamlCalendarViewNativeComponent';

const nativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
const ConditionallyIgnoredEventHandlers = require('react-native/Libraries/NativeComponent/ViewConfigIgnore').ConditionallyIgnoredEventHandlers;

/*
const __INTERNAL_VIEW_CONFIG = {
    uiViewClassName:'CalendarView',
    directEventTypes:{
        topSelectedDatesChanged:{
            registrationName:'onSelectedDatesChanged'}},
    validAttributes:Object.assign({label:true},
        ConditionallyIgnoredEventHandlers({onSelectedDatesChanged:true}))};
*/
const FabricXamlControl = nativeComponentRegistry.get('FabricXamlControl', () => ({
    uiViewClassName:'CalendarView',
    directEventTypes:{
        topSelectedDatesChanged:{
            registrationName:'onSelectedDatesChanged'}},
    validAttributes:Object.assign({label:true},
        ConditionallyIgnoredEventHandlers({onSelectedDatesChanged:true}))}));


const StackPanel = nativeComponentRegistry.get('StackPanel', () => ({
    uiViewClassName:'FX_StackPanel',
    directEventTypes:{},
    validAttributes:{label:true, content:true},
    }));

const Button = nativeComponentRegistry.get('Button', () => ({
    uiViewClassName:'FX_Button',
    directEventTypes:{},
    validAttributes:{label:true, content:true, text:true, davis:true},
    }));


//import FabricXamlControl from './FabricXamlControl';




export {
    DrawingIsland,
    MovingLight,
    MovingLightHandle,
    XamlHost,
    FabricXamlControl,
    StackPanel,
    Button
};
