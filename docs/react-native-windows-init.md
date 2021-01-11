# React Native Windows initialization

This document describes how the template initialization works for `react-native-windows init`.

## Where is the code:
The main logic that determines the template values and which files to apply is function [`copyProjectTemplateAndReplace` in `packages/@react-native-windows/cli/src/generator-windows/index.ts`](../packages/@react-native-windows/cli/src/generator-windows/index.ts#L48).
The parameters for `init` is located in variable [`argv` in `packages/react-native-windows-init/src/Cli.ts`](../packages/react-native-windows-init/src/Cli.ts#L27)

The template files can be found in [`vnext/template`](../vnext/template)

## Debugging:
To debug the template, see [Contributing to the CLI](https://github.com/microsoft/react-native-windows/wiki/Contributing-to-the-CLI) on our wiki.

## Templating tech (Mustache)
PR [#5126](https://github.com/microsoft/react-native-windows/pull/5126) updated the template generation for react-native-windows from regex replace to use the mustache template library. 

### Benefits
This has the following benefits:
* We can use conditionals and `foreach` in the template allowing for better expressivity
* This expressivity shows that we don't have to construct XML fragments in JavaScript anymore to be inserted by `regex.replace`
* We don't need to have have a copy and edit for large differences like we had for experimental NuGet, it is now a single template for each language.
* We can now generate a cpp template that use both NuGet AND WinUI3.

### Research before setteling on Mustache
There are several templating schemes we could have chosen. We had a quick discussion and chose [mustache](https://www.npmjs.com/package/mustache) as it is a well-adopted, lightweight library dedicated for templating.
Alternatives considered:
 * js template expression: This would have required all files to become js programs that needed to be imported and evaluated. Since a lot of files contain data format or code, we would have likely had quite a few ugly escaping cases.
 * T4:  This would have required visual studio to be installed on the users machine and therefore only work on Windows. There were also some perf concerns and we would have to write and ship a standalone executable for the MSBuild tasks so we can call it from JavaScript.

### Quick mustache tutorial:
For proper docs see the [manual](http://mustache.github.io/mustache.5.html) of [mustache](http://mustache.github.io/)
But in short:
You run `mustache` via `const text = mustache.render(inputText, obj);` where obj is a regular JavaScript object. 
For example:
```js
{
   name: 'you',
   condition1: true,
   condition2: false,
   myIds: [
    { id: 'abc' },
    { id: 'xyz' }
   ]
}
```

We only use 4 mustache elements replacement patterns:
 * The template `Hello {{ name }}!` will result in `Hello you!`.
 * The template `Hello {{#condition1}}you!{{/condition1}}` will result in `Hello you!`
 * The template `Hello {{^condition2}}you!{{/condition2}}` will result in `Hello you!`
 * The template:
   ```
   MyIds:
   {{#myIds}}
   Id: {{ id }}
   {{/myIds}}
   ```
   will result in:
   ```
   MyIds
   Id: abc
   Id: xyz
   ```

## Future
In the future we hope this to be integrated with the react-native CLI templating scheme. At the moment there is only a very basic replacement scheme for a set of well-defined variables, so at the moment it is not suitable. When we get to that point we'll see if we can leverage that or contribute some improvements in the templating for `react-native-init`.
