# Copyright 2016 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os
import re

from testrunner.local import testsuite
from testrunner.objects import testcase

FLAGS_PATTERN = re.compile(r"//\s+Flags:(.*)")

class DebuggerTestSuite(testsuite.TestSuite):

  def __init__(self, name, root):
    super(DebuggerTestSuite, self).__init__(name, root)

  def ListTests(self, context):
    tests = []
    for dirname, dirs, files in os.walk(self.root):
      for dotted in [x for x in dirs if x.startswith('.')]:
        dirs.remove(dotted)
      dirs.sort()
      files.sort()
      for filename in files:
        if (filename.endswith(".js") and filename != "test-api.js"):
          fullpath = os.path.join(dirname, filename)
          relpath = fullpath[len(self.root) + 1 : -3]
          testname = relpath.replace(os.path.sep, "/")
          test = testcase.TestCase(self, testname)
          tests.append(test)
    return tests

  def GetFlagsForTestCase(self, testcase, context):
    source = self.GetSourceForTest(testcase)
    flags = ["--enable-inspector", "--allow-natives-syntax"] + context.mode_flags
    flags_match = re.findall(FLAGS_PATTERN, source)
    for match in flags_match:
      flags += match.strip().split()

    files = []
    files.append(os.path.normpath(os.path.join(self.root, "..", "mjsunit", "mjsunit.js")))
    files.append(os.path.join(self.root, "test-api.js"))
    files.append(os.path.join(self.root, testcase.path + self.suffix()))

    flags += files
    if context.isolates:
      flags.append("--isolate")
      flags += files

    return testcase.flags + flags

  def GetSourceForTest(self, testcase):
    filename = os.path.join(self.root, testcase.path + self.suffix())
    with open(filename) as f:
      return f.read()

def GetSuite(name, root):
  return DebuggerTestSuite(name, root)
