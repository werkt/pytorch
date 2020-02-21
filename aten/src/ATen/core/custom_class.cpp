#include <ATen/core/jit_type.h>
#include <torch/custom_class.h>

#include <atomic>

namespace torch {
namespace jit {

std::vector<c10::RegisterOperators>& registeredOps() {
  static std::vector<c10::RegisterOperators> ops;
  return ops;
}

std::unordered_map<std::string, detail::RegisteredClassRecord>& registeredClasses() {
  static std::unordered_map<std::string, detail::RegisteredClassRecord> registry;
  return registry;
}

namespace {
  std::vector<ClassRegistrationCallback> &class_callbacks() {
    static std::vector<ClassRegistrationCallback> cbs;
    return cbs;
  }

  std::vector<MethodRegistrationCallback> &method_callbacks() {
    static std::vector<MethodRegistrationCallback> cbs;
    return cbs;
  }
}  // namespace

void registerClassRegistrationCallback(ClassRegistrationCallback cb) {
  class_callbacks().emplace_back(std::move(cb));
}

void registerMethodRegistrationCallback(MethodRegistrationCallback cb) {
  method_callbacks().emplace_back(std::move(cb));
}

void invokeClassRegistrationCallbacks(const detail::RegisteredClassRecord& class_record) {
  for (auto & cb : class_callbacks()) {
    cb(class_record);
  }
}

void invokeMethodRegistrationCallbacks(const detail::RegisteredClassRecord& class_record, const std::string& method_name) {
  for (auto & cb : method_callbacks()) {
    cb(class_record, method_name);
  }
}

at::TypePtr getCustomClass(const std::string& name) {
  if (registeredClasses().count(name)) {
    return registeredClasses().at(name).classTypePtr;
  }
  return nullptr;
}


} // namespace jit
} // namespace torch