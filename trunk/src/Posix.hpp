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

#ifndef EASYTC_POSIX_HPP_INCLUDED
#define EASYTC_POSIX_HPP_INCLUDED

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <stdexcept>
#include <vector>
#include <string>

struct unix_error : public std::runtime_error
{
    inline unix_error(int errorCode)
    : std::runtime_error(strerror(errorCode))
    {
    }

    static inline void check(int result)
    {
        if(result != -1)
        {
            return;
        }
        
        const int errorCode = errno;
        
        if(errorCode == 0)
        {
            return;
        }

        throw unix_error(errorCode);
    }
};

/**
 * Checks whether current user is root.
 */
bool amIRoot();

struct PipeResult
{
    int readFd;
    int writeFd;
    
    inline PipeResult(int* pipeEnds)
    {
        readFd = pipeEnds[0];
        writeFd = pipeEnds[1];
    }
};

PipeResult createPipe();

/**
 * Replace target file descriptor with the source one.
 */
void replaceFileDescriptor(int source, int target);

/**
 * Replace standard output with the given file descriptor.
 */
void replaceStdout(int source);

/**
 * Replace standard output with the given file descriptor.
 */
void replaceStderr(int source);

/**
 * Execute the executable with args capturing the output.
 *
 * @return the output of the execution
 */
std::string executeCommand(char const* executable, std::vector<std::string> args, int& exitCode);

inline std::string executeCommand(char const* executable, int& exitCode)
{
    return executeCommand(executable,  std::vector<std::string>(), exitCode);
}

inline std::string executeCommand(char const* executable, std::string arg0, int& exitCode)
{
    std::vector<std::string> args;
    
    args.push_back(arg0);

    return executeCommand(executable, args, exitCode);
}

inline std::string executeCommand(char const* executable, std::string arg0, std::string arg1, int& exitCode)
{
    std::vector<std::string> args;
    
    args.push_back(arg0);
    args.push_back(arg1);

    return executeCommand(executable, args, exitCode);
}

/**
 * Calls fork(2) and executes given functors in the appropriate process.
 */
template <typename ParentFunctionT, typename ChildFunctionT>
void forkProcess(ParentFunctionT& parentFun, ChildFunctionT& childFun)
{
    const pid_t forkResult = fork();
    
    unix_error::check(forkResult);
    
    if(forkResult == 0)
    {
        childFun();
    }
    else
    {
        return parentFun(forkResult);
    }
}

#endif
