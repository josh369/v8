// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/handles.h"
#include "src/interface-descriptors.h"
#include "src/isolate.h"
#include "test/cctest/compiler/function-tester.h"

namespace v8 {
namespace internal {
namespace compiler {

class ZoneHolder {
 public:
  explicit ZoneHolder(Isolate* isolate)
      : held_zone_(isolate->allocator(), ZONE_NAME) {}
  Zone* held_zone() { return &held_zone_; }

 private:
  Zone held_zone_;
};

// Inherit from ZoneHolder in order to create a zone that can be passed to
// CodeAssembler base class constructor.
template <typename CodeAssemblerT>
class CodeAssemblerTesterImpl : private ZoneHolder, public CodeAssemblerT {
 public:
  // Test generating code for a stub.
  CodeAssemblerTesterImpl(Isolate* isolate,
                          const CallInterfaceDescriptor& descriptor)
      : ZoneHolder(isolate),
        CodeAssemblerT(isolate, ZoneHolder::held_zone(), descriptor,
                       Code::ComputeFlags(Code::STUB), "test"),
        scope_(isolate) {}

  // Test generating code for a JS function (e.g. builtins).
  CodeAssemblerTesterImpl(Isolate* isolate, int parameter_count,
                          Code::Kind kind = Code::FUNCTION)
      : ZoneHolder(isolate),
        CodeAssemblerT(isolate, ZoneHolder::held_zone(), parameter_count,
                       Code::ComputeFlags(kind), "test"),
        scope_(isolate) {}

  // This constructor is intended to be used for creating code objects with
  // specific flags.
  CodeAssemblerTesterImpl(Isolate* isolate, Code::Flags flags)
      : ZoneHolder(isolate),
        CodeAssemblerT(isolate, ZoneHolder::held_zone(), 0, flags, "test"),
        scope_(isolate) {}

  Handle<Code> GenerateCodeCloseAndEscape() {
    return scope_.CloseAndEscape(CodeAssemblerT::GenerateCode());
  }

 private:
  HandleScope scope_;
  LocalContext context_;
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8
