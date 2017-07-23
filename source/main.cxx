#include "lexer.hxx"
#include "parser.hxx"
#include "tree_formatter.hxx"

#include <sstream>
#include <iostream>

int
main (int argc, char **argv)
{
  try
    {
      // Parser example
      std::stringstream s;
      s << "f = x -> x + 2 * -3^2;\n"
	<< "let = (bind, val, body) => ((bind -> body)(val));\n"
	<< "f(10);\n"
	<< "do\n"
	<< "  10 + 2 * -3^2 : f(a, b, c) : &&a() : nil;\n"
	<< "  @DONE\n"
	<< "end;\n"
	<< "a <- {\n"
	<< " 10   if a,\n"
	<< " 11   if b,\n"
	<< " 12   if a == b or b == c or a == c,\n"
	<< " 13   else\n"
	<< "};\n"
	<< "(a) -> (a);\n"
	<< "(a, b) -> a + b;\n"
	<< "(a, b, c) -> a + b + c;\n"
	<< "(() -> do []; [1, 2, 3]; a:[b] end)()";
      istream_wrapper wrap(s);
      parser_info info(wrap);
      tree_formatter formatter;
      formatter.visit(*parse(info));
      std::cout << formatter.get_text() << std::endl;
    }
  catch (std::exception &err)
    {
      std::cerr << err.what() << std::endl;
      return 1;
    }
  catch (...)
    {
      std::cerr << "Wtf?" << std::endl;
      return 1;
    }
  return 0;
}
