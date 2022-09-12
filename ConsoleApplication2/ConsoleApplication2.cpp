// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include "Command.h"
#include "FileExplorer.h"
#include <windows.h>
#include <stdio.h>
#include <map>
#ifndef UNICODE
#define UNICODE
#endif 
using namespace std;
using namespace std::filesystem;
DWORD DEFAULT_PRIORITY = NORMAL_PRIORITY_CLASS;
CommandExtractor extractor = CommandExtractor();
FileExplorer fileExplorer = FileExplorer();
STARTUPINFO gitStartupInfo = { sizeof(gitStartupInfo) };
PROCESS_INFORMATION gitProcessInfo;
Command getCommand() {
    cout << fileExplorer.path.string() << ">";
    string command;
    getline(cin, command);
    Command cmd = extractor.extract(command);
    return cmd;
}

void fileSystemCommands(Command command) {
    if (!command.compare("dir")) {
        cout << "\t" << "Type";
        cout << "\t" << "Name" << endl;
        cout << "\t" << "----" << "\t" << "---------" << endl;

        for (directory_iterator next(fileExplorer.path), end; next != end; ++next) {
            if (is_directory(next->path())) {
                cout << "\t" << "d";
            }
            else {
                cout << "\t" << "f";
            }
            cout << "\t" + (next->path()).filename().string() << endl;
        }

    }
    if (!command.compare("cd")) {
        string s_path = command.arguments.back();
        if (s_path != ".") {
            path cdpath;
            if (s_path == "..") {
                cdpath = fileExplorer.path.parent_path();
            }
            else {
                cdpath = path(s_path);
            }

            if (is_directory(cdpath)) {
                fileExplorer.goTo(absolute(cdpath));
            }
            else {
                // give it a second chance
                cdpath = path(fileExplorer.path.string() + "\\" + cdpath.string());
                if (is_directory(cdpath)) {
                    fileExplorer.goTo(absolute(cdpath));
                }
            }
        }


        // go to the directory
    }
}
vector<pair<PROCESS_INFORMATION, path>> processes;
void gitCommands(Command command) {
    if (!command.compare("fetch")) {
        for (auto &e : processes) {
            CloseHandle((e.first).hProcess);
            CloseHandle((e.first).hThread);
        }
        processes.clear();

        // look for 
        for (directory_iterator next(fileExplorer.path), end; next != end; ++next) {
            if (is_directory(next->path())) {

                TCHAR check[260] = { L"git rev-parse --is-inside-work-tree" };
                TCHAR fetch[260] = { L"git fetch origin" };
                if (CreateProcess(L"C:\\Program Files\\Git\\cmd\\git.exe", check, NULL, NULL, FALSE, NULL,
                    NULL, absolute(next->path()).wstring().c_str(), &gitStartupInfo, &gitProcessInfo))
                {
                    SetPriorityClass(gitProcessInfo.hProcess, DEFAULT_PRIORITY);
                    cout << "Git process started. ID: " << gitProcessInfo.dwProcessId << endl;
                    WaitForSingleObject(gitProcessInfo.hProcess, INFINITE);
                    DWORD dwExitCode = 0;
                    GetExitCodeProcess(gitProcessInfo.hProcess, &dwExitCode);

                    CloseHandle(gitProcessInfo.hProcess);
                    CloseHandle(gitProcessInfo.hThread);
                    if (dwExitCode == 0) {
                        cout << "Oh, I found a git repo: " << next->path().string() << endl;
                        if (CreateProcess(L"C:\\Program Files\\Git\\cmd\\git.exe", fetch, NULL,
                            NULL, TRUE, CREATE_NEW_CONSOLE, NULL, absolute(next->path()).wstring().c_str(),
                            &gitStartupInfo, &gitProcessInfo)) {
                            SuspendThread(gitProcessInfo.hThread);
                            SetPriorityClass(gitProcessInfo.hProcess, DEFAULT_PRIORITY);
                            cout << "Fetching with git. ID: " << gitProcessInfo.dwProcessId << endl;
                            processes.push_back(make_pair(gitProcessInfo,next->path()));
                        }
                    }
                    else {
                        cout << "It's not a git repo." << endl;
                    }
                }

            }

        }
    }
}
void processCommands(Command command) {
    if (!command.compare("terminate")) {
        for (auto& e : processes) {
            cout << "Process " << e.first.dwProcessId << " has been terminated!" << endl;
            TerminateProcess(e.first.hProcess, 1);
            CloseHandle(e.first.hProcess);
            CloseHandle(e.first.hThread);
        }
        processes.clear();
    }
    if (!command.compare("suspend")) {
        for (auto& e : processes) {
            cout << "Process " << e.first.dwProcessId << " has been suspended! Thread id: " << e.first.dwThreadId << endl;
            SuspendThread(e.first.hThread);
        }
    }
    if (!command.compare("resume")) {
        for (auto& e : processes)    {
            cout << "Process " << e.first.dwProcessId << " has been resumed! Thread id: " << e.first.dwThreadId << endl;
            ResumeThread(e.first.hThread);
        }
    }
    if (!command.compare("result")) {
        if (processes.size() == 0) cout << "There is no result!" << endl;
        for (auto& e : processes) {
            DWORD dwExitCode = 0;
            GetExitCodeProcess(e.first.hProcess, &dwExitCode);
            if (dwExitCode == 0) {
                cout << "Success ---- " << e.second << endl;
            }
            else {
                cout << "Failure " << dwExitCode << "----" << e.second << endl;
            }
            CloseHandle(e.first.hProcess);
            CloseHandle(e.first.hThread);
        }
        processes.clear();
    }
    if (!command.compare("priority")) {
        try {
            
            map<int, DWORD> priorities{
                make_pair(1, IDLE_PRIORITY_CLASS),
                make_pair(2, BELOW_NORMAL_PRIORITY_CLASS),
                make_pair(3, NORMAL_PRIORITY_CLASS), 
                make_pair(4, ABOVE_NORMAL_PRIORITY_CLASS), 
                make_pair(5, HIGH_PRIORITY_CLASS), 
                make_pair(6, REALTIME_PRIORITY_CLASS)};
            int p = stoi(command.arguments.front());
            if (p < 1) {
                p = 1;
            }
            else if (p > 6) {
                p = 6;
            }
            for (auto& e : processes) {
                SetPriorityClass(e.first.hProcess, priorities[p]);
            }
            cout << "Priority has been changed to " << p << endl;
            if (command.arguments.back() == "default") {
                DEFAULT_PRIORITY = priorities[p];
            }
        }
        catch (std::invalid_argument const& e) {
            cout << "Invalid Argument" << endl;
            cout << "Priority should be integer! ----- priority 1-/2/3/4/5/6+" << endl;
        }
    }
}


int main() {
    Command command = getCommand();
    while (command.compare("exit")) {
        fileSystemCommands(command);
        gitCommands(command);
        processCommands(command);
        command = getCommand();
        
    }
    for (auto& e : processes) {
        CloseHandle(e.first.hProcess);
        CloseHandle(e.first.hThread);
    }
}

