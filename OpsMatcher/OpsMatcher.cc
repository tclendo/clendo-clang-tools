// Declares clang::SyntaxOnlyAction
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
// Declares llvm::cl::extrahelp
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text.\n");

// matcher for variable floating point binary operations 
auto flopsMatcher =
  binaryOperator(hasEitherOperand(hasDescendant(declRefExpr(to(varDecl(hasType(realFloatingPointType()))))))
		 ).bind("float");
int floatCount = 0;

// matcher for variable references
auto memopsMatcher = declRefExpr(to(varDecl())).bind("reference");
int memopsCount = 0;

class MemopsPrinter : public MatchFinder::MatchCallback {
public :
  virtual void run(const MatchFinder::MatchResult &Result) {

    if (const auto *MR = Result.Nodes.getNodeAs<clang::DeclRefExpr>("reference")) {
      MR->dump();
      memopsCount++;
    }
  }
};

class FlopsPrinter : public MatchFinder::MatchCallback {
public :
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const auto *VD = Result.Nodes.getNodeAs<clang::BinaryOperator>("float")) {
      VD->dump();
      floatCount++;
    }
  }
};

int main(int argc, const char **argv) {

  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
		 OptionsParser.getSourcePathList());
    
  FlopsPrinter Printer;
  MatchFinder Finder;
  Finder.addMatcher(flopsMatcher, &Printer);
  
  MemopsPrinter mPrinter;
  MatchFinder mFinder;
  mFinder.addMatcher(memopsMatcher, &mPrinter);
  
  Tool.run(newFrontendActionFactory(&Finder).get());
  Tool.run(newFrontendActionFactory(&mFinder).get());
  
  llvm::outs() << "flops: " << floatCount << "\n"
               << "memops: " << memopsCount << "\n";

}

