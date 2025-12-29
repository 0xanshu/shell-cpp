#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <filesystem>
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

int main()
{
  // Flush after every cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  while (true)
  {
    // TODO: Uncomment the code below to pass the first stage
    cout << "$ ";

    string cmd;
    getline(cin, cmd);
    int pos = cmd.find(" ");

    if (cmd == "exit")
    {
      break;
    }
    else if (cmd.substr(0, 3) == "pwd")
    {
      string path_pwd = fs::current_path();
      cout << path_pwd.substr(0, -1) << endl;
    }
    else if (cmd.substr(0, 4) == "type")
    {
      string comd = cmd.substr(5);
      if (comd == "echo" ||
          comd == "exit" ||
          comd == "type" ||
          comd == "pwd" ||
          comd == "PATH")
      {
        cout << cmd.substr(5) << " is a shell builtin" << endl;
      }
      else
      {
        string res = doesItExist(comd);
        if (res != " ")
        {
          cout << comd << " is " << res << "/" << comd << endl;
        }
        else
        {
          cout << comd << ": not found" << endl;
        }
      }
    }
    else if (doesItExist(cmd.substr(0, pos)) != " ")
    {
      int ret = system(cmd.c_str());
      if (ret == -1)
      {
        cout << cmd << ": failed to execute" << endl;
      }
    }
    else if (cmd.substr(0, 4) == "echo")
    {
      cout << cmd.substr(5) << endl;
    }
    else
    {
      cout << cmd << ": command not found" << endl;
    }
  }
}
