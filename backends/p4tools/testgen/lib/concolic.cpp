#include "backends/p4tools/testgen/lib/concolic.h"

#include <utility>
#include <vector>

#include <boost/optional/optional.hpp>

#include "backends/p4tools/common/lib/formulae.h"
#include "lib/exceptions.h"
#include "lib/null.h"

#include "backends/p4tools/testgen/lib/execution_state.h"

namespace P4Tools {

namespace P4Testgen {

bool ConcolicMethodImpls::matches(const std::vector<cstring>& paramNames,
                                  const IR::Vector<IR::Argument>* args) {
    CHECK_NULL(args);

    // Number of parameters should match the number of arguments.
    if (paramNames.size() != args->size()) {
        return false;
    }
    // Any named argument should match the name of the corresponding parameter.
    for (size_t idx = 0; idx < paramNames.size(); idx++) {
        const auto& paramName = paramNames.at(idx);
        const auto& arg = args->at(idx);

        if (arg->name.name == nullptr) {
            continue;
        }
        if (paramName != arg->name.name) {
            return false;
        }
    }

    return true;
}

bool ConcolicMethodImpls::exec(cstring qualifiedMethodName, const IR::ConcolicVariable* var,
                               const ExecutionState& state, const Model* completedModel,
                               ConcolicVariableMap* resolvedConcolicVariables) const {
    if (impls.count(qualifiedMethodName) == 0) {
        return false;
    }

    const auto* args = var->arguments;

    const auto& submap = impls.at(qualifiedMethodName);
    if (submap.count(args->size()) == 0) {
        return false;
    }

    // Find matching methods: if any arguments are named, then the parameter name must match.
    boost::optional<MethodImpl> matchingImpl;
    for (const auto& pair : submap.at(args->size())) {
        const auto& paramNames = pair.first;
        const auto& methodImpl = pair.second;

        if (matches(paramNames, args)) {
            BUG_CHECK(!matchingImpl, "Ambiguous extern method call: %1%", qualifiedMethodName);
            matchingImpl = methodImpl;
        }
    }

    if (!matchingImpl) {
        return false;
    }
    (*matchingImpl)(qualifiedMethodName, var, state, completedModel, resolvedConcolicVariables);
    return true;
}

void ConcolicMethodImpls::add(const ImplList& inputImplList) {
    for (const auto& implSpec : inputImplList) {
        cstring name;
        std::vector<cstring> paramNames;
        MethodImpl impl;
        std::tie(name, paramNames, impl) = implSpec;

        auto& tmpImplList = impls[name][paramNames.size()];

        // Make sure that we have at most one implementation for each set of parameter names.
        // This is a quadratic-time algorithm, but should be fine, since we expect the number of
        // overloads to be small in practice.
        for (auto& pair : tmpImplList) {
            BUG_CHECK(pair.first != paramNames, "Multiple implementations of %1%(%2%)", name,
                      paramNames);
        }

        tmpImplList.emplace_back(paramNames, impl);
    }
}

ConcolicMethodImpls::ConcolicMethodImpls(const ImplList& implList) { add(implList); }

bool ConcolicResolver::preorder(const IR::ConcolicVariable* var) {
    cstring concolicMethodName = var->concolicMethodName;
    // Convert the concolic member variable to a state variable.
    StateVariable concolicVarName = var->concolicMember;
    auto concolicReplacment = resolvedConcolicVariables.find(concolicVarName);
    if (concolicReplacment == resolvedConcolicVariables.end()) {
        bool found = concolicMethodImpls->exec(concolicMethodName, var, state, completedModel,
                                               &resolvedConcolicVariables);
        BUG_CHECK(found, "Unknown or unimplemented concolic method: %1%", concolicMethodName);
    }
    return false;
}

const ConcolicVariableMap* ConcolicResolver::getResolvedConcolicVariables() const {
    return &resolvedConcolicVariables;
}

ConcolicResolver::ConcolicResolver(const Model* completedModel, const ExecutionState& state,
                                   const ConcolicMethodImpls* concolicMethodImpls)
    : state(state), completedModel(completedModel), concolicMethodImpls(concolicMethodImpls) {
    visitDagOnce = false;
}

ConcolicResolver::ConcolicResolver(const Model* completedModel, const ExecutionState& state,
                                   const ConcolicMethodImpls* concolicMethodImpls,
                                   ConcolicVariableMap resolvedConcolicVariables)
    : state(state),
      completedModel(completedModel),
      resolvedConcolicVariables(std::move(resolvedConcolicVariables)),
      concolicMethodImpls(concolicMethodImpls) {
    visitDagOnce = false;
}

static const ConcolicMethodImpls::ImplList coreConcolicMethodImpls({});

const ConcolicMethodImpls::ImplList* Concolic::getCoreConcolicMethodImpls() {
    return &coreConcolicMethodImpls;
}

}  // namespace P4Testgen

}  // namespace P4Tools
