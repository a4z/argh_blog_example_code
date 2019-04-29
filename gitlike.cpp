#include <iostream>
#include <functional>
#include <set>
#include <string>
#include <cassert>
#include "argh.h"


// takes the functions, return if the command could be executed successfull
using Action = std::function<int (int, const char**)>;

struct Command {
  std::string name ;
  std::string help;
  Action exec;
};

struct CommandCompare {
  bool operator()(const Command& first, const Command& second) {
    return first.name < second.name ;
  }
};

using Commands = std::set<Command, CommandCompare> ;

std::string general_help = R"~(
  gitlike argh example program.
  A program to demonstrate git like subcommand parsing with argh.

  Usage: program COMMAND [OPTIONS]

  Commands:
    help COMMAND
        print the help text to the given command
        use this to get more information about the following commands

    echo ARGS
        runs the test command example doing nothing

    commands 
        print available commands

  General options:
    -v/--version
        print the program version and exit

    -h/--help
        print this hel text and exit
)~";

int main(int argc, const char** argv) {

  Commands cmds ;
  cmds.insert (
    Command{
      "echo" ,
      R"~(
  usage:
     echo [--repeat=<REP>] <MSG>

     --repeat, optional, integer. default is 1
        How often MSG shall be echoed out
        Values from 1 to 10 are valid

     MSG, required, the message to show
        )~", 
        [](int argc, const char** argv) -> int {
          auto cmdArgs = argh::parser (argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
          assert(cmdArgs.pos_args().at (0) == "echo") ;
          auto repeat_iter = cmdArgs.params ().find ("repeat");
          int repeat = 1 ;
          if (repeat_iter != cmdArgs.params ().end ()) {
            repeat = std::stoi(repeat_iter->second);
          }
          if(repeat < 1 || repeat > 10) {
            std::cerr << "invalid repeat cout\n";
            return EXIT_FAILURE ;
          }
          if (cmdArgs.pos_args ().size () < 2) {
            std::cerr << "Message argument missing\n";
            return EXIT_FAILURE ;
          }

          for (int i = 0; i < repeat; ++i) {
            std::cout << cmdArgs.pos_args().at (1) ;
          }
          return EXIT_SUCCESS ;
        }
    });
    
    cmds.insert (Command {
      "help" ,
      R"~(
  usage:
     help <COMMAND>

     COMMAND, required, the command for which help shall be shown
        )~", 
        [&](int argc, const char** argv) -> int{
          
          auto cmdArgs = argh::parser (argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
          assert(cmdArgs.pos_args().at (0) == "help");
          if (cmdArgs.pos_args ().size () < 2) {
            std::cerr << "Command argument missing\n";
            return EXIT_FAILURE ;
          }
          const auto cmd = cmds.find(Command{cmdArgs.pos_args().at (1)}) ;
          if (cmd == cmds.end ()) {
            std::cerr << "No help for <not found> available\n";
            return EXIT_FAILURE ;
          }
          std::cout << cmd->help << std::endl ;
          return EXIT_SUCCESS ;
        }
    });

    cmds.insert (Command {
      "commands" ,
      R"~(
  usage:
     commands

     prints the available commands
        )~", 
        [&cmds](int /*argc*/, const char** /*argv*/) -> int{
          for (const auto cmd : cmds) {
            std::cout << cmd.name << "\n" ;
          }
          return EXIT_SUCCESS ;
        }
    }    
  );
  
  if(argc < 2 ) {
    std::cerr << "arguments missing\n";
    return EXIT_FAILURE ;
  }

  std::string firstArg = argv[1] ;

  if (firstArg == "--help" || firstArg == "-h") {
    std::cout << general_help ;
    return EXIT_SUCCESS;
  }

  if (firstArg == "--version" || firstArg == "-v") {
    std::cout << "Version 0.0.0\n";
    return EXIT_SUCCESS;
  }

  const auto cmd = cmds.find(Command{firstArg}) ;
  if(cmd == cmds.end()) {
    std::cerr << "No such command: "<< firstArg << "\n";
    return EXIT_FAILURE ;
  }

  return cmd->exec(argc-1, argv+1);

}
