// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// Sample ECMAScript code for exercising the debugger sample. It's not meant to instruct actual usage of the library.
//

if (typeof host === "undefined") {
  // Shims for running under Node.js.
  global.host = {
    "arguments": process.argv,
    echo: console.log,
    throw: () => { /* no-op on Node.js. */ }
  };
}

// Print the arguments that the script received.
for (const arg of host.arguments) {
  host.echo(`Found arg: ${arg}`);
}

function testScriptThrow() {
  try {
    throw { foo: 4, bar: [1, 2, 3] };
  } catch (e) {
    host.echo(e);
  }

  // Uncomment this to test second chance exceptions.
  ////throw new Error("second chance");
}

function testNativeThrow() {
  try {
    host.throw();
  } catch (e) {
    host.echo(e);
  }

  // Uncomment this to test second chance exceptions.
  ////host.throw();
}

function testScopes() {
  function deepScope() {
    let j = 42;
    j += i;
    return j;
  }

  let i = 47;
  i += deepScope();
  i += 4584;
  host.echo(i);
}

function testValues() {
  let nullVal = null;
  let nanVal = NaN;
  let negZeroVal = -0;
  let posInfVal = Number.POSITIVE_INFINITY;
  let negInfVal = Number.NEGATIVE_INFINITY;

  host.echo(nullVal);
  host.echo(nanVal);
  host.echo(negZeroVal);
  host.echo(posInfVal);
  host.echo(negInfVal);
}

function testConsoleAPIs() {
  let obj1 = {x:1};
  console.log('first string')
  console.log(100);
  console.error('An error');
  console.debug(obj1);
  console.info('This is an info');
}

// Run the test scenarios.
testScriptThrow();
testNativeThrow();
testScopes();
testValues();
testConsoleAPIs();

// The exit code
0;
