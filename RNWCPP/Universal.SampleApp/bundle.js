(function(global) {
global.__DEV__=true;

global.__BUNDLE_START_TIME__=Date.now();
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {








'use strict';

global.require=_require;
global.__d=define;

var modules=Object.create(null);
if(__DEV__){
var verboseNamesToModuleIds=Object.create(null);
}

function define(moduleId,factory){
if(moduleId in modules){


return;
}
modules[moduleId]={
factory:factory,
hasError:false,
isInitialized:false,
exports:undefined};

if(__DEV__){

modules[moduleId].hot=createHotReloadingObject();



var verboseName=modules[moduleId].verboseName=arguments[2];
verboseNamesToModuleIds[verboseName]=moduleId;
}
}

function _require(moduleId){
var module=__DEV__?
modules[moduleId]||modules[verboseNamesToModuleIds[moduleId]]:
modules[moduleId];
return module&&module.isInitialized?
module.exports:
guardedLoadModule(moduleId,module);
}

var inGuard=false;
function guardedLoadModule(moduleId,module){
if(!inGuard&&global.ErrorUtils){
inGuard=true;
var returnValue=void 0;
try{
returnValue=loadModuleImplementation(moduleId,module);
}catch(e){
global.ErrorUtils.reportFatalError(e);
}
inGuard=false;
return returnValue;
}else{
return loadModuleImplementation(moduleId,module);
}
}

function loadModuleImplementation(moduleId,module){
var nativeRequire=global.nativeRequire;
if(!module&&nativeRequire){
nativeRequire(moduleId);
module=modules[moduleId];
}

if(__DEV__&&!module){

module=modules[verboseNamesToModuleIds[moduleId]];
if(module){
console.warn(
'Requiring module \''+moduleId+'\' by name is only supported for '+
'debugging purposes and will break in production');

}
}

if(!module){
throw unknownModuleError(moduleId);
}

if(module.hasError){
throw moduleThrewError(moduleId);
}






if(__DEV__){var
Systrace=_require.Systrace;
}




module.isInitialized=true;
var exports=module.exports={};var _module=
module;var factory=_module.factory;
try{
if(__DEV__){
Systrace.beginEvent('JS_require_'+(module.verboseName||moduleId));
}

var moduleObject={exports:exports};
if(__DEV__&&module.hot){
moduleObject.hot=module.hot;
}



factory(global,_require,moduleObject,exports);


if(!__DEV__){
module.factory=undefined;
}

if(__DEV__){
Systrace.endEvent();
}
return module.exports=moduleObject.exports;
}catch(e){
module.hasError=true;
module.isInitialized=false;
module.exports=undefined;
throw e;
}
}

function unknownModuleError(id){
var message='Requiring unknown module "'+id+'".';
if(__DEV__){
message+=
'If you are sure the module is there, try restarting the packager or running "npm install".';
}
return Error(message);
}

function moduleThrewError(id){
return Error('Requiring module "'+id+'", which threw an exception.');
}

if(__DEV__){var



createHotReloadingObject;(function(){_require.Systrace={beginEvent:function beginEvent(){},endEvent:function endEvent(){}};createHotReloadingObject=function createHotReloadingObject(){
var hot={
acceptCallback:null,
accept:function accept(callback){hot.acceptCallback=callback;}};

return hot;
};

var acceptAll=function acceptAll(dependentModules,inverseDependencies){
if(!dependentModules||dependentModules.length===0){
return true;
}

var notAccepted=dependentModules.filter(
function(module){return!accept(module,undefined,inverseDependencies);});

var parents=[];
for(var i=0;i<notAccepted.length;i++){

if(inverseDependencies[notAccepted[i]].length===0){
return false;
}

parents.pushAll(inverseDependencies[notAccepted[i]]);
}

return acceptAll(parents,inverseDependencies);
};

var accept=function accept(id,factory,inverseDependencies){
var mod=modules[id];

if(!mod){
define(id,factory);
return true;
}var

hot=mod.hot;
if(!hot){
console.warn(
'Cannot accept module because Hot Module Replacement '+
'API was not installed.');

return false;
}


if(factory){
mod.factory=factory;
}
mod.hasError=false;
mod.isInitialized=false;
_require(id);

if(hot.acceptCallback){
hot.acceptCallback();
return true;
}else{

if(!inverseDependencies){
throw new Error('Undefined `inverseDependencies`');
}


return acceptAll(inverseDependencies[id],inverseDependencies);
}
};

global.__accept=accept;})();
}
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {

















Object.assign=function(target,sources){
if(__DEV__){
if(target==null){
throw new TypeError('Object.assign target cannot be null or undefined');
}
if(typeof target!=='object'&&typeof target!=='function'){
throw new TypeError(
'In this environment the target of assign MUST be an object.'+
'This error is a performance optimization and not spec compliant.');

}
}

for(var nextIndex=1;nextIndex<arguments.length;nextIndex++){
var nextSource=arguments[nextIndex];
if(nextSource==null){
continue;
}

if(__DEV__){
if(typeof nextSource!=='object'&&
typeof nextSource!=='function'){
throw new TypeError(
'In this environment the sources for assign MUST be an object.'+
'This error is a performance optimization and not spec compliant.');

}
}





for(var key in nextSource){
if(__DEV__){
var hasOwnProperty=Object.prototype.hasOwnProperty;
if(!hasOwnProperty.call(nextSource,key)){
throw new TypeError(
'One of the sources for assign has an enumerable key on the '+
'prototype chain. This is an edge case that we do not support. '+
'This error is a performance optimization and not spec compliant.');

}
}
target[key]=nextSource[key];
}
}

return target;
};
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {

















var inspect=function(){























function inspect(obj,opts){
var ctx={
seen:[],
stylize:stylizeNoColor};

return formatValue(ctx,obj,opts.depth);
}

function stylizeNoColor(str,styleType){
return str;
}

function arrayToHash(array){
var hash={};

array.forEach(function(val,idx){
hash[val]=true;
});

return hash;
}


function formatValue(ctx,value,recurseTimes){

var primitive=formatPrimitive(ctx,value);
if(primitive){
return primitive;
}


var keys=Object.keys(value);
var visibleKeys=arrayToHash(keys);



if(isError(value)&&(
keys.indexOf('message')>=0||keys.indexOf('description')>=0)){
return formatError(value);
}


if(keys.length===0){
if(isFunction(value)){
var name=value.name?': '+value.name:'';
return ctx.stylize('[Function'+name+']','special');
}
if(isRegExp(value)){
return ctx.stylize(RegExp.prototype.toString.call(value),'regexp');
}
if(isDate(value)){
return ctx.stylize(Date.prototype.toString.call(value),'date');
}
if(isError(value)){
return formatError(value);
}
}

var base='',array=false,braces=['{','}'];


if(isArray(value)){
array=true;
braces=['[',']'];
}


if(isFunction(value)){
var n=value.name?': '+value.name:'';
base=' [Function'+n+']';
}


if(isRegExp(value)){
base=' '+RegExp.prototype.toString.call(value);
}


if(isDate(value)){
base=' '+Date.prototype.toUTCString.call(value);
}


if(isError(value)){
base=' '+formatError(value);
}

if(keys.length===0&&(!array||value.length==0)){
return braces[0]+base+braces[1];
}

if(recurseTimes<0){
if(isRegExp(value)){
return ctx.stylize(RegExp.prototype.toString.call(value),'regexp');
}else{
return ctx.stylize('[Object]','special');
}
}

ctx.seen.push(value);

var output;
if(array){
output=formatArray(ctx,value,recurseTimes,visibleKeys,keys);
}else{
output=keys.map(function(key){
return formatProperty(ctx,value,recurseTimes,visibleKeys,key,array);
});
}

ctx.seen.pop();

return reduceToSingleString(output,base,braces);
}


function formatPrimitive(ctx,value){
if(isUndefined(value))
return ctx.stylize('undefined','undefined');
if(isString(value)){
var simple='\''+JSON.stringify(value).replace(/^"|"$/g,'').
replace(/'/g,"\\'").
replace(/\\"/g,'"')+'\'';
return ctx.stylize(simple,'string');
}
if(isNumber(value))
return ctx.stylize(''+value,'number');
if(isBoolean(value))
return ctx.stylize(''+value,'boolean');

if(isNull(value))
return ctx.stylize('null','null');
}


function formatError(value){
return'['+Error.prototype.toString.call(value)+']';
}


function formatArray(ctx,value,recurseTimes,visibleKeys,keys){
var output=[];
for(var i=0,l=value.length;i<l;++i){
if(hasOwnProperty(value,String(i))){
output.push(formatProperty(ctx,value,recurseTimes,visibleKeys,
String(i),true));
}else{
output.push('');
}
}
keys.forEach(function(key){
if(!key.match(/^\d+$/)){
output.push(formatProperty(ctx,value,recurseTimes,visibleKeys,
key,true));
}
});
return output;
}


function formatProperty(ctx,value,recurseTimes,visibleKeys,key,array){
var name,str,desc;
desc=Object.getOwnPropertyDescriptor(value,key)||{value:value[key]};
if(desc.get){
if(desc.set){
str=ctx.stylize('[Getter/Setter]','special');
}else{
str=ctx.stylize('[Getter]','special');
}
}else{
if(desc.set){
str=ctx.stylize('[Setter]','special');
}
}
if(!hasOwnProperty(visibleKeys,key)){
name='['+key+']';
}
if(!str){
if(ctx.seen.indexOf(desc.value)<0){
if(isNull(recurseTimes)){
str=formatValue(ctx,desc.value,null);
}else{
str=formatValue(ctx,desc.value,recurseTimes-1);
}
if(str.indexOf('\n')>-1){
if(array){
str=str.split('\n').map(function(line){
return'  '+line;
}).join('\n').substr(2);
}else{
str='\n'+str.split('\n').map(function(line){
return'   '+line;
}).join('\n');
}
}
}else{
str=ctx.stylize('[Circular]','special');
}
}
if(isUndefined(name)){
if(array&&key.match(/^\d+$/)){
return str;
}
name=JSON.stringify(''+key);
if(name.match(/^"([a-zA-Z_][a-zA-Z_0-9]*)"$/)){
name=name.substr(1,name.length-2);
name=ctx.stylize(name,'name');
}else{
name=name.replace(/'/g,"\\'").
replace(/\\"/g,'"').
replace(/(^"|"$)/g,"'");
name=ctx.stylize(name,'string');
}
}

return name+': '+str;
}


function reduceToSingleString(output,base,braces){
var numLinesEst=0;
var length=output.reduce(function(prev,cur){
numLinesEst++;
if(cur.indexOf('\n')>=0)numLinesEst++;
return prev+cur.replace(/\u001b\[\d\d?m/g,'').length+1;
},0);

if(length>60){
return braces[0]+(
base===''?'':base+'\n ')+
' '+
output.join(',\n  ')+
' '+
braces[1];
}

return braces[0]+base+' '+output.join(', ')+' '+braces[1];
}




function isArray(ar){
return Array.isArray(ar);
}

function isBoolean(arg){
return typeof arg==='boolean';
}

function isNull(arg){
return arg===null;
}

function isNullOrUndefined(arg){
return arg==null;
}

function isNumber(arg){
return typeof arg==='number';
}

function isString(arg){
return typeof arg==='string';
}

function isSymbol(arg){
return typeof arg==='symbol';
}

function isUndefined(arg){
return arg===void 0;
}

function isRegExp(re){
return isObject(re)&&objectToString(re)==='[object RegExp]';
}

function isObject(arg){
return typeof arg==='object'&&arg!==null;
}

function isDate(d){
return isObject(d)&&objectToString(d)==='[object Date]';
}

function isError(e){
return isObject(e)&&(
objectToString(e)==='[object Error]'||e instanceof Error);
}

function isFunction(arg){
return typeof arg==='function';
}

function isPrimitive(arg){
return arg===null||
typeof arg==='boolean'||
typeof arg==='number'||
typeof arg==='string'||
typeof arg==='symbol'||
typeof arg==='undefined';
}

function objectToString(o){
return Object.prototype.toString.call(o);
}

function hasOwnProperty(obj,prop){
return Object.prototype.hasOwnProperty.call(obj,prop);
}

return inspect;
}();


var OBJECT_COLUMN_NAME='(index)';
var LOG_LEVELS={
trace:0,
info:1,
warn:2,
error:3};


function setupConsole(global){
if(!global.nativeLoggingHook){
return;
}

function getNativeLogFunction(level){
return function(){
var str;
if(arguments.length===1&&typeof arguments[0]==='string'){
str=arguments[0];
}else{
str=Array.prototype.map.call(arguments,function(arg){
return inspect(arg,{depth:10});
}).join(', ');
}

var logLevel=level;
if(str.slice(0,9)==='Warning: '&&logLevel>=LOG_LEVELS.error){



logLevel=LOG_LEVELS.warn;
}
global.nativeLoggingHook(str,logLevel);
};
}

var repeat=function repeat(element,n){
return Array.apply(null,Array(n)).map(function(){return element;});
};

function consoleTablePolyfill(rows){

if(!Array.isArray(rows)){
var data=rows;
rows=[];
for(var key in data){
if(data.hasOwnProperty(key)){
var row=data[key];
row[OBJECT_COLUMN_NAME]=key;
rows.push(row);
}
}
}
if(rows.length===0){
global.nativeLoggingHook('',LOG_LEVELS.info);
return;
}

var columns=Object.keys(rows[0]).sort();
var stringRows=[];
var columnWidths=[];



columns.forEach(function(k,i){
columnWidths[i]=k.length;
for(var j=0;j<rows.length;j++){
var cellStr=(rows[j][k]||'?').toString();
stringRows[j]=stringRows[j]||[];
stringRows[j][i]=cellStr;
columnWidths[i]=Math.max(columnWidths[i],cellStr.length);
}
});



var joinRow=function joinRow(row,space){
var cells=row.map(function(cell,i){
var extraSpaces=repeat(' ',columnWidths[i]-cell.length).join('');
return cell+extraSpaces;
});
space=space||' ';
return cells.join(space+'|'+space);
};

var separators=columnWidths.map(function(columnWidth){
return repeat('-',columnWidth).join('');
});
var separatorRow=joinRow(separators,'-');
var header=joinRow(columns);
var table=[header,separatorRow];

for(var i=0;i<rows.length;i++){
table.push(joinRow(stringRows[i]));
}





global.nativeLoggingHook('\n'+table.join('\n'),LOG_LEVELS.info);
}


var originalConsole=global.console;
var descriptor=Object.getOwnPropertyDescriptor(global,'console');
if(descriptor){
Object.defineProperty(global,'originalConsole',descriptor);
}

var console={
error:getNativeLogFunction(LOG_LEVELS.error),
info:getNativeLogFunction(LOG_LEVELS.info),
log:getNativeLogFunction(LOG_LEVELS.info),
warn:getNativeLogFunction(LOG_LEVELS.warn),
trace:getNativeLogFunction(LOG_LEVELS.trace),
debug:getNativeLogFunction(LOG_LEVELS.trace),
table:consoleTablePolyfill};



Object.defineProperty(global,'console',{
value:console,
configurable:descriptor?descriptor.configurable:true,
enumerable:descriptor?descriptor.enumerable:true,
writable:descriptor?descriptor.writable:true});





if(__DEV__&&originalConsole){
Object.keys(console).forEach(function(methodName){
var reactNativeMethod=console[methodName];
if(originalConsole[methodName]){
console[methodName]=function(){
originalConsole[methodName].apply(originalConsole,arguments);
reactNativeMethod.apply(console,arguments);
};
}
});
}
}

if(typeof module!=='undefined'){
module.exports=setupConsole;
}else{
setupConsole(global);
}
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {















var ErrorUtils={
_inGuard:0,
_globalHandler:null,
setGlobalHandler:function setGlobalHandler(fun){
ErrorUtils._globalHandler=fun;
},
getGlobalHandler:function getGlobalHandler(){
return ErrorUtils._globalHandler;
},
reportError:function reportError(error){
ErrorUtils._globalHandler&&ErrorUtils._globalHandler(error);
},
reportFatalError:function reportFatalError(error){
ErrorUtils._globalHandler&&ErrorUtils._globalHandler(error,true);
},
applyWithGuard:function applyWithGuard(fun,context,args){
try{
ErrorUtils._inGuard++;
return fun.apply(context,args);
}catch(e){
ErrorUtils.reportError(e);
}finally{
ErrorUtils._inGuard--;
}
},
applyWithGuardIfNeeded:function applyWithGuardIfNeeded(fun,context,args){
if(ErrorUtils.inGuard()){
return fun.apply(context,args);
}else{
ErrorUtils.applyWithGuard(fun,context,args);
}
},
inGuard:function inGuard(){
return ErrorUtils._inGuard;
},
guard:function guard(fun,name,context){
if(typeof fun!=='function'){
console.warn('A function must be passed to ErrorUtils.guard, got ',fun);
return null;
}
name=name||fun.name||'<generated guard>';
function guarded(){
return(
ErrorUtils.applyWithGuard(
fun,
context||this,
arguments,
null,
name));


}

return guarded;
}};

global.ErrorUtils=ErrorUtils;






function setupErrorGuard(){
var onError=function onError(e){
throw e;
};
global.ErrorUtils.setGlobalHandler(onError);
}

setupErrorGuard();
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {











if(Number.EPSILON===undefined){
Object.defineProperty(Number,'EPSILON',{
value:Math.pow(2,-52)});

}
if(Number.MAX_SAFE_INTEGER===undefined){
Object.defineProperty(Number,'MAX_SAFE_INTEGER',{
value:Math.pow(2,53)-1});

}
if(Number.MIN_SAFE_INTEGER===undefined){
Object.defineProperty(Number,'MIN_SAFE_INTEGER',{
value:-(Math.pow(2,53)-1)});

}
if(!Number.isNaN){(function(){

var globalIsNaN=global.isNaN;
Object.defineProperty(Number,'isNaN',{
configurable:true,
enumerable:false,
value:function isNaN(value){
return typeof value==='number'&&globalIsNaN(value);
},
writable:true});})();

}
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {











if(!String.prototype.startsWith){
String.prototype.startsWith=function(search){
'use strict';
if(this==null){
throw TypeError();
}
var string=String(this);
var pos=arguments.length>1?
Number(arguments[1])||0:0;
var start=Math.min(Math.max(pos,0),string.length);
return string.indexOf(String(search),pos)===start;
};
}

if(!String.prototype.endsWith){
String.prototype.endsWith=function(search){
'use strict';
if(this==null){
throw TypeError();
}
var string=String(this);
var stringLength=string.length;
var searchString=String(search);
var pos=arguments.length>1?
Number(arguments[1])||0:stringLength;
var end=Math.min(Math.max(pos,0),stringLength);
var start=end-searchString.length;
if(start<0){
return false;
}
return string.lastIndexOf(searchString,start)===start;
};
}

if(!String.prototype.repeat){
String.prototype.repeat=function(count){
'use strict';
if(this==null){
throw TypeError();
}
var string=String(this);
count=Number(count)||0;
if(count<0||count===Infinity){
throw RangeError();
}
if(count===1){
return string;
}
var result='';
while(count){
if(count&1){
result+=string;
}
if(count>>=1){
string+=string;
}
}
return result;
};
}

if(!String.prototype.includes){
String.prototype.includes=function(search,start){
'use strict';
if(typeof start!=='number'){
start=0;
}

if(start+search.length>this.length){
return false;
}else{
return this.indexOf(search,start)!==-1;
}
};
}
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {









function findIndex(predicate,context){
if(this==null){
throw new TypeError(
'Array.prototype.findIndex called on null or undefined');

}
if(typeof predicate!=='function'){
throw new TypeError('predicate must be a function');
}
var list=Object(this);
var length=list.length>>>0;
for(var i=0;i<length;i++){
if(predicate.call(context,list[i],i,list)){
return i;
}
}
return-1;
}

if(!Array.prototype.findIndex){
Object.defineProperty(Array.prototype,'findIndex',{
enumerable:false,
writable:true,
configurable:true,
value:findIndex});

}


if(!Array.prototype.find){
Object.defineProperty(Array.prototype,'find',{
enumerable:false,
writable:true,
configurable:true,
value:function value(predicate,context){
if(this==null){
throw new TypeError(
'Array.prototype.find called on null or undefined');

}
var index=findIndex.call(this,predicate,context);
return index===-1?undefined:this[index];
}});

}


if(!Array.prototype.includes){
Object.defineProperty(Array.prototype,'includes',{
enumerable:false,
writable:true,
configurable:true,
value:function value(searchElement){
var O=Object(this);
var len=parseInt(O.length)||0;
if(len===0){
return false;
}
var n=parseInt(arguments[1])||0;
var k;
if(n>=0){
k=n;
}else{
k=len+n;
if(k<0){
k=0;
}
}
var currentElement;
while(k<len){
currentElement=O[k];
if(searchElement===currentElement||
searchElement!==searchElement&&currentElement!==currentElement){
return true;
}
k++;
}
return false;
}});

}
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {












if(!Array.from){
Array.from=function(arrayLike){
if(arrayLike==null){
throw new TypeError('Object is null or undefined');
}


var mapFn=arguments[1];
var thisArg=arguments[2];

var C=this;
var items=Object(arrayLike);
var symbolIterator=typeof Symbol==='function'?typeof Symbol==='function'?
Symbol.iterator:'@@iterator':
'@@iterator';
var mapping=typeof mapFn==='function';
var usingIterator=typeof items[symbolIterator]==='function';
var key=0;
var ret;
var value;

if(usingIterator){
ret=typeof C==='function'?
new C():
[];
var it=items[symbolIterator]();
var next;

while(!(next=it.next()).done){
value=next.value;

if(mapping){
value=mapFn.call(thisArg,value,key);
}

ret[key]=value;
key+=1;
}

ret.length=key;
return ret;
}

var len=items.length;
if(isNaN(len)||len<0){
len=0;
}

ret=typeof C==='function'?
new C(len):
new Array(len);

while(key<len){
value=items[key];

if(mapping){
value=mapFn.call(thisArg,value,key);
}

ret[key]=value;

key+=1;
}

ret.length=key;
return ret;
};
}
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {






(function(){

var hasOwnProperty=Object.prototype.hasOwnProperty;






if(typeof Object.entries!=='function'){
Object.entries=function(object){

if(object==null){
throw new TypeError('Object.entries called on non-object');
}

var entries=[];
for(var key in object){
if(hasOwnProperty.call(object,key)){
entries.push([key,object[key]]);
}
}
return entries;
};
}






if(typeof Object.values!=='function'){
Object.values=function(object){

if(object==null){
throw new TypeError('Object.values called on non-object');
}

var values=[];
for(var key in object){
if(hasOwnProperty.call(object,key)){
values.push(object[key]);
}
}
return values;
};
}

})();
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
(function(global) {
















var babelHelpers=global.babelHelpers={};

babelHelpers.createRawReactElement=function(){
var REACT_ELEMENT_TYPE=typeof Symbol==="function"&&(typeof Symbol==="function"?Symbol.for:"@@for")&&(typeof Symbol==="function"?Symbol.for:"@@for")("react.element")||0xeac7;
return function createRawReactElement(type,key,props){
return{
$$typeof:REACT_ELEMENT_TYPE,
type:type,
key:key,
ref:null,
props:props,
_owner:null};

};
}();

babelHelpers.classCallCheck=function(instance,Constructor){
if(!(instance instanceof Constructor)){
throw new TypeError("Cannot call a class as a function");
}
};

babelHelpers.createClass=function(){
function defineProperties(target,props){
for(var i=0;i<props.length;i++){
var descriptor=props[i];
descriptor.enumerable=descriptor.enumerable||false;
descriptor.configurable=true;
if("value"in descriptor)descriptor.writable=true;
Object.defineProperty(target,descriptor.key,descriptor);
}
}

return function(Constructor,protoProps,staticProps){
if(protoProps)defineProperties(Constructor.prototype,protoProps);
if(staticProps)defineProperties(Constructor,staticProps);
return Constructor;
};
}();

babelHelpers.defineProperty=function(obj,key,value){
if(key in obj){
Object.defineProperty(obj,key,{
value:value,
enumerable:true,
configurable:true,
writable:true});

}else{
obj[key]=value;
}

return obj;
};

babelHelpers._extends=babelHelpers.extends=Object.assign||function(target){
for(var i=1;i<arguments.length;i++){
var source=arguments[i];

for(var key in source){
if(Object.prototype.hasOwnProperty.call(source,key)){
target[key]=source[key];
}
}
}

return target;
};

babelHelpers.get=function get(object,property,receiver){
if(object===null)object=Function.prototype;
var desc=Object.getOwnPropertyDescriptor(object,property);

if(desc===undefined){
var parent=Object.getPrototypeOf(object);

if(parent===null){
return undefined;
}else{
return get(parent,property,receiver);
}
}else if("value"in desc){
return desc.value;
}else{
var getter=desc.get;

if(getter===undefined){
return undefined;
}

return getter.call(receiver);
}
};

babelHelpers.inherits=function(subClass,superClass){
if(typeof superClass!=="function"&&superClass!==null){
throw new TypeError("Super expression must either be null or a function, not "+typeof superClass);
}

subClass.prototype=Object.create(superClass&&superClass.prototype,{
constructor:{
value:subClass,
enumerable:false,
writable:true,
configurable:true}});


if(superClass)Object.setPrototypeOf?Object.setPrototypeOf(subClass,superClass):subClass.__proto__=superClass;
};

babelHelpers.interopRequireDefault=function(obj){
return obj&&obj.__esModule?obj:{
default:obj};

};

babelHelpers.interopRequireWildcard=function(obj){
if(obj&&obj.__esModule){
return obj;
}else{
var newObj={};

if(obj!=null){
for(var key in obj){
if(Object.prototype.hasOwnProperty.call(obj,key))newObj[key]=obj[key];
}
}

newObj.default=obj;
return newObj;
}
};

babelHelpers.objectWithoutProperties=function(obj,keys){
var target={};

for(var i in obj){
if(keys.indexOf(i)>=0)continue;
if(!Object.prototype.hasOwnProperty.call(obj,i))continue;
target[i]=obj[i];
}

return target;
};

babelHelpers.possibleConstructorReturn=function(self,call){
if(!self){
throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
}

return call&&(typeof call==="object"||typeof call==="function")?call:self;
};

babelHelpers.slicedToArray=function(){
function sliceIterator(arr,i){
var _arr=[];
var _n=true;
var _d=false;
var _e=undefined;

try{
for(var _i=arr[typeof Symbol==="function"?Symbol.iterator:"@@iterator"](),_s;!(_n=(_s=_i.next()).done);_n=true){
_arr.push(_s.value);

if(i&&_arr.length===i)break;
}
}catch(err){
_d=true;
_e=err;
}finally{
try{
if(!_n&&_i["return"])_i["return"]();
}finally{
if(_d)throw _e;
}
}

return _arr;
}

return function(arr,i){
if(Array.isArray(arr)){
return arr;
}else if((typeof Symbol==="function"?Symbol.iterator:"@@iterator")in Object(arr)){
return sliceIterator(arr,i);
}else{
throw new TypeError("Invalid attempt to destructure non-iterable instance");
}
};
}();

babelHelpers.taggedTemplateLiteral=function(strings,raw){
return Object.freeze(Object.defineProperties(strings,{
raw:{
value:Object.freeze(raw)}}));


};

babelHelpers.toArray=function(arr){
return Array.isArray(arr)?arr:Array.from(arr);
};

babelHelpers.toConsumableArray=function(arr){
if(Array.isArray(arr)){
for(var i=0,arr2=Array(arr.length);i<arr.length;i++){arr2[i]=arr[i];}

return arr2;
}else{
return Array.from(arr);
}
};
})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
__d(0 /* AwesomeProject/index.node.js */, function(global, require, module, exports) {





global.process=global.process||{};
global.process.env=global.process.env||{};

if(!global.process.env.NODE_ENV)
{
global.process.env.NODE_ENV=__DEV__?'development':'production';
}

var setupDevtools=require(12 /* setupDevtools */);
setupDevtools();
}, "AwesomeProject/index.node.js");
__d(12 /* setupDevtools */, function(global, require, module, exports) {










'use strict';

var Platform=require(13 /* Platform */);
var NativeModules=require(14 /* NativeModules */);

function setupDevtools(){
var messageListeners=[];
var closeListeners=[];
var hostname='localhost';
if(Platform.OS==='android'&&NativeModules.AndroidConstants){
hostname=NativeModules.AndroidConstants.ServerHost.split(':')[0];
}
var ws=new window.WebSocket('ws://'+hostname+':8097/devtools');

var FOR_BACKEND={
resolveRNStyle:require(59 /* flattenStyle */),
wall:{
listen:function listen(fn){
messageListeners.push(fn);
},
onClose:function onClose(fn){
closeListeners.push(fn);
},
send:function send(data){
ws.send(JSON.stringify(data));
}}};


ws.onclose=handleClose;
ws.onerror=handleClose;
ws.onopen=function(){
tryToConnect();
};

var hasClosed=false;
function handleClose(){
if(!hasClosed){
hasClosed=true;
setTimeout(setupDevtools,2000);
closeListeners.forEach(function(fn){return fn();});
}
}

function tryToConnect(){
ws.send('attach:agent');
var _interval=setInterval(function(){return ws.send('attach:agent');},500);
ws.onmessage=function(evt){
if(evt.data.indexOf('eval:')===0){
clearInterval(_interval);
initialize(evt.data.slice('eval:'.length));
}
};
}

function initialize(text){
try{

eval(text);
}catch(e){
console.error('Failed to eval: '+e.message);
return;
}

var ReactNativeComponentTree=require(61 /* react/lib/ReactNativeComponentTree */);
window.__REACT_DEVTOOLS_GLOBAL_HOOK__.inject({
ComponentTree:{
getClosestInstanceFromNode:function getClosestInstanceFromNode(node){
return ReactNativeComponentTree.getClosestInstanceFromNode(node);
},
getNodeFromInstance:function getNodeFromInstance(inst){

while(inst._renderedComponent){
inst=inst._renderedComponent;
}
if(inst){
return ReactNativeComponentTree.getNodeFromInstance(inst);
}else{
return null;
}
}},

Mount:require(62 /* react/lib/ReactNativeMount */),
Reconciler:require(69 /* react/lib/ReactReconciler */)});

ws.onmessage=handleMessage;
}

function handleMessage(evt){


var data;
try{
data=JSON.parse(evt.data);
}catch(e){
return console.error('failed to parse json: '+evt.data);
}

if(data.$close||data.$error){
closeListeners.forEach(function(fn){return fn();});
window.__REACT_DEVTOOLS_GLOBAL_HOOK__.emit('shutdown');
tryToConnect();
return;
}
if(data.$open){
return;
}
messageListeners.forEach(function(fn){
try{
fn(data);
}catch(e){



console.log(data);
throw e;
}
});
}
}

module.exports=setupDevtools;
}, "setupDevtools");
__d(13 /* Platform */, function(global, require, module, exports) {











'use strict';

var Platform={
OS:'android',
get Version(){return require(14 /* NativeModules */).AndroidConstants.Version;},
select:function select(obj){return obj.android;}};


module.exports=Platform;
}, "Platform");
__d(14 /* NativeModules */, function(global, require, module, exports) {










'use strict';

var BatchedBridge=require(15 /* BatchedBridge */);
var RemoteModules=BatchedBridge.RemoteModules;

function normalizePrefix(moduleName){
return moduleName.replace(/^(RCT|RK)/,'');
}




Object.keys(RemoteModules).forEach(function(moduleName){
var strippedName=normalizePrefix(moduleName);
if(RemoteModules['RCT'+strippedName]&&RemoteModules['RK'+strippedName]){
throw new Error(
'Module cannot be registered as both RCT and RK: '+moduleName);

}
if(strippedName!==moduleName){
RemoteModules[strippedName]=RemoteModules[moduleName];
delete RemoteModules[moduleName];
}
});





var NativeModules={};
Object.keys(RemoteModules).forEach(function(moduleName){
Object.defineProperty(NativeModules,moduleName,{
configurable:true,
enumerable:true,
get:function get(){
var module=RemoteModules[moduleName];
if(module&&typeof module.moduleID==='number'&&global.nativeRequireModuleConfig){



var rawConfig=global.nativeRequireModuleConfig(moduleName);
var config=typeof rawConfig==='string'?JSON.parse(rawConfig):rawConfig;
module=config&&BatchedBridge.processModuleConfig(config,module.moduleID);
RemoteModules[moduleName]=module;
}
Object.defineProperty(NativeModules,moduleName,{
configurable:true,
enumerable:true,
value:module});

return module;
}});

});

module.exports=NativeModules;
}, "NativeModules");
__d(15 /* BatchedBridge */, function(global, require, module, exports) {









'use strict';

var MessageQueue=require(16 /* MessageQueue */);

var serializeNativeParams=typeof global.__fbBatchedBridgeSerializeNativeParams!=='undefined';

var BatchedBridge=new MessageQueue(
function(){return global.__fbBatchedBridgeConfig;},
serializeNativeParams);




var Systrace=require(17 /* Systrace */);
var JSTimersExecution=require(33 /* JSTimersExecution */);

BatchedBridge.registerCallableModule('Systrace',Systrace);
BatchedBridge.registerCallableModule('JSTimersExecution',JSTimersExecution);
BatchedBridge.registerCallableModule('HeapCapture',require(37 /* HeapCapture */));
BatchedBridge.registerCallableModule('SamplingProfiler',require(38 /* SamplingProfiler */));

if(__DEV__){
BatchedBridge.registerCallableModule('HMRClient',require(39 /* HMRClient */));
}







Object.defineProperty(global,'__fbBatchedBridge',{
configurable:true,
value:BatchedBridge});


module.exports=BatchedBridge;
}, "BatchedBridge");
__d(16 /* MessageQueue */, function(global, require, module, exports) {












'use strict';

var Systrace=require(17 /* Systrace */);
var ErrorUtils=require(32 /* ErrorUtils */);
var JSTimersExecution=require(33 /* JSTimersExecution */);

var invariant=require(22 /* fbjs/lib/invariant */);
var keyMirror=require(34 /* fbjs/lib/keyMirror */);
var stringifySafe=require(36 /* stringifySafe */);

var MODULE_IDS=0;
var METHOD_IDS=1;
var PARAMS=2;
var MIN_TIME_BETWEEN_FLUSHES_MS=5;

var TO_NATIVE=1;
var TO_JS=0;

var TRACE_TAG_REACT_APPS=1<<17;

var MethodTypes=keyMirror({
remote:null,
remoteAsync:null,
syncHook:null});


var guard=function guard(fn){
try{
fn();
}catch(error){
ErrorUtils.reportFatalError(error);
}
};var





MessageQueue=function(){
function MessageQueue(configProvider,serializeNativeParams){var _this=this;babelHelpers.classCallCheck(this,MessageQueue);
this._callableModules={};
this._queue=[[],[],[],0];
this._callbacks=[];
this._callbackID=0;
this._callID=0;
this._lastFlush=0;
this._eventLoopStartTime=new Date().getTime();
this._serializeNativeParams=serializeNativeParams;

if(__DEV__){
this._debugInfo={};
this._remoteModuleTable={};
this._remoteMethodTable={};
}

[
'invokeCallbackAndReturnFlushedQueue',
'callFunctionReturnFlushedQueue',
'callFunction',
'flushedQueue'].
forEach(function(fn){return _this[fn]=_this[fn].bind(_this);});

lazyProperty(this,'RemoteModules',function(){var _configProvider=
configProvider();var remoteModuleConfig=_configProvider.remoteModuleConfig;
var modulesConfig=_this._genModulesConfig(remoteModuleConfig);
var modules=_this._genModules(modulesConfig);

if(__DEV__){
_this._genLookupTables(
modulesConfig,_this._remoteModuleTable,_this._remoteMethodTable);

}

return modules;
});
}babelHelpers.createClass(MessageQueue,[{key:'callFunctionReturnFlushedQueue',value:function callFunctionReturnFlushedQueue(



















module,method,args){var _this2=this;
guard(function(){
_this2.__callFunction(module,method,args);
_this2.__callImmediates();
});

return this.flushedQueue();
}},{key:'callFunction',value:function callFunction(

module,method,args){var _this3=this;
var result=void 0;
guard(function(){
result=_this3.__callFunction(module,method,args);
_this3.__callImmediates();
});

return result;
}},{key:'invokeCallbackAndReturnFlushedQueue',value:function invokeCallbackAndReturnFlushedQueue(

cbID,args){var _this4=this;
guard(function(){
_this4.__invokeCallback(cbID,args);
_this4.__callImmediates();
});

return this.flushedQueue();
}},{key:'flushedQueue',value:function flushedQueue()

{
this.__callImmediates();

var queue=this._queue;
this._queue=[[],[],[],this._callID];
return queue[0].length?queue:null;
}},{key:'processModuleConfig',value:function processModuleConfig(

config,moduleID){
var info=this._genModule(config,moduleID);
this.RemoteModules[info.name]=info.module;
if(__DEV__){
this._genLookup(config,moduleID,this._remoteModuleTable,this._remoteMethodTable);
}
return info.module;
}},{key:'getEventLoopRunningTime',value:function getEventLoopRunningTime()

{
return new Date().getTime()-this._eventLoopStartTime;
}},{key:'__callImmediates',value:function __callImmediates()





{
Systrace.beginEvent('JSTimersExecution.callImmediates()');
guard(function(){return JSTimersExecution.callImmediates();});
Systrace.endEvent();
}},{key:'__nativeCall',value:function __nativeCall(

module,method,params,onFail,onSucc){
if(onFail||onSucc){
if(__DEV__){

this._callbackID>1<<5&&(
this._debugInfo[this._callbackID>>5]=null);
this._debugInfo[this._callbackID>>1]=[module,method];
}
onFail&&params.push(this._callbackID);
this._callbacks[this._callbackID++]=onFail;
onSucc&&params.push(this._callbackID);
this._callbacks[this._callbackID++]=onSucc;
}
var preparedParams=this._serializeNativeParams?JSON.stringify(params):params;

if(__DEV__){
global.nativeTraceBeginAsyncFlow&&
global.nativeTraceBeginAsyncFlow(TRACE_TAG_REACT_APPS,'native',this._callID);
}
this._callID++;

this._queue[MODULE_IDS].push(module);
this._queue[METHOD_IDS].push(method);
this._queue[PARAMS].push(preparedParams);

var now=new Date().getTime();
if(global.nativeFlushQueueImmediate&&
now-this._lastFlush>=MIN_TIME_BETWEEN_FLUSHES_MS){
global.nativeFlushQueueImmediate(this._queue);
this._queue=[[],[],[],this._callID];
this._lastFlush=now;
}
Systrace.counterEvent('pending_js_to_native_queue',this._queue[0].length);
if(__DEV__&&this.__spy&&isFinite(module)){
this.__spy(
{type:TO_NATIVE,
module:this._remoteModuleTable[module],
method:this._remoteMethodTable[module][method],
args:params});

}
}},{key:'__callFunction',value:function __callFunction(

module,method,args){
this._lastFlush=new Date().getTime();
this._eventLoopStartTime=this._lastFlush;
Systrace.beginEvent(module+'.'+method+'()');
if(__DEV__&&this.__spy){
this.__spy({type:TO_JS,module:module,method:method,args:args});
}
var moduleMethods=this._callableModules[module];
invariant(
!!moduleMethods,
'Module %s is not a registered callable module (calling %s)',
module,method);

invariant(
!!moduleMethods[method],
'Method %s does not exist on module %s',
method,module);

var result=moduleMethods[method].apply(moduleMethods,args);
Systrace.endEvent();
return result;
}},{key:'__invokeCallback',value:function __invokeCallback(

cbID,args){
this._lastFlush=new Date().getTime();
this._eventLoopStartTime=this._lastFlush;
var callback=this._callbacks[cbID];

if(__DEV__){
var debug=this._debugInfo[cbID>>1];
var _module=debug&&this._remoteModuleTable[debug[0]];
var method=debug&&this._remoteMethodTable[debug[0]][debug[1]];
if(!callback){
var errorMessage='Callback with id '+cbID+': '+_module+'.'+method+'() not found';
if(method){
errorMessage='The callback '+method+'() exists in module '+_module+', '+
'but only one callback may be registered to a function in a native module.';
}
invariant(
callback,
errorMessage);

}
var profileName=debug?'<callback for '+_module+'.'+method+'>':cbID;
if(callback&&this.__spy&&__DEV__){
this.__spy({type:TO_JS,module:null,method:profileName,args:args});
}
Systrace.beginEvent('MessageQueue.invokeCallback('+
profileName+', '+stringifySafe(args)+')');
}else{
if(!callback){
return;
}
}

this._callbacks[cbID&~1]=null;
this._callbacks[cbID|1]=null;
callback.apply(null,args);

if(__DEV__){
Systrace.endEvent();
}
}},{key:'_genModulesConfig',value:function _genModulesConfig(










modules){
if(Array.isArray(modules)){
return modules;
}else{
var moduleArray=[];
var moduleNames=Object.keys(modules);
for(var i=0,l=moduleNames.length;i<l;i++){
var moduleName=moduleNames[i];
var moduleConfig=modules[moduleName];
var _module2=[moduleName];
if(moduleConfig.constants){
_module2.push(moduleConfig.constants);
}
var methodsConfig=moduleConfig.methods;
if(methodsConfig){
var methods=[];
var asyncMethods=[];
var syncHooks=[];
var methodNames=Object.keys(methodsConfig);
for(var j=0,ll=methodNames.length;j<ll;j++){
var methodName=methodNames[j];
var methodConfig=methodsConfig[methodName];
methods[methodConfig.methodID]=methodName;
if(methodConfig.type===MethodTypes.remoteAsync){
asyncMethods.push(methodConfig.methodID);
}else if(methodConfig.type===MethodTypes.syncHook){
syncHooks.push(methodConfig.methodID);
}
}
if(methods.length){
_module2.push(methods);
_module2.push(asyncMethods);
_module2.push(syncHooks);
}
}
moduleArray[moduleConfig.moduleID]=_module2;
}
return moduleArray;
}
}},{key:'_genLookupTables',value:function _genLookupTables(

modulesConfig,moduleTable,methodTable){var _this5=this;
modulesConfig.forEach(function(config,moduleID){
_this5._genLookup(config,moduleID,moduleTable,methodTable);
});
}},{key:'_genLookup',value:function _genLookup(

config,moduleID,moduleTable,methodTable){
if(!config){
return;
}

var moduleName=void 0,methods=void 0;
if(moduleHasConstants(config)){var _config=babelHelpers.slicedToArray(
config,3);moduleName=_config[0];methods=_config[2];
}else{var _config2=babelHelpers.slicedToArray(
config,2);moduleName=_config2[0];methods=_config2[1];
}

moduleTable[moduleID]=moduleName;
methodTable[moduleID]=babelHelpers.extends({},methods);
}},{key:'_genModules',value:function _genModules(

remoteModules){var _this6=this;
var modules={};

remoteModules.forEach(function(config,moduleID){
var info=_this6._genModule(config,moduleID);
if(info){
modules[info.name]=info.module;
}
});

return modules;
}},{key:'_genModule',value:function _genModule(

config,moduleID){var _this7=this;
if(!config){
return null;
}

var moduleName=void 0,constants=void 0,methods=void 0,asyncMethods=void 0,syncHooks=void 0;
if(moduleHasConstants(config)){var _config3=babelHelpers.slicedToArray(
config,5);moduleName=_config3[0];constants=_config3[1];methods=_config3[2];asyncMethods=_config3[3];syncHooks=_config3[4];
}else{var _config4=babelHelpers.slicedToArray(
config,4);moduleName=_config4[0];methods=_config4[1];asyncMethods=_config4[2];syncHooks=_config4[3];
}

var module={};
methods&&methods.forEach(function(methodName,methodID){
var isAsync=asyncMethods&&arrayContains(asyncMethods,methodID);
var isSyncHook=syncHooks&&arrayContains(syncHooks,methodID);
invariant(!isAsync||!isSyncHook,'Cannot have a method that is both async and a sync hook');
var methodType=isAsync?MethodTypes.remoteAsync:
isSyncHook?MethodTypes.syncHook:
MethodTypes.remote;
module[methodName]=_this7._genMethod(moduleID,methodID,methodType);
});
babelHelpers.extends(module,constants);

if(!constants&&!methods&&!asyncMethods){
module.moduleID=moduleID;
}

return{name:moduleName,module:module};
}},{key:'_genMethod',value:function _genMethod(

module,method,type){
var fn=null;
var self=this;
if(type===MethodTypes.remoteAsync){
fn=function fn(){for(var _len=arguments.length,args=Array(_len),_key=0;_key<_len;_key++){args[_key]=arguments[_key];}
return new Promise(function(resolve,reject){
self.__nativeCall(
module,
method,
args,
function(data){
resolve(data);
},
function(errorData){
var error=createErrorFromErrorData(errorData);
reject(error);
});
});
};
}else if(type===MethodTypes.syncHook){
return function(){for(var _len2=arguments.length,args=Array(_len2),_key2=0;_key2<_len2;_key2++){args[_key2]=arguments[_key2];}
return global.nativeCallSyncHook(module,method,args);
};
}else{
fn=function fn(){for(var _len3=arguments.length,args=Array(_len3),_key3=0;_key3<_len3;_key3++){args[_key3]=arguments[_key3];}
var lastArg=args.length>0?args[args.length-1]:null;
var secondLastArg=args.length>1?args[args.length-2]:null;
var hasSuccCB=typeof lastArg==='function';
var hasErrorCB=typeof secondLastArg==='function';
hasErrorCB&&invariant(
hasSuccCB,
'Cannot have a non-function arg after a function arg.');

var numCBs=hasSuccCB+hasErrorCB;
var onSucc=hasSuccCB?lastArg:null;
var onFail=hasErrorCB?secondLastArg:null;
args=args.slice(0,args.length-numCBs);
return self.__nativeCall(module,method,args,onFail,onSucc);
};
}
fn.type=type;
return fn;
}},{key:'registerCallableModule',value:function registerCallableModule(

name,methods){
this._callableModules[name]=methods;
}}],[{key:'spy',value:function spy(spyOrToggle){if(spyOrToggle===true){MessageQueue.prototype.__spy=function(info){console.log((info.type==TO_JS?'N->JS':'JS->N')+' : '+(''+(info.module?info.module+'.':'')+info.method)+('('+JSON.stringify(info.args)+')'));};}else if(spyOrToggle===false){MessageQueue.prototype.__spy=null;}else{MessageQueue.prototype.__spy=spyOrToggle;}}}]);return MessageQueue;}();



function moduleHasConstants(moduleArray){
return!Array.isArray(moduleArray[1]);
}

function arrayContains(array,value){
return array.indexOf(value)!==-1;
}

function createErrorFromErrorData(errorData){var

message=

errorData.message;var extraErrorInfo=babelHelpers.objectWithoutProperties(errorData,['message']);
var error=new Error(message);
error.framesToPop=1;
return babelHelpers.extends(error,extraErrorInfo);
}

function lazyProperty(target,name,f){
Object.defineProperty(target,name,{
configurable:true,
enumerable:true,
get:function get(){
var value=f();
Object.defineProperty(target,name,{
configurable:true,
enumerable:true,
writeable:true,
value:value});

return value;
}});

}

module.exports=MessageQueue;
}, "MessageQueue");
__d(17 /* Systrace */, function(global, require, module, exports) {










'use strict';














var TRACE_TAG_REACT_APPS=1<<17;
var TRACE_TAG_JSC_CALLS=1<<27;

var _enabled=false;
var _asyncCookie=0;

var ReactSystraceDevtool=__DEV__?{
onBeforeMountComponent:function onBeforeMountComponent(debugID){
var displayName=require(18 /* react/lib/ReactComponentTreeDevtool */).getDisplayName(debugID);
Systrace.beginEvent('ReactReconciler.mountComponent('+displayName+')');
},
onMountComponent:function onMountComponent(debugID){
Systrace.endEvent();
},
onBeforeUpdateComponent:function onBeforeUpdateComponent(debugID){
var displayName=require(18 /* react/lib/ReactComponentTreeDevtool */).getDisplayName(debugID);
Systrace.beginEvent('ReactReconciler.updateComponent('+displayName+')');
},
onUpdateComponent:function onUpdateComponent(debugID){
Systrace.endEvent();
},
onBeforeUnmountComponent:function onBeforeUnmountComponent(debugID){
var displayName=require(18 /* react/lib/ReactComponentTreeDevtool */).getDisplayName(debugID);
Systrace.beginEvent('ReactReconciler.unmountComponent('+displayName+')');
},
onUnmountComponent:function onUnmountComponent(debugID){
Systrace.endEvent();
},
onBeginLifeCycleTimer:function onBeginLifeCycleTimer(debugID,timerType){
var displayName=require(18 /* react/lib/ReactComponentTreeDevtool */).getDisplayName(debugID);
Systrace.beginEvent(displayName+'.'+timerType+'()');
},
onEndLifeCycleTimer:function onEndLifeCycleTimer(debugID,timerType){
Systrace.endEvent();
}}:
null;

var Systrace={
setEnabled:function setEnabled(enabled){
if(_enabled!==enabled){
if(__DEV__){
if(enabled){
global.nativeTraceBeginLegacy&&global.nativeTraceBeginLegacy(TRACE_TAG_JSC_CALLS);
require(25 /* react/lib/ReactDebugTool */).addDevtool(ReactSystraceDevtool);
}else{
global.nativeTraceEndLegacy&&global.nativeTraceEndLegacy(TRACE_TAG_JSC_CALLS);
require(25 /* react/lib/ReactDebugTool */).removeDevtool(ReactSystraceDevtool);
}
}
_enabled=enabled;
}
},




beginEvent:function beginEvent(profileName,args){
if(_enabled){
profileName=typeof profileName==='function'?
profileName():profileName;
global.nativeTraceBeginSection(TRACE_TAG_REACT_APPS,profileName,args);
}
},

endEvent:function endEvent(){
if(_enabled){
global.nativeTraceEndSection(TRACE_TAG_REACT_APPS);
}
},






beginAsyncEvent:function beginAsyncEvent(profileName){
var cookie=_asyncCookie;
if(_enabled){
_asyncCookie++;
profileName=typeof profileName==='function'?
profileName():profileName;
global.nativeTraceBeginAsyncSection(TRACE_TAG_REACT_APPS,profileName,cookie,0);
}
return cookie;
},

endAsyncEvent:function endAsyncEvent(profileName,cookie){
if(_enabled){
profileName=typeof profileName==='function'?
profileName():profileName;
global.nativeTraceEndAsyncSection(TRACE_TAG_REACT_APPS,profileName,cookie,0);
}
},




counterEvent:function counterEvent(profileName,value){
if(_enabled){
profileName=typeof profileName==='function'?
profileName():profileName;
global.nativeTraceCounter&&
global.nativeTraceCounter(TRACE_TAG_REACT_APPS,profileName,value);
}
},





attachToRelayProfiler:function attachToRelayProfiler(relayProfiler){
relayProfiler.attachProfileHandler('*',function(name){
var cookie=Systrace.beginAsyncEvent(name);
return function(){
Systrace.endAsyncEvent(name,cookie);
};
});

relayProfiler.attachAggregateHandler('*',function(name,callback){
Systrace.beginEvent(name);
callback();
Systrace.endEvent();
});
},



swizzleJSON:function swizzleJSON(){
Systrace.measureMethods(JSON,'JSON',[
'parse',
'stringify']);

},









measureMethods:function measureMethods(object,objectName,methodNames){
if(!__DEV__){
return;
}

methodNames.forEach(function(methodName){
object[methodName]=Systrace.measure(
objectName,
methodName,
object[methodName]);

});
},










measure:function measure(objName,fnName,func){
if(!__DEV__){
return func;
}

var profileName=objName+'.'+fnName;
return function(){
if(!_enabled){
return func.apply(this,arguments);
}

Systrace.beginEvent(profileName);
var ret=func.apply(this,arguments);
Systrace.endEvent();
return ret;
};
}};


if(__DEV__){




require.Systrace=Systrace;
}

module.exports=Systrace;
}, "Systrace");
__d(18 /* react/lib/ReactComponentTreeDevtool.js */, function(global, require, module, exports) {









'use strict';



module.exports=require(19 /* ./ReactComponentTreeHook */);
}, "react/lib/ReactComponentTreeDevtool.js");
__d(19 /* react/lib/ReactComponentTreeHook.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var ReactCurrentOwner=require(21 /* ./ReactCurrentOwner */);

var invariant=require(22 /* fbjs/lib/invariant */);
var warning=require(23 /* fbjs/lib/warning */);

function isNative(fn){

var funcToString=Function.prototype.toString;
var hasOwnProperty=Object.prototype.hasOwnProperty;
var reIsNative=RegExp('^'+funcToString.

call(hasOwnProperty).

replace(/[\\^$.*+?()[\]{}|]/g,'\\$&').

replace(/hasOwnProperty|(function).*?(?=\\\()| for .+?(?=\\\])/g,'$1.*?')+'$');
try{
var source=funcToString.call(fn);
return reIsNative.test(source);
}catch(err){
return false;
}
}

var canUseCollections=

typeof Array.from==='function'&&

typeof Map==='function'&&isNative(Map)&&

Map.prototype!=null&&typeof Map.prototype.keys==='function'&&isNative(Map.prototype.keys)&&

typeof Set==='function'&&isNative(Set)&&

Set.prototype!=null&&typeof Set.prototype.keys==='function'&&isNative(Set.prototype.keys);

var itemMap;
var rootIDSet;

var itemByKey;
var rootByKey;

if(canUseCollections){
itemMap=new Map();
rootIDSet=new Set();
}else{
itemByKey={};
rootByKey={};
}

var unmountedIDs=[];



function getKeyFromID(id){
return'.'+id;
}
function getIDFromKey(key){
return parseInt(key.substr(1),10);
}

function get(id){
if(canUseCollections){
return itemMap.get(id);
}else{
var key=getKeyFromID(id);
return itemByKey[key];
}
}

function remove(id){
if(canUseCollections){
itemMap['delete'](id);
}else{
var key=getKeyFromID(id);
delete itemByKey[key];
}
}

function create(id,element,parentID){
var item={
element:element,
parentID:parentID,
text:null,
childIDs:[],
isMounted:false,
updateCount:0};


if(canUseCollections){
itemMap.set(id,item);
}else{
var key=getKeyFromID(id);
itemByKey[key]=item;
}
}

function addRoot(id){
if(canUseCollections){
rootIDSet.add(id);
}else{
var key=getKeyFromID(id);
rootByKey[key]=true;
}
}

function removeRoot(id){
if(canUseCollections){
rootIDSet['delete'](id);
}else{
var key=getKeyFromID(id);
delete rootByKey[key];
}
}

function getRegisteredIDs(){
if(canUseCollections){
return Array.from(itemMap.keys());
}else{
return Object.keys(itemByKey).map(getIDFromKey);
}
}

function getRootIDs(){
if(canUseCollections){
return Array.from(rootIDSet.keys());
}else{
return Object.keys(rootByKey).map(getIDFromKey);
}
}

function purgeDeep(id){
var item=get(id);
if(item){
var childIDs=item.childIDs;

remove(id);
childIDs.forEach(purgeDeep);
}
}

function describeComponentFrame(name,source,ownerName){
return'\n    in '+name+(source?' (at '+source.fileName.replace(/^.*[\\\/]/,'')+':'+source.lineNumber+')':ownerName?' (created by '+ownerName+')':'');
}

function _getDisplayName(element){
if(element==null){
return'#empty';
}else if(typeof element==='string'||typeof element==='number'){
return'#text';
}else if(typeof element.type==='string'){
return element.type;
}else{
return element.type.displayName||element.type.name||'Unknown';
}
}

function describeID(id){
var name=ReactComponentTreeHook.getDisplayName(id);
var element=ReactComponentTreeHook.getElement(id);
var ownerID=ReactComponentTreeHook.getOwnerID(id);
var ownerName;
if(ownerID){
ownerName=ReactComponentTreeHook.getDisplayName(ownerID);
}
process.env.NODE_ENV!=='production'?warning(element,'ReactComponentTreeHook: Missing React element for debugID %s when '+'building stack',id):void 0;
return describeComponentFrame(name,element&&element._source,ownerName);
}

var ReactComponentTreeHook={
onSetChildren:function onSetChildren(id,nextChildIDs){
var item=get(id);
item.childIDs=nextChildIDs;

for(var i=0;i<nextChildIDs.length;i++){
var nextChildID=nextChildIDs[i];
var nextChild=get(nextChildID);
!nextChild?process.env.NODE_ENV!=='production'?invariant(false,'Expected hook events to fire for the child before its parent includes it in onSetChildren().'):_prodInvariant('140'):void 0;
!(nextChild.childIDs!=null||typeof nextChild.element!=='object'||nextChild.element==null)?process.env.NODE_ENV!=='production'?invariant(false,'Expected onSetChildren() to fire for a container child before its parent includes it in onSetChildren().'):_prodInvariant('141'):void 0;
!nextChild.isMounted?process.env.NODE_ENV!=='production'?invariant(false,'Expected onMountComponent() to fire for the child before its parent includes it in onSetChildren().'):_prodInvariant('71'):void 0;
if(nextChild.parentID==null){
nextChild.parentID=id;



}
!(nextChild.parentID===id)?process.env.NODE_ENV!=='production'?invariant(false,'Expected onBeforeMountComponent() parent and onSetChildren() to be consistent (%s has parents %s and %s).',nextChildID,nextChild.parentID,id):_prodInvariant('142',nextChildID,nextChild.parentID,id):void 0;
}
},
onBeforeMountComponent:function onBeforeMountComponent(id,element,parentID){
create(id,element,parentID);
},
onBeforeUpdateComponent:function onBeforeUpdateComponent(id,element){
var item=get(id);
if(!item||!item.isMounted){


return;
}
item.element=element;
},
onMountComponent:function onMountComponent(id){
var item=get(id);
item.isMounted=true;
var isRoot=item.parentID===0;
if(isRoot){
addRoot(id);
}
},
onUpdateComponent:function onUpdateComponent(id){
var item=get(id);
if(!item||!item.isMounted){


return;
}
item.updateCount++;
},
onUnmountComponent:function onUnmountComponent(id){
var item=get(id);
if(item){





item.isMounted=false;
var isRoot=item.parentID===0;
if(isRoot){
removeRoot(id);
}
}
unmountedIDs.push(id);
},
purgeUnmountedComponents:function purgeUnmountedComponents(){
if(ReactComponentTreeHook._preventPurging){

return;
}

for(var i=0;i<unmountedIDs.length;i++){
var id=unmountedIDs[i];
purgeDeep(id);
}
unmountedIDs.length=0;
},
isMounted:function isMounted(id){
var item=get(id);
return item?item.isMounted:false;
},
getCurrentStackAddendum:function getCurrentStackAddendum(topElement){
var info='';
if(topElement){
var type=topElement.type;
var name=typeof type==='function'?type.displayName||type.name:type;
var owner=topElement._owner;
info+=describeComponentFrame(name||'Unknown',topElement._source,owner&&owner.getName());
}

var currentOwner=ReactCurrentOwner.current;
var id=currentOwner&&currentOwner._debugID;

info+=ReactComponentTreeHook.getStackAddendumByID(id);
return info;
},
getStackAddendumByID:function getStackAddendumByID(id){
var info='';
while(id){
info+=describeID(id);
id=ReactComponentTreeHook.getParentID(id);
}
return info;
},
getChildIDs:function getChildIDs(id){
var item=get(id);
return item?item.childIDs:[];
},
getDisplayName:function getDisplayName(id){
var element=ReactComponentTreeHook.getElement(id);
if(!element){
return null;
}
return _getDisplayName(element);
},
getElement:function getElement(id){
var item=get(id);
return item?item.element:null;
},
getOwnerID:function getOwnerID(id){
var element=ReactComponentTreeHook.getElement(id);
if(!element||!element._owner){
return null;
}
return element._owner._debugID;
},
getParentID:function getParentID(id){
var item=get(id);
return item?item.parentID:null;
},
getSource:function getSource(id){
var item=get(id);
var element=item?item.element:null;
var source=element!=null?element._source:null;
return source;
},
getText:function getText(id){
var element=ReactComponentTreeHook.getElement(id);
if(typeof element==='string'){
return element;
}else if(typeof element==='number'){
return''+element;
}else{
return null;
}
},
getUpdateCount:function getUpdateCount(id){
var item=get(id);
return item?item.updateCount:0;
},


getRegisteredIDs:getRegisteredIDs,

getRootIDs:getRootIDs};


module.exports=ReactComponentTreeHook;
}, "react/lib/ReactComponentTreeHook.js");
__d(20 /* react/lib/reactProdInvariant.js */, function(global, require, module, exports) {










'use strict';








function reactProdInvariant(code){
var argCount=arguments.length-1;

var message='Minified React error #'+code+'; visit '+'http://facebook.github.io/react/docs/error-decoder.html?invariant='+code;

for(var argIdx=0;argIdx<argCount;argIdx++){
message+='&args[]='+encodeURIComponent(arguments[argIdx+1]);
}

message+=' for the full message or use the non-minified dev environment'+' for full errors and additional helpful warnings.';

var error=new Error(message);
error.name='Invariant Violation';
error.framesToPop=1;

throw error;
}

module.exports=reactProdInvariant;
}, "react/lib/reactProdInvariant.js");
__d(21 /* react/lib/ReactCurrentOwner.js */, function(global, require, module, exports) {










'use strict';








var ReactCurrentOwner={





current:null};



module.exports=ReactCurrentOwner;
}, "react/lib/ReactCurrentOwner.js");
__d(22 /* fbjs/lib/invariant.js */, function(global, require, module, exports) {









'use strict';












function invariant(condition,format,a,b,c,d,e,f){
if(process.env.NODE_ENV!=='production'){
if(format===undefined){
throw new Error('invariant requires an error message argument');
}
}

if(!condition){
var error;
if(format===undefined){
error=new Error('Minified exception occurred; use the non-minified dev environment '+'for the full error message and additional helpful warnings.');
}else{
var args=[a,b,c,d,e,f];
var argIndex=0;
error=new Error(format.replace(/%s/g,function(){
return args[argIndex++];
}));
error.name='Invariant Violation';
}

error.framesToPop=1;
throw error;
}
}

module.exports=invariant;
}, "fbjs/lib/invariant.js");
__d(23 /* fbjs/lib/warning.js */, function(global, require, module, exports) {









'use strict';

var emptyFunction=require(24 /* ./emptyFunction */);








var warning=emptyFunction;

if(process.env.NODE_ENV!=='production'){
(function(){
var printWarning=function printWarning(format){
for(var _len=arguments.length,args=Array(_len>1?_len-1:0),_key=1;_key<_len;_key++){
args[_key-1]=arguments[_key];
}

var argIndex=0;
var message='Warning: '+format.replace(/%s/g,function(){
return args[argIndex++];
});
if(typeof console!=='undefined'){
console.error(message);
}
try{



throw new Error(message);
}catch(x){}
};

warning=function warning(condition,format){
if(format===undefined){
throw new Error('`warning(condition, format, ...args)` requires a warning '+'message argument');
}

if(format.indexOf('Failed Composite propType: ')===0){
return;
}

if(!condition){
for(var _len2=arguments.length,args=Array(_len2>2?_len2-2:0),_key2=2;_key2<_len2;_key2++){
args[_key2-2]=arguments[_key2];
}

printWarning.apply(undefined,[format].concat(args));
}
};
})();
}

module.exports=warning;
}, "fbjs/lib/warning.js");
__d(24 /* fbjs/lib/emptyFunction.js */, function(global, require, module, exports) {"use strict";












function makeEmptyFunction(arg){
return function(){
return arg;
};
}






var emptyFunction=function emptyFunction(){};

emptyFunction.thatReturns=makeEmptyFunction;
emptyFunction.thatReturnsFalse=makeEmptyFunction(false);
emptyFunction.thatReturnsTrue=makeEmptyFunction(true);
emptyFunction.thatReturnsNull=makeEmptyFunction(null);
emptyFunction.thatReturnsThis=function(){
return this;
};
emptyFunction.thatReturnsArgument=function(arg){
return arg;
};

module.exports=emptyFunction;
}, "fbjs/lib/emptyFunction.js");
__d(25 /* react/lib/ReactDebugTool.js */, function(global, require, module, exports) {










'use strict';

var ReactInvalidSetStateWarningHook=require(26 /* ./ReactInvalidSetStateWarningHook */);
var ReactHostOperationHistoryHook=require(27 /* ./ReactHostOperationHistoryHook */);
var ReactComponentTreeHook=require(19 /* ./ReactComponentTreeHook */);
var ReactChildrenMutationWarningHook=require(28 /* ./ReactChildrenMutationWarningHook */);
var ExecutionEnvironment=require(29 /* fbjs/lib/ExecutionEnvironment */);

var performanceNow=require(30 /* fbjs/lib/performanceNow */);
var warning=require(23 /* fbjs/lib/warning */);

var hooks=[];
var didHookThrowForEvent={};

function callHook(event,fn,context,arg1,arg2,arg3,arg4,arg5){
try{
fn.call(context,arg1,arg2,arg3,arg4,arg5);
}catch(e){
process.env.NODE_ENV!=='production'?warning(didHookThrowForEvent[event],'Exception thrown by hook while handling %s: %s',event,e+'\n'+e.stack):void 0;
didHookThrowForEvent[event]=true;
}
}

function emitEvent(event,arg1,arg2,arg3,arg4,arg5){
for(var i=0;i<hooks.length;i++){
var hook=hooks[i];
var fn=hook[event];
if(fn){
callHook(event,fn,hook,arg1,arg2,arg3,arg4,arg5);
}
}
}

var _isProfiling=false;
var flushHistory=[];
var lifeCycleTimerStack=[];
var currentFlushNesting=0;
var currentFlushMeasurements=null;
var currentFlushStartTime=null;
var currentTimerDebugID=null;
var currentTimerStartTime=null;
var currentTimerNestedFlushDuration=null;
var currentTimerType=null;

var lifeCycleTimerHasWarned=false;

function clearHistory(){
ReactComponentTreeHook.purgeUnmountedComponents();
ReactHostOperationHistoryHook.clearHistory();
}

function getTreeSnapshot(registeredIDs){
return registeredIDs.reduce(function(tree,id){
var ownerID=ReactComponentTreeHook.getOwnerID(id);
var parentID=ReactComponentTreeHook.getParentID(id);
tree[id]={
displayName:ReactComponentTreeHook.getDisplayName(id),
text:ReactComponentTreeHook.getText(id),
updateCount:ReactComponentTreeHook.getUpdateCount(id),
childIDs:ReactComponentTreeHook.getChildIDs(id),

ownerID:ownerID||ReactComponentTreeHook.getOwnerID(parentID),
parentID:parentID};

return tree;
},{});
}

function resetMeasurements(){
var previousStartTime=currentFlushStartTime;
var previousMeasurements=currentFlushMeasurements||[];
var previousOperations=ReactHostOperationHistoryHook.getHistory();

if(currentFlushNesting===0){
currentFlushStartTime=null;
currentFlushMeasurements=null;
clearHistory();
return;
}

if(previousMeasurements.length||previousOperations.length){
var registeredIDs=ReactComponentTreeHook.getRegisteredIDs();
flushHistory.push({
duration:performanceNow()-previousStartTime,
measurements:previousMeasurements||[],
operations:previousOperations||[],
treeSnapshot:getTreeSnapshot(registeredIDs)});

}

clearHistory();
currentFlushStartTime=performanceNow();
currentFlushMeasurements=[];
}

function checkDebugID(debugID){
var allowRoot=arguments.length<=1||arguments[1]===undefined?false:arguments[1];

if(allowRoot&&debugID===0){
return;
}
if(!debugID){
process.env.NODE_ENV!=='production'?warning(false,'ReactDebugTool: debugID may not be empty.'):void 0;
}
}

function beginLifeCycleTimer(debugID,timerType){
if(currentFlushNesting===0){
return;
}
if(currentTimerType&&!lifeCycleTimerHasWarned){
process.env.NODE_ENV!=='production'?warning(false,'There is an internal error in the React performance measurement code. '+'Did not expect %s timer to start while %s timer is still in '+'progress for %s instance.',timerType,currentTimerType||'no',debugID===currentTimerDebugID?'the same':'another'):void 0;
lifeCycleTimerHasWarned=true;
}
currentTimerStartTime=performanceNow();
currentTimerNestedFlushDuration=0;
currentTimerDebugID=debugID;
currentTimerType=timerType;
}

function endLifeCycleTimer(debugID,timerType){
if(currentFlushNesting===0){
return;
}
if(currentTimerType!==timerType&&!lifeCycleTimerHasWarned){
process.env.NODE_ENV!=='production'?warning(false,'There is an internal error in the React performance measurement code. '+'We did not expect %s timer to stop while %s timer is still in '+'progress for %s instance. Please report this as a bug in React.',timerType,currentTimerType||'no',debugID===currentTimerDebugID?'the same':'another'):void 0;
lifeCycleTimerHasWarned=true;
}
if(_isProfiling){
currentFlushMeasurements.push({
timerType:timerType,
instanceID:debugID,
duration:performanceNow()-currentTimerStartTime-currentTimerNestedFlushDuration});

}
currentTimerStartTime=null;
currentTimerNestedFlushDuration=null;
currentTimerDebugID=null;
currentTimerType=null;
}

function pauseCurrentLifeCycleTimer(){
var currentTimer={
startTime:currentTimerStartTime,
nestedFlushStartTime:performanceNow(),
debugID:currentTimerDebugID,
timerType:currentTimerType};

lifeCycleTimerStack.push(currentTimer);
currentTimerStartTime=null;
currentTimerNestedFlushDuration=null;
currentTimerDebugID=null;
currentTimerType=null;
}

function resumeCurrentLifeCycleTimer(){
var _lifeCycleTimerStack$=lifeCycleTimerStack.pop();

var startTime=_lifeCycleTimerStack$.startTime;
var nestedFlushStartTime=_lifeCycleTimerStack$.nestedFlushStartTime;
var debugID=_lifeCycleTimerStack$.debugID;
var timerType=_lifeCycleTimerStack$.timerType;

var nestedFlushDuration=performanceNow()-nestedFlushStartTime;
currentTimerStartTime=startTime;
currentTimerNestedFlushDuration+=nestedFlushDuration;
currentTimerDebugID=debugID;
currentTimerType=timerType;
}

var ReactDebugTool={
addHook:function addHook(hook){
hooks.push(hook);
},
removeHook:function removeHook(hook){
for(var i=0;i<hooks.length;i++){
if(hooks[i]===hook){
hooks.splice(i,1);
i--;
}
}
},
isProfiling:function isProfiling(){
return _isProfiling;
},
beginProfiling:function beginProfiling(){
if(_isProfiling){
return;
}

_isProfiling=true;
flushHistory.length=0;
resetMeasurements();
ReactDebugTool.addHook(ReactHostOperationHistoryHook);
},
endProfiling:function endProfiling(){
if(!_isProfiling){
return;
}

_isProfiling=false;
resetMeasurements();
ReactDebugTool.removeHook(ReactHostOperationHistoryHook);
},
getFlushHistory:function getFlushHistory(){
return flushHistory;
},
onBeginFlush:function onBeginFlush(){
currentFlushNesting++;
resetMeasurements();
pauseCurrentLifeCycleTimer();
emitEvent('onBeginFlush');
},
onEndFlush:function onEndFlush(){
resetMeasurements();
currentFlushNesting--;
resumeCurrentLifeCycleTimer();
emitEvent('onEndFlush');
},
onBeginLifeCycleTimer:function onBeginLifeCycleTimer(debugID,timerType){
checkDebugID(debugID);
emitEvent('onBeginLifeCycleTimer',debugID,timerType);
beginLifeCycleTimer(debugID,timerType);
},
onEndLifeCycleTimer:function onEndLifeCycleTimer(debugID,timerType){
checkDebugID(debugID);
endLifeCycleTimer(debugID,timerType);
emitEvent('onEndLifeCycleTimer',debugID,timerType);
},
onBeginProcessingChildContext:function onBeginProcessingChildContext(){
emitEvent('onBeginProcessingChildContext');
},
onEndProcessingChildContext:function onEndProcessingChildContext(){
emitEvent('onEndProcessingChildContext');
},
onHostOperation:function onHostOperation(debugID,type,payload){
checkDebugID(debugID);
emitEvent('onHostOperation',debugID,type,payload);
},
onSetState:function onSetState(){
emitEvent('onSetState');
},
onSetChildren:function onSetChildren(debugID,childDebugIDs){
checkDebugID(debugID);
childDebugIDs.forEach(checkDebugID);
emitEvent('onSetChildren',debugID,childDebugIDs);
},
onBeforeMountComponent:function onBeforeMountComponent(debugID,element,parentDebugID){
checkDebugID(debugID);
checkDebugID(parentDebugID,true);
emitEvent('onBeforeMountComponent',debugID,element,parentDebugID);
},
onMountComponent:function onMountComponent(debugID){
checkDebugID(debugID);
emitEvent('onMountComponent',debugID);
},
onBeforeUpdateComponent:function onBeforeUpdateComponent(debugID,element){
checkDebugID(debugID);
emitEvent('onBeforeUpdateComponent',debugID,element);
},
onUpdateComponent:function onUpdateComponent(debugID){
checkDebugID(debugID);
emitEvent('onUpdateComponent',debugID);
},
onBeforeUnmountComponent:function onBeforeUnmountComponent(debugID){
checkDebugID(debugID);
emitEvent('onBeforeUnmountComponent',debugID);
},
onUnmountComponent:function onUnmountComponent(debugID){
checkDebugID(debugID);
emitEvent('onUnmountComponent',debugID);
},
onTestEvent:function onTestEvent(){
emitEvent('onTestEvent');
}};



ReactDebugTool.addDevtool=ReactDebugTool.addHook;
ReactDebugTool.removeDevtool=ReactDebugTool.removeHook;

ReactDebugTool.addHook(ReactInvalidSetStateWarningHook);
ReactDebugTool.addHook(ReactComponentTreeHook);
ReactDebugTool.addHook(ReactChildrenMutationWarningHook);
var url=ExecutionEnvironment.canUseDOM&&window.location.href||'';
if(/[?&]react_perf\b/.test(url)){
ReactDebugTool.beginProfiling();
}

module.exports=ReactDebugTool;
}, "react/lib/ReactDebugTool.js");
__d(26 /* react/lib/ReactInvalidSetStateWarningHook.js */, function(global, require, module, exports) {










'use strict';

var warning=require(23 /* fbjs/lib/warning */);

if(process.env.NODE_ENV!=='production'){
var processingChildContext=false;

var warnInvalidSetState=function warnInvalidSetState(){
process.env.NODE_ENV!=='production'?warning(!processingChildContext,'setState(...): Cannot call setState() inside getChildContext()'):void 0;
};
}

var ReactInvalidSetStateWarningHook={
onBeginProcessingChildContext:function onBeginProcessingChildContext(){
processingChildContext=true;
},
onEndProcessingChildContext:function onEndProcessingChildContext(){
processingChildContext=false;
},
onSetState:function onSetState(){
warnInvalidSetState();
}};


module.exports=ReactInvalidSetStateWarningHook;
}, "react/lib/ReactInvalidSetStateWarningHook.js");
__d(27 /* react/lib/ReactHostOperationHistoryHook.js */, function(global, require, module, exports) {










'use strict';

var history=[];

var ReactHostOperationHistoryHook={
onHostOperation:function onHostOperation(debugID,type,payload){
history.push({
instanceID:debugID,
type:type,
payload:payload});

},
clearHistory:function clearHistory(){
if(ReactHostOperationHistoryHook._preventClearing){

return;
}

history=[];
},
getHistory:function getHistory(){
return history;
}};


module.exports=ReactHostOperationHistoryHook;
}, "react/lib/ReactHostOperationHistoryHook.js");
__d(28 /* react/lib/ReactChildrenMutationWarningHook.js */, function(global, require, module, exports) {










'use strict';

var ReactComponentTreeHook=require(19 /* ./ReactComponentTreeHook */);

var warning=require(23 /* fbjs/lib/warning */);

function handleElement(debugID,element){
if(element==null){
return;
}
if(element._shadowChildren===undefined){
return;
}
if(element._shadowChildren===element.props.children){
return;
}
var isMutated=false;
if(Array.isArray(element._shadowChildren)){
if(element._shadowChildren.length===element.props.children.length){
for(var i=0;i<element._shadowChildren.length;i++){
if(element._shadowChildren[i]!==element.props.children[i]){
isMutated=true;
}
}
}else{
isMutated=true;
}
}
if(!Array.isArray(element._shadowChildren)||isMutated){
process.env.NODE_ENV!=='production'?warning(false,'Component\'s children should not be mutated.%s',ReactComponentTreeHook.getStackAddendumByID(debugID)):void 0;
}
}

var ReactChildrenMutationWarningHook={
onMountComponent:function onMountComponent(debugID){
handleElement(debugID,ReactComponentTreeHook.getElement(debugID));
},
onUpdateComponent:function onUpdateComponent(debugID){
handleElement(debugID,ReactComponentTreeHook.getElement(debugID));
}};


module.exports=ReactChildrenMutationWarningHook;
}, "react/lib/ReactChildrenMutationWarningHook.js");
__d(29 /* fbjs/lib/ExecutionEnvironment.js */, function(global, require, module, exports) {









'use strict';

var canUseDOM=!!(typeof window!=='undefined'&&window.document&&window.document.createElement);







var ExecutionEnvironment={

canUseDOM:canUseDOM,

canUseWorkers:typeof Worker!=='undefined',

canUseEventListeners:canUseDOM&&!!(window.addEventListener||window.attachEvent),

canUseViewport:canUseDOM&&!!window.screen,

isInWorker:!canUseDOM};



module.exports=ExecutionEnvironment;
}, "fbjs/lib/ExecutionEnvironment.js");
__d(30 /* fbjs/lib/performanceNow.js */, function(global, require, module, exports) {'use strict';












var performance=require(31 /* ./performance */);

var performanceNow;






if(performance.now){
performanceNow=function performanceNow(){
return performance.now();
};
}else{
performanceNow=function performanceNow(){
return Date.now();
};
}

module.exports=performanceNow;
}, "fbjs/lib/performanceNow.js");
__d(31 /* fbjs/lib/performance.js */, function(global, require, module, exports) {










'use strict';

var ExecutionEnvironment=require(29 /* ./ExecutionEnvironment */);

var performance;

if(ExecutionEnvironment.canUseDOM){
performance=window.performance||window.msPerformance||window.webkitPerformance;
}

module.exports=performance||{};
}, "fbjs/lib/performance.js");
__d(32 /* ErrorUtils */, function(global, require, module, exports) {























module.exports=global.ErrorUtils;
}, "ErrorUtils");
__d(33 /* JSTimersExecution */, function(global, require, module, exports) {









'use strict';

var Systrace=require(17 /* Systrace */);

var invariant=require(22 /* fbjs/lib/invariant */);
var keyMirror=require(34 /* fbjs/lib/keyMirror */);
var performanceNow=require(30 /* fbjs/lib/performanceNow */);
var warning=require(23 /* fbjs/lib/warning */);



var FRAME_DURATION=1000/60;
var IDLE_CALLBACK_FRAME_DEADLINE=1;

var hasEmittedTimeDriftWarning=false;






var JSTimersExecution={
GUID:1,
Type:keyMirror({
setTimeout:null,
setInterval:null,
requestAnimationFrame:null,
setImmediate:null,
requestIdleCallback:null}),



callbacks:[],
types:[],
timerIDs:[],
immediates:[],
requestIdleCallbacks:[],






callTimer:function callTimer(timerID,frameTime){
warning(
timerID<=JSTimersExecution.GUID,
'Tried to call timer with ID %s but no such timer exists.',
timerID);

var timerIndex=JSTimersExecution.timerIDs.indexOf(timerID);





if(timerIndex===-1){
return;
}
var type=JSTimersExecution.types[timerIndex];
var callback=JSTimersExecution.callbacks[timerIndex];


if(type===JSTimersExecution.Type.setTimeout||
type===JSTimersExecution.Type.setImmediate||
type===JSTimersExecution.Type.requestAnimationFrame||
type===JSTimersExecution.Type.requestIdleCallback){
JSTimersExecution._clearIndex(timerIndex);
}

try{
if(type===JSTimersExecution.Type.setTimeout||
type===JSTimersExecution.Type.setInterval||
type===JSTimersExecution.Type.setImmediate){
callback();
}else if(type===JSTimersExecution.Type.requestAnimationFrame){
var currentTime=performanceNow();
callback(currentTime);
}else if(type===JSTimersExecution.Type.requestIdleCallback){
callback({
timeRemaining:function timeRemaining(){



return Math.max(0,FRAME_DURATION-(performanceNow()-frameTime));
}});

}else{
console.error('Tried to call a callback with invalid type: '+type);
return;
}
}catch(e){

JSTimersExecution.errors=JSTimersExecution.errors||[];
JSTimersExecution.errors.push(e);
}
},





callTimers:function callTimers(timerIDs){
invariant(
timerIDs.length!==0,
'Cannot call `callTimers` with an empty list of IDs.');


JSTimersExecution.errors=null;
timerIDs.forEach(function(id){JSTimersExecution.callTimer(id);});

var errors=JSTimersExecution.errors;
if(errors){
var errorCount=errors.length;
if(errorCount>1){


for(var ii=1;ii<errorCount;ii++){
require(35 /* JSTimers */).setTimeout(
function(error){throw error;}.bind(null,errors[ii]),
0);

}
}
throw errors[0];
}
},

callIdleCallbacks:function callIdleCallbacks(frameTime){var _require=
require(14 /* NativeModules */);var Timing=_require.Timing;

if(FRAME_DURATION-(performanceNow()-frameTime)<IDLE_CALLBACK_FRAME_DEADLINE){
return;
}

JSTimersExecution.errors=null;

if(JSTimersExecution.requestIdleCallbacks.length>0){
var passIdleCallbacks=JSTimersExecution.requestIdleCallbacks.slice();
JSTimersExecution.requestIdleCallbacks=[];

for(var i=0;i<passIdleCallbacks.length;++i){
JSTimersExecution.callTimer(passIdleCallbacks[i],frameTime);
}
}

if(JSTimersExecution.requestIdleCallbacks.length===0){
Timing.setSendIdleEvents(false);
}

if(JSTimersExecution.errors){
JSTimersExecution.errors.forEach(function(error){return(
require(35 /* JSTimers */).setTimeout(function(){throw error;},0));});

}
},





callImmediatesPass:function callImmediatesPass(){
Systrace.beginEvent('JSTimersExecution.callImmediatesPass()');



if(JSTimersExecution.immediates.length>0){
var passImmediates=JSTimersExecution.immediates.slice();
JSTimersExecution.immediates=[];



for(var i=0;i<passImmediates.length;++i){
JSTimersExecution.callTimer(passImmediates[i]);
}
}

Systrace.endEvent();

return JSTimersExecution.immediates.length>0;
},





callImmediates:function callImmediates(){
JSTimersExecution.errors=null;
while(JSTimersExecution.callImmediatesPass()){}
if(JSTimersExecution.errors){
JSTimersExecution.errors.forEach(function(error){return(
require(35 /* JSTimers */).setTimeout(function(){throw error;},0));});

}
},




emitTimeDriftWarning:function emitTimeDriftWarning(warningMessage){
if(hasEmittedTimeDriftWarning){
return;
}
hasEmittedTimeDriftWarning=true;
console.warn(warningMessage);
},

_clearIndex:function _clearIndex(i){
JSTimersExecution.timerIDs[i]=null;
JSTimersExecution.callbacks[i]=null;
JSTimersExecution.types[i]=null;
}};


module.exports=JSTimersExecution;
}, "JSTimersExecution");
__d(34 /* fbjs/lib/keyMirror.js */, function(global, require, module, exports) {










'use strict';

var invariant=require(22 /* ./invariant */);



















var keyMirror=function keyMirror(obj){
var ret={};
var key;
!(obj instanceof Object&&!Array.isArray(obj))?process.env.NODE_ENV!=='production'?invariant(false,'keyMirror(...): Argument must be an object.'):invariant(false):void 0;
for(key in obj){
if(!obj.hasOwnProperty(key)){
continue;
}
ret[key]=key;
}
return ret;
};

module.exports=keyMirror;
}, "fbjs/lib/keyMirror.js");
__d(35 /* JSTimers */, function(global, require, module, exports) {









'use strict';



var RCTTiming=require(14 /* NativeModules */).Timing;
var JSTimersExecution=require(33 /* JSTimersExecution */);






var JSTimers={
Types:JSTimersExecution.Types,





_getFreeIndex:function _getFreeIndex(){
var freeIndex=JSTimersExecution.timerIDs.indexOf(null);
if(freeIndex===-1){
freeIndex=JSTimersExecution.timerIDs.length;
}
return freeIndex;
},





setTimeout:function setTimeout(func,duration){for(var _len=arguments.length,args=Array(_len>2?_len-2:0),_key=2;_key<_len;_key++){args[_key-2]=arguments[_key];}
var newID=JSTimersExecution.GUID++;
var freeIndex=JSTimers._getFreeIndex();
JSTimersExecution.timerIDs[freeIndex]=newID;
JSTimersExecution.callbacks[freeIndex]=function(){
return func.apply(undefined,args);
};
JSTimersExecution.types[freeIndex]=JSTimersExecution.Type.setTimeout;
RCTTiming.createTimer(newID,duration||0,Date.now(),false);
return newID;
},





setInterval:function setInterval(func,duration){for(var _len2=arguments.length,args=Array(_len2>2?_len2-2:0),_key2=2;_key2<_len2;_key2++){args[_key2-2]=arguments[_key2];}
var newID=JSTimersExecution.GUID++;
var freeIndex=JSTimers._getFreeIndex();
JSTimersExecution.timerIDs[freeIndex]=newID;
JSTimersExecution.callbacks[freeIndex]=function(){
return func.apply(undefined,args);
};
JSTimersExecution.types[freeIndex]=JSTimersExecution.Type.setInterval;
RCTTiming.createTimer(newID,duration||0,Date.now(),true);
return newID;
},





setImmediate:function setImmediate(func){for(var _len3=arguments.length,args=Array(_len3>1?_len3-1:0),_key3=1;_key3<_len3;_key3++){args[_key3-1]=arguments[_key3];}
var newID=JSTimersExecution.GUID++;
var freeIndex=JSTimers._getFreeIndex();
JSTimersExecution.timerIDs[freeIndex]=newID;
JSTimersExecution.callbacks[freeIndex]=function(){
return func.apply(undefined,args);
};
JSTimersExecution.types[freeIndex]=JSTimersExecution.Type.setImmediate;
JSTimersExecution.immediates.push(newID);
return newID;
},




requestAnimationFrame:function requestAnimationFrame(func){
var newID=JSTimersExecution.GUID++;
var freeIndex=JSTimers._getFreeIndex();
JSTimersExecution.timerIDs[freeIndex]=newID;
JSTimersExecution.callbacks[freeIndex]=func;
JSTimersExecution.types[freeIndex]=JSTimersExecution.Type.requestAnimationFrame;
RCTTiming.createTimer(newID,1,Date.now(),false);
return newID;
},





requestIdleCallback:function requestIdleCallback(func){
if(JSTimersExecution.requestIdleCallbacks.length===0){
RCTTiming.setSendIdleEvents(true);
}

var newID=JSTimersExecution.GUID++;
var freeIndex=JSTimers._getFreeIndex();
JSTimersExecution.timerIDs[freeIndex]=newID;
JSTimersExecution.callbacks[freeIndex]=func;
JSTimersExecution.types[freeIndex]=JSTimersExecution.Type.requestIdleCallback;
JSTimersExecution.requestIdleCallbacks.push(newID);
return newID;
},

cancelIdleCallback:function cancelIdleCallback(timerID){
JSTimers._clearTimerID(timerID);
var index=JSTimersExecution.requestIdleCallbacks.indexOf(timerID);
if(index!==-1){
JSTimersExecution.requestIdleCallbacks.splice(index,1);
}

if(JSTimersExecution.requestIdleCallbacks.length===0){
RCTTiming.setSendIdleEvents(false);
}
},

clearTimeout:function clearTimeout(timerID){
JSTimers._clearTimerID(timerID);
},

clearInterval:function clearInterval(timerID){
JSTimers._clearTimerID(timerID);
},

clearImmediate:function clearImmediate(timerID){
JSTimers._clearTimerID(timerID);
var index=JSTimersExecution.immediates.indexOf(timerID);
if(index!==-1){
JSTimersExecution.immediates.splice(index,1);
}
},

cancelAnimationFrame:function cancelAnimationFrame(timerID){
JSTimers._clearTimerID(timerID);
},

_clearTimerID:function _clearTimerID(timerID){


if(timerID==null){
return;
}

var index=JSTimersExecution.timerIDs.indexOf(timerID);

if(index!==-1){
JSTimersExecution._clearIndex(index);
var type=JSTimersExecution.types[index];
if(type!==JSTimersExecution.Type.setImmediate&&
type!==JSTimersExecution.Type.requestIdleCallback){
RCTTiming.deleteTimer(timerID);
}
}
}};


module.exports=JSTimers;
}, "JSTimers");
__d(36 /* stringifySafe */, function(global, require, module, exports) {










'use strict';





function stringifySafe(arg){
var ret;
var type=typeof arg;
if(arg===undefined){
ret='undefined';
}else if(arg===null){
ret='null';
}else if(type==='string'){
ret='"'+arg+'"';
}else if(type==='function'){
try{
ret=arg.toString();
}catch(e){
ret='[function unknown]';
}
}else{


try{
ret=JSON.stringify(arg);
}catch(e){
if(typeof arg.toString==='function'){
try{
ret=arg.toString();
}catch(E){}
}
}
}
return ret||'["'+type+'" failed to stringify]';
}

module.exports=stringifySafe;
}, "stringifySafe");
__d(37 /* HeapCapture */, function(global, require, module, exports) {










'use strict';

var HeapCapture={
captureHeap:function captureHeap(path){
var error=null;
try{
global.nativeCaptureHeap(path);
console.log('HeapCapture.captureHeap succeeded: '+path);
}catch(e){
console.log('HeapCapture.captureHeap error: '+e.toString());
error=e.toString();
}
require(14 /* NativeModules */).JSCHeapCapture.captureComplete(path,error);
}};


module.exports=HeapCapture;
}, "HeapCapture");
__d(38 /* SamplingProfiler */, function(global, require, module, exports) {










'use strict';

var SamplingProfiler={
poke:function poke(token){
var error=null;
var result=null;
try{
result=global.pokeSamplingProfiler();
if(result===null){
console.log('The JSC Sampling Profiler has started');
}else{
console.log('The JSC Sampling Profiler has stopped');
}
}catch(e){
console.log(
'Error occured when restarting Sampling Profiler: '+e.toString());
error=e.toString();
}
require(14 /* NativeModules */).JSCSamplingProfiler.operationComplete(
token,result,error);
}};


module.exports=SamplingProfiler;
}, "SamplingProfiler");
__d(39 /* HMRClient */, function(global, require, module, exports) {










'use strict';

var Platform=require(13 /* Platform */);
var invariant=require(22 /* fbjs/lib/invariant */);





var HMRClient={
enable:function enable(platform,bundleEntry,host,port){
invariant(platform,'Missing required parameter `platform`');
invariant(bundleEntry,'Missing required paramenter `bundleEntry`');
invariant(host,'Missing required paramenter `host`');




var WebSocket=require(40 /* WebSocket */);

var wsHostPort=port!==null&&port!==''?
host+':'+port:
host;


var wsUrl='ws://'+wsHostPort+'/hot?'+('platform='+
platform+'&')+('bundleEntry='+
bundleEntry.replace('.bundle','.js'));

var activeWS=new WebSocket(wsUrl);
activeWS.onerror=function(e){
var error='Hot loading isn\'t working because it cannot connect to the development server.\n\nTry the following to fix the issue:\n- Ensure that the packager server is running and available on the same network';






if(Platform.OS==='ios'){
error+='\n- Ensure that the Packager server URL is correctly set in AppDelegate';



}else{
error+='\n- Ensure that your device/emulator is connected to your machine and has USB debugging enabled - run \'adb devices\' to see a list of connected devices\n- If you\'re on a physical device connected to the same machine, run \'adb reverse tcp:8081 tcp:8081\' to forward requests from your device\n- If your device is on the same Wi-Fi network, set \'Debug server host & port for device\' in \'Dev settings\' to your machine\'s IP address and the port of the local dev server - e.g. 10.0.1.1:8081';





}

error+='\n\nURL: '+


host+':'+port+'\n\nError: '+

e.message;


throw new Error(error);
};
activeWS.onmessage=function(_ref){var data=_ref.data;

var HMRLoadingView=require(57 /* HMRLoadingView */);

data=JSON.parse(data);

switch(data.type){
case'update-start':{
HMRLoadingView.showMessage('Hot Loading...');
break;
}
case'update':{var _ret=function(){var _data$body=





data.body;var modules=_data$body.modules;var sourceMappingURLs=_data$body.sourceMappingURLs;var sourceURLs=_data$body.sourceURLs;var inverseDependencies=_data$body.inverseDependencies;

if(Platform.OS==='ios'){
var RCTRedBox=require(14 /* NativeModules */).RedBox;
RCTRedBox&&RCTRedBox.dismiss&&RCTRedBox.dismiss();
}else{
var RCTExceptionsManager=require(14 /* NativeModules */).ExceptionsManager;
RCTExceptionsManager&&RCTExceptionsManager.dismissRedbox&&RCTExceptionsManager.dismissRedbox();
}

modules.forEach(function(_ref2,i){var id=_ref2.id;var code=_ref2.code;
code=code+'\n\n'+sourceMappingURLs[i];




var injectFunction=typeof global.nativeInjectHMRUpdate==='function'?
global.nativeInjectHMRUpdate:
eval;

code=[
'__accept(',
id+',',
'function(global,require,module,exports){',''+
code,
'\n},',''+
JSON.stringify(inverseDependencies),
');'].
join('');

injectFunction(code,sourceURLs[i]);
});

HMRLoadingView.hide();
return'break';}();if(_ret==='break')break;
}
case'update-done':{
HMRLoadingView.hide();
break;
}
case'error':{
HMRLoadingView.hide();
throw new Error(data.body.type+' '+data.body.description);
}
default:{
throw new Error('Unexpected message: '+data);
}}

};
}};


module.exports=HMRClient;
}, "HMRClient");
__d(40 /* WebSocket */, function(global, require, module, exports) {










'use strict';

var NativeEventEmitter=require(41 /* NativeEventEmitter */);
var Platform=require(13 /* Platform */);
var RCTWebSocketModule=require(14 /* NativeModules */).WebSocketModule;
var WebSocketEvent=require(51 /* WebSocketEvent */);

var EventTarget=require(52 /* event-target-shim */);
var base64=require(56 /* base64-js */);















var CONNECTING=0;
var OPEN=1;
var CLOSING=2;
var CLOSED=3;

var CLOSE_NORMAL=1000;

var WEBSOCKET_EVENTS=[
'close',
'error',
'message',
'open'];


var nextWebSocketId=0;var







WebSocket=function(_EventTarget){babelHelpers.inherits(WebSocket,_EventTarget);


























function WebSocket(url,protocols,options){babelHelpers.classCallCheck(this,WebSocket);var _this=babelHelpers.possibleConstructorReturn(this,(WebSocket.__proto__||Object.getPrototypeOf(WebSocket)).call(this));_this.CONNECTING=CONNECTING;_this.OPEN=OPEN;_this.CLOSING=CLOSING;_this.CLOSED=CLOSED;_this.readyState=CONNECTING;

if(typeof protocols==='string'){
protocols=[protocols];
}

if(!Array.isArray(protocols)){
protocols=null;
}

_this._eventEmitter=new NativeEventEmitter(RCTWebSocketModule);
_this._socketId=nextWebSocketId++;
RCTWebSocketModule.connect(url,protocols,options,_this._socketId);
_this._registerEvents();return _this;
}babelHelpers.createClass(WebSocket,[{key:'close',value:function close(

code,reason){
if(this.readyState===this.CLOSING||
this.readyState===this.CLOSED){
return;
}

this.readyState=this.CLOSING;
this._close(code,reason);
}},{key:'send',value:function send(

data){
if(this.readyState===this.CONNECTING){
throw new Error('INVALID_STATE_ERR');
}

if(typeof data==='string'){
RCTWebSocketModule.send(data,this._socketId);
return;
}


if(typeof ArrayBuffer!=='undefined'&&
typeof Uint8Array!=='undefined'){
if(ArrayBuffer.isView(data)){

data=data.buffer;
}
if(data instanceof ArrayBuffer){
data=base64.fromByteArray(new Uint8Array(data));
RCTWebSocketModule.sendBinary(data,this._socketId);
return;
}
}

throw new Error('Unsupported data type');
}},{key:'ping',value:function ping()

{
if(this.readyState===this.CONNECTING){
throw new Error('INVALID_STATE_ERR');
}

RCTWebSocketModule.ping(this._socketId);
}},{key:'_close',value:function _close(

code,reason){
if(Platform.OS==='android'){

var statusCode=typeof code==='number'?code:CLOSE_NORMAL;
var closeReason=typeof reason==='string'?reason:'';
RCTWebSocketModule.close(statusCode,closeReason,this._socketId);
}else{
RCTWebSocketModule.close(this._socketId);
}
}},{key:'_unregisterEvents',value:function _unregisterEvents()

{
this._subscriptions.forEach(function(e){return e.remove();});
this._subscriptions=[];
}},{key:'_registerEvents',value:function _registerEvents()

{var _this2=this;
this._subscriptions=[
this._eventEmitter.addListener('websocketMessage',function(ev){
if(ev.id!==_this2._socketId){
return;
}
_this2.dispatchEvent(new WebSocketEvent('message',{
data:ev.type==='binary'?base64.toByteArray(ev.data).buffer:ev.data}));

}),
this._eventEmitter.addListener('websocketOpen',function(ev){
if(ev.id!==_this2._socketId){
return;
}
_this2.readyState=_this2.OPEN;
_this2.dispatchEvent(new WebSocketEvent('open'));
}),
this._eventEmitter.addListener('websocketClosed',function(ev){
if(ev.id!==_this2._socketId){
return;
}
_this2.readyState=_this2.CLOSED;
_this2.dispatchEvent(new WebSocketEvent('close',{
code:ev.code,
reason:ev.reason}));

_this2._unregisterEvents();
_this2.close();
}),
this._eventEmitter.addListener('websocketFailed',function(ev){
if(ev.id!==_this2._socketId){
return;
}
_this2.dispatchEvent(new WebSocketEvent('error',{
message:ev.message}));

_this2.dispatchEvent(new WebSocketEvent('close',{
message:ev.message}));

_this2._unregisterEvents();
_this2.close();
})];

}}]);return WebSocket;}(EventTarget.apply(undefined,WEBSOCKET_EVENTS));WebSocket.CONNECTING=CONNECTING;WebSocket.OPEN=OPEN;WebSocket.CLOSING=CLOSING;WebSocket.CLOSED=CLOSED;


module.exports=WebSocket;
}, "WebSocket");
__d(41 /* NativeEventEmitter */, function(global, require, module, exports) {










'use strict';

var EventEmitter=require(42 /* EventEmitter */);
var Platform=require(13 /* Platform */);
var RCTDeviceEventEmitter=require(46 /* RCTDeviceEventEmitter */);
var invariant=require(22 /* fbjs/lib/invariant */);var







NativeEventEmitter=function(_EventEmitter){babelHelpers.inherits(NativeEventEmitter,_EventEmitter);



function NativeEventEmitter(nativeModule){babelHelpers.classCallCheck(this,NativeEventEmitter);var _this=babelHelpers.possibleConstructorReturn(this,(NativeEventEmitter.__proto__||Object.getPrototypeOf(NativeEventEmitter)).call(this,
RCTDeviceEventEmitter.sharedSubscriber));
if(Platform.OS==='ios'){
invariant(nativeModule,'Native module cannot be null.');
_this._nativeModule=nativeModule;
}return _this;
}babelHelpers.createClass(NativeEventEmitter,[{key:'addListener',value:function addListener(

eventType,listener,context){
if(Platform.OS==='ios'){
this._nativeModule.addListener(eventType);
}
return babelHelpers.get(NativeEventEmitter.prototype.__proto__||Object.getPrototypeOf(NativeEventEmitter.prototype),'addListener',this).call(this,eventType,listener,context);
}},{key:'removeAllListeners',value:function removeAllListeners(

eventType){
invariant(eventType,'eventType argument is required.');
if(Platform.OS==='ios'){
var count=this.listeners(eventType).length;
this._nativeModule.removeListeners(count);
}
babelHelpers.get(NativeEventEmitter.prototype.__proto__||Object.getPrototypeOf(NativeEventEmitter.prototype),'removeAllListeners',this).call(this,eventType);
}},{key:'removeSubscription',value:function removeSubscription(

subscription){
if(Platform.OS==='ios'){
this._nativeModule.removeListeners(1);
}
babelHelpers.get(NativeEventEmitter.prototype.__proto__||Object.getPrototypeOf(NativeEventEmitter.prototype),'removeSubscription',this).call(this,subscription);
}}]);return NativeEventEmitter;}(EventEmitter);


module.exports=NativeEventEmitter;
}, "NativeEventEmitter");
__d(42 /* EventEmitter */, function(global, require, module, exports) {












var EmitterSubscription=require(43 /* EmitterSubscription */);
var EventSubscriptionVendor=require(45 /* EventSubscriptionVendor */);
var emptyFunction=require(24 /* fbjs/lib/emptyFunction */);
var invariant=require(22 /* fbjs/lib/invariant */);var














EventEmitter=function(){










function EventEmitter(subscriber){babelHelpers.classCallCheck(this,EventEmitter);
this._subscriber=subscriber||new EventSubscriptionVendor();
}babelHelpers.createClass(EventEmitter,[{key:'addListener',value:function addListener(
















eventType,listener,context){

return this._subscriber.addSubscription(
eventType,
new EmitterSubscription(this,this._subscriber,listener,context));

}},{key:'once',value:function once(











eventType,listener,context){var _this=this;
return this.addListener(eventType,function(){for(var _len=arguments.length,args=Array(_len),_key=0;_key<_len;_key++){args[_key]=arguments[_key];}
_this.removeCurrentListener();
listener.apply(context,args);
});
}},{key:'removeAllListeners',value:function removeAllListeners(








eventType){
this._subscriber.removeAllSubscriptions(eventType);
}},{key:'removeCurrentListener',value:function removeCurrentListener()






















{
invariant(
!!this._currentSubscription,
'Not in an emitting cycle; there is no current subscription');

this.removeSubscription(this._currentSubscription);
}},{key:'removeSubscription',value:function removeSubscription(





subscription){
invariant(
subscription.emitter===this,
'Subscription does not belong to this emitter.');

this._subscriber.removeSubscription(subscription);
}},{key:'listeners',value:function listeners(








eventType){
var subscriptions=this._subscriber.getSubscriptionsForType(eventType);
return subscriptions?
subscriptions.filter(emptyFunction.thatReturnsTrue).map(
function(subscription){
return subscription.listener;
}):
[];
}},{key:'emit',value:function emit(















eventType){
var subscriptions=this._subscriber.getSubscriptionsForType(eventType);
if(subscriptions){
for(var i=0,l=subscriptions.length;i<l;i++){
var subscription=subscriptions[i];


if(subscription){
this._currentSubscription=subscription;
subscription.listener.apply(
subscription.context,
Array.prototype.slice.call(arguments,1));

}
}
this._currentSubscription=null;
}
}},{key:'removeListener',value:function removeListener(














eventType,listener){
var subscriptions=this._subscriber.getSubscriptionsForType(eventType);
if(subscriptions){
for(var i=0,l=subscriptions.length;i<l;i++){
var subscription=subscriptions[i];



if(subscription&&subscription.listener===listener){
subscription.remove();
}
}
}
}}]);return EventEmitter;}();


module.exports=EventEmitter;
}, "EventEmitter");
__d(43 /* EmitterSubscription */, function(global, require, module, exports) {











'use strict';

var EventSubscription=require(44 /* EventSubscription */);var







EmitterSubscription=function(_EventSubscription){babelHelpers.inherits(EmitterSubscription,_EventSubscription);















function EmitterSubscription(
emitter,
subscriber,
listener,
context)
{babelHelpers.classCallCheck(this,EmitterSubscription);var _this=babelHelpers.possibleConstructorReturn(this,(EmitterSubscription.__proto__||Object.getPrototypeOf(EmitterSubscription)).call(this,
subscriber));
_this.emitter=emitter;
_this.listener=listener;
_this.context=context;return _this;
}babelHelpers.createClass(EmitterSubscription,[{key:'remove',value:function remove()







{
this.emitter.removeSubscription(this);
}}]);return EmitterSubscription;}(EventSubscription);


module.exports=EmitterSubscription;
}, "EmitterSubscription");
__d(44 /* EventSubscription */, function(global, require, module, exports) {










'use strict';var







EventSubscription=function(){









function EventSubscription(subscriber){babelHelpers.classCallCheck(this,EventSubscription);
this.subscriber=subscriber;
}babelHelpers.createClass(EventSubscription,[{key:'remove',value:function remove()




{
this.subscriber.removeSubscription(this);
}}]);return EventSubscription;}();


module.exports=EventSubscription;
}, "EventSubscription");
__d(45 /* EventSubscriptionVendor */, function(global, require, module, exports) {










'use strict';

var invariant=require(22 /* fbjs/lib/invariant */);var







EventSubscriptionVendor=function(){




function EventSubscriptionVendor(){babelHelpers.classCallCheck(this,EventSubscriptionVendor);
this._subscriptionsForType={};
this._currentSubscription=null;
}babelHelpers.createClass(EventSubscriptionVendor,[{key:'addSubscription',value:function addSubscription(








eventType,subscription){
invariant(
subscription.subscriber===this,
'The subscriber of the subscription is incorrectly set.');
if(!this._subscriptionsForType[eventType]){
this._subscriptionsForType[eventType]=[];
}
var key=this._subscriptionsForType[eventType].length;
this._subscriptionsForType[eventType].push(subscription);
subscription.eventType=eventType;
subscription.key=key;
return subscription;
}},{key:'removeAllSubscriptions',value:function removeAllSubscriptions(







eventType){
if(eventType===undefined){
this._subscriptionsForType={};
}else{
delete this._subscriptionsForType[eventType];
}
}},{key:'removeSubscription',value:function removeSubscription(







subscription){
var eventType=subscription.eventType;
var key=subscription.key;

var subscriptionsForType=this._subscriptionsForType[eventType];
if(subscriptionsForType){
delete subscriptionsForType[key];
}
}},{key:'getSubscriptionsForType',value:function getSubscriptionsForType(













eventType){
return this._subscriptionsForType[eventType];
}}]);return EventSubscriptionVendor;}();


module.exports=EventSubscriptionVendor;
}, "EventSubscriptionVendor");
__d(46 /* RCTDeviceEventEmitter */, function(global, require, module, exports) {










'use strict';

var EventEmitter=require(42 /* EventEmitter */);
var EventSubscriptionVendor=require(45 /* EventSubscriptionVendor */);
var BatchedBridge=require(15 /* BatchedBridge */);var







RCTDeviceEventEmitter=function(_EventEmitter){babelHelpers.inherits(RCTDeviceEventEmitter,_EventEmitter);



function RCTDeviceEventEmitter(){babelHelpers.classCallCheck(this,RCTDeviceEventEmitter);
var sharedSubscriber=new EventSubscriptionVendor();var _this=babelHelpers.possibleConstructorReturn(this,(RCTDeviceEventEmitter.__proto__||Object.getPrototypeOf(RCTDeviceEventEmitter)).call(this,
sharedSubscriber));
_this.sharedSubscriber=sharedSubscriber;return _this;
}babelHelpers.createClass(RCTDeviceEventEmitter,[{key:'_nativeEventModule',value:function _nativeEventModule(

eventType){
if(eventType){
if(eventType.lastIndexOf('statusBar',0)===0){
console.warn('`%s` event should be registered via the StatusBarIOS module',eventType);
return require(47 /* StatusBarIOS */);
}
if(eventType.lastIndexOf('keyboard',0)===0){
console.warn('`%s` event should be registered via the Keyboard module',eventType);
return require(48 /* Keyboard */);
}
if(eventType==='appStateDidChange'||eventType==='memoryWarning'){
console.warn('`%s` event should be registered via the AppState module',eventType);
return require(49 /* AppState */);
}
}
return null;
}},{key:'addListener',value:function addListener(

eventType,listener,context){
var eventModule=this._nativeEventModule(eventType);
return eventModule?eventModule.addListener(eventType,listener,context):babelHelpers.get(RCTDeviceEventEmitter.prototype.__proto__||Object.getPrototypeOf(RCTDeviceEventEmitter.prototype),'addListener',this).call(this,
eventType,listener,context);
}},{key:'removeAllListeners',value:function removeAllListeners(

eventType){
var eventModule=this._nativeEventModule(eventType);
eventModule&&eventType?eventModule.removeAllListeners(eventType):babelHelpers.get(RCTDeviceEventEmitter.prototype.__proto__||Object.getPrototypeOf(RCTDeviceEventEmitter.prototype),'removeAllListeners',this).call(this,
eventType);
}},{key:'removeSubscription',value:function removeSubscription(

subscription){
if(subscription.emitter!==this){
subscription.emitter.removeSubscription(subscription);
}else{
babelHelpers.get(RCTDeviceEventEmitter.prototype.__proto__||Object.getPrototypeOf(RCTDeviceEventEmitter.prototype),'removeSubscription',this).call(this,subscription);
}
}}]);return RCTDeviceEventEmitter;}(EventEmitter);


RCTDeviceEventEmitter=new RCTDeviceEventEmitter();

BatchedBridge.registerCallableModule(
'RCTDeviceEventEmitter',
RCTDeviceEventEmitter);


module.exports=RCTDeviceEventEmitter;
}, "RCTDeviceEventEmitter");
__d(47 /* StatusBarIOS */, function(global, require, module, exports) {










'use strict';

var NativeEventEmitter=require(41 /* NativeEventEmitter */);

module.exports=new NativeEventEmitter('StatusBarManager');
}, "StatusBarIOS");
__d(48 /* Keyboard */, function(global, require, module, exports) {










'use strict';

var NativeEventEmitter=require(41 /* NativeEventEmitter */);
var KeyboardObserver=require(14 /* NativeModules */).KeyboardObserver;

module.exports=new NativeEventEmitter(KeyboardObserver);
}, "Keyboard");
__d(49 /* AppState */, function(global, require, module, exports) {










'use strict';

var NativeEventEmitter=require(41 /* NativeEventEmitter */);
var NativeModules=require(14 /* NativeModules */);
var RCTAppState=NativeModules.AppState;

var logError=require(50 /* logError */);
var invariant=require(22 /* fbjs/lib/invariant */);var





















































AppState=function(_NativeEventEmitter){babelHelpers.inherits(AppState,_NativeEventEmitter);




function AppState(){babelHelpers.classCallCheck(this,AppState);var _this=babelHelpers.possibleConstructorReturn(this,(AppState.__proto__||Object.getPrototypeOf(AppState)).call(this,
RCTAppState));

_this._eventHandlers={
change:new Map(),
memoryWarning:new Map()};




_this.currentState=RCTAppState.initialAppState||'active';





_this.addListener(
'appStateDidChange',
function(appStateData){
_this.currentState=appStateData.app_state;
});





RCTAppState.getCurrentAppState(
function(appStateData){
_this.currentState=appStateData.app_state;
},
logError);return _this;

}babelHelpers.createClass(AppState,[{key:'addEventListener',value:function addEventListener(












type,
handler)
{
invariant(
['change','memoryWarning'].indexOf(type)!==-1,
'Trying to subscribe to unknown event: "%s"',type);

if(type==='change'){
this._eventHandlers[type].set(handler,this.addListener(
'appStateDidChange',
function(appStateData){
handler(appStateData.app_state);
}));

}else if(type==='memoryWarning'){
this._eventHandlers[type].set(handler,this.addListener(
'memoryWarning',
handler));

}
}},{key:'removeEventListener',value:function removeEventListener(





type,
handler)
{
invariant(
['change','memoryWarning'].indexOf(type)!==-1,
'Trying to remove listener for unknown event: "%s"',type);

if(!this._eventHandlers[type].has(handler)){
return;
}
this._eventHandlers[type].get(handler).remove();
this._eventHandlers[type].delete(handler);
}}]);return AppState;}(NativeEventEmitter);


AppState=new AppState();

module.exports=AppState;
}, "AppState");
__d(50 /* logError */, function(global, require, module, exports) {










'use strict';






var logError=function logError(){
if(arguments.length===1&&arguments[0]instanceof Error){
var err=arguments[0];
console.error('Error: "'+err.message+'".  Stack:\n'+err.stack);
}else{
console.error.apply(console,arguments);
}
};

module.exports=logError;
}, "logError");
__d(51 /* WebSocketEvent */, function(global, require, module, exports) {










'use strict';var









WebSocketEvent=
function WebSocketEvent(type,eventInitDict){babelHelpers.classCallCheck(this,WebSocketEvent);
this.type=type.toString();
babelHelpers.extends(this,eventInitDict);
};


module.exports=WebSocketEvent;
}, "WebSocketEvent");
__d(52 /* event-target-shim/lib/event-target.js */, function(global, require, module, exports) {





"use strict";





var Commons=require(53 /* ./commons */);
var CustomEventTarget=require(54 /* ./custom-event-target */);
var EventWrapper=require(55 /* ./event-wrapper */);
var LISTENERS=Commons.LISTENERS;
var CAPTURE=Commons.CAPTURE;
var BUBBLE=Commons.BUBBLE;
var ATTRIBUTE=Commons.ATTRIBUTE;
var newNode=Commons.newNode;
var defineCustomEventTarget=CustomEventTarget.defineCustomEventTarget;
var createEventWrapper=EventWrapper.createEventWrapper;
var STOP_IMMEDIATE_PROPAGATION_FLAG=
EventWrapper.STOP_IMMEDIATE_PROPAGATION_FLAG;











var HAS_EVENTTARGET_INTERFACE=
typeof window!=="undefined"&&
typeof window.EventTarget!=="undefined";












var EventTarget=module.exports=function EventTarget(){
if(this instanceof EventTarget){









Object.defineProperty(this,LISTENERS,{value:Object.create(null)});
}else
if(arguments.length===1&&Array.isArray(arguments[0])){
return defineCustomEventTarget(EventTarget,arguments[0]);
}else
if(arguments.length>0){
var types=Array(arguments.length);
for(var i=0;i<arguments.length;++i){
types[i]=arguments[i];
}






return defineCustomEventTarget(EventTarget,types);
}else
{
throw new TypeError("Cannot call a class as a function");
}
};

EventTarget.prototype=Object.create(
(HAS_EVENTTARGET_INTERFACE?window.EventTarget:Object).prototype,
{
constructor:{
value:EventTarget,
writable:true,
configurable:true},


addEventListener:{
value:function addEventListener(type,listener,capture){
if(listener==null){
return false;
}
if(typeof listener!=="function"&&typeof listener!=="object"){
throw new TypeError("\"listener\" is not an object.");
}

var kind=capture?CAPTURE:BUBBLE;
var node=this[LISTENERS][type];
if(node==null){
this[LISTENERS][type]=newNode(listener,kind);
return true;
}

var prev=null;
while(node!=null){
if(node.listener===listener&&node.kind===kind){

return false;
}
prev=node;
node=node.next;
}

prev.next=newNode(listener,kind);
return true;
},
configurable:true,
writable:true},


removeEventListener:{
value:function removeEventListener(type,listener,capture){
if(listener==null){
return false;
}

var kind=capture?CAPTURE:BUBBLE;
var prev=null;
var node=this[LISTENERS][type];
while(node!=null){
if(node.listener===listener&&node.kind===kind){
if(prev==null){
this[LISTENERS][type]=node.next;
}else
{
prev.next=node.next;
}
return true;
}

prev=node;
node=node.next;
}

return false;
},
configurable:true,
writable:true},


dispatchEvent:{
value:function dispatchEvent(event){

var node=this[LISTENERS][event.type];
if(node==null){
return true;
}


var wrapped=createEventWrapper(event,this);



while(node!=null){
if(typeof node.listener==="function"){
node.listener.call(this,wrapped);
}else
if(node.kind!==ATTRIBUTE&&typeof node.listener.handleEvent==="function"){
node.listener.handleEvent(wrapped);
}

if(wrapped[STOP_IMMEDIATE_PROPAGATION_FLAG]){
break;
}
node=node.next;
}

return!wrapped.defaultPrevented;
},
configurable:true,
writable:true}});
}, "event-target-shim/lib/event-target.js");
__d(53 /* event-target-shim/lib/commons.js */, function(global, require, module, exports) {





"use strict";








var createUniqueKey=exports.createUniqueKey=typeof Symbol!=="undefined"?
Symbol:
function createUniqueKey(name){
return"[["+name+"_"+Math.random().toFixed(8).slice(2)+"]]";
};







exports.LISTENERS=createUniqueKey("listeners");







exports.CAPTURE=1;







exports.BUBBLE=2;







exports.ATTRIBUTE=3;
















exports.newNode=function newNode(listener,kind){
return{listener:listener,kind:kind,next:null};
};
}, "event-target-shim/lib/commons.js");
__d(54 /* event-target-shim/lib/custom-event-target.js */, function(global, require, module, exports) {





"use strict";





var Commons=require(53 /* ./commons */);
var LISTENERS=Commons.LISTENERS;
var ATTRIBUTE=Commons.ATTRIBUTE;
var newNode=Commons.newNode;












function getAttributeListener(eventTarget,type){
var node=eventTarget[LISTENERS][type];
while(node!=null){
if(node.kind===ATTRIBUTE){
return node.listener;
}
node=node.next;
}
return null;
}









function setAttributeListener(eventTarget,type,listener){
if(typeof listener!=="function"&&typeof listener!=="object"){
listener=null;
}

var prev=null;
var node=eventTarget[LISTENERS][type];
while(node!=null){
if(node.kind===ATTRIBUTE){

if(prev==null){
eventTarget[LISTENERS][type]=node.next;
}else
{
prev.next=node.next;
}
}else
{
prev=node;
}

node=node.next;
}


if(listener!=null){
if(prev==null){
eventTarget[LISTENERS][type]=newNode(listener,ATTRIBUTE);
}else
{
prev.next=newNode(listener,ATTRIBUTE);
}
}
}












exports.defineCustomEventTarget=function(EventTargetBase,types){
function EventTarget(){
EventTargetBase.call(this);
}

var descripter={
constructor:{
value:EventTarget,
configurable:true,
writable:true}};



types.forEach(function(type){
descripter["on"+type]={
get:function get(){return getAttributeListener(this,type);},
set:function set(listener){setAttributeListener(this,type,listener);},
configurable:true,
enumerable:true};

});

EventTarget.prototype=Object.create(EventTargetBase.prototype,descripter);

return EventTarget;
};
}, "event-target-shim/lib/custom-event-target.js");
__d(55 /* event-target-shim/lib/event-wrapper.js */, function(global, require, module, exports) {





"use strict";





var createUniqueKey=require(53 /* ./commons */).createUniqueKey;











var STOP_IMMEDIATE_PROPAGATION_FLAG=
createUniqueKey("stop_immediate_propagation_flag");







var CANCELED_FLAG=createUniqueKey("canceled_flag");







var ORIGINAL_EVENT=createUniqueKey("original_event");







var wrapperPrototypeDefinition=Object.freeze({
stopPropagation:Object.freeze({
value:function stopPropagation(){
var e=this[ORIGINAL_EVENT];
if(typeof e.stopPropagation==="function"){
e.stopPropagation();
}
},
writable:true,
configurable:true}),


stopImmediatePropagation:Object.freeze({
value:function stopImmediatePropagation(){
this[STOP_IMMEDIATE_PROPAGATION_FLAG]=true;

var e=this[ORIGINAL_EVENT];
if(typeof e.stopImmediatePropagation==="function"){
e.stopImmediatePropagation();
}
},
writable:true,
configurable:true}),


preventDefault:Object.freeze({
value:function preventDefault(){
if(this.cancelable===true){
this[CANCELED_FLAG]=true;
}

var e=this[ORIGINAL_EVENT];
if(typeof e.preventDefault==="function"){
e.preventDefault();
}
},
writable:true,
configurable:true}),


defaultPrevented:Object.freeze({
get:function defaultPrevented(){return this[CANCELED_FLAG];},
enumerable:true,
configurable:true})});







exports.STOP_IMMEDIATE_PROPAGATION_FLAG=STOP_IMMEDIATE_PROPAGATION_FLAG;












exports.createEventWrapper=function createEventWrapper(event,eventTarget){
var timeStamp=
typeof event.timeStamp==="number"?event.timeStamp:Date.now();

var propertyDefinition={
type:{value:event.type,enumerable:true},
target:{value:eventTarget,enumerable:true},
currentTarget:{value:eventTarget,enumerable:true},
eventPhase:{value:2,enumerable:true},
bubbles:{value:Boolean(event.bubbles),enumerable:true},
cancelable:{value:Boolean(event.cancelable),enumerable:true},
timeStamp:{value:timeStamp,enumerable:true},
isTrusted:{value:false,enumerable:true}};

propertyDefinition[STOP_IMMEDIATE_PROPAGATION_FLAG]={value:false,writable:true};
propertyDefinition[CANCELED_FLAG]={value:false,writable:true};
propertyDefinition[ORIGINAL_EVENT]={value:event};


if(typeof event.detail!=="undefined"){
propertyDefinition.detail={value:event.detail,enumerable:true};
}

return Object.create(
Object.create(event,wrapperPrototypeDefinition),
propertyDefinition);

};
}, "event-target-shim/lib/event-wrapper.js");
__d(56 /* base64-js/lib/b64.js */, function(global, require, module, exports) {var lookup='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

;(function(exports){
'use strict';

var Arr=typeof Uint8Array!=='undefined'?
Uint8Array:
Array;

var PLUS='+'.charCodeAt(0);
var SLASH='/'.charCodeAt(0);
var NUMBER='0'.charCodeAt(0);
var LOWER='a'.charCodeAt(0);
var UPPER='A'.charCodeAt(0);
var PLUS_URL_SAFE='-'.charCodeAt(0);
var SLASH_URL_SAFE='_'.charCodeAt(0);

function decode(elt){
var code=elt.charCodeAt(0);
if(code===PLUS||
code===PLUS_URL_SAFE)
return 62;
if(code===SLASH||
code===SLASH_URL_SAFE)
return 63;
if(code<NUMBER)
return-1;
if(code<NUMBER+10)
return code-NUMBER+26+26;
if(code<UPPER+26)
return code-UPPER;
if(code<LOWER+26)
return code-LOWER+26;
}

function b64ToByteArray(b64){
var i,j,l,tmp,placeHolders,arr;

if(b64.length%4>0){
throw new Error('Invalid string. Length must be a multiple of 4');
}






var len=b64.length;
placeHolders='='===b64.charAt(len-2)?2:'='===b64.charAt(len-1)?1:0;


arr=new Arr(b64.length*3/4-placeHolders);


l=placeHolders>0?b64.length-4:b64.length;

var L=0;

function push(v){
arr[L++]=v;
}

for(i=0,j=0;i<l;i+=4,j+=3){
tmp=decode(b64.charAt(i))<<18|decode(b64.charAt(i+1))<<12|decode(b64.charAt(i+2))<<6|decode(b64.charAt(i+3));
push((tmp&0xFF0000)>>16);
push((tmp&0xFF00)>>8);
push(tmp&0xFF);
}

if(placeHolders===2){
tmp=decode(b64.charAt(i))<<2|decode(b64.charAt(i+1))>>4;
push(tmp&0xFF);
}else if(placeHolders===1){
tmp=decode(b64.charAt(i))<<10|decode(b64.charAt(i+1))<<4|decode(b64.charAt(i+2))>>2;
push(tmp>>8&0xFF);
push(tmp&0xFF);
}

return arr;
}

function uint8ToBase64(uint8){
var i,
extraBytes=uint8.length%3,
output="",
temp,length;

function encode(num){
return lookup.charAt(num);
}

function tripletToBase64(num){
return encode(num>>18&0x3F)+encode(num>>12&0x3F)+encode(num>>6&0x3F)+encode(num&0x3F);
}


for(i=0,length=uint8.length-extraBytes;i<length;i+=3){
temp=(uint8[i]<<16)+(uint8[i+1]<<8)+uint8[i+2];
output+=tripletToBase64(temp);
}


switch(extraBytes){
case 1:
temp=uint8[uint8.length-1];
output+=encode(temp>>2);
output+=encode(temp<<4&0x3F);
output+='==';
break;
case 2:
temp=(uint8[uint8.length-2]<<8)+uint8[uint8.length-1];
output+=encode(temp>>10);
output+=encode(temp>>4&0x3F);
output+=encode(temp<<2&0x3F);
output+='=';
break;}


return output;
}

exports.toByteArray=b64ToByteArray;
exports.fromByteArray=uint8ToBase64;
})(typeof exports==='undefined'?this.base64js={}:exports);
}, "base64-js/lib/b64.js");
__d(57 /* HMRLoadingView */, function(global, require, module, exports) {











'use strict';

var ToastAndroid=require(58 /* ToastAndroid */);

var TOAST_SHORT_DELAY=2000;var

HMRLoadingView=function(){function HMRLoadingView(){babelHelpers.classCallCheck(this,HMRLoadingView);}babelHelpers.createClass(HMRLoadingView,null,[{key:'showMessage',value:function showMessage(


message){
if(HMRLoadingView._showing){
return;
}
ToastAndroid.show(message,ToastAndroid.SHORT);
HMRLoadingView._showing=true;
setTimeout(function(){
HMRLoadingView._showing=false;
},TOAST_SHORT_DELAY);
}},{key:'hide',value:function hide()

{

}}]);return HMRLoadingView;}();


module.exports=HMRLoadingView;
}, "HMRLoadingView");
__d(58 /* ToastAndroid */, function(global, require, module, exports) {










'use strict';

var RCTToastAndroid=require(14 /* NativeModules */).ToastAndroid;












var ToastAndroid={


SHORT:RCTToastAndroid.SHORT,
LONG:RCTToastAndroid.LONG,


TOP:RCTToastAndroid.TOP,
BOTTOM:RCTToastAndroid.BOTTOM,
CENTER:RCTToastAndroid.CENTER,

show:function show(
message,
duration)
{
RCTToastAndroid.show(message,duration);
},

showWithGravity:function showWithGravity(
message,
duration,
gravity)
{
RCTToastAndroid.showWithGravity(message,duration,gravity);
}};


module.exports=ToastAndroid;
}, "ToastAndroid");
__d(59 /* flattenStyle */, function(global, require, module, exports) {










'use strict';

var ReactNativePropRegistry=require(60 /* react/lib/ReactNativePropRegistry */);
var invariant=require(22 /* fbjs/lib/invariant */);



function getStyle(style){
if(typeof style==='number'){
return ReactNativePropRegistry.getByID(style);
}
return style;
}

function flattenStyle(style){
if(!style){
return undefined;
}
invariant(style!==true,'style may be false but not true');

if(!Array.isArray(style)){
return getStyle(style);
}

var result={};
for(var i=0,styleLength=style.length;i<styleLength;++i){
var computedStyle=flattenStyle(style[i]);
if(computedStyle){
for(var key in computedStyle){
result[key]=computedStyle[key];
}
}
}
return result;
}

module.exports=flattenStyle;
}, "flattenStyle");
__d(60 /* react/lib/ReactNativePropRegistry.js */, function(global, require, module, exports) {










'use strict';

function _classCallCheck(instance,Constructor){if(!(instance instanceof Constructor)){throw new TypeError("Cannot call a class as a function");}}

var objects={};
var uniqueID=1;
var emptyObject={};

var ReactNativePropRegistry=function(){
function ReactNativePropRegistry(){
_classCallCheck(this,ReactNativePropRegistry);
}

ReactNativePropRegistry.register=function register(object){
var id=++uniqueID;
if(process.env.NODE_ENV!=='production'){
Object.freeze(object);
}
objects[id]=object;
return id;
};

ReactNativePropRegistry.getByID=function getByID(id){
if(!id){


return emptyObject;
}

var object=objects[id];
if(!object){
console.warn('Invalid style with id `'+id+'`. Skipping ...');
return emptyObject;
}
return object;
};

return ReactNativePropRegistry;
}();

module.exports=ReactNativePropRegistry;
}, "react/lib/ReactNativePropRegistry.js");
__d(61 /* react/lib/ReactNativeComponentTree.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var invariant=require(22 /* fbjs/lib/invariant */);

var instanceCache={};








function getRenderedHostOrTextFromComponent(component){
var rendered;
while(rendered=component._renderedComponent){
component=rendered;
}
return component;
}





function precacheNode(inst,tag){
var nativeInst=getRenderedHostOrTextFromComponent(inst);
instanceCache[tag]=nativeInst;
}

function uncacheNode(inst){
var tag=inst._rootNodeID;
if(tag){
delete instanceCache[tag];
}
}

function getInstanceFromTag(tag){
return instanceCache[tag]||null;
}

function getTagFromInstance(inst){
!inst._rootNodeID?process.env.NODE_ENV!=='production'?invariant(false,'All native instances should have a tag.'):_prodInvariant('17'):void 0;
return inst._rootNodeID;
}

var ReactNativeComponentTree={
getClosestInstanceFromNode:getInstanceFromTag,
getInstanceFromNode:getInstanceFromTag,
getNodeFromInstance:getTagFromInstance,
precacheNode:precacheNode,
uncacheNode:uncacheNode};


module.exports=ReactNativeComponentTree;
}, "react/lib/ReactNativeComponentTree.js");
__d(62 /* react/lib/ReactNativeMount.js */, function(global, require, module, exports) {










'use strict';

var ReactElement=require(63 /* ./ReactElement */);
var ReactInstrumentation=require(66 /* ./ReactInstrumentation */);
var ReactNativeContainerInfo=require(67 /* ./ReactNativeContainerInfo */);
var ReactNativeTagHandles=require(68 /* ./ReactNativeTagHandles */);
var ReactReconciler=require(69 /* ./ReactReconciler */);
var ReactUpdateQueue=require(72 /* ./ReactUpdateQueue */);
var ReactUpdates=require(74 /* ./ReactUpdates */);
var UIManager=require(79 /* react-native/lib/UIManager */);

var emptyObject=require(82 /* fbjs/lib/emptyObject */);
var instantiateReactComponent=require(83 /* ./instantiateReactComponent */);
var shouldUpdateReactComponent=require(93 /* ./shouldUpdateReactComponent */);






var TopLevelWrapper=function TopLevelWrapper(){};
TopLevelWrapper.prototype.isReactComponent={};
if(process.env.NODE_ENV!=='production'){
TopLevelWrapper.displayName='TopLevelWrapper';
}
TopLevelWrapper.prototype.render=function(){

return this.props;
};









function mountComponentIntoNode(componentInstance,containerTag,transaction){
var markup=ReactReconciler.mountComponent(componentInstance,transaction,null,ReactNativeContainerInfo(containerTag),emptyObject,0);

componentInstance._renderedComponent._topLevelWrapper=componentInstance;
ReactNativeMount._mountImageIntoNode(markup,containerTag);
}








function batchedMountComponentIntoNode(componentInstance,containerTag){
var transaction=ReactUpdates.ReactReconcileTransaction.getPooled();
transaction.perform(mountComponentIntoNode,null,componentInstance,containerTag,transaction);
ReactUpdates.ReactReconcileTransaction.release(transaction);
}





var ReactNativeMount={
_instancesByContainerID:{},


findNodeHandle:require(81 /* ./findNodeHandle */),





renderComponent:function renderComponent(nextElement,containerTag,callback){
var nextWrappedElement=new ReactElement(TopLevelWrapper,null,null,null,null,null,nextElement);

var topRootNodeID=containerTag;
var prevComponent=ReactNativeMount._instancesByContainerID[topRootNodeID];
if(prevComponent){
var prevWrappedElement=prevComponent._currentElement;
var prevElement=prevWrappedElement.props;
if(shouldUpdateReactComponent(prevElement,nextElement)){
ReactUpdateQueue.enqueueElementInternal(prevComponent,nextWrappedElement,emptyObject);
if(callback){
ReactUpdateQueue.enqueueCallbackInternal(prevComponent,callback);
}
return prevComponent;
}else{
ReactNativeMount.unmountComponentAtNode(containerTag);
}
}

if(!ReactNativeTagHandles.reactTagIsNativeTopRootID(containerTag)){
console.error('You cannot render into anything but a top root');
return null;
}

ReactNativeTagHandles.assertRootTag(containerTag);

var instance=instantiateReactComponent(nextWrappedElement,false);
ReactNativeMount._instancesByContainerID[containerTag]=instance;





ReactUpdates.batchedUpdates(batchedMountComponentIntoNode,instance,containerTag);
var component=instance.getPublicInstance();
if(callback){
callback.call(component);
}
return component;
},





_mountImageIntoNode:function _mountImageIntoNode(mountImage,containerID){


var childTag=mountImage;
UIManager.setChildren(containerID,[childTag]);
},









unmountComponentAtNodeAndRemoveContainer:function unmountComponentAtNodeAndRemoveContainer(containerTag){
ReactNativeMount.unmountComponentAtNode(containerTag);

UIManager.removeRootView(containerTag);
},






unmountComponentAtNode:function unmountComponentAtNode(containerTag){
if(!ReactNativeTagHandles.reactTagIsNativeTopRootID(containerTag)){
console.error('You cannot render into anything but a top root');
return false;
}

var instance=ReactNativeMount._instancesByContainerID[containerTag];
if(!instance){
return false;
}
if(process.env.NODE_ENV!=='production'){
ReactInstrumentation.debugTool.onBeginFlush();
}
ReactNativeMount.unmountComponentFromNode(instance,containerTag);
delete ReactNativeMount._instancesByContainerID[containerTag];
if(process.env.NODE_ENV!=='production'){
ReactInstrumentation.debugTool.onEndFlush();
}
return true;
},










unmountComponentFromNode:function unmountComponentFromNode(instance,containerID){

ReactReconciler.unmountComponent(instance);
UIManager.removeSubviewsFromContainerWithID(containerID);
}};



module.exports=ReactNativeMount;
}, "react/lib/ReactNativeMount.js");
__d(63 /* react/lib/ReactElement.js */, function(global, require, module, exports) {










'use strict';

var _assign=require(64 /* object-assign */);

var ReactCurrentOwner=require(21 /* ./ReactCurrentOwner */);

var warning=require(23 /* fbjs/lib/warning */);
var canDefineProperty=require(65 /* ./canDefineProperty */);
var hasOwnProperty=Object.prototype.hasOwnProperty;



var REACT_ELEMENT_TYPE=typeof Symbol==='function'&&Symbol['for']&&Symbol['for']('react.element')||0xeac7;

var RESERVED_PROPS={
key:true,
ref:true,
__self:true,
__source:true};


var specialPropKeyWarningShown,specialPropRefWarningShown;

function hasValidRef(config){
if(process.env.NODE_ENV!=='production'){
if(hasOwnProperty.call(config,'ref')){
var getter=Object.getOwnPropertyDescriptor(config,'ref').get;
if(getter&&getter.isReactWarning){
return false;
}
}
}
return config.ref!==undefined;
}

function hasValidKey(config){
if(process.env.NODE_ENV!=='production'){
if(hasOwnProperty.call(config,'key')){
var getter=Object.getOwnPropertyDescriptor(config,'key').get;
if(getter&&getter.isReactWarning){
return false;
}
}
}
return config.key!==undefined;
}

function defineKeyPropWarningGetter(props,displayName){
var warnAboutAccessingKey=function warnAboutAccessingKey(){
if(!specialPropKeyWarningShown){
specialPropKeyWarningShown=true;
process.env.NODE_ENV!=='production'?warning(false,'%s: `key` is not a prop. Trying to access it will result '+'in `undefined` being returned. If you need to access the same '+'value within the child component, you should pass it as a different '+'prop. (https://fb.me/react-special-props)',displayName):void 0;
}
};
warnAboutAccessingKey.isReactWarning=true;
Object.defineProperty(props,'key',{
get:warnAboutAccessingKey,
configurable:true});

}

function defineRefPropWarningGetter(props,displayName){
var warnAboutAccessingRef=function warnAboutAccessingRef(){
if(!specialPropRefWarningShown){
specialPropRefWarningShown=true;
process.env.NODE_ENV!=='production'?warning(false,'%s: `ref` is not a prop. Trying to access it will result '+'in `undefined` being returned. If you need to access the same '+'value within the child component, you should pass it as a different '+'prop. (https://fb.me/react-special-props)',displayName):void 0;
}
};
warnAboutAccessingRef.isReactWarning=true;
Object.defineProperty(props,'ref',{
get:warnAboutAccessingRef,
configurable:true});

}





















var ReactElement=function ReactElement(type,key,ref,self,source,owner,props){
var element={

$$typeof:REACT_ELEMENT_TYPE,


type:type,
key:key,
ref:ref,
props:props,


_owner:owner};


if(process.env.NODE_ENV!=='production'){




element._store={};
var shadowChildren=Array.isArray(props.children)?props.children.slice(0):props.children;





if(canDefineProperty){
Object.defineProperty(element._store,'validated',{
configurable:false,
enumerable:false,
writable:true,
value:false});


Object.defineProperty(element,'_self',{
configurable:false,
enumerable:false,
writable:false,
value:self});

Object.defineProperty(element,'_shadowChildren',{
configurable:false,
enumerable:false,
writable:false,
value:shadowChildren});



Object.defineProperty(element,'_source',{
configurable:false,
enumerable:false,
writable:false,
value:source});

}else{
element._store.validated=false;
element._self=self;
element._shadowChildren=shadowChildren;
element._source=source;
}
if(Object.freeze){
Object.freeze(element.props);
Object.freeze(element);
}
}

return element;
};





ReactElement.createElement=function(type,config,children){
var propName;


var props={};

var key=null;
var ref=null;
var self=null;
var source=null;

if(config!=null){
if(hasValidRef(config)){
ref=config.ref;
}
if(hasValidKey(config)){
key=''+config.key;
}

self=config.__self===undefined?null:config.__self;
source=config.__source===undefined?null:config.__source;

for(propName in config){
if(hasOwnProperty.call(config,propName)&&!RESERVED_PROPS.hasOwnProperty(propName)){
props[propName]=config[propName];
}
}
}



var childrenLength=arguments.length-2;
if(childrenLength===1){
props.children=children;
}else if(childrenLength>1){
var childArray=Array(childrenLength);
for(var i=0;i<childrenLength;i++){
childArray[i]=arguments[i+2];
}
props.children=childArray;
}


if(type&&type.defaultProps){
var defaultProps=type.defaultProps;
for(propName in defaultProps){
if(props[propName]===undefined){
props[propName]=defaultProps[propName];
}
}
}
if(process.env.NODE_ENV!=='production'){
if(key||ref){
if(typeof props.$$typeof==='undefined'||props.$$typeof!==REACT_ELEMENT_TYPE){
var displayName=typeof type==='function'?type.displayName||type.name||'Unknown':type;
if(key){
defineKeyPropWarningGetter(props,displayName);
}
if(ref){
defineRefPropWarningGetter(props,displayName);
}
}
}
}
return ReactElement(type,key,ref,self,source,ReactCurrentOwner.current,props);
};





ReactElement.createFactory=function(type){
var factory=ReactElement.createElement.bind(null,type);





factory.type=type;
return factory;
};

ReactElement.cloneAndReplaceKey=function(oldElement,newKey){
var newElement=ReactElement(oldElement.type,newKey,oldElement.ref,oldElement._self,oldElement._source,oldElement._owner,oldElement.props);

return newElement;
};





ReactElement.cloneElement=function(element,config,children){
var propName;


var props=_assign({},element.props);


var key=element.key;
var ref=element.ref;

var self=element._self;



var source=element._source;


var owner=element._owner;

if(config!=null){
if(hasValidRef(config)){

ref=config.ref;
owner=ReactCurrentOwner.current;
}
if(hasValidKey(config)){
key=''+config.key;
}


var defaultProps;
if(element.type&&element.type.defaultProps){
defaultProps=element.type.defaultProps;
}
for(propName in config){
if(hasOwnProperty.call(config,propName)&&!RESERVED_PROPS.hasOwnProperty(propName)){
if(config[propName]===undefined&&defaultProps!==undefined){

props[propName]=defaultProps[propName];
}else{
props[propName]=config[propName];
}
}
}
}



var childrenLength=arguments.length-2;
if(childrenLength===1){
props.children=children;
}else if(childrenLength>1){
var childArray=Array(childrenLength);
for(var i=0;i<childrenLength;i++){
childArray[i]=arguments[i+2];
}
props.children=childArray;
}

return ReactElement(element.type,key,ref,self,source,owner,props);
};








ReactElement.isValidElement=function(object){
return typeof object==='object'&&object!==null&&object.$$typeof===REACT_ELEMENT_TYPE;
};

ReactElement.REACT_ELEMENT_TYPE=REACT_ELEMENT_TYPE;

module.exports=ReactElement;
}, "react/lib/ReactElement.js");
__d(64 /* object-assign/index.js */, function(global, require, module, exports) {'use strict';

var hasOwnProperty=Object.prototype.hasOwnProperty;
var propIsEnumerable=Object.prototype.propertyIsEnumerable;

function toObject(val){
if(val===null||val===undefined){
throw new TypeError('Object.assign cannot be called with null or undefined');
}

return Object(val);
}

function shouldUseNative(){
try{
if(!Object.assign){
return false;
}




var test1=new String('abc');
test1[5]='de';
if(Object.getOwnPropertyNames(test1)[0]==='5'){
return false;
}


var test2={};
for(var i=0;i<10;i++){
test2['_'+String.fromCharCode(i)]=i;
}
var order2=Object.getOwnPropertyNames(test2).map(function(n){
return test2[n];
});
if(order2.join('')!=='0123456789'){
return false;
}


var test3={};
'abcdefghijklmnopqrst'.split('').forEach(function(letter){
test3[letter]=letter;
});
if(Object.keys(babelHelpers.extends({},test3)).join('')!==
'abcdefghijklmnopqrst'){
return false;
}

return true;
}catch(e){

return false;
}
}

module.exports=shouldUseNative()?Object.assign:function(target,source){
var from;
var to=toObject(target);
var symbols;

for(var s=1;s<arguments.length;s++){
from=Object(arguments[s]);

for(var key in from){
if(hasOwnProperty.call(from,key)){
to[key]=from[key];
}
}

if(Object.getOwnPropertySymbols){
symbols=Object.getOwnPropertySymbols(from);
for(var i=0;i<symbols.length;i++){
if(propIsEnumerable.call(from,symbols[i])){
to[symbols[i]]=from[symbols[i]];
}
}
}
}

return to;
};
}, "object-assign/index.js");
__d(65 /* react/lib/canDefineProperty.js */, function(global, require, module, exports) {










'use strict';

var canDefineProperty=false;
if(process.env.NODE_ENV!=='production'){
try{
Object.defineProperty({},'x',{get:function get(){}});
canDefineProperty=true;
}catch(x){

}
}

module.exports=canDefineProperty;
}, "react/lib/canDefineProperty.js");
__d(66 /* react/lib/ReactInstrumentation.js */, function(global, require, module, exports) {










'use strict';

var debugTool=null;

if(process.env.NODE_ENV!=='production'){
var ReactDebugTool=require(25 /* ./ReactDebugTool */);
debugTool=ReactDebugTool;
}

module.exports={debugTool:debugTool};
}, "react/lib/ReactInstrumentation.js");
__d(67 /* react/lib/ReactNativeContainerInfo.js */, function(global, require, module, exports) {










'use strict';

function ReactNativeContainerInfo(tag){
var info={
_tag:tag};

return info;
}

module.exports=ReactNativeContainerInfo;
}, "react/lib/ReactNativeContainerInfo.js");
__d(68 /* react/lib/ReactNativeTagHandles.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var invariant=require(22 /* fbjs/lib/invariant */);














var INITIAL_TAG_COUNT=1;
var ReactNativeTagHandles={
tagsStartAt:INITIAL_TAG_COUNT,
tagCount:INITIAL_TAG_COUNT,

allocateTag:function allocateTag(){

while(this.reactTagIsNativeTopRootID(ReactNativeTagHandles.tagCount)){
ReactNativeTagHandles.tagCount++;
}
var tag=ReactNativeTagHandles.tagCount;
ReactNativeTagHandles.tagCount++;
return tag;
},

assertRootTag:function assertRootTag(tag){
!this.reactTagIsNativeTopRootID(tag)?process.env.NODE_ENV!=='production'?invariant(false,'Expect a native root tag, instead got %s',tag):_prodInvariant('19',tag):void 0;
},

reactTagIsNativeTopRootID:function reactTagIsNativeTopRootID(reactTag){

return reactTag%10===1;
}};


module.exports=ReactNativeTagHandles;
}, "react/lib/ReactNativeTagHandles.js");
__d(69 /* react/lib/ReactReconciler.js */, function(global, require, module, exports) {










'use strict';

var ReactRef=require(70 /* ./ReactRef */);
var ReactInstrumentation=require(66 /* ./ReactInstrumentation */);

var warning=require(23 /* fbjs/lib/warning */);





function attachRefs(){
ReactRef.attachRefs(this,this._currentElement);
}

var ReactReconciler={












mountComponent:function mountComponent(internalInstance,transaction,hostParent,hostContainerInfo,context,parentDebugID)
{
if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onBeforeMountComponent(internalInstance._debugID,internalInstance._currentElement,parentDebugID);
}
}
var markup=internalInstance.mountComponent(transaction,hostParent,hostContainerInfo,context,parentDebugID);
if(internalInstance._currentElement&&internalInstance._currentElement.ref!=null){
transaction.getReactMountReady().enqueue(attachRefs,internalInstance);
}
if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onMountComponent(internalInstance._debugID);
}
}
return markup;
},





getHostNode:function getHostNode(internalInstance){
return internalInstance.getHostNode();
},







unmountComponent:function unmountComponent(internalInstance,safely){
if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onBeforeUnmountComponent(internalInstance._debugID);
}
}
ReactRef.detachRefs(internalInstance,internalInstance._currentElement);
internalInstance.unmountComponent(safely);
if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onUnmountComponent(internalInstance._debugID);
}
}
},










receiveComponent:function receiveComponent(internalInstance,nextElement,transaction,context){
var prevElement=internalInstance._currentElement;

if(nextElement===prevElement&&context===internalInstance._context){










return;
}

if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onBeforeUpdateComponent(internalInstance._debugID,nextElement);
}
}

var refsChanged=ReactRef.shouldUpdateRefs(prevElement,nextElement);

if(refsChanged){
ReactRef.detachRefs(internalInstance,prevElement);
}

internalInstance.receiveComponent(nextElement,transaction,context);

if(refsChanged&&internalInstance._currentElement&&internalInstance._currentElement.ref!=null){
transaction.getReactMountReady().enqueue(attachRefs,internalInstance);
}

if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onUpdateComponent(internalInstance._debugID);
}
}
},








performUpdateIfNecessary:function performUpdateIfNecessary(internalInstance,transaction,updateBatchNumber){
if(internalInstance._updateBatchNumber!==updateBatchNumber){


process.env.NODE_ENV!=='production'?warning(internalInstance._updateBatchNumber==null||internalInstance._updateBatchNumber===updateBatchNumber+1,'performUpdateIfNecessary: Unexpected batch number (current %s, '+'pending %s)',updateBatchNumber,internalInstance._updateBatchNumber):void 0;
return;
}
if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onBeforeUpdateComponent(internalInstance._debugID,internalInstance._currentElement);
}
}
internalInstance.performUpdateIfNecessary(transaction);
if(process.env.NODE_ENV!=='production'){
if(internalInstance._debugID!==0){
ReactInstrumentation.debugTool.onUpdateComponent(internalInstance._debugID);
}
}
}};



module.exports=ReactReconciler;
}, "react/lib/ReactReconciler.js");
__d(70 /* react/lib/ReactRef.js */, function(global, require, module, exports) {










'use strict';

var ReactOwner=require(71 /* ./ReactOwner */);

var ReactRef={};

function attachRef(ref,component,owner){
if(typeof ref==='function'){
ref(component.getPublicInstance());
}else{

ReactOwner.addComponentAsRefTo(component,ref,owner);
}
}

function detachRef(ref,component,owner){
if(typeof ref==='function'){
ref(null);
}else{

ReactOwner.removeComponentAsRefFrom(component,ref,owner);
}
}

ReactRef.attachRefs=function(instance,element){
if(element===null||element===false){
return;
}
var ref=element.ref;
if(ref!=null){
attachRef(ref,instance,element._owner);
}
};

ReactRef.shouldUpdateRefs=function(prevElement,nextElement){












var prevEmpty=prevElement===null||prevElement===false;
var nextEmpty=nextElement===null||nextElement===false;

return(

prevEmpty||nextEmpty||nextElement.ref!==prevElement.ref||

typeof nextElement.ref==='string'&&nextElement._owner!==prevElement._owner);

};

ReactRef.detachRefs=function(instance,element){
if(element===null||element===false){
return;
}
var ref=element.ref;
if(ref!=null){
detachRef(ref,instance,element._owner);
}
};

module.exports=ReactRef;
}, "react/lib/ReactRef.js");
__d(71 /* react/lib/ReactOwner.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var invariant=require(22 /* fbjs/lib/invariant */);































var ReactOwner={






isValidOwner:function isValidOwner(object){
return!!(object&&typeof object.attachRef==='function'&&typeof object.detachRef==='function');
},










addComponentAsRefTo:function addComponentAsRefTo(component,ref,owner){
!ReactOwner.isValidOwner(owner)?process.env.NODE_ENV!=='production'?invariant(false,'addComponentAsRefTo(...): Only a ReactOwner can have refs. You might be adding a ref to a component that was not created inside a component\'s `render` method, or you have multiple copies of React loaded (details: https://fb.me/react-refs-must-have-owner).'):_prodInvariant('119'):void 0;
owner.attachRef(ref,component);
},










removeComponentAsRefFrom:function removeComponentAsRefFrom(component,ref,owner){
!ReactOwner.isValidOwner(owner)?process.env.NODE_ENV!=='production'?invariant(false,'removeComponentAsRefFrom(...): Only a ReactOwner can have refs. You might be removing a ref to a component that was not created inside a component\'s `render` method, or you have multiple copies of React loaded (details: https://fb.me/react-refs-must-have-owner).'):_prodInvariant('120'):void 0;
var ownerPublicInstance=owner.getPublicInstance();


if(ownerPublicInstance&&ownerPublicInstance.refs[ref]===component.getPublicInstance()){
owner.detachRef(ref);
}
}};



module.exports=ReactOwner;
}, "react/lib/ReactOwner.js");
__d(72 /* react/lib/ReactUpdateQueue.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var ReactCurrentOwner=require(21 /* ./ReactCurrentOwner */);
var ReactInstanceMap=require(73 /* ./ReactInstanceMap */);
var ReactInstrumentation=require(66 /* ./ReactInstrumentation */);
var ReactUpdates=require(74 /* ./ReactUpdates */);

var invariant=require(22 /* fbjs/lib/invariant */);
var warning=require(23 /* fbjs/lib/warning */);

function enqueueUpdate(internalInstance){
ReactUpdates.enqueueUpdate(internalInstance);
}

function formatUnexpectedArgument(arg){
var type=typeof arg;
if(type!=='object'){
return type;
}
var displayName=arg.constructor&&arg.constructor.name||type;
var keys=Object.keys(arg);
if(keys.length>0&&keys.length<20){
return displayName+' (keys: '+keys.join(', ')+')';
}
return displayName;
}

function getInternalInstanceReadyForUpdate(publicInstance,callerName){
var internalInstance=ReactInstanceMap.get(publicInstance);
if(!internalInstance){
if(process.env.NODE_ENV!=='production'){
var ctor=publicInstance.constructor;



process.env.NODE_ENV!=='production'?warning(!callerName,'%s(...): Can only update a mounted or mounting component. '+'This usually means you called %s() on an unmounted component. '+'This is a no-op. Please check the code for the %s component.',callerName,callerName,ctor&&(ctor.displayName||ctor.name)||'ReactClass'):void 0;
}
return null;
}

if(process.env.NODE_ENV!=='production'){
process.env.NODE_ENV!=='production'?warning(ReactCurrentOwner.current==null,'%s(...): Cannot update during an existing state transition (such as '+'within `render` or another component\'s constructor). Render methods '+'should be a pure function of props and state; constructor '+'side-effects are an anti-pattern, but can be moved to '+'`componentWillMount`.',callerName):void 0;
}

return internalInstance;
}





var ReactUpdateQueue={








isMounted:function isMounted(publicInstance){
if(process.env.NODE_ENV!=='production'){
var owner=ReactCurrentOwner.current;
if(owner!==null){
process.env.NODE_ENV!=='production'?warning(owner._warnedAboutRefsInRender,'%s is accessing isMounted inside its render() function. '+'render() should be a pure function of props and state. It should '+'never access something that requires stale data from the previous '+'render, such as refs. Move this logic to componentDidMount and '+'componentDidUpdate instead.',owner.getName()||'A component'):void 0;
owner._warnedAboutRefsInRender=true;
}
}
var internalInstance=ReactInstanceMap.get(publicInstance);
if(internalInstance){



return!!internalInstance._renderedComponent;
}else{
return false;
}
},










enqueueCallback:function enqueueCallback(publicInstance,callback,callerName){
ReactUpdateQueue.validateCallback(callback,callerName);
var internalInstance=getInternalInstanceReadyForUpdate(publicInstance);






if(!internalInstance){
return null;
}

if(internalInstance._pendingCallbacks){
internalInstance._pendingCallbacks.push(callback);
}else{
internalInstance._pendingCallbacks=[callback];
}




enqueueUpdate(internalInstance);
},

enqueueCallbackInternal:function enqueueCallbackInternal(internalInstance,callback){
if(internalInstance._pendingCallbacks){
internalInstance._pendingCallbacks.push(callback);
}else{
internalInstance._pendingCallbacks=[callback];
}
enqueueUpdate(internalInstance);
},














enqueueForceUpdate:function enqueueForceUpdate(publicInstance){
var internalInstance=getInternalInstanceReadyForUpdate(publicInstance,'forceUpdate');

if(!internalInstance){
return;
}

internalInstance._pendingForceUpdate=true;

enqueueUpdate(internalInstance);
},












enqueueReplaceState:function enqueueReplaceState(publicInstance,completeState){
var internalInstance=getInternalInstanceReadyForUpdate(publicInstance,'replaceState');

if(!internalInstance){
return;
}

internalInstance._pendingStateQueue=[completeState];
internalInstance._pendingReplaceState=true;

enqueueUpdate(internalInstance);
},











enqueueSetState:function enqueueSetState(publicInstance,partialState){
if(process.env.NODE_ENV!=='production'){
ReactInstrumentation.debugTool.onSetState();
process.env.NODE_ENV!=='production'?warning(partialState!=null,'setState(...): You passed an undefined or null state object; '+'instead, use forceUpdate().'):void 0;
}

var internalInstance=getInternalInstanceReadyForUpdate(publicInstance,'setState');

if(!internalInstance){
return;
}

var queue=internalInstance._pendingStateQueue||(internalInstance._pendingStateQueue=[]);
queue.push(partialState);

enqueueUpdate(internalInstance);
},

enqueueElementInternal:function enqueueElementInternal(internalInstance,nextElement,nextContext){
internalInstance._pendingElement=nextElement;

internalInstance._context=nextContext;
enqueueUpdate(internalInstance);
},

validateCallback:function validateCallback(callback,callerName){
!(!callback||typeof callback==='function')?process.env.NODE_ENV!=='production'?invariant(false,'%s(...): Expected the last optional `callback` argument to be a function. Instead received: %s.',callerName,formatUnexpectedArgument(callback)):_prodInvariant('122',callerName,formatUnexpectedArgument(callback)):void 0;
}};



module.exports=ReactUpdateQueue;
}, "react/lib/ReactUpdateQueue.js");
__d(73 /* react/lib/ReactInstanceMap.js */, function(global, require, module, exports) {










'use strict';










var ReactInstanceMap={






remove:function remove(key){
key._reactInternalInstance=undefined;
},

get:function get(key){
return key._reactInternalInstance;
},

has:function has(key){
return key._reactInternalInstance!==undefined;
},

set:function set(key,value){
key._reactInternalInstance=value;
}};



module.exports=ReactInstanceMap;
}, "react/lib/ReactInstanceMap.js");
__d(74 /* react/lib/ReactUpdates.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */),
_assign=require(64 /* object-assign */);

var CallbackQueue=require(75 /* ./CallbackQueue */);
var PooledClass=require(76 /* ./PooledClass */);
var ReactFeatureFlags=require(77 /* ./ReactFeatureFlags */);
var ReactReconciler=require(69 /* ./ReactReconciler */);
var Transaction=require(78 /* ./Transaction */);

var invariant=require(22 /* fbjs/lib/invariant */);

var dirtyComponents=[];
var updateBatchNumber=0;
var asapCallbackQueue=CallbackQueue.getPooled();
var asapEnqueued=false;

var batchingStrategy=null;

function ensureInjected(){
!(ReactUpdates.ReactReconcileTransaction&&batchingStrategy)?process.env.NODE_ENV!=='production'?invariant(false,'ReactUpdates: must inject a reconcile transaction class and batching strategy'):_prodInvariant('123'):void 0;
}

var NESTED_UPDATES={
initialize:function initialize(){
this.dirtyComponentsLength=dirtyComponents.length;
},
close:function close(){
if(this.dirtyComponentsLength!==dirtyComponents.length){





dirtyComponents.splice(0,this.dirtyComponentsLength);
flushBatchedUpdates();
}else{
dirtyComponents.length=0;
}
}};


var UPDATE_QUEUEING={
initialize:function initialize(){
this.callbackQueue.reset();
},
close:function close(){
this.callbackQueue.notifyAll();
}};


var TRANSACTION_WRAPPERS=[NESTED_UPDATES,UPDATE_QUEUEING];

function ReactUpdatesFlushTransaction(){
this.reinitializeTransaction();
this.dirtyComponentsLength=null;
this.callbackQueue=CallbackQueue.getPooled();
this.reconcileTransaction=ReactUpdates.ReactReconcileTransaction.getPooled(
true);
}

_assign(ReactUpdatesFlushTransaction.prototype,Transaction.Mixin,{
getTransactionWrappers:function getTransactionWrappers(){
return TRANSACTION_WRAPPERS;
},

destructor:function destructor(){
this.dirtyComponentsLength=null;
CallbackQueue.release(this.callbackQueue);
this.callbackQueue=null;
ReactUpdates.ReactReconcileTransaction.release(this.reconcileTransaction);
this.reconcileTransaction=null;
},

perform:function perform(method,scope,a){


return Transaction.Mixin.perform.call(this,this.reconcileTransaction.perform,this.reconcileTransaction,method,scope,a);
}});


PooledClass.addPoolingTo(ReactUpdatesFlushTransaction);

function batchedUpdates(callback,a,b,c,d,e){
ensureInjected();
batchingStrategy.batchedUpdates(callback,a,b,c,d,e);
}








function mountOrderComparator(c1,c2){
return c1._mountOrder-c2._mountOrder;
}

function runBatchedUpdates(transaction){
var len=transaction.dirtyComponentsLength;
!(len===dirtyComponents.length)?process.env.NODE_ENV!=='production'?invariant(false,'Expected flush transaction\'s stored dirty-components length (%s) to match dirty-components array length (%s).',len,dirtyComponents.length):_prodInvariant('124',len,dirtyComponents.length):void 0;




dirtyComponents.sort(mountOrderComparator);






updateBatchNumber++;

for(var i=0;i<len;i++){



var component=dirtyComponents[i];




var callbacks=component._pendingCallbacks;
component._pendingCallbacks=null;

var markerName;
if(ReactFeatureFlags.logTopLevelRenders){
var namedComponent=component;

if(component._currentElement.props===component._renderedComponent._currentElement){
namedComponent=component._renderedComponent;
}
markerName='React update: '+namedComponent.getName();
console.time(markerName);
}

ReactReconciler.performUpdateIfNecessary(component,transaction.reconcileTransaction,updateBatchNumber);

if(markerName){
console.timeEnd(markerName);
}

if(callbacks){
for(var j=0;j<callbacks.length;j++){
transaction.callbackQueue.enqueue(callbacks[j],component.getPublicInstance());
}
}
}
}

var flushBatchedUpdates=function flushBatchedUpdates(){




while(dirtyComponents.length||asapEnqueued){
if(dirtyComponents.length){
var transaction=ReactUpdatesFlushTransaction.getPooled();
transaction.perform(runBatchedUpdates,null,transaction);
ReactUpdatesFlushTransaction.release(transaction);
}

if(asapEnqueued){
asapEnqueued=false;
var queue=asapCallbackQueue;
asapCallbackQueue=CallbackQueue.getPooled();
queue.notifyAll();
CallbackQueue.release(queue);
}
}
};





function enqueueUpdate(component){
ensureInjected();







if(!batchingStrategy.isBatchingUpdates){
batchingStrategy.batchedUpdates(enqueueUpdate,component);
return;
}

dirtyComponents.push(component);
if(component._updateBatchNumber==null){
component._updateBatchNumber=updateBatchNumber+1;
}
}





function asap(callback,context){
!batchingStrategy.isBatchingUpdates?process.env.NODE_ENV!=='production'?invariant(false,'ReactUpdates.asap: Can\'t enqueue an asap callback in a context whereupdates are not being batched.'):_prodInvariant('125'):void 0;
asapCallbackQueue.enqueue(callback,context);
asapEnqueued=true;
}

var ReactUpdatesInjection={
injectReconcileTransaction:function injectReconcileTransaction(ReconcileTransaction){
!ReconcileTransaction?process.env.NODE_ENV!=='production'?invariant(false,'ReactUpdates: must provide a reconcile transaction class'):_prodInvariant('126'):void 0;
ReactUpdates.ReactReconcileTransaction=ReconcileTransaction;
},

injectBatchingStrategy:function injectBatchingStrategy(_batchingStrategy){
!_batchingStrategy?process.env.NODE_ENV!=='production'?invariant(false,'ReactUpdates: must provide a batching strategy'):_prodInvariant('127'):void 0;
!(typeof _batchingStrategy.batchedUpdates==='function')?process.env.NODE_ENV!=='production'?invariant(false,'ReactUpdates: must provide a batchedUpdates() function'):_prodInvariant('128'):void 0;
!(typeof _batchingStrategy.isBatchingUpdates==='boolean')?process.env.NODE_ENV!=='production'?invariant(false,'ReactUpdates: must provide an isBatchingUpdates boolean attribute'):_prodInvariant('129'):void 0;
batchingStrategy=_batchingStrategy;
}};


var ReactUpdates={






ReactReconcileTransaction:null,

batchedUpdates:batchedUpdates,
enqueueUpdate:enqueueUpdate,
flushBatchedUpdates:flushBatchedUpdates,
injection:ReactUpdatesInjection,
asap:asap};


module.exports=ReactUpdates;
}, "react/lib/ReactUpdates.js");
__d(75 /* react/lib/CallbackQueue.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */),
_assign=require(64 /* object-assign */);

var PooledClass=require(76 /* ./PooledClass */);

var invariant=require(22 /* fbjs/lib/invariant */);












function CallbackQueue(){
this._callbacks=null;
this._contexts=null;
}

_assign(CallbackQueue.prototype,{








enqueue:function enqueue(callback,context){
this._callbacks=this._callbacks||[];
this._contexts=this._contexts||[];
this._callbacks.push(callback);
this._contexts.push(context);
},







notifyAll:function notifyAll(){
var callbacks=this._callbacks;
var contexts=this._contexts;
if(callbacks){
!(callbacks.length===contexts.length)?process.env.NODE_ENV!=='production'?invariant(false,'Mismatched list of contexts in callback queue'):_prodInvariant('24'):void 0;
this._callbacks=null;
this._contexts=null;
for(var i=0;i<callbacks.length;i++){
callbacks[i].call(contexts[i]);
}
callbacks.length=0;
contexts.length=0;
}
},

checkpoint:function checkpoint(){
return this._callbacks?this._callbacks.length:0;
},

rollback:function rollback(len){
if(this._callbacks){
this._callbacks.length=len;
this._contexts.length=len;
}
},






reset:function reset(){
this._callbacks=null;
this._contexts=null;
},




destructor:function destructor(){
this.reset();
}});



PooledClass.addPoolingTo(CallbackQueue);

module.exports=CallbackQueue;
}, "react/lib/CallbackQueue.js");
__d(76 /* react/lib/PooledClass.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var invariant=require(22 /* fbjs/lib/invariant */);








var oneArgumentPooler=function oneArgumentPooler(copyFieldsFrom){
var Klass=this;
if(Klass.instancePool.length){
var instance=Klass.instancePool.pop();
Klass.call(instance,copyFieldsFrom);
return instance;
}else{
return new Klass(copyFieldsFrom);
}
};

var twoArgumentPooler=function twoArgumentPooler(a1,a2){
var Klass=this;
if(Klass.instancePool.length){
var instance=Klass.instancePool.pop();
Klass.call(instance,a1,a2);
return instance;
}else{
return new Klass(a1,a2);
}
};

var threeArgumentPooler=function threeArgumentPooler(a1,a2,a3){
var Klass=this;
if(Klass.instancePool.length){
var instance=Klass.instancePool.pop();
Klass.call(instance,a1,a2,a3);
return instance;
}else{
return new Klass(a1,a2,a3);
}
};

var fourArgumentPooler=function fourArgumentPooler(a1,a2,a3,a4){
var Klass=this;
if(Klass.instancePool.length){
var instance=Klass.instancePool.pop();
Klass.call(instance,a1,a2,a3,a4);
return instance;
}else{
return new Klass(a1,a2,a3,a4);
}
};

var fiveArgumentPooler=function fiveArgumentPooler(a1,a2,a3,a4,a5){
var Klass=this;
if(Klass.instancePool.length){
var instance=Klass.instancePool.pop();
Klass.call(instance,a1,a2,a3,a4,a5);
return instance;
}else{
return new Klass(a1,a2,a3,a4,a5);
}
};

var standardReleaser=function standardReleaser(instance){
var Klass=this;
!(instance instanceof Klass)?process.env.NODE_ENV!=='production'?invariant(false,'Trying to release an instance into a pool of a different type.'):_prodInvariant('25'):void 0;
instance.destructor();
if(Klass.instancePool.length<Klass.poolSize){
Klass.instancePool.push(instance);
}
};

var DEFAULT_POOL_SIZE=10;
var DEFAULT_POOLER=oneArgumentPooler;










var addPoolingTo=function addPoolingTo(CopyConstructor,pooler){
var NewKlass=CopyConstructor;
NewKlass.instancePool=[];
NewKlass.getPooled=pooler||DEFAULT_POOLER;
if(!NewKlass.poolSize){
NewKlass.poolSize=DEFAULT_POOL_SIZE;
}
NewKlass.release=standardReleaser;
return NewKlass;
};

var PooledClass={
addPoolingTo:addPoolingTo,
oneArgumentPooler:oneArgumentPooler,
twoArgumentPooler:twoArgumentPooler,
threeArgumentPooler:threeArgumentPooler,
fourArgumentPooler:fourArgumentPooler,
fiveArgumentPooler:fiveArgumentPooler};


module.exports=PooledClass;
}, "react/lib/PooledClass.js");
__d(77 /* react/lib/ReactFeatureFlags.js */, function(global, require, module, exports) {











'use strict';

var ReactFeatureFlags={



logTopLevelRenders:false};


module.exports=ReactFeatureFlags;
}, "react/lib/ReactFeatureFlags.js");
__d(78 /* react/lib/Transaction.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var invariant=require(22 /* fbjs/lib/invariant */);






























































var Mixin={







reinitializeTransaction:function reinitializeTransaction(){
this.transactionWrappers=this.getTransactionWrappers();
if(this.wrapperInitData){
this.wrapperInitData.length=0;
}else{
this.wrapperInitData=[];
}
this._isInTransaction=false;
},

_isInTransaction:false,





getTransactionWrappers:null,

isInTransaction:function isInTransaction(){
return!!this._isInTransaction;
},


















perform:function perform(method,scope,a,b,c,d,e,f){
!!this.isInTransaction()?process.env.NODE_ENV!=='production'?invariant(false,'Transaction.perform(...): Cannot initialize a transaction when there is already an outstanding transaction.'):_prodInvariant('27'):void 0;
var errorThrown;
var ret;
try{
this._isInTransaction=true;




errorThrown=true;
this.initializeAll(0);
ret=method.call(scope,a,b,c,d,e,f);
errorThrown=false;
}finally{
try{
if(errorThrown){


try{
this.closeAll(0);
}catch(err){}
}else{


this.closeAll(0);
}
}finally{
this._isInTransaction=false;
}
}
return ret;
},

initializeAll:function initializeAll(startIndex){
var transactionWrappers=this.transactionWrappers;
for(var i=startIndex;i<transactionWrappers.length;i++){
var wrapper=transactionWrappers[i];
try{




this.wrapperInitData[i]=Transaction.OBSERVED_ERROR;
this.wrapperInitData[i]=wrapper.initialize?wrapper.initialize.call(this):null;
}finally{
if(this.wrapperInitData[i]===Transaction.OBSERVED_ERROR){



try{
this.initializeAll(i+1);
}catch(err){}
}
}
}
},







closeAll:function closeAll(startIndex){
!this.isInTransaction()?process.env.NODE_ENV!=='production'?invariant(false,'Transaction.closeAll(): Cannot close transaction when none are open.'):_prodInvariant('28'):void 0;
var transactionWrappers=this.transactionWrappers;
for(var i=startIndex;i<transactionWrappers.length;i++){
var wrapper=transactionWrappers[i];
var initData=this.wrapperInitData[i];
var errorThrown;
try{




errorThrown=true;
if(initData!==Transaction.OBSERVED_ERROR&&wrapper.close){
wrapper.close.call(this,initData);
}
errorThrown=false;
}finally{
if(errorThrown){



try{
this.closeAll(i+1);
}catch(e){}
}
}
}
this.wrapperInitData.length=0;
}};


var Transaction={

Mixin:Mixin,




OBSERVED_ERROR:{}};



module.exports=Transaction;
}, "react/lib/Transaction.js");
__d(79 /* react-native/lib/UIManager.js */, function(global, require, module, exports) {









'use strict';



module.exports=require(80 /* UIManager */);
}, "react-native/lib/UIManager.js");
__d(80 /* UIManager */, function(global, require, module, exports) {










'use strict';

var Platform=require(13 /* Platform */);
var NativeModules=require(14 /* NativeModules */);var
UIManager=NativeModules.UIManager;

var findNodeHandle=require(81 /* react/lib/findNodeHandle */);
var invariant=require(22 /* fbjs/lib/invariant */);

invariant(UIManager,'UIManager is undefined. The native module config is probably incorrect.');

var _takeSnapshot=UIManager.takeSnapshot;


















UIManager.takeSnapshot=function _callee(
view,
options){return regeneratorRuntime.async(function _callee$(_context){while(1){switch(_context.prev=_context.next){case 0:if(






_takeSnapshot){_context.next=3;break;}
console.warn('UIManager.takeSnapshot is not available on this platform');return _context.abrupt('return');case 3:


if(typeof view!=='number'&&view!=='window'){
view=findNodeHandle(view)||'window';
}return _context.abrupt('return',
_takeSnapshot(view,options));case 5:case'end':return _context.stop();}}},null,this);};







if(Platform.OS==='ios'){(function(){

function normalizePrefix(moduleName){
return moduleName.replace(/^(RCT|RK)/,'');
}

Object.keys(UIManager).forEach(function(viewName){
var viewConfig=UIManager[viewName];
if(viewConfig.Manager){(function(){
var constants=void 0;

Object.defineProperty(viewConfig,'Constants',{
configurable:true,
enumerable:true,
get:function get(){
if(constants){
return constants;
}
constants={};
var viewManager=NativeModules[normalizePrefix(viewConfig.Manager)];
viewManager&&Object.keys(viewManager).forEach(function(key){
var value=viewManager[key];
if(typeof value!=='function'){
constants[key]=value;
}
});
return constants;
}});

var commands=void 0;

Object.defineProperty(viewConfig,'Commands',{
configurable:true,
enumerable:true,
get:function get(){
if(commands){
return commands;
}
commands={};
var viewManager=NativeModules[normalizePrefix(viewConfig.Manager)];
var index=0;
viewManager&&Object.keys(viewManager).forEach(function(key){
var value=viewManager[key];
if(typeof value==='function'){
commands[key]=index++;
}
});
return commands;
}});})();

}
});})();
}

module.exports=UIManager;
}, "UIManager");
__d(81 /* react/lib/findNodeHandle.js */, function(global, require, module, exports) {











'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var ReactCurrentOwner=require(21 /* ./ReactCurrentOwner */);
var ReactInstanceMap=require(73 /* ./ReactInstanceMap */);

var invariant=require(22 /* fbjs/lib/invariant */);
var warning=require(23 /* fbjs/lib/warning */);































function findNodeHandle(componentOrHandle){
if(process.env.NODE_ENV!=='production'){
var owner=ReactCurrentOwner.current;
if(owner!==null){
process.env.NODE_ENV!=='production'?warning(owner._warnedAboutRefsInRender,'%s is accessing findNodeHandle inside its render(). '+'render() should be a pure function of props and state. It should '+'never access something that requires stale data from the previous '+'render, such as refs. Move this logic to componentDidMount and '+'componentDidUpdate instead.',owner.getName()||'A component'):void 0;
owner._warnedAboutRefsInRender=true;
}
}
if(componentOrHandle==null){
return null;
}
if(typeof componentOrHandle==='number'){

return componentOrHandle;
}

var component=componentOrHandle;



var internalInstance=ReactInstanceMap.get(component);
if(internalInstance){
return internalInstance.getHostNode();
}else{
var rootNodeID=component._rootNodeID;
if(rootNodeID){
return rootNodeID;
}else{
!(

typeof component==='object'&&'_rootNodeID'in component||

component.render!=null&&typeof component.render==='function')?process.env.NODE_ENV!=='production'?invariant(false,'findNodeHandle(...): Argument is not a component (type: %s, keys: %s)',typeof component,Object.keys(component)):_prodInvariant('21',typeof component,Object.keys(component)):void 0;
!false?process.env.NODE_ENV!=='production'?invariant(false,'findNodeHandle(...): Unable to find node handle for unmounted component.'):_prodInvariant('22'):void 0;
}
}
}

module.exports=findNodeHandle;
}, "react/lib/findNodeHandle.js");
__d(82 /* fbjs/lib/emptyObject.js */, function(global, require, module, exports) {









'use strict';

var emptyObject={};

if(process.env.NODE_ENV!=='production'){
Object.freeze(emptyObject);
}

module.exports=emptyObject;
}, "fbjs/lib/emptyObject.js");
__d(83 /* react/lib/instantiateReactComponent.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */),
_assign=require(64 /* object-assign */);

var ReactCompositeComponent=require(84 /* ./ReactCompositeComponent */);
var ReactEmptyComponent=require(94 /* ./ReactEmptyComponent */);
var ReactHostComponent=require(95 /* ./ReactHostComponent */);

var invariant=require(22 /* fbjs/lib/invariant */);
var warning=require(23 /* fbjs/lib/warning */);


var ReactCompositeComponentWrapper=function ReactCompositeComponentWrapper(element){
this.construct(element);
};
_assign(ReactCompositeComponentWrapper.prototype,ReactCompositeComponent.Mixin,{
_instantiateReactComponent:instantiateReactComponent});


function getDeclarationErrorAddendum(owner){
if(owner){
var name=owner.getName();
if(name){
return' Check the render method of `'+name+'`.';
}
}
return'';
}








function isInternalComponentType(type){
return typeof type==='function'&&typeof type.prototype!=='undefined'&&typeof type.prototype.mountComponent==='function'&&typeof type.prototype.receiveComponent==='function';
}

var nextDebugID=1;









function instantiateReactComponent(node,shouldHaveDebugID){
var instance;

if(node===null||node===false){
instance=ReactEmptyComponent.create(instantiateReactComponent);
}else if(typeof node==='object'){
var element=node;
!(element&&(typeof element.type==='function'||typeof element.type==='string'))?process.env.NODE_ENV!=='production'?invariant(false,'Element type is invalid: expected a string (for built-in components) or a class/function (for composite components) but got: %s.%s',element.type==null?element.type:typeof element.type,getDeclarationErrorAddendum(element._owner)):_prodInvariant('130',element.type==null?element.type:typeof element.type,getDeclarationErrorAddendum(element._owner)):void 0;


if(typeof element.type==='string'){
instance=ReactHostComponent.createInternalComponent(element);
}else if(isInternalComponentType(element.type)){



instance=new element.type(element);


if(!instance.getHostNode){
instance.getHostNode=instance.getNativeNode;
}
}else{
instance=new ReactCompositeComponentWrapper(element);
}
}else if(typeof node==='string'||typeof node==='number'){
instance=ReactHostComponent.createInstanceForText(node);
}else{
!false?process.env.NODE_ENV!=='production'?invariant(false,'Encountered invalid React node of type %s',typeof node):_prodInvariant('131',typeof node):void 0;
}

if(process.env.NODE_ENV!=='production'){
process.env.NODE_ENV!=='production'?warning(typeof instance.mountComponent==='function'&&typeof instance.receiveComponent==='function'&&typeof instance.getHostNode==='function'&&typeof instance.unmountComponent==='function','Only React Components can be mounted.'):void 0;
}




instance._mountIndex=0;
instance._mountImage=null;

if(process.env.NODE_ENV!=='production'){
instance._debugID=shouldHaveDebugID?nextDebugID++:0;
}



if(process.env.NODE_ENV!=='production'){
if(Object.preventExtensions){
Object.preventExtensions(instance);
}
}

return instance;
}

module.exports=instantiateReactComponent;
}, "react/lib/instantiateReactComponent.js");
__d(84 /* react/lib/ReactCompositeComponent.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */),
_assign=require(64 /* object-assign */);

var ReactComponentEnvironment=require(85 /* ./ReactComponentEnvironment */);
var ReactCurrentOwner=require(21 /* ./ReactCurrentOwner */);
var ReactElement=require(63 /* ./ReactElement */);
var ReactErrorUtils=require(86 /* ./ReactErrorUtils */);
var ReactInstanceMap=require(73 /* ./ReactInstanceMap */);
var ReactInstrumentation=require(66 /* ./ReactInstrumentation */);
var ReactNodeTypes=require(87 /* ./ReactNodeTypes */);
var ReactPropTypeLocations=require(88 /* ./ReactPropTypeLocations */);
var ReactReconciler=require(69 /* ./ReactReconciler */);

var checkReactTypeSpec=require(89 /* ./checkReactTypeSpec */);
var emptyObject=require(82 /* fbjs/lib/emptyObject */);
var invariant=require(22 /* fbjs/lib/invariant */);
var shallowEqual=require(92 /* fbjs/lib/shallowEqual */);
var shouldUpdateReactComponent=require(93 /* ./shouldUpdateReactComponent */);
var warning=require(23 /* fbjs/lib/warning */);

var CompositeTypes={
ImpureClass:0,
PureClass:1,
StatelessFunctional:2};


function StatelessComponent(Component){}
StatelessComponent.prototype.render=function(){
var Component=ReactInstanceMap.get(this)._currentElement.type;
var element=Component(this.props,this.context,this.updater);
warnIfInvalidElement(Component,element);
return element;
};

function warnIfInvalidElement(Component,element){
if(process.env.NODE_ENV!=='production'){
process.env.NODE_ENV!=='production'?warning(element===null||element===false||ReactElement.isValidElement(element),'%s(...): A valid React element (or null) must be returned. You may have '+'returned undefined, an array or some other invalid object.',Component.displayName||Component.name||'Component'):void 0;
process.env.NODE_ENV!=='production'?warning(!Component.childContextTypes,'%s(...): childContextTypes cannot be defined on a functional component.',Component.displayName||Component.name||'Component'):void 0;
}
}

function shouldConstruct(Component){
return!!(Component.prototype&&Component.prototype.isReactComponent);
}

function isPureComponent(Component){
return!!(Component.prototype&&Component.prototype.isPureReactComponent);
}


function measureLifeCyclePerf(fn,debugID,timerType){
if(debugID===0){



return fn();
}

ReactInstrumentation.debugTool.onBeginLifeCycleTimer(debugID,timerType);
try{
return fn();
}finally{
ReactInstrumentation.debugTool.onEndLifeCycleTimer(debugID,timerType);
}
}


































var nextMountID=1;




var ReactCompositeComponentMixin={








construct:function construct(element){
this._currentElement=element;
this._rootNodeID=0;
this._compositeType=null;
this._instance=null;
this._hostParent=null;
this._hostContainerInfo=null;


this._updateBatchNumber=null;
this._pendingElement=null;
this._pendingStateQueue=null;
this._pendingReplaceState=false;
this._pendingForceUpdate=false;

this._renderedNodeType=null;
this._renderedComponent=null;
this._context=null;
this._mountOrder=0;
this._topLevelWrapper=null;


this._pendingCallbacks=null;


this._calledComponentWillUnmount=false;

if(process.env.NODE_ENV!=='production'){
this._warnedAboutRefsInRender=false;
}
},












mountComponent:function mountComponent(transaction,hostParent,hostContainerInfo,context){
var _this=this;

this._context=context;
this._mountOrder=nextMountID++;
this._hostParent=hostParent;
this._hostContainerInfo=hostContainerInfo;

var publicProps=this._currentElement.props;
var publicContext=this._processContext(context);

var Component=this._currentElement.type;

var updateQueue=transaction.getUpdateQueue();


var doConstruct=shouldConstruct(Component);
var inst=this._constructComponent(doConstruct,publicProps,publicContext,updateQueue);
var renderedElement;


if(!doConstruct&&(inst==null||inst.render==null)){
renderedElement=inst;
warnIfInvalidElement(Component,renderedElement);
!(inst===null||inst===false||ReactElement.isValidElement(inst))?process.env.NODE_ENV!=='production'?invariant(false,'%s(...): A valid React element (or null) must be returned. You may have returned undefined, an array or some other invalid object.',Component.displayName||Component.name||'Component'):_prodInvariant('105',Component.displayName||Component.name||'Component'):void 0;
inst=new StatelessComponent(Component);
this._compositeType=CompositeTypes.StatelessFunctional;
}else{
if(isPureComponent(Component)){
this._compositeType=CompositeTypes.PureClass;
}else{
this._compositeType=CompositeTypes.ImpureClass;
}
}

if(process.env.NODE_ENV!=='production'){


if(inst.render==null){
process.env.NODE_ENV!=='production'?warning(false,'%s(...): No `render` method found on the returned component '+'instance: you may have forgotten to define `render`.',Component.displayName||Component.name||'Component'):void 0;
}

var propsMutated=inst.props!==publicProps;
var componentName=Component.displayName||Component.name||'Component';

process.env.NODE_ENV!=='production'?warning(inst.props===undefined||!propsMutated,'%s(...): When calling super() in `%s`, make sure to pass '+'up the same props that your component\'s constructor was passed.',componentName,componentName):void 0;
}



inst.props=publicProps;
inst.context=publicContext;
inst.refs=emptyObject;
inst.updater=updateQueue;

this._instance=inst;


ReactInstanceMap.set(inst,this);

if(process.env.NODE_ENV!=='production'){



process.env.NODE_ENV!=='production'?warning(!inst.getInitialState||inst.getInitialState.isReactClassApproved,'getInitialState was defined on %s, a plain JavaScript class. '+'This is only supported for classes created using React.createClass. '+'Did you mean to define a state property instead?',this.getName()||'a component'):void 0;
process.env.NODE_ENV!=='production'?warning(!inst.getDefaultProps||inst.getDefaultProps.isReactClassApproved,'getDefaultProps was defined on %s, a plain JavaScript class. '+'This is only supported for classes created using React.createClass. '+'Use a static property to define defaultProps instead.',this.getName()||'a component'):void 0;
process.env.NODE_ENV!=='production'?warning(!inst.propTypes,'propTypes was defined as an instance property on %s. Use a static '+'property to define propTypes instead.',this.getName()||'a component'):void 0;
process.env.NODE_ENV!=='production'?warning(!inst.contextTypes,'contextTypes was defined as an instance property on %s. Use a '+'static property to define contextTypes instead.',this.getName()||'a component'):void 0;
process.env.NODE_ENV!=='production'?warning(typeof inst.componentShouldUpdate!=='function','%s has a method called '+'componentShouldUpdate(). Did you mean shouldComponentUpdate()? '+'The name is phrased as a question because the function is '+'expected to return a value.',this.getName()||'A component'):void 0;
process.env.NODE_ENV!=='production'?warning(typeof inst.componentDidUnmount!=='function','%s has a method called '+'componentDidUnmount(). But there is no such lifecycle method. '+'Did you mean componentWillUnmount()?',this.getName()||'A component'):void 0;
process.env.NODE_ENV!=='production'?warning(typeof inst.componentWillRecieveProps!=='function','%s has a method called '+'componentWillRecieveProps(). Did you mean componentWillReceiveProps()?',this.getName()||'A component'):void 0;
}

var initialState=inst.state;
if(initialState===undefined){
inst.state=initialState=null;
}
!(typeof initialState==='object'&&!Array.isArray(initialState))?process.env.NODE_ENV!=='production'?invariant(false,'%s.state: must be set to an object or null',this.getName()||'ReactCompositeComponent'):_prodInvariant('106',this.getName()||'ReactCompositeComponent'):void 0;

this._pendingStateQueue=null;
this._pendingReplaceState=false;
this._pendingForceUpdate=false;

var markup;
if(inst.unstable_handleError){
markup=this.performInitialMountWithErrorHandling(renderedElement,hostParent,hostContainerInfo,transaction,context);
}else{
markup=this.performInitialMount(renderedElement,hostParent,hostContainerInfo,transaction,context);
}

if(inst.componentDidMount){
if(process.env.NODE_ENV!=='production'){
transaction.getReactMountReady().enqueue(function(){
measureLifeCyclePerf(function(){
return inst.componentDidMount();
},_this._debugID,'componentDidMount');
});
}else{
transaction.getReactMountReady().enqueue(inst.componentDidMount,inst);
}
}

return markup;
},

_constructComponent:function _constructComponent(doConstruct,publicProps,publicContext,updateQueue){
if(process.env.NODE_ENV!=='production'){
ReactCurrentOwner.current=this;
try{
return this._constructComponentWithoutOwner(doConstruct,publicProps,publicContext,updateQueue);
}finally{
ReactCurrentOwner.current=null;
}
}else{
return this._constructComponentWithoutOwner(doConstruct,publicProps,publicContext,updateQueue);
}
},

_constructComponentWithoutOwner:function _constructComponentWithoutOwner(doConstruct,publicProps,publicContext,updateQueue){
var Component=this._currentElement.type;

if(doConstruct){
if(process.env.NODE_ENV!=='production'){
return measureLifeCyclePerf(function(){
return new Component(publicProps,publicContext,updateQueue);
},this._debugID,'ctor');
}else{
return new Component(publicProps,publicContext,updateQueue);
}
}



if(process.env.NODE_ENV!=='production'){
return measureLifeCyclePerf(function(){
return Component(publicProps,publicContext,updateQueue);
},this._debugID,'render');
}else{
return Component(publicProps,publicContext,updateQueue);
}
},

performInitialMountWithErrorHandling:function performInitialMountWithErrorHandling(renderedElement,hostParent,hostContainerInfo,transaction,context){
var markup;
var checkpoint=transaction.checkpoint();
try{
markup=this.performInitialMount(renderedElement,hostParent,hostContainerInfo,transaction,context);
}catch(e){

transaction.rollback(checkpoint);
this._instance.unstable_handleError(e);
if(this._pendingStateQueue){
this._instance.state=this._processPendingState(this._instance.props,this._instance.context);
}
checkpoint=transaction.checkpoint();

this._renderedComponent.unmountComponent(true);
transaction.rollback(checkpoint);



markup=this.performInitialMount(renderedElement,hostParent,hostContainerInfo,transaction,context);
}
return markup;
},

performInitialMount:function performInitialMount(renderedElement,hostParent,hostContainerInfo,transaction,context){
var inst=this._instance;

var debugID=0;
if(process.env.NODE_ENV!=='production'){
debugID=this._debugID;
}

if(inst.componentWillMount){
if(process.env.NODE_ENV!=='production'){
measureLifeCyclePerf(function(){
return inst.componentWillMount();
},debugID,'componentWillMount');
}else{
inst.componentWillMount();
}


if(this._pendingStateQueue){
inst.state=this._processPendingState(inst.props,inst.context);
}
}


if(renderedElement===undefined){
renderedElement=this._renderValidatedComponent();
}

var nodeType=ReactNodeTypes.getType(renderedElement);
this._renderedNodeType=nodeType;
var child=this._instantiateReactComponent(renderedElement,nodeType!==ReactNodeTypes.EMPTY);

this._renderedComponent=child;

var markup=ReactReconciler.mountComponent(child,transaction,hostParent,hostContainerInfo,this._processChildContext(context),debugID);

if(process.env.NODE_ENV!=='production'){
if(debugID!==0){
var childDebugIDs=child._debugID!==0?[child._debugID]:[];
ReactInstrumentation.debugTool.onSetChildren(debugID,childDebugIDs);
}
}

return markup;
},

getHostNode:function getHostNode(){
return ReactReconciler.getHostNode(this._renderedComponent);
},







unmountComponent:function unmountComponent(safely){
if(!this._renderedComponent){
return;
}

var inst=this._instance;

if(inst.componentWillUnmount&&!inst._calledComponentWillUnmount){
inst._calledComponentWillUnmount=true;

if(safely){
var name=this.getName()+'.componentWillUnmount()';
ReactErrorUtils.invokeGuardedCallback(name,inst.componentWillUnmount.bind(inst));
}else{
if(process.env.NODE_ENV!=='production'){
measureLifeCyclePerf(function(){
return inst.componentWillUnmount();
},this._debugID,'componentWillUnmount');
}else{
inst.componentWillUnmount();
}
}
}

if(this._renderedComponent){
ReactReconciler.unmountComponent(this._renderedComponent,safely);
this._renderedNodeType=null;
this._renderedComponent=null;
this._instance=null;
}




this._pendingStateQueue=null;
this._pendingReplaceState=false;
this._pendingForceUpdate=false;
this._pendingCallbacks=null;
this._pendingElement=null;



this._context=null;
this._rootNodeID=0;
this._topLevelWrapper=null;




ReactInstanceMap.remove(inst);






},









_maskContext:function _maskContext(context){
var Component=this._currentElement.type;
var contextTypes=Component.contextTypes;
if(!contextTypes){
return emptyObject;
}
var maskedContext={};
for(var contextName in contextTypes){
maskedContext[contextName]=context[contextName];
}
return maskedContext;
},









_processContext:function _processContext(context){
var maskedContext=this._maskContext(context);
if(process.env.NODE_ENV!=='production'){
var Component=this._currentElement.type;
if(Component.contextTypes){
this._checkContextTypes(Component.contextTypes,maskedContext,ReactPropTypeLocations.context);
}
}
return maskedContext;
},






_processChildContext:function _processChildContext(currentContext){
var Component=this._currentElement.type;
var inst=this._instance;
var childContext;

if(inst.getChildContext){
if(process.env.NODE_ENV!=='production'){
ReactInstrumentation.debugTool.onBeginProcessingChildContext();
try{
childContext=inst.getChildContext();
}finally{
ReactInstrumentation.debugTool.onEndProcessingChildContext();
}
}else{
childContext=inst.getChildContext();
}
}

if(childContext){
!(typeof Component.childContextTypes==='object')?process.env.NODE_ENV!=='production'?invariant(false,'%s.getChildContext(): childContextTypes must be defined in order to use getChildContext().',this.getName()||'ReactCompositeComponent'):_prodInvariant('107',this.getName()||'ReactCompositeComponent'):void 0;
if(process.env.NODE_ENV!=='production'){
this._checkContextTypes(Component.childContextTypes,childContext,ReactPropTypeLocations.childContext);
}
for(var name in childContext){
!(name in Component.childContextTypes)?process.env.NODE_ENV!=='production'?invariant(false,'%s.getChildContext(): key "%s" is not defined in childContextTypes.',this.getName()||'ReactCompositeComponent',name):_prodInvariant('108',this.getName()||'ReactCompositeComponent',name):void 0;
}
return _assign({},currentContext,childContext);
}
return currentContext;
},









_checkContextTypes:function _checkContextTypes(typeSpecs,values,location){
checkReactTypeSpec(typeSpecs,values,location,this.getName(),null,this._debugID);
},

receiveComponent:function receiveComponent(nextElement,transaction,nextContext){
var prevElement=this._currentElement;
var prevContext=this._context;

this._pendingElement=null;

this.updateComponent(transaction,prevElement,nextElement,prevContext,nextContext);
},








performUpdateIfNecessary:function performUpdateIfNecessary(transaction){
if(this._pendingElement!=null){
ReactReconciler.receiveComponent(this,this._pendingElement,transaction,this._context);
}else if(this._pendingStateQueue!==null||this._pendingForceUpdate){
this.updateComponent(transaction,this._currentElement,this._currentElement,this._context,this._context);
}else{
this._updateBatchNumber=null;
}
},
















updateComponent:function updateComponent(transaction,prevParentElement,nextParentElement,prevUnmaskedContext,nextUnmaskedContext){
var inst=this._instance;
!(inst!=null)?process.env.NODE_ENV!=='production'?invariant(false,'Attempted to update component `%s` that has already been unmounted (or failed to mount).',this.getName()||'ReactCompositeComponent'):_prodInvariant('136',this.getName()||'ReactCompositeComponent'):void 0;

var willReceive=false;
var nextContext;


if(this._context===nextUnmaskedContext){
nextContext=inst.context;
}else{
nextContext=this._processContext(nextUnmaskedContext);
willReceive=true;
}

var prevProps=prevParentElement.props;
var nextProps=nextParentElement.props;


if(prevParentElement!==nextParentElement){
willReceive=true;
}




if(willReceive&&inst.componentWillReceiveProps){
if(process.env.NODE_ENV!=='production'){
measureLifeCyclePerf(function(){
return inst.componentWillReceiveProps(nextProps,nextContext);
},this._debugID,'componentWillReceiveProps');
}else{
inst.componentWillReceiveProps(nextProps,nextContext);
}
}

var nextState=this._processPendingState(nextProps,nextContext);
var shouldUpdate=true;

if(!this._pendingForceUpdate){
if(inst.shouldComponentUpdate){
if(process.env.NODE_ENV!=='production'){
shouldUpdate=measureLifeCyclePerf(function(){
return inst.shouldComponentUpdate(nextProps,nextState,nextContext);
},this._debugID,'shouldComponentUpdate');
}else{
shouldUpdate=inst.shouldComponentUpdate(nextProps,nextState,nextContext);
}
}else{
if(this._compositeType===CompositeTypes.PureClass){
shouldUpdate=!shallowEqual(prevProps,nextProps)||!shallowEqual(inst.state,nextState);
}
}
}

if(process.env.NODE_ENV!=='production'){
process.env.NODE_ENV!=='production'?warning(shouldUpdate!==undefined,'%s.shouldComponentUpdate(): Returned undefined instead of a '+'boolean value. Make sure to return true or false.',this.getName()||'ReactCompositeComponent'):void 0;
}

this._updateBatchNumber=null;
if(shouldUpdate){
this._pendingForceUpdate=false;

this._performComponentUpdate(nextParentElement,nextProps,nextState,nextContext,transaction,nextUnmaskedContext);
}else{


this._currentElement=nextParentElement;
this._context=nextUnmaskedContext;
inst.props=nextProps;
inst.state=nextState;
inst.context=nextContext;
}
},

_processPendingState:function _processPendingState(props,context){
var inst=this._instance;
var queue=this._pendingStateQueue;
var replace=this._pendingReplaceState;
this._pendingReplaceState=false;
this._pendingStateQueue=null;

if(!queue){
return inst.state;
}

if(replace&&queue.length===1){
return queue[0];
}

var nextState=_assign({},replace?queue[0]:inst.state);
for(var i=replace?1:0;i<queue.length;i++){
var partial=queue[i];
_assign(nextState,typeof partial==='function'?partial.call(inst,nextState,props,context):partial);
}

return nextState;
},













_performComponentUpdate:function _performComponentUpdate(nextElement,nextProps,nextState,nextContext,transaction,unmaskedContext){
var _this2=this;

var inst=this._instance;

var hasComponentDidUpdate=Boolean(inst.componentDidUpdate);
var prevProps;
var prevState;
var prevContext;
if(hasComponentDidUpdate){
prevProps=inst.props;
prevState=inst.state;
prevContext=inst.context;
}

if(inst.componentWillUpdate){
if(process.env.NODE_ENV!=='production'){
measureLifeCyclePerf(function(){
return inst.componentWillUpdate(nextProps,nextState,nextContext);
},this._debugID,'componentWillUpdate');
}else{
inst.componentWillUpdate(nextProps,nextState,nextContext);
}
}

this._currentElement=nextElement;
this._context=unmaskedContext;
inst.props=nextProps;
inst.state=nextState;
inst.context=nextContext;

this._updateRenderedComponent(transaction,unmaskedContext);

if(hasComponentDidUpdate){
if(process.env.NODE_ENV!=='production'){
transaction.getReactMountReady().enqueue(function(){
measureLifeCyclePerf(inst.componentDidUpdate.bind(inst,prevProps,prevState,prevContext),_this2._debugID,'componentDidUpdate');
});
}else{
transaction.getReactMountReady().enqueue(inst.componentDidUpdate.bind(inst,prevProps,prevState,prevContext),inst);
}
}
},







_updateRenderedComponent:function _updateRenderedComponent(transaction,context){
var prevComponentInstance=this._renderedComponent;
var prevRenderedElement=prevComponentInstance._currentElement;
var nextRenderedElement=this._renderValidatedComponent();

var debugID=0;
if(process.env.NODE_ENV!=='production'){
debugID=this._debugID;
}

if(shouldUpdateReactComponent(prevRenderedElement,nextRenderedElement)){
ReactReconciler.receiveComponent(prevComponentInstance,nextRenderedElement,transaction,this._processChildContext(context));
}else{
var oldHostNode=ReactReconciler.getHostNode(prevComponentInstance);
ReactReconciler.unmountComponent(prevComponentInstance,false);

var nodeType=ReactNodeTypes.getType(nextRenderedElement);
this._renderedNodeType=nodeType;
var child=this._instantiateReactComponent(nextRenderedElement,nodeType!==ReactNodeTypes.EMPTY);

this._renderedComponent=child;

var nextMarkup=ReactReconciler.mountComponent(child,transaction,this._hostParent,this._hostContainerInfo,this._processChildContext(context),debugID);

if(process.env.NODE_ENV!=='production'){
if(debugID!==0){
var childDebugIDs=child._debugID!==0?[child._debugID]:[];
ReactInstrumentation.debugTool.onSetChildren(debugID,childDebugIDs);
}
}

this._replaceNodeWithMarkup(oldHostNode,nextMarkup,prevComponentInstance);
}
},






_replaceNodeWithMarkup:function _replaceNodeWithMarkup(oldHostNode,nextMarkup,prevInstance){
ReactComponentEnvironment.replaceNodeWithMarkup(oldHostNode,nextMarkup,prevInstance);
},




_renderValidatedComponentWithoutOwnerOrContext:function _renderValidatedComponentWithoutOwnerOrContext(){
var inst=this._instance;
var renderedComponent;

if(process.env.NODE_ENV!=='production'){
renderedComponent=measureLifeCyclePerf(function(){
return inst.render();
},this._debugID,'render');
}else{
renderedComponent=inst.render();
}

if(process.env.NODE_ENV!=='production'){

if(renderedComponent===undefined&&inst.render._isMockFunction){


renderedComponent=null;
}
}

return renderedComponent;
},




_renderValidatedComponent:function _renderValidatedComponent(){
var renderedComponent;
if(process.env.NODE_ENV!=='production'||this._compositeType!==CompositeTypes.StatelessFunctional){
ReactCurrentOwner.current=this;
try{
renderedComponent=this._renderValidatedComponentWithoutOwnerOrContext();
}finally{
ReactCurrentOwner.current=null;
}
}else{
renderedComponent=this._renderValidatedComponentWithoutOwnerOrContext();
}
!(

renderedComponent===null||renderedComponent===false||ReactElement.isValidElement(renderedComponent))?process.env.NODE_ENV!=='production'?invariant(false,'%s.render(): A valid React element (or null) must be returned. You may have returned undefined, an array or some other invalid object.',this.getName()||'ReactCompositeComponent'):_prodInvariant('109',this.getName()||'ReactCompositeComponent'):void 0;

return renderedComponent;
},









attachRef:function attachRef(ref,component){
var inst=this.getPublicInstance();
!(inst!=null)?process.env.NODE_ENV!=='production'?invariant(false,'Stateless function components cannot have refs.'):_prodInvariant('110'):void 0;
var publicComponentInstance=component.getPublicInstance();
if(process.env.NODE_ENV!=='production'){
var componentName=component&&component.getName?component.getName():'a component';
process.env.NODE_ENV!=='production'?warning(publicComponentInstance!=null||component._compositeType!==CompositeTypes.StatelessFunctional,'Stateless function components cannot be given refs '+'(See ref "%s" in %s created by %s). '+'Attempts to access this ref will fail.',ref,componentName,this.getName()):void 0;
}
var refs=inst.refs===emptyObject?inst.refs={}:inst.refs;
refs[ref]=publicComponentInstance;
},








detachRef:function detachRef(ref){
var refs=this.getPublicInstance().refs;
delete refs[ref];
},







getName:function getName(){
var type=this._currentElement.type;
var constructor=this._instance&&this._instance.constructor;
return type.displayName||constructor&&constructor.displayName||type.name||constructor&&constructor.name||null;
},









getPublicInstance:function getPublicInstance(){
var inst=this._instance;
if(this._compositeType===CompositeTypes.StatelessFunctional){
return null;
}
return inst;
},


_instantiateReactComponent:null};



var ReactCompositeComponent={

Mixin:ReactCompositeComponentMixin};



module.exports=ReactCompositeComponent;
}, "react/lib/ReactCompositeComponent.js");
__d(85 /* react/lib/ReactComponentEnvironment.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var invariant=require(22 /* fbjs/lib/invariant */);

var injected=false;

var ReactComponentEnvironment={





replaceNodeWithMarkup:null,





processChildrenUpdates:null,

injection:{
injectEnvironment:function injectEnvironment(environment){
!!injected?process.env.NODE_ENV!=='production'?invariant(false,'ReactCompositeComponent: injectEnvironment() can only be called once.'):_prodInvariant('104'):void 0;
ReactComponentEnvironment.replaceNodeWithMarkup=environment.replaceNodeWithMarkup;
ReactComponentEnvironment.processChildrenUpdates=environment.processChildrenUpdates;
injected=true;
}}};




module.exports=ReactComponentEnvironment;
}, "react/lib/ReactComponentEnvironment.js");
__d(86 /* react/lib/ReactErrorUtils.js */, function(global, require, module, exports) {










'use strict';

var caughtError=null;









function invokeGuardedCallback(name,func,a,b){
try{
return func(a,b);
}catch(x){
if(caughtError===null){
caughtError=x;
}
return undefined;
}
}

var ReactErrorUtils={
invokeGuardedCallback:invokeGuardedCallback,





invokeGuardedCallbackWithCatch:invokeGuardedCallback,





rethrowCaughtError:function rethrowCaughtError(){
if(caughtError){
var error=caughtError;
caughtError=null;
throw error;
}
}};


if(process.env.NODE_ENV!=='production'){




if(typeof window!=='undefined'&&typeof window.dispatchEvent==='function'&&typeof document!=='undefined'&&typeof document.createEvent==='function'){
var fakeNode=document.createElement('react');
ReactErrorUtils.invokeGuardedCallback=function(name,func,a,b){
var boundFunc=func.bind(null,a,b);
var evtType='react-'+name;
fakeNode.addEventListener(evtType,boundFunc,false);
var evt=document.createEvent('Event');
evt.initEvent(evtType,false,false);
fakeNode.dispatchEvent(evt);
fakeNode.removeEventListener(evtType,boundFunc,false);
};
}
}

module.exports=ReactErrorUtils;
}, "react/lib/ReactErrorUtils.js");
__d(87 /* react/lib/ReactNodeTypes.js */, function(global, require, module, exports) {











'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var ReactElement=require(63 /* ./ReactElement */);

var invariant=require(22 /* fbjs/lib/invariant */);

var ReactNodeTypes={
HOST:0,
COMPOSITE:1,
EMPTY:2,

getType:function getType(node){
if(node===null||node===false){
return ReactNodeTypes.EMPTY;
}else if(ReactElement.isValidElement(node)){
if(typeof node.type==='function'){
return ReactNodeTypes.COMPOSITE;
}else{
return ReactNodeTypes.HOST;
}
}
!false?process.env.NODE_ENV!=='production'?invariant(false,'Unexpected node: %s',node):_prodInvariant('26',node):void 0;
}};


module.exports=ReactNodeTypes;
}, "react/lib/ReactNodeTypes.js");
__d(88 /* react/lib/ReactPropTypeLocations.js */, function(global, require, module, exports) {










'use strict';

var keyMirror=require(34 /* fbjs/lib/keyMirror */);

var ReactPropTypeLocations=keyMirror({
prop:null,
context:null,
childContext:null});


module.exports=ReactPropTypeLocations;
}, "react/lib/ReactPropTypeLocations.js");
__d(89 /* react/lib/checkReactTypeSpec.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */);

var ReactPropTypeLocationNames=require(90 /* ./ReactPropTypeLocationNames */);
var ReactPropTypesSecret=require(91 /* ./ReactPropTypesSecret */);

var invariant=require(22 /* fbjs/lib/invariant */);
var warning=require(23 /* fbjs/lib/warning */);

var ReactComponentTreeHook;

if(typeof process!=='undefined'&&process.env&&process.env.NODE_ENV==='test'){





ReactComponentTreeHook=require(19 /* ./ReactComponentTreeHook */);
}

var loggedTypeFailures={};













function checkReactTypeSpec(typeSpecs,values,location,componentName,element,debugID){
for(var typeSpecName in typeSpecs){
if(typeSpecs.hasOwnProperty(typeSpecName)){
var error;



try{


!(typeof typeSpecs[typeSpecName]==='function')?process.env.NODE_ENV!=='production'?invariant(false,'%s: %s type `%s` is invalid; it must be a function, usually from React.PropTypes.',componentName||'React class',ReactPropTypeLocationNames[location],typeSpecName):_prodInvariant('84',componentName||'React class',ReactPropTypeLocationNames[location],typeSpecName):void 0;
error=typeSpecs[typeSpecName](values,typeSpecName,componentName,location,null,ReactPropTypesSecret);
}catch(ex){
error=ex;
}
process.env.NODE_ENV!=='production'?warning(!error||error instanceof Error,'%s: type specification of %s `%s` is invalid; the type checker '+'function must return `null` or an `Error` but returned a %s. '+'You may have forgotten to pass an argument to the type checker '+'creator (arrayOf, instanceOf, objectOf, oneOf, oneOfType, and '+'shape all require an argument).',componentName||'React class',ReactPropTypeLocationNames[location],typeSpecName,typeof error):void 0;
if(error instanceof Error&&!(error.message in loggedTypeFailures)){


loggedTypeFailures[error.message]=true;

var componentStackInfo='';

if(process.env.NODE_ENV!=='production'){
if(!ReactComponentTreeHook){
ReactComponentTreeHook=require(19 /* ./ReactComponentTreeHook */);
}
if(debugID!==null){
componentStackInfo=ReactComponentTreeHook.getStackAddendumByID(debugID);
}else if(element!==null){
componentStackInfo=ReactComponentTreeHook.getCurrentStackAddendum(element);
}
}

process.env.NODE_ENV!=='production'?warning(false,'Failed %s type: %s%s',location,error.message,componentStackInfo):void 0;
}
}
}
}

module.exports=checkReactTypeSpec;
}, "react/lib/checkReactTypeSpec.js");
__d(90 /* react/lib/ReactPropTypeLocationNames.js */, function(global, require, module, exports) {










'use strict';

var ReactPropTypeLocationNames={};

if(process.env.NODE_ENV!=='production'){
ReactPropTypeLocationNames={
prop:'prop',
context:'context',
childContext:'child context'};

}

module.exports=ReactPropTypeLocationNames;
}, "react/lib/ReactPropTypeLocationNames.js");
__d(91 /* react/lib/ReactPropTypesSecret.js */, function(global, require, module, exports) {










'use strict';

var ReactPropTypesSecret='SECRET_DO_NOT_PASS_THIS_OR_YOU_WILL_BE_FIRED';

module.exports=ReactPropTypesSecret;
}, "react/lib/ReactPropTypesSecret.js");
__d(92 /* fbjs/lib/shallowEqual.js */, function(global, require, module, exports) {













'use strict';

var hasOwnProperty=Object.prototype.hasOwnProperty;





function is(x,y){

if(x===y){


return x!==0||1/x===1/y;
}else{

return x!==x&&y!==y;
}
}






function shallowEqual(objA,objB){
if(is(objA,objB)){
return true;
}

if(typeof objA!=='object'||objA===null||typeof objB!=='object'||objB===null){
return false;
}

var keysA=Object.keys(objA);
var keysB=Object.keys(objB);

if(keysA.length!==keysB.length){
return false;
}


for(var i=0;i<keysA.length;i++){
if(!hasOwnProperty.call(objB,keysA[i])||!is(objA[keysA[i]],objB[keysA[i]])){
return false;
}
}

return true;
}

module.exports=shallowEqual;
}, "fbjs/lib/shallowEqual.js");
__d(93 /* react/lib/shouldUpdateReactComponent.js */, function(global, require, module, exports) {










'use strict';













function shouldUpdateReactComponent(prevElement,nextElement){
var prevEmpty=prevElement===null||prevElement===false;
var nextEmpty=nextElement===null||nextElement===false;
if(prevEmpty||nextEmpty){
return prevEmpty===nextEmpty;
}

var prevType=typeof prevElement;
var nextType=typeof nextElement;
if(prevType==='string'||prevType==='number'){
return nextType==='string'||nextType==='number';
}else{
return nextType==='object'&&prevElement.type===nextElement.type&&prevElement.key===nextElement.key;
}
}

module.exports=shouldUpdateReactComponent;
}, "react/lib/shouldUpdateReactComponent.js");
__d(94 /* react/lib/ReactEmptyComponent.js */, function(global, require, module, exports) {










'use strict';

var emptyComponentFactory;

var ReactEmptyComponentInjection={
injectEmptyComponentFactory:function injectEmptyComponentFactory(factory){
emptyComponentFactory=factory;
}};


var ReactEmptyComponent={
create:function create(instantiate){
return emptyComponentFactory(instantiate);
}};


ReactEmptyComponent.injection=ReactEmptyComponentInjection;

module.exports=ReactEmptyComponent;
}, "react/lib/ReactEmptyComponent.js");
__d(95 /* react/lib/ReactHostComponent.js */, function(global, require, module, exports) {










'use strict';

var _prodInvariant=require(20 /* ./reactProdInvariant */),
_assign=require(64 /* object-assign */);

var invariant=require(22 /* fbjs/lib/invariant */);

var genericComponentClass=null;

var tagToComponentClass={};
var textComponentClass=null;

var ReactHostComponentInjection={


injectGenericComponentClass:function injectGenericComponentClass(componentClass){
genericComponentClass=componentClass;
},


injectTextComponentClass:function injectTextComponentClass(componentClass){
textComponentClass=componentClass;
},


injectComponentClasses:function injectComponentClasses(componentClasses){
_assign(tagToComponentClass,componentClasses);
}};








function createInternalComponent(element){
!genericComponentClass?process.env.NODE_ENV!=='production'?invariant(false,'There is no registered component for the tag %s',element.type):_prodInvariant('111',element.type):void 0;
return new genericComponentClass(element);
}





function createInstanceForText(text){
return new textComponentClass(text);
}





function isTextComponent(component){
return component instanceof textComponentClass;
}

var ReactHostComponent={
createInternalComponent:createInternalComponent,
createInstanceForText:createInstanceForText,
isTextComponent:isTextComponent,
injection:ReactHostComponentInjection};


module.exports=ReactHostComponent;
}, "react/lib/ReactHostComponent.js");
;require(0);