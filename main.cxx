#include "eval.h"
#include "mp_env.h"
#include "lexer.hxx"
#include "parser.hxx"

#include <fstream>

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf(stderr, "MPlus  BSD 3 Clause license\nRun like this: mplus [MPlus script]\n");
      return 1;
    }

  try
    {
      std::ifstream s(argv[1], std::ios::in);
      if (!s.is_open())
	{
	  fprintf(stderr, "File %s cannot be read\n", argv[1]);
	  return 2;
	}

      istream_wrapper wrap(s);
      parser_info info(wrap);

      auto tree = parse(info);
      // // Uncomment to print tree parse tree!
      // {
      //   auto str = expr_to_str(tree);
      //   printf("%s\n", str);
      //   free(str);
      // }

      auto env = new_mp_env(nullptr);
      init_default_env(env);

      auto ret = eval(env, tree);
      {
        auto str = expr_to_str(ret);
        printf("%s\n", str);
        free(str);
      }
      dealloc(&ret);
      auto denv = reinterpret_cast<rt_data_t *>(env);
      dealloc(&denv);
      dealloc(&tree);
    }
  catch (std::exception &err)
    {
      fprintf(stderr, "error: %s\n", err.what());
      return 1;
    }
  return 0;
}
