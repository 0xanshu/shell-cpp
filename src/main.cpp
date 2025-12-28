#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

// const char* path_env ="/usr/bin:/usr/local/bin";

bool is_executable(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return (st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH);
    }
    return false;
}

bool doesItExist(string diff_path, string comd){
    for (const auto& entry : fs::directory_iterator(diff_path)) {
        if(entry.path().filename() == fs::path(comd) ){
          if(is_executable(entry.path())){
            return true;
          }
        }
    }
  return false;
}

int main() {
  // Flush after every cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  const char* path_env = getenv("PATH");
  vector<string> diff_paths;
  if(path_env){
    string path_str(path_env);
    stringstream ss(path_str);
    string dir;
    while(getline(ss, dir, ':')) {
      diff_paths.push_back(dir);
    }
  }

  while(true){
  // TODO: Uncomment the code below to pass the first stage
  cout << "$ ";
  
  // Inputing users commands
  string cmd;
  getline(cin, cmd);

  if(cmd == "exit"){
    break;
  } else if (cmd.substr(0,4) == "type") {
    string comd = cmd.substr(5);
    if( comd == "echo" || 
        comd == "exit" ||
        comd == "type" || 
        comd == "PATH" )
    {
      cout << cmd.substr(5) << " is a shell builtin" << endl;
    } else if (true) {
      for(int i = 0; i < diff_paths.size(); i++) {
        string select_path = diff_paths[i];
        if(doesItExist(select_path, comd)) {
          cout << comd << " is " << select_path << endl;
        }
      }
    }
    else {
      cout << cmd.substr(5) << ": not found" << endl;
    }
  } else if (cmd.substr(0,4) == "echo") {
    cout << cmd.substr(5) << endl;
  } else {
    cout << cmd << ": command not found" << endl; 
  }
  }
}
