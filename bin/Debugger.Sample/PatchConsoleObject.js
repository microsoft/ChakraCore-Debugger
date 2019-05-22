function patchConsoleObject$$1(global, console, debugConsole) {
    var obj = {};
    for (var fn in console) {
        if (typeof console[fn] === "function") {
            (function(name) {
                obj[name] = function(...args) {
                    console[name](...args);
                    if (name in debugConsole && typeof debugConsole[name] === "function") {
                        debugConsole[name](...args);
                    }
                }
            })(fn);
        }
    }
    global.console = obj;
}
patchConsoleObject$$1;

module.exports = patchConsoleObject$$1;