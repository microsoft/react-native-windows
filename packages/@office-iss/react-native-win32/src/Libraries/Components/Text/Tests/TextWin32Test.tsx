'use strict';
import React from 'react'
import { Button, View } from 'react-native';
import { TextWin32 } from '../../Text/TextWin32';
import { ViewWin32 } from '../../View/ViewWin32';

// Disabling no-jsx-lambda so functional components are more convenient to use

const TextRunsTest: React.FC<{}> = () => {
  return (
    <View focusable>
      <TextWin32>
        <TextWin32>Text With </TextWin32>
        <TextWin32>Multiple Text </TextWin32>
        <TextWin32>Children [Runs]</TextWin32>
      </TextWin32>
    </View>
  );
};

const FocusableTextTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <TextWin32 focusable>This TextWin32 demonstrates focusable</TextWin32>
    </ViewWin32>
  );
};

const SelectableTextTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <TextWin32 selectable>This TextWin32 demonstrates selectable</TextWin32>
    </ViewWin32>
  );
};

const TextStyleTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <TextWin32 textStyle={'MediumBold'}>Normal Text Display</TextWin32>
    </ViewWin32>
  );
};

const TooltipTextTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <TextWin32 tooltip="Example Tooltip">This TextWin32 demonstrates a tooltip</TextWin32>
    </ViewWin32>
  );
};

const TextPromotionTest: React.FC<{}> = () => {
  enum FocusabilityState {
    NoFocus,
    Focusable,
    Selectable,
  }
  const [focusabilityState, setFocusabilityState] = React.useState(FocusabilityState.NoFocus);
  return (
    <ViewWin32>
      <Button
      title="Click to Promote Focusability"
      onPress={() => 
        {
          if (focusabilityState === FocusabilityState.NoFocus)
          {
            setFocusabilityState(FocusabilityState.Focusable);
          }
          else if (focusabilityState === FocusabilityState.Focusable)
          {
            setFocusabilityState(FocusabilityState.Selectable);
          }
        }}  />
      <TextWin32
        focusable={focusabilityState === FocusabilityState.Focusable}
        selectable={focusabilityState === FocusabilityState.Selectable}
      >
        This text is currently...
      </TextWin32>
      <TextWin32>
        {(focusabilityState === FocusabilityState.NoFocus) ? 'No Focusability' : 
         (focusabilityState === FocusabilityState.Focusable) ? 'Focusable' :
         'Selectable'}
      </TextWin32>
    </ViewWin32>
  );
};

const BlurringAndFocusingTextTest: React.FC<{}> = () => {
  const [isFocusableFocused, setIsFocusableFocused] = React.useState(false);
  const [isSelectableFocused, setIsSelectableFocused] = React.useState(false);
  return (
    <View>
      <TextWin32
        focusable
        onBlur={() => setIsFocusableFocused(false)}
        onFocus={() => setIsFocusableFocused(true)}>Focusable text is focused:</TextWin32>
        <TextWin32
        textStyle={isFocusableFocused ? 'MediumBold' : 'MediumStandard'}>{isFocusableFocused ? 'true' : 'false'}</TextWin32>
      <TextWin32
        selectable
        onBlur={() => setIsSelectableFocused(false)}
        onFocus={() => setIsSelectableFocused(true)}>Selectable text is focused:</TextWin32>
        <TextWin32
        textStyle={isSelectableFocused ? 'MediumBold' : 'MediumStandard'}>{isSelectableFocused ? 'true' : 'false'}</TextWin32>
    </View>
  );
};

export const title = 'TextWin32';
export const displayName = 'TextWin32';
export const description = 'TextWin32 Examples and Tests';
export const examples = [
    {
      title: 'Text Runs Example',
      description: 'text runs in action',
      render(): JSX.Element {
        return (<TextRunsTest />);
      },
    },
    {
      title: 'Focusable Example',
      description: 'focusable in action',
      render(): JSX.Element {
        return (<FocusableTextTest />);
      },
    },
    {
      title: 'Selectable Example',
      description: 'selectable in action',
      render(): JSX.Element {
        return (<SelectableTextTest />);
      },
    },
    {
      title: 'TextStyle Example',
      description: 'TextStyles in action',
      render(): JSX.Element {
        return (<TextStyleTest />);
      },
    },
    {
      title: 'Tooltip Example',
      description: 'tooltips in action',
      render(): JSX.Element {
        return (<TooltipTextTest />);
      },
    },
    {
      title: 'TextPromotion Example',
      description: 'dynamic increases in focusability in action',
      render(): JSX.Element {
        return (<TextPromotionTest />);
      },
    },
    {
      title: 'Focus and Blur Example',
      description: 'onFocus/onBlur in action',
      render(): JSX.Element {
        return (<BlurringAndFocusingTextTest />);
      },
    },
  ];