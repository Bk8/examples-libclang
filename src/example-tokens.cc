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

static size_t fileSize(const char *FileName) {
  struct stat Stat;
  if (stat(FileName, &Stat)) {
    return 0;
  }
  return Stat.st_size;
}

static const char *getTokenKindSpelling(CXTokenKind Kind) {
  switch (Kind) {
  case CXToken_Punctuation:
    return "Punctuation";
  case CXToken_Keyword:
    return "Keyword";
  case CXToken_Identifier:
    return "Identifier";
  case CXToken_Literal:
    return "Literal";
  case CXToken_Comment:
    return "Comment";
    break;
  }
  return "";
}

static void visitTokens(CXTranslationUnit TU, CXToken *Tokens,
                        CXCursor *Cursors, unsigned NumOfTokens) {
  for (unsigned Pos = 0; Pos < NumOfTokens; ++Pos) {
    auto Token = Tokens[Pos];
    auto Cursor = Cursors[Pos];
    auto Kind = clang_getTokenKind(Token);
    auto Range = clang_getTokenExtent(TU, Token);
    unsigned RSL, RSC, REL, REC;
    clang_getSpellingLocation(clang_getRangeStart(Range), nullptr, &RSL, &RSC,
                              nullptr);
    clang_getSpellingLocation(clang_getRangeEnd(Range), nullptr, &REL, &REC,
                              nullptr);
    CXCursorKind CursorKind = clang_getCursorKind(Cursor);
    CXString Spelling = clang_getTokenSpelling(TU, Token);
    CXString KindSpelling = clang_getCursorKindSpelling(CursorKind);
    printf("%s(%s) %p <%d:%d, %d:%d> '%s'\n", getTokenKindSpelling(Kind),
           clang_getCString(KindSpelling), &Token, RSL, RSC, REL, REC,
           clang_getCString(Spelling));
    clang_disposeString(KindSpelling);
    clang_disposeString(Spelling);
  }
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
  CXToken *Tokens;
  unsigned NumOfTokens;
  CXCursor *Cursors;
  CXFile File = clang_getFile(TU, argv[1]);
  CXSourceLocation FileBegin = clang_getLocationForOffset(TU, File, 0);
  CXSourceLocation FileEnd =
      clang_getLocationForOffset(TU, File, fileSize(argv[1]));
  CXSourceRange FileRange = clang_getRange(FileBegin, FileEnd);
  clang_tokenize(TU, FileRange, &Tokens, &NumOfTokens);
  Cursors = new CXCursor[NumOfTokens];
  clang_annotateTokens(TU, Tokens, NumOfTokens, Cursors);
  visitTokens(TU, Tokens, Cursors, NumOfTokens);
  delete Cursors;
  clang_disposeTokens(TU, Tokens, NumOfTokens);
  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
}
