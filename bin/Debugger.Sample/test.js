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
  return i;
}

// Run the test scenarios.
testScriptThrow();
testNativeThrow();
host.echo(testScopes());

// The exit code
0;
