// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/execution/arguments-inl.h"
#include "src/logging/counters.h"
#include "src/objects/js-promise.h"
#include "src/objects/objects-inl.h"
#include "src/objects/source-text-module.h"
#include "src/runtime/runtime-utils.h"

namespace v8 {
namespace internal {

namespace {
Handle<Script> GetEvalOrigin(Isolate* isolate, Script origin_script) {
  DisallowGarbageCollection no_gc;
  while (origin_script.has_eval_from_shared()) {
    HeapObject maybe_script = origin_script.eval_from_shared().script();
    CHECK(maybe_script.IsScript());
    origin_script = Script::cast(maybe_script);
  }
  return handle(origin_script, isolate);
}
}  // namespace

RUNTIME_FUNCTION(Runtime_DynamicImportCall) {
  HandleScope scope(isolate);
  DCHECK_LE(2, args.length());
  DCHECK_GE(3, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, function, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, specifier, 1);

  MaybeHandle<Object> import_assertions;
  if (args.length() == 3) {
    CHECK(args[2].IsObject());
    import_assertions = args.at<Object>(2);
  }

  Handle<Script> referrer_script =
      GetEvalOrigin(isolate, Script::cast(function->shared().script()));
  RETURN_RESULT_OR_FAILURE(isolate,
                           isolate->RunHostImportModuleDynamicallyCallback(
                               referrer_script, specifier, import_assertions));
}

RUNTIME_FUNCTION(Runtime_GetModuleNamespace) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_SMI_ARG_CHECKED(module_request, 0);
  Handle<SourceTextModule> module(isolate->context().module(), isolate);
  return *SourceTextModule::GetModuleNamespace(isolate, module, module_request);
}

RUNTIME_FUNCTION(Runtime_GetImportMetaObject) {
  HandleScope scope(isolate);
  DCHECK_EQ(0, args.length());
  Handle<SourceTextModule> module(isolate->context().module(), isolate);
  RETURN_RESULT_OR_FAILURE(isolate,
                           SourceTextModule::GetImportMeta(isolate, module));
}

}  // namespace internal
}  // namespace v8
