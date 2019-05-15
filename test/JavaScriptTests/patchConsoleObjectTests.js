var assert = require('assert');
var patchConsoleObject = require("../../bin/Debugger.Sample/PatchConsoleObject.js")

var consoleOutput = "";
var consoleArgType = "";
var debuggerArgType = "";

var consoleDebugger = {
    log(msg){
        consoleOutput += "DEBUGGER LOG: "+ msg + "\n";
    },
    error(msg){
        consoleOutput += "DEBUGGER ERROR: "+ msg + "\n";
    },
    warn(msg){
        consoleOutput += "DEBUGGER WARN: "+ msg + "\n";
    },
    debug(msg){
        consoleOutput += "DEBUGGER DEBUG: "+ msg + "\n";
    },
    info(msg){
        consoleOutput += "DEBUGGER INFO: "+ msg + "\n";
    }
};

var consoleObjectEmpty = {
   
};

var consoleLogOnly = {
    log(msg){
        consoleOutput += "CONSOLE LOG: "+ msg + "\n";
    }
};

var consoleObjectWithNoLoggingMethods = {
    noLog(msg){
        consoleOutput += "CONSOLE NOLOG: "+ msg + "\n";
    }
};

var consoleObjectCompelete = {
    log(msg){
        consoleOutput += "CONSOLE LOG: "+ msg + "\n";
    },
    error(msg){
        consoleOutput += "CONSOLE ERROR: "+ msg + "\n";
    },
    warn(msg){
        consoleOutput += "CONSOLE WARN: "+ msg + "\n";
    },
    debug(msg){
        consoleOutput += "CONSOLE DEBUG: "+ msg + "\n";
    },
    info(msg){
        consoleOutput += "CONSOLE INFO: "+ msg + "\n";
    }
};

var consoleObjectTypeVerification = {
    log(msg){
        consoleArgType = typeof msg;
    },
}

var consoleDebuggerTypeVerification = {
    log(msg){
        debuggerArgType = typeof msg;
    },
}

describe("patchConsoleObjectTests", function() {
    var global = {};
    beforeEach(function() {
        global = {};
        consoleOutput = "";
    });
    it("case with empty console object", function() {
        patchConsoleObject(global, consoleObjectEmpty, consoleDebugger);
        assert.strictEqual(Object.keys(global.console).length, 0);
    });
    it("case with console object not containing logging methods", function() {
        patchConsoleObject(global, consoleObjectWithNoLoggingMethods, consoleDebugger);
        assert.strictEqual(Object.keys(global.console).length, 1);
        assert.strictEqual(Object.keys(global.console)[0], "noLog");
    });
    it("case where console object only has log method", function() {
        patchConsoleObject(global, consoleLogOnly, consoleDebugger);
        global.console.log("log test");
        assert.strictEqual(consoleOutput.trim().split("\n").length, 2);
        assert.strictEqual(consoleOutput.split("\n")[0], "CONSOLE LOG: log test");
        assert.strictEqual(consoleOutput.split("\n")[1], "DEBUGGER LOG: log test");
    });
    it("case with complete console object", function() {
        patchConsoleObject(global, consoleObjectCompelete, consoleDebugger);
        global.console.log("log test");
        global.console.info("info test");
        global.console.warn("warn test");
        global.console.error("error test");
        global.console.debug("debug test");
        assert.strictEqual(consoleOutput.trim().split("\n").length, 10);
        assert.strictEqual(consoleOutput.split("\n")[0], "CONSOLE LOG: log test");
        assert.strictEqual(consoleOutput.split("\n")[1], "DEBUGGER LOG: log test");
        assert.strictEqual(consoleOutput.split("\n")[2], "CONSOLE INFO: info test");
        assert.strictEqual(consoleOutput.split("\n")[3], "DEBUGGER INFO: info test");
        assert.strictEqual(consoleOutput.split("\n")[4], "CONSOLE WARN: warn test");
        assert.strictEqual(consoleOutput.split("\n")[5], "DEBUGGER WARN: warn test");
        assert.strictEqual(consoleOutput.split("\n")[6], "CONSOLE ERROR: error test");
        assert.strictEqual(consoleOutput.split("\n")[7], "DEBUGGER ERROR: error test");
        assert.strictEqual(consoleOutput.split("\n")[8], "CONSOLE DEBUG: debug test");
        assert.strictEqual(consoleOutput.split("\n")[9], "DEBUGGER DEBUG: debug test");
    });
    it("type verification case", function() {
        patchConsoleObject(global, consoleObjectTypeVerification, consoleDebuggerTypeVerification);
        global.console.log(1);
        assert.strictEqual(consoleArgType, "number");
        assert.strictEqual(debuggerArgType, "number");
        global.console.log("");
        assert.strictEqual(consoleArgType, "string");
        assert.strictEqual(debuggerArgType, "string");
        global.console.log({});
        assert.strictEqual(consoleArgType, "object");
        assert.strictEqual(debuggerArgType, "object");
    });
});
