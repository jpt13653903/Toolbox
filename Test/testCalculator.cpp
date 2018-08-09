#include "test.h"
#include "FileWrapper.h"
#include "Calculator.h"
//------------------------------------------------------------------------------

static Calculator Calc;
//------------------------------------------------------------------------------

void Calculate(const char* Expression){
  info("  %s = %lg", Expression, (double)Calc.Calculate(Expression));
}
//------------------------------------------------------------------------------

bool TestConstants(){
  Start("Constants");

  Calculate("e"        );
  Calculate("pi"       );
  Calculate("\\c"      );
  Calculate("\\G"      );
  Calculate("\\g"      );
  Calculate("\\me"     );
  Calculate("\\mp"     );
  Calculate("\\mn"     );
  Calculate("\\u"      );
  Calculate("\\e"      );
  Calculate("\\h"      );
  Calculate("\\k"      );
  Calculate("\\mu"     );
  Calculate("\\epsilon");
  Calculate("\\Phi"    );
  Calculate("\\NA"     );
  Calculate("\\R"      );
  Calculate("\\rhoAl"  );
  Calculate("\\rhoCu"  );
  Calculate("\\F"      );
  Calculate("\\AU"     );
  Calculate("\\pc"     );
  Calculate("\\mil"    );

  Done(); return true;
}
//------------------------------------------------------------------------------

bool TestExpressions(){
  Start("Expressions");

  Calculate(" 3 + 4  * 5");
  Calculate("(3 + 4) * 5");
  Calculate("sin(log(e^pi))");

  Done(); return true;
}
//------------------------------------------------------------------------------

void TreeView(const char* Expression){
  std::string Tree;
  Calc.BuildTree(Expression);
  Calc.ShowTree(&Tree);
  info("%s\n%s", Expression, Tree.c_str());
}
//------------------------------------------------------------------------------

bool TestTreeView(){
  Start("Viewing the tree");

  TreeView(" 3 + x  * 5");
  TreeView("(3 + x) * 5");
  TreeView("sin(log(e^(pi + x)))");
  TreeView("d(sin(log(e^(pi + x))))dx");

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  printf("\n\n");
  if(!TestConstants  ()) goto main_Error;
  if(!TestExpressions()) goto main_Error;
  if(!TestTreeView   ()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors"); Done();
    return -1;
}
//------------------------------------------------------------------------------

