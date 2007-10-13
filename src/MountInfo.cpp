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

#include "MountInfo.hpp"
#include "Posix.hpp"

#include <sstream>
#include <iostream>
#include <stdexcept>

namespace
{

typedef std::vector<std::string> StringVec;

StringVec splitToLines(std::string output)
{
    std::istringstream iss(output);
    char buf[1024];
    StringVec lines;
    
    iss.getline(buf, 1023);
    while(!iss.fail())
    {
        std::string line(buf);
        
        lines.push_back(line);
        iss.getline(buf, 1023);
    }

    return lines;
}

StringVec splitToWords(std::string line)
{
    std::istringstream iss(line);
    StringVec words;
    
    std::string word;
    iss >> word;
    while(!iss.fail())
    {
        words.push_back(word);
        iss >> word;
    }
    
    return words;
}
    
} // namespace <unnamed>

MountInfoVec getMountInfo()
{
    int exitCode;
    
    StringVec tcOutput = splitToLines(executeCommand("truecrypt", "-l", exitCode));

    if(exitCode != 0)
    {
        if(tcOutput.size() > 0)
        {
            throw std::runtime_error(tcOutput[0]);
        }
        else
        {
            throw std::runtime_error("unknown error while querying mounted images");
        }
    }
    
    StringVec mntOutput = splitToLines(executeCommand("mount", exitCode));
    MountInfoVec info;
    
    for(StringVec::const_iterator tcIt = tcOutput.begin(); tcIt != tcOutput.end(); ++tcIt)
    {
        StringVec tcWords = splitToWords(*tcIt);
        
        std::string device = tcWords[0];
        std::string image = tcWords[1];
        
        for(StringVec::const_iterator mntIt = mntOutput.begin(); mntIt != mntOutput.end(); ++mntIt) 
        {
            StringVec mntWords = splitToWords(*mntIt);
            
            std::string mntDev = mntWords[0];
            std::string mntPoint = mntWords[2];
            
            if(mntDev == device)
            {
                info.push_back(MountInfo(image, mntPoint));
                break;
            }
        }
    }

    return info;
}
