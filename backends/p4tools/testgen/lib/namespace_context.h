#ifndef TESTGEN_LIB_NAMESPACE_CONTEXT_H_
#define TESTGEN_LIB_NAMESPACE_CONTEXT_H_

#include <set>

#include <boost/optional/optional.hpp>

#include "ir/ir.h"
#include "lib/cstring.h"

namespace P4Tools {

namespace P4Testgen {

/// Represents a stack of namespaces.
class NamespaceContext {
 private:
    const IR::INamespace* curNamespace;
    const NamespaceContext* outer;

    /// All names that appear in this context and all outer contexts. Populated lazily.
    mutable boost::optional<std::set<cstring>> usedNames;

    NamespaceContext(const IR::INamespace* ns, const NamespaceContext* outer)
        : curNamespace(ns), outer(outer) {}

    /// @returns either a declaration or a nullptr if no matching declaration was found in this
    /// nested namespace. If the nested namespace contains another nested namespace, this function
    /// recurses. It does not step into outer namespaces.
    const IR::IDeclaration* findNestedDecl(const IR::INestedNamespace* nestedNameSpace,
                                           const IR::Path* path) const;

 public:
    /// Represents the empty namespace context.
    static const NamespaceContext* Empty;

    /// @returns a new namespace context, representing the given namespace @ns pushed onto this
    /// namespace context.
    const NamespaceContext* push(const IR::INamespace* ns) const;

    /// @returns the namespace context surrounding this context.
    const NamespaceContext* pop() const;

    /// Looks up a declaration in this context. A BUG occurs if the declaration cannot be found.
    const IR::IDeclaration* findDecl(const IR::Path* path) const;

    /// @returns all names that appear in this context and all outer contexts.
    const std::set<cstring>& getUsedNames() const;

    /// @returns a name that is fresh in this context. The given @name and @sep are passed to
    /// cstring::make_unique.
    const cstring genName(cstring name, char sep) const;
};

}  // namespace P4Testgen

}  // namespace P4Tools

#endif /* TESTGEN_LIB_NAMESPACE_CONTEXT_H_ */
