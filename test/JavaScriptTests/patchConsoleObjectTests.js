var assert = require('assert');
var patchConsoleObject = require("../../bin/Debugger.Sample/PatchConsoleObject.js")

var dConsoleDebug = "";
var dConsoleError = "";
var dConsoleWarn = "";
var dConsoleLog = "";
var dConsoleInfo = "";
var consoleDebug = "";
var consoleError = "";
var consoleWarn = "";
var consoleLog = "";
var consoleInfo = "";

var consoleDebugger = {
    log(msg){
        dConsoleLog += msg;
    },
    error(msg){
        dConsoleError += msg;
    },
    warn(msg){
        dConsoleWarn += msg;
    },
    debug(msg){
        dConsoleDebug += msg;
    },
    info(msg){
        dConsoleInfo += msg;
    }
};

var consoleObjectEmpty = {
   
};

var consoleLogOnly = {
    log(msg){
        consoleLog += msg;
    }
};

var consoleObjectWithNoLoggingMethods = {
    noLog(msg){
        consoleLog += msg;
    }
};

var consoleObjectCompelete = {
    log(msg){
        consoleLog += msg;
    },
    error(msg){
        consoleError += msg;
    },
    warn(msg){
        consoleWarn += msg;
    },
    debug(msg){
        consoleDebug += msg;
    },
    info(msg){
        consoleInfo += msg;
    }
};

describe("patchConsoleObjectTests", function() {
    var global = {};
    beforeEach(function() {
        global = {};
        consoleLog = "";
        consoleDebug = "";
        consoleError = "";
        consoleWarn = "";
        consoleLog = "";
        consoleInfo = "";
        dConsoleLog = "";
        dConsoleDebug = "";
        dConsoleError = "";
        dConsoleWarn = "";
        dConsoleLog = "";
        dConsoleInfo = "";
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
        assert.strictEqual(dConsoleLog, "log test");
        assert.strictEqual(dConsoleError, "");
        assert.strictEqual(dConsoleWarn, "");
        assert.strictEqual(dConsoleDebug, "");
        assert.strictEqual(dConsoleInfo, "");
        assert.strictEqual(consoleLog,"log test");
        assert.strictEqual(consoleError, "");
        assert.strictEqual(consoleWarn, "");
        assert.strictEqual(consoleDebug, "");
        assert.strictEqual(consoleInfo, "");
    });
    it("case with compelete console object", function() {
        patchConsoleObject(global, consoleObjectCompelete, consoleDebugger);
        global.console.log("log test");
        global.console.info("info test");
        global.console.warn("warn test");
        global.console.error("error test");
        global.console.debug("debug test");
        assert.strictEqual(dConsoleLog, "log test");
        assert.strictEqual(dConsoleError, "error test");
        assert.strictEqual(dConsoleWarn, "warn test");
        assert.strictEqual(dConsoleDebug, "debug test");
        assert.strictEqual(dConsoleInfo, "info test");
        assert.strictEqual(consoleLog,"log test");
        assert.strictEqual(consoleError, "error test");
        assert.strictEqual(consoleWarn, "warn test");
        assert.strictEqual(consoleDebug, "debug test");
        assert.strictEqual(consoleInfo, "info test");
    });
});
