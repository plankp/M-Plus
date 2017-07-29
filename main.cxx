#include "lexer.hxx"
#include "parser.hxx"
#include "tree_formatter.hxx"

#include "ext_unary_func.hxx"

#include <fstream>
#include <iostream>

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      std::cerr << "MPlus  BSD 3 Clause license\nRun like this: mplus [MPlus script]" << std::endl;
      return 1;
    }

  try
    {
      std::ifstream s(argv[1], std::ios::in);
      istream_wrapper wrap(s);
      parser_info info(wrap);

      std::map<std::string, std::shared_ptr<rt::mp_value>> env
      {
	{ "print", std::shared_ptr<rt::mp_value>(new rt::ext::unary_func([](std::unique_ptr<rt::mp_value> x) {
		std::cout << x->to_str() << std::endl;
		return x; })) }
      };

      auto tree = parse(info);
      tree_formatter formatter;
      formatter.visit(*tree);
      std::cout << "\nEval demo:" << std::endl
       		<< formatter.get_text() << std::endl;
      auto res = tree->eval(env);
      if (res)
	{
	  std::cout << res->to_str() << std::endl;
	}
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
