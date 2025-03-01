#ifndef COMMON_COMPILER_CONTEXT_H_
#define COMMON_COMPILER_CONTEXT_H_

#include "backends/p4tools/common/compiler/configuration.h"
#include "frontends/common/options.h"

namespace P4Tools {

/// A compilation context for P4Tools that provides a custom compiler configuration.
template <typename OptionsType>
class CompileContext : public virtual P4CContext {
 public:
    /// @return the current compilation context, which must be of type
    /// CompileContext<OptionsType>.
    static CompileContext& get() { return CompileContextStack::top<CompileContext>(); }

    CompileContext() {}

    template <typename OptionsDerivedType>
    CompileContext(CompileContext<OptionsDerivedType>& context)
        : optionsInstance(context.options()) {}

    /// @return the compiler options for this compilation context.
    OptionsType& options() override { return optionsInstance; }

 protected:
    virtual const CompilerConfiguration& getConfigImpl() { return CompilerConfiguration::get(); }

 private:
    /// The compiler options for this compilation context.
    OptionsType optionsInstance;
};

}  // namespace P4Tools

#endif /* COMMON_COMPILER_CONTEXT_H_ */
