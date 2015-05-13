// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-

// install the librados-dev package to get this
#include <rados/librados.hpp>
#include <iostream>
#include <string>

int main(int argc, const char **argv)
{
  int ret = 0;

  const char *pool_name = "seri";
  std::string obj_name("foo");
  librados::IoCtx io_ctx;

  librados::Rados rados;
  {
    ret = rados.init("admin"); // just use the client.admin keyring
    if (ret < 0) { // let's handle any error that might have come back
      std::cerr << "couldn't initialize rados! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just set up a rados cluster object" << std::endl;
    }
  }

  {
    ret = rados.conf_parse_argv(argc, argv);
    if (ret < 0) {
      // This really can't happen, but we need to check to be a good citizen.
      std::cerr << "failed to parse config options! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just parsed our config options" << std::endl;
      // We also want to apply the config file if the user specified
      // one, and conf_parse_argv won't do that for us.
      for (int i = 0; i < argc; ++i) {
	if ((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--conf") == 0)) {
	  ret = rados.conf_read_file(argv[i+1]);
	  if (ret < 0) {
	    // This could fail if the config file is malformed, but it'd be hard.
	    std::cerr << "failed to parse config file " << argv[i+1]
	              << "! error" << ret << std::endl;
	    ret = EXIT_FAILURE;
	    goto out;
	  }
	  break;
	}
      }
    }
  }

  {
    ret = rados.connect();
    if (ret < 0) {
      std::cerr << "couldn't connect to cluster! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just connected to the rados cluster" << std::endl;
    }
  }


  {
    ret = rados.ioctx_create(pool_name, io_ctx);
    if (ret < 0) {
      std::cerr << "couldn't set up ioctx! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just created an ioctx for our pool" << std::endl;
    }
  }


  {
    librados::bufferlist bl;
    bl.append("hello world!");

    ret = io_ctx.write_full(obj_name, bl);
    if (ret < 0) {
      std::cerr << "couldn't write object! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just wrote new object " << obj_name << std::endl;
    }
  }


  // do the redir magic here
  {
    ret = io_ctx.stub(obj_name);
    if (ret < 0) {
      std::cerr << "couldn't stub " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just stubbed " << obj_name << std::endl;
    }
  }


  ret = EXIT_SUCCESS;
out:
  rados.shutdown();

  return ret;
}
