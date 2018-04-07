if (typeof host === "undefined") {
  global.host = {
    echo: console.log
  };
}

for (const arg of host.arguments) {
    host.echo(`Found arg: ${arg}`);
}

function doStuff() {
  let i = 0;
  i += 4584;
  return i;
}

throw 3;

var result = doStuff();
host.echo(result);

0;
