#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Copyright (C) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.
#---------------------------------------------------------------------------------------------------

import os
import sys
import argparse
import subprocess

try:
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument("--generator_script")
    cmdline_parser.add_argument("--markupsafe_dir")
    arg_options, args = cmdline_parser.parse_known_args()

    generator_script = arg_options.generator_script
    if not generator_script:
        raise Exception("Generator script must be specified")

    markupsafe_dir = arg_options.markupsafe_dir
    if not markupsafe_dir:
        raise Exception("markupsafe directory must be specified")

except Exception:
    exc = sys.exc_info()[1]
    sys.stderr.write("Failed to parse command-line arguments: %s\n\n" % exc)
    exit(1)

env = os.environ.copy()
path = os.path.abspath(markupsafe_dir)
env["PYTHONPATH"] = env.get("PYTHONPATH", "") + ";" + path
subprocess.Popen([sys.executable, generator_script] + args, env=env)
