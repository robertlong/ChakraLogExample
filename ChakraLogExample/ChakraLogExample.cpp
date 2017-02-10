#define _JSRT_
#include <stdint.h>
#include "ChakraCore.h"
#include "Log.h"
#include <string>
#include <iostream>


using namespace std;

/**
 * Log Example
 * Demonstrates usage of JSSetPrototype, JSCreateExternalObject, and JSGetExternalData
 **/

// Reference the Log.prototype value. Stored so we can reuse it in the constructor.
JsValueRef JSLogPrototype;

// Callback to print to the message to the console.
// TODO: implement argument exception handling.
JsValueRef CALLBACK JSLogConsole(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	void* maybeLog;

	// Try to get the external data from the "this" argument.
	if (JsGetExternalData(arguments[0], &maybeLog) == JsNoError) {
		// Cast the external data to the Log class.
		Log* log = static_cast<Log*>(maybeLog);

		// Convert the passed string argument to a string literal.
		JsValueRef stringValue;
		JsConvertValueToString(arguments[1], &stringValue);
		const wchar_t *string;
		size_t length;
		JsStringToPointer(stringValue, &string, &length);

		// Print to console using the native print method.
		log->print(string);
	}
		

	return JS_INVALID_REFERENCE;
}

// Constructor callback for creating a Log object in JS. Accepts one parameter: type
// TODO: implement argument exception handling.
JsValueRef CALLBACK JSLogConstructor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	JsValueRef logObject = JS_INVALID_REFERENCE;

	// Convert the passed string argument to a string literal.
	JsValueRef stringValue;
	JsConvertValueToString(arguments[1], &stringValue);
	const wchar_t *string;
	size_t length;
	JsStringToPointer(stringValue, &string, &length);

	// Create the native Log object.
	Log* log = new Log(string);

	// Set the external object to the native Log object.
	JsCreateExternalObject(log, nullptr, &logObject);

	// Set the prototype for the Log class.
	JsSetPrototype(logObject, JSLogPrototype);

	// Return the newly created Log instance.
	return logObject;
}

int main()
{
	JsRuntimeHandle runtime;
	JsContextRef context;
	JsValueRef result;
	JsValueRef global;
	unsigned currentSourceContext = 0;

	// Your script, try replace the basic hello world with something else
	wstring script = L"(()=>{let log = new Log(\'Test\'); log.console(\'Hey!\'); return \'Hello world!\';})()";

	// Create a runtime. 
	JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime);

	// Create an execution context. 
	JsCreateContext(runtime, &context);

	// Now set the execution context as being the current one on this thread.
	JsSetCurrentContext(context);

	JsGetGlobalObject(&global);

	// Build the JSLogPrototype
	JsCreateObject(&JSLogPrototype);

	// Add the .console function
	JsValueRef consoleFunc;
	JsCreateFunction(JSLogConsole, nullptr, &consoleFunc);
	JsPropertyIdRef consolePropertyId;
	JsGetPropertyIdFromName(L"console", &consolePropertyId);
	JsSetProperty(JSLogPrototype, consolePropertyId, consoleFunc, true);

	// Build the JSLogConstructor and set the prototype;
	JsValueRef JSLog;
	JsCreateFunction(JSLogConstructor, nullptr, &JSLog);
	JsSetPrototype(JSLog, JSLogPrototype);

	// Set the Log class on the global object.
	JsPropertyIdRef propertyId;
	JsGetPropertyIdFromName(L"Log", &propertyId);
	JsSetProperty(global, propertyId, JSLog, true);

	// Run the script.
	JsRunScript(script.c_str(), currentSourceContext++, L"", &result);

	// Convert your script result to String in JavaScript; redundant if your script returns a String
	JsValueRef resultJSString;
	JsConvertValueToString(result, &resultJSString);

	// Project script result in JS back to C++.
	const wchar_t *resultWC;
	size_t stringLength;
	JsStringToPointer(resultJSString, &resultWC, &stringLength);

	wstring resultW(resultWC);
	cout << string(resultW.begin(), resultW.end()) << endl;
	system("pause");

	// Dispose runtime
	JsSetCurrentContext(JS_INVALID_REFERENCE);
	JsDisposeRuntime(runtime);

	return 0;
}