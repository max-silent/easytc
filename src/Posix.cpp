/**
 * Copyright (c) 2007, Emir Uner
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "Posix.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <sstream>

namespace
{
    void execProcess(char const* executable, std::vector<std::string> args)
    {
        char** arguments = new char*[args.size() + 2];
    
        arguments[0] = const_cast<char*>(executable);
        
        for(unsigned int i = 0; i < args.size(); ++i)
        {
            arguments[i + 1] = const_cast<char*>(args[i].c_str());
        }
        
        arguments[args.size() + 1] = 0;
    
        unix_error::check(execvp(executable, arguments));
    }

    struct ChildProcess
    {
        PipeResult pipeResult;
        char const* executable;
        std::vector<std::string> args;
        
        inline ChildProcess(PipeResult pipeResultp, char const* execp, std::vector<std::string> argsp)
        :pipeResult(pipeResultp), executable(execp), args(argsp)
        {
        }
        
        inline void operator()()
        {
            close(pipeResult.readFd);
            
            replaceStdout(pipeResult.writeFd);
            replaceStderr(pipeResult.writeFd);
            execProcess(executable, args);
        }
    };
    
    struct ParentProcess
    {
        PipeResult pipeResult;
        std::string output;
        int exitCode;
        
        inline ParentProcess(PipeResult pipeResultp)
        :pipeResult(pipeResultp)
        {
        }
        
        inline void operator()(int childPid)
        {
            close(pipeResult.writeFd);
            std::ostringstream oss;
            
            char ch;
            int count;
            int readFd = pipeResult.readFd;
            
            count = read(readFd, &ch, 1);
            while(count != 0)
            {        
                oss << ch;
                
                count = read(readFd, &ch, 1);            
                unix_error::check(count);
            }
            
            output = oss.str();

            unix_error::check(waitpid(childPid, &exitCode, 0));
        }
        
        std::string getOutput() 
        {
            return output;
        }

        int getExitCode()
        {
            return exitCode;
        }
    };
}

bool amIRoot()
{
    return getuid() == 0;
}

PipeResult createPipe()
{
    int pipeEnds[2];
    
    if(pipe(pipeEnds) == -1)
    {
        unix_error::check(pipe(pipeEnds));
    }

    return PipeResult(pipeEnds);
}

void replaceFileDescriptor(int source, int target)
{
    unix_error::check(dup2(source, target));
}

void replaceStdout(int source)
{
    replaceFileDescriptor(source, STDOUT_FILENO);
}

void replaceStderr(int source)
{
    replaceFileDescriptor(source, STDERR_FILENO);
}

std::string executeCommand(char const* executable, std::vector<std::string> args, int& exitCode)
{
    PipeResult pipeResult = createPipe();
    ChildProcess child(pipeResult, executable, args);
    ParentProcess parent(pipeResult);
    
    forkProcess(parent, child);
    
    exitCode = parent.getExitCode();
    
    return parent.getOutput();
}
