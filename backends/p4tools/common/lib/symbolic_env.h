#ifndef COMMON_LIB_SYMBOLIC_ENV_H_
#define COMMON_LIB_SYMBOLIC_ENV_H_

#include <map>

#include "backends/p4tools/common/lib/formulae.h"
#include "backends/p4tools/common/lib/model.h"
#include "ir/ir.h"

namespace P4Tools {

/// A symbolic environment maps variables to their symbolic value. A symbolic value is just an
/// expression on the program's initial state.
class SymbolicEnv {
 private:
    SymbolicMapType map;

 public:
    // Maybe coerce from Model for concrete execution?

    /// @returns the symbolic value for the given variable.
    const IR::Expression* get(const StateVariable& var) const;

    /// Checks whether the given variable exists in the symbolic environment.
    bool exists(const StateVariable& var) const;

    /// Sets the symbolic value of the given state variable to the given value. Constant folding is
    /// done on the given value before updating the symbolic state.
    void set(const StateVariable& var, const IR::Expression* value);

    /// Completes the model with all variables referenced in the symbolic environment.
    Model* complete(const Model& model) const;

    /// Evaluates this symbolic environment in the context of the given model.
    ///
    /// A BUG occurs if any symbolic value in this environment refers to a variable that is not
    /// bound by the given model.
    Model* evaluate(const Model& model) const;

    /// Substitutes state variables in @expr for their symbolic value in this environment.
    /// Variables that are unbound by this environment are left untouched.
    const IR::Expression* subst(const IR::Expression* expr) const;

    /// @returns The immutable map that is internal to this symbolic environment.
    const SymbolicMapType& getInternalMap() const;

    /// Determines whether the given node represents a symbolic value. Symbolic values may be
    /// stored in the symbolic environment.
    static bool isSymbolicValue(const IR::Node*);
};

}  // namespace P4Tools

#endif /* COMMON_LIB_SYMBOLIC_ENV_H_ */
