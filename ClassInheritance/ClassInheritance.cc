#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace clang;

// ASTVisitor class for our clang tool
class ClassInheritanceVisitor : public RecursiveASTVisitor<ClassInheritanceVisitor> {
public:
    explicit ClassInheritanceVisitor(ASTContext *Context)
        : Context(Context), SM(Context->getSourceManager()) {}

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
        if (Declaration != nullptr) {
            FullSourceLoc Location = Context->getFullLoc(Declaration->getBeginLoc());
            if (SM.isInMainFile(Location)) {
                outs() << "Found class declaration: "
                    << Declaration->getNameAsString() << " <"
                    << Location.getSpellingLineNumber() << ", "
                    << Location.getSpellingColumnNumber() << ">\n";

                Classes.push_back(Declaration);
            }
        }

        return true;
    }

    const std::vector<CXXRecordDecl*> getClassDeclarations() {
        return Classes;
    }

private:
    // The ASTContext contains all of the info of the AST for our source file
    ASTContext *Context;
    // The source manager allows us to perform various actions based on the
    // source file.
    SourceManager &SM;
    // This stores all of our class declarations
    std::vector<CXXRecordDecl*> Classes;
};

// ASTConsumer class for our clang tool
class ClassInheritanceConsumer : public ASTConsumer {
public:
    explicit ClassInheritanceConsumer(ASTContext *Context)
        : Visitor(Context) {}

    // This will call the ASTConsumer on the top-level node in the clang AST
    // We leave the actual traversing of the AST to the Visitor class
    virtual void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

    void printCxxDecls() {
        auto Classes = Visitor.getClassDeclarations();
        for ( CXXRecordDecl* cxx : Classes ) {
            StringRef Name = cxx->getName();
            unsigned Bases = cxx->getNumBases();
            bool DerivesFromClassInMainFile = false;

            for ( CXXRecordDecl* PotentialParent : Classes ) {
                if (cxx->isDerivedFrom(PotentialParent)) {
                    outs() << Name << " derives from class: "
                           << PotentialParent->getName() << "\n";

                    DerivesFromClassInMainFile = true;
                }
            }

            if (!DerivesFromClassInMainFile) {
                if (Bases > 0) {
                    outs() << Name << " has " << Bases << " base class(es and"
                           << " derives from class(es) outside main file\n";
                } else {
                    outs() << Name
                           << " is a base class\n";
                }
            }
        }
    }

private:
    ClassInheritanceVisitor Visitor;
};

// Frontend action class for our clang tool
class ClassInheritanceAction : public ASTFrontendAction {
public:
    virtual bool PrepareToExecuteAction(CompilerInstance &CI) override {
        outs() << "Prepare to Execute Action\n";
        outs() << "-----\n";
        return true;
    }

    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, StringRef Input) override {

        auto RetVal = std::make_unique<ClassInheritanceConsumer>(&CI.getASTContext());
        Consumer = RetVal.get();

        return RetVal;
    }

    virtual void EndSourceFileAction() override {
        outs() << "End Source File Action\n";
        outs() << "-----\n";
        Consumer->printCxxDecls();
    }

private:
    ClassInheritanceConsumer *Consumer;
};

static llvm::cl::OptionCategory ctCategory("");

int main(int argc, const char** argv) {

    auto ExpectedParser = tooling::CommonOptionsParser::create(argc, argv, ctCategory);
    if (!ExpectedParser) {
        errs() << ExpectedParser.takeError();
        return 1;
    }

    tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();
    tooling::ClangTool Tool(OptionsParser.getCompilations(),
                            OptionsParser.getSourcePathList());

    return Tool.run(tooling::newFrontendActionFactory<ClassInheritanceAction>().get());
}
