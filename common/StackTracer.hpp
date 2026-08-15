/**
 * @file       StackPrint.h
 * @brief      文件功能描述
 * @author     Aquito
 * @date       2025-09-08
 * @version    v1.0.0
 * @copyright  Copyright (c) 2025
 * @par 修改日志：
 * <table>
 * <tr><th>日期       <th>版本                     <th>作者    <th>描述
 * <tr><td>2025/09/08 Time: 16:23   <td>1.0    <td>Aquito <td>创建文件
 * </table>
 */


#ifndef __StackPrint_H__
#define __StackPrint_H__
#include <cstdio>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>

class StackTracer
{
public:
    static void print()
    {
        const int max_frames = 128;
        void* frames[max_frames];
        int frame_count = backtrace(frames, max_frames);

        std::cout << "=========================================" << std::endl
                  << "Call stack (" << frame_count << " frames)" << std::endl
                  << "=========================================" << std::endl;

        for (int i = 0; i < frame_count; i++)
        {
            printFrameInfo(i, frames[i]);
        }
    }

private:
    static void printFrameInfo(int frame_num, void* address)
    {
        Dl_info info;
        if (dladdr(address, &info))
        {
            // Demangle C++ 名称
            int status;
            char* demangled = nullptr;

            if (info.dli_sname)
            {
                demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            }

            std::cout << "  #" << frame_num << " 0x" << std::hex << (uintptr_t)address << " : ";

            if (demangled && status == 0)
            {
                std::cout << demangled;
            }
            else if (info.dli_sname)
            {
                std::cout << info.dli_sname;
            }
            else
            {
                std::cout << "??";
            }

            // 计算偏移量
            if (info.dli_sname && info.dli_saddr)
            {
                size_t offset = (char*)address - (char*)info.dli_saddr;
                std::cout << " + 0x" << std::hex << offset;
            }

            std::cout << std::endl;

            // 尝试获取行号信息
            if (info.dli_fname)
            {
                printSourceLineInfo(address);
            }

            if (demangled)
            {
                free(demangled);
            }
        }
        else
        {
            std::cout << "  #" << frame_num << " 0x" << std::hex << (uintptr_t)address << " : ??" << std::endl;
        }
    }

    static void printSourceLineInfo(void* address)
    {
        std::stringstream cmd;
        cmd << "addr2line -e /proc/" << getpid() << "/exe -f -C -p " << address << " 2>/dev/null";

        auto pipe_closer = [](FILE* f) { if (f) pclose(f); };
        std::unique_ptr<FILE, decltype(pipe_closer)> pipe(popen(cmd.str().c_str(), "r"), pipe_closer);

        if (pipe)
        {
            char buffer[512];
            std::string result;
            while (fgets(buffer, sizeof(buffer), pipe.get()))
            {
                result += buffer;
            }

            if (!result.empty() && result.find("??") == std::string::npos)
            {
                std::cout << "      " << result;
            }
        }
    }
};

#define PRINT_STACK_TRACE() StackTracer::print()


#endif