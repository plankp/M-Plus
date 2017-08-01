#include "eval.h"
#include "mp_env.h"
#include "lexer.hxx"
#include "parser.hxx"

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
      if (!s.is_open())
	{
	  std::cerr << "File " << argv[1] << " cannot be read" << std::endl;
	  return 2;
	}

      istream_wrapper wrap(s);
      parser_info info(wrap);

      auto tree = parse(info);
      // debug print tree here!
      auto str = expr_to_str(tree);
      std::cout << str << std::endl;
      free(str);

      std::cout << "Attempt to execute code" << std::endl;
      auto env = new_mp_env();
      init_default_env(env);

      auto ret = eval(env, tree);
      str = expr_to_str(ret);
      std::cout << str << std::endl;
      free(str);
	  auto denv = reinterpret_cast<rt_data_t *>(env);
	  dealloc(&denv);
      dealloc(&tree);
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
