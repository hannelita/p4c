#ifndef COMMON_COMPILER_COMPILER_TARGET_H_
#define COMMON_COMPILER_COMPILER_TARGET_H_

#include <string>
#include <vector>

#include <boost/optional/optional.hpp>

#include "backends/p4tools/common/compiler/midend.h"
#include "backends/p4tools/common/core/target.h"
#include "frontends/common/options.h"
#include "frontends/p4/frontend.h"
#include "ir/ir.h"
#include "lib/compile_context.h"

namespace P4Tools {

/// Encapsulates the details of invoking the P4 compiler for a target device and architecture.
class CompilerTarget : public Target {
 public:
    /// @returns a new compilation context for the compiler.
    static ICompileContext* makeContext();

    /// Initializes the P4 compiler with the given compiler-specific command-line arguments.
    ///
    /// @returns any unprocessed arguments, or nullptr if there was an error.
    static std::vector<const char*>* initCompiler(int argc, char** argv);

    /// Runs the P4 compiler to produce an IR.
    ///
    /// @returns boost::none if an error occurs during compilation.
    static boost::optional<const IR::P4Program*> runCompiler();

    /// Runs the P4 compiler to produce an IR for the given source code.
    ///
    /// @returns boost::none if an error occurs during compilation.
    static boost::optional<const IR::P4Program*> runCompiler(const std::string& source);

 private:
    /// Runs the front and mid ends on the given parsed program.
    ///
    /// @returns boost::none if an error occurs during compilation.
    static boost::optional<const IR::P4Program*> runCompiler(const IR::P4Program*);

 protected:
    /// @see @makeContext.
    virtual ICompileContext* makeContext_impl() const;

    /// This implementation just forwards the given arguments to the compiler.
    ///
    /// @see @initCompiler.
    virtual std::vector<const char*>* initCompiler_impl(int argc, char** argv) const;

    /// Parses the P4 program specified on the command line.
    ///
    /// @returns nullptr if an error occurs during parsing.
    static const IR::P4Program* runParser();

    /// Runs the front end of the P4 compiler on the given program.
    ///
    /// @returns nullptr if an error occurs during compilation.
    const IR::P4Program* runFrontend(const IR::P4Program* program) const;

    /// A factory method for providing a target-specific mid end implementation.
    virtual MidEnd mkMidEnd(const CompilerOptions& options) const;

    /// A factory method for providing a target-specific front end implementation.
    virtual P4::FrontEnd mkFrontEnd() const;

    /// Runs the mid end provided by @mkMidEnd on the given program.
    ///
    /// @returns nullptr if an error occurs during compilation.
    const IR::P4Program* runMidEnd(const IR::P4Program* program) const;

    explicit CompilerTarget(std::string deviceName, std::string archName);

 private:
    /// @returns the singleton instance for the current target.
    static const CompilerTarget& get();
};

}  // namespace P4Tools

#endif /* COMMON_COMPILER_COMPILER_TARGET_H_ */
