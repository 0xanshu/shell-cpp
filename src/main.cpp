#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <filesystem>
#include <algorithm>
using namespace std;
namespace fs = std::filesystem;

bool is_executable(const std::string &path)
{
  struct stat st;
  if (stat(path.c_str(), &st) == 0)
  {
    return (st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH);
  }
  return false;
}

string doesItExist(string comd)
{
  const char *path_env = getenv("PATH");
  vector<string> diff_paths;
  if (path_env)
  {
    string path_str(path_env);
    stringstream ss(path_str);
    string dir;
    while (getline(ss, dir, ':'))
    {
      diff_paths.push_back(dir);
    }
  }
  for (int i = 0; i < diff_paths.size(); i++)
  {
    string select_path = diff_paths[i];
    if (fs::exists(select_path))
    {
      for (const auto &entry : fs::directory_iterator(select_path))
      {
        if (entry.path().filename() == fs::path(comd))
        {
          if (is_executable(entry.path()))
          {
            return select_path;
          }
        }
      }
    }
  }
  return " ";
}

vector<string> tokenize(const string &input)
{
  vector<string> tokens;
  istringstream iss(input);
  string token;
  char c;
  while (iss >> ws)
  {
    if (iss.peek() == '"' || iss.peek() == '\'')
    {
      char quote = iss.get();
      getline(iss, token, quote);
      tokens.push_back(token);
    }
    else
    {
      iss >> token;
      tokens.push_back(token);
    }
  }
  return tokens;
}

int main()
{
  // Flush after every cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  while (true)
  {
    // TODO: Uncomment the code below to pass the first stage
    cout << "$ ";

    string line;
    getline(cin, line);
    vector<string> tokens = tokenize(line);
    if (tokens.empty())
      continue;

    string cmd = tokens[0];
    vector<string> args(tokens.begin() + 1, tokens.end());

    int pos = cmd.find(" ");
    string cmnd = cmd.substr(pos + 1);

    // Exit command
    if (cmd == "exit")
    {
      break;
    }

    // pwd command
    else if (cmd == "pwd")
    {
      string path_pwd = fs::current_path();
      cout << path_pwd.substr(0, -1) << endl;
    }

    // cd command
    else if (cmd == "cd")
    {
      if (args.empty() || args[0] == "~")
      {
        const char *home_dir_c = getenv("HOME");
        string home_dir(home_dir_c);
        fs::current_path(home_dir);
      }
      else if (fs::is_directory(args[0]))
      {
        string p = args[0];
        fs::current_path(p);
      }
      else if (cmd.substr(3, 2) == "./")
      {
        string path_pwd = fs::current_path();
        int pos = path_pwd.find_last_of('/');
        string new_pwd = path_pwd.substr(0, pos);
        string lat_pwd = new_pwd + "/" + cmd.substr(5);
        cout << lat_pwd << endl;
        fs::current_path(lat_pwd);
      }
      else if (cmd.substr(3, 3) == "../")
      {
        int lvl_to_go_down = 0;
        int pos = 0;
        while ((pos = (args[0]).find("../", pos)) != string::npos)
        {
          lvl_to_go_down++;
          pos += 3;
        }
        string sub = args[0];
        if (sub.size() >= 2 && sub.substr(sub.size() - 2) == "..")
        {
          lvl_to_go_down++;
          pos += 2;
        }
        string path_pwd = fs::current_path();
        string new_pwd = path_pwd;
        for (int i = 0; i < lvl_to_go_down; i++)
        {
          int pos = new_pwd.find_last_of('/');
          if (pos != string::npos)
            new_pwd = new_pwd.substr(0, pos);
        }
        fs::current_path(new_pwd);
      }
      else if (cmd.substr(3, 2) == "..")
      {
        string path_pwd = fs::current_path();
        int pos = path_pwd.find_last_of('/');
        string new_pwd = path_pwd.substr(0, pos);
        fs::current_path(new_pwd);
      }
      else
      {
        cout << "cd: " << args[0] << ": No such file or directory" << endl;
      }
    }

    // type command
    else if (cmd == "type")
    {
      if (args.empty())
      {
        cout << endl;
      }
      else if (args[0] == "echo" ||
               args[0] == "exit" ||
               args[0] == "pwd" ||
               args[0] == "type" ||
               args[0] == "cd" ||
               args[0] == "PATH")
      {
        cout << args[0] << " is a shell builtin" << endl;
      }
      else
      {
        string res = doesItExist(args[0]);
        if (res != " ")
        {
          cout << args[0] << " is " << res << "/" << args[0] << endl;
        }
        else
        {
          cout << args[0] << ": not found" << endl;
        }
      }
    }

    else if (cmd.size() > 0 && (cmd[0] == '"' || cmd[0] == '\''))
    {
      char quote = cmd[0];
      size_t end_quote = cmd.find(quote, 1);
      if (end_quote != string::npos)
      {
        string exe_name = cmd.substr(1, end_quote - 1);

        exe_name.erase(remove(exe_name.begin(), exe_name.end(), '\''), exe_name.end());
        exe_name.erase(remove(exe_name.begin(), exe_name.end(), '"'), exe_name.end());

        string rest = cmd.substr(end_quote + 1);
        size_t arg_start = rest.find_first_not_of(" \t");
        if (arg_start != string::npos)
          rest = rest.substr(arg_start);
        else
          rest = "";

        string full_cmd = "\"" + exe_name + "\"";
        if (!rest.empty())
          full_cmd += " " + rest;

        int ret = system(full_cmd.c_str());
        if (ret == -1)
        {
          string exe_path = doesItExist(exe_name);
          if (exe_path != " ")
          {
            full_cmd = "\"" + exe_path + "/" + exe_name + "\"";
            if (!rest.empty())
              full_cmd += " " + rest;
            ret = system(full_cmd.c_str());
            if (ret == -1)
              cout << exe_name << ": failed to execute" << endl;
          }
          else
          {
            cout << exe_name << ": not found" << endl;
          }
        }
      }
      else
      {
        cout << "Unmatched quote in command!" << endl;
        continue;
      }
    }

    // if a executable exists or not
    else if (doesItExist(cmd.substr(0, pos)) != " ")
    {
      int ret = system(cmd.c_str());
      if (ret == -1)
      {
        cout << cmd << ": failed to execute" << endl;
      }
    }

    // echo command
    else if (cmd == "echo")
    {
      cout << args[0] << endl;
    }

    // command is not found
    else
    {
      string exe_path = doesItExist(cmd);
      if (exe_path != " ")
      {
        // Build command string for system()
        string full_cmd = "\"" + exe_path + "/" + cmd + "\"";
        for (const auto &arg : args)
        {
          full_cmd += " \"" + arg + "\"";
        }
        int ret = system(full_cmd.c_str());
        if (ret == -1)
        {
          cout << cmd << ": failed to execute" << endl;
        }
      }
      else
      {
        cout << cmd << ": command not found" << endl;
      }
    }
  }
}
