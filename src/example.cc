#include <clang-c/Index.h>
#include <sys/stat.h>
#include <cstdio>

static bool exists(const char *FileName) {
  struct stat Stat;
  if (stat(FileName, &Stat)) {
    return false;
  }
  return true;
}

static void visitCursor(CXCursor Cursor, unsigned Level = 0) {
  clang_visitChildren(
      Cursor,
      [](CXCursor Cursor, CXCursor parent,
         CXClientData ClientData) -> CXChildVisitResult {
        const unsigned *const Level = static_cast<unsigned *>(ClientData);
        auto Kind = clang_getCursorKind(Cursor);
        auto Range = clang_getCursorExtent(Cursor);
        auto RangeEnd = clang_getRangeEnd(Range);
        unsigned RSL, RSC, REL, REC;
        clang_getSpellingLocation(clang_getRangeStart(Range), nullptr, &RSL,
                                  &RSC, nullptr);
        clang_getSpellingLocation(clang_getRangeEnd(Range), nullptr, &REL, &REC,
                                  nullptr);
        CXString KindSpelling = clang_getCursorKindSpelling(Kind);
        CXString Spelling = clang_getCursorSpelling(Cursor);
        for (unsigned Pos = *Level; Pos > 0; --Pos) {
          printf("  ");
        }
        printf("%s %p <%d:%d, %d:%d> '%s'\n", clang_getCString(KindSpelling),
               &Cursor, RSL, RSC, REL, REC, clang_getCString(Spelling));
        visitCursor(Cursor, *Level + 1);
        clang_disposeString(KindSpelling);
        clang_disposeString(Spelling);
        return CXChildVisit_Continue;
      },
      &Level);
}

int main(int argc, char *argv[]) {
  clang_enableStackTraces();
  if (argc < 2 || !exists(argv[1])) {
    fprintf(stderr, "Invalid Arguments\n");
    return -1;
  }
  auto Index = clang_createIndex(0, 0);
  CXTranslationUnit TU;
  if (CXError_Success != clang_parseTranslationUnit2(
                             Index, nullptr, argv + 1, argc - 1, nullptr, 0,
                             clang_defaultEditingTranslationUnitOptions(),
                             &TU)) {
    fprintf(stderr, "Unable to parse File");
    clang_disposeIndex(Index);
    return -1;
  }
  visitCursor(clang_getTranslationUnitCursor(TU));
  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
}
