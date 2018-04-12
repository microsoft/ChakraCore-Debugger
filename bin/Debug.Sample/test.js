if (typeof host === "undefined") {
  global.host = {
    echo: console.log
  };
}

for (const arg of host.arguments) {
    host.echo(`Found arg: ${arg}`);
}

function doStuff() {
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

function errorStuff() {
  try {
    throw { foo: 4, bar: [ 1, 2, 3 ] };
    return true;
  } catch (e) {
    return false;
  }
}

var result = doStuff();
host.echo(errorStuff());
host.echo(result);

0;
