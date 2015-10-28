// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// TODO(jochen): Remove this after the setting is turned on globally.
#define V8_IMMINENT_DEPRECATION_WARNINGS

#include "test/cctest/compiler/function-tester.h"

using namespace v8::internal;
using namespace v8::internal::compiler;

TEST(ArgumentsMapped) {
  FunctionTester T("(function(a) { return arguments; })");

  Handle<Object> arguments;
  T.Call(T.Val(19), T.Val(23), T.Val(42), T.Val(65)).ToHandle(&arguments);
  CHECK(arguments->IsJSObject() && !arguments->IsJSArray());
  CHECK(JSObject::cast(*arguments)->HasSloppyArgumentsElements());
  Handle<String> l = T.isolate->factory()->length_string();
  Handle<Object> length = JSObject::GetProperty(arguments, l).ToHandleChecked();
  CHECK_EQ(4, length->Number());
}


TEST(ArgumentsUnmapped) {
  FunctionTester T("(function(a) { 'use strict'; return arguments; })");

  Handle<Object> arguments;
  T.Call(T.Val(19), T.Val(23), T.Val(42), T.Val(65)).ToHandle(&arguments);
  CHECK(arguments->IsJSObject() && !arguments->IsJSArray());
  CHECK(!JSObject::cast(*arguments)->HasSloppyArgumentsElements());
  Handle<String> l = T.isolate->factory()->length_string();
  Handle<Object> length = JSObject::GetProperty(arguments, l).ToHandleChecked();
  CHECK_EQ(4, length->Number());
}


TEST(ArgumentsRest) {
  FLAG_harmony_rest_parameters = true;
  FunctionTester T("(function(a, ...args) { return args; })");

  Handle<Object> arguments;
  T.Call(T.Val(19), T.Val(23), T.Val(42), T.Val(65)).ToHandle(&arguments);
  CHECK(arguments->IsJSObject() && arguments->IsJSArray());
  CHECK(!JSObject::cast(*arguments)->HasSloppyArgumentsElements());
  Handle<String> l = T.isolate->factory()->length_string();
  Handle<Object> length = JSObject::GetProperty(arguments, l).ToHandleChecked();
  CHECK_EQ(3, length->Number());
}
