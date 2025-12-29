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
    string cmnd = cmd.substr(pos + 1);

    // Exit command
    if (cmd == "exit")
    {
      break;
    }

    // pwd command
    else if (cmd.substr(0, 3) == "pwd")
    {
      string path_pwd = fs::current_path();
      cout << path_pwd.substr(0, -1) << endl;
    }

    // cd command
    else if (cmd.substr(0, 2) == "cd")
    {
      if (fs::is_directory(cmd.substr(3)))
      {
        string p = cmd.substr(3);
        fs::current_path(p);
      }
      else if (!fs::is_directory(cmd.substr(3)))
      {
        cout << "cd: " << cmd.substr(3) << ": No such file or directory" << endl;
      }
      else if (cmd.substr(3, 1) == "~")
      {
        fs::current_path("/");
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
        {
          string arg = cmd.substr(3);
          // Trim leading/trailing spaces
          size_t start = arg.find_first_not_of(" \t");
          size_t end = arg.find_last_not_of(" \t");
          if (start != string::npos && end != string::npos)
            arg = arg.substr(start, end - start + 1);
          else
            arg = "";

          if (arg.empty())
          {
            // No argument, go to HOME
            const char *home = getenv("HOME");
            if (home && fs::is_directory(home))
            {
              fs::current_path(home);
            }
            else
            {
              cout << "cd: HOME not set" << endl;
            }
          }
          else if (arg == "~")
          {
            const char *home = getenv("HOME");
            if (home && fs::is_directory(home))
            {
              fs::current_path(home);
            }
            else
            {
              cout << "cd: HOME not set" << endl;
            }
          }
          else if (arg.size() >= 2 && arg.substr(arg.size() - 2) == "..")
          {
            // If last part is "..", go up one directory
            string path_pwd = fs::current_path();
            int pos = path_pwd.find_last_of('/');
            string new_pwd = path_pwd.substr(0, pos);
            fs::current_path(new_pwd);
          }
          else if (fs::is_directory(arg))
          {
            fs::current_path(arg);
          }
          else
          {
            cout << "cd: " << arg << ": No such file or directory" << endl;
          }
        }
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
    else if (cmd.substr(0, 4) == "echo")
    {
      cout << cmd.substr(5) << endl;
    }

    // command is not found
    else
    {
      cout << cmd << ": command not found" << endl;
    }
  }
}
