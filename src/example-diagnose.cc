#include <clang-c/Index.h>
#include <cstdio>

int main(int argc, char *argv[]) {
  clang_enableStackTraces();
  auto Index = clang_createIndex(0, 0);
  CXTranslationUnit TU;
  if (CXError_Success != clang_parseTranslationUnit2(
                             Index, nullptr, argv + 1, argc - 1, nullptr, 0,
                             ::clang_defaultEditingTranslationUnitOptions(),
                             &TU)) {
    fprintf(stderr, "Unable to parse File");
    clang_disposeIndex(Index);
    return -1;
  }
  for (unsigned I = 0, N = clang_getNumDiagnostics(TU); I != N; ++I) {
    CXDiagnostic Diag = clang_getDiagnostic(TU, I);
    CXString String =
        clang_formatDiagnostic(Diag, clang_defaultDiagnosticDisplayOptions());
    fprintf(stderr, "%s\n", clang_getCString(String));
    clang_disposeString(String);
  }
  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
}
