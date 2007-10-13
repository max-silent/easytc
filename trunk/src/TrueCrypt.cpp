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

#include "TrueCrypt.hpp"
#include "Posix.hpp"

#include <stdexcept>
#include <sstream>

void unmount(char const* image)
{
    int exitCode;
    
    std::string output = executeCommand("truecrypt", "-d", image, exitCode);

    if(exitCode != 0)
    {
        throw std::runtime_error(output);
    }
}

void unmountAll()
{
    int exitCode;

    std::string output = executeCommand("truecrypt", "-d", exitCode);

    if(exitCode != 0)
    {
        throw std::runtime_error(output);
    }
}

void mount(std::string image, std::string mountPoint, std::string password)
{
    std::vector<std::string> args;
    
    args.push_back("-p");
    args.push_back(password);
    args.push_back(image);
    args.push_back(mountPoint);

    int exitCode;
    
    std::string output = executeCommand("truecrypt", args, exitCode);

    if(exitCode != 0)
    {
        throw std::runtime_error(output);
    }
}

void createImage(std::string imageFile, std::string password, int size)
{
    std::vector<std::string> args;
    std::ostringstream oss;
    
    oss << size << "M";
    
    args.push_back("--type");
    args.push_back("normal");
    args.push_back("--filesystem");
    args.push_back("FAT");
    args.push_back("--size");
    args.push_back(oss.str());
    args.push_back("--hash");
    args.push_back("RIPEMD-160");
    args.push_back("--encryption");
    args.push_back("AES");
    args.push_back("-p");
    args.push_back(password);    
    args.push_back("-k");
    args.push_back("/dev/null");
    args.push_back("--random-source");
    args.push_back("/dev/urandom");
    args.push_back("--create");
    args.push_back(imageFile);
    
    int exitCode;
    
    std::string output = executeCommand("truecrypt", args, exitCode);

    if(exitCode != 0)
    {
        throw std::runtime_error(output);
    }
}
