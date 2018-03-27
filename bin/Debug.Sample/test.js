if (typeof host === "undefined") {
  global.host = {
    echo: console.log
  };
}

function doStuff() {
  let i = 0;
  i += 4584;
  return i;
}

var result = doStuff();
host.echo(result);

0;
