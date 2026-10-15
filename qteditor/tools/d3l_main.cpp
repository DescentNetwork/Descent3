// Standalone .d3l chunk inspector for the Qt editor build.
//
// Reuses descar's posix streams and the shared D3LV chunk framing to list,
// test for, or extract top-level chunks without loading a level or touching any
// editor/engine state.
//
//   d3l list    <file.d3l> [file.d3l ...]
//   d3l has     <file.d3l> <TAG> [TAG ...]
//   d3l extract <file.d3l> <TAG> [outfile]

#include "d3l_chunks.h"

#include <posix_stream.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

void printUsage(const char* argv0)
{
  std::cerr << "usage:\n"
            << "  " << argv0 << " list <file.d3l> [file.d3l ...]\n"
            << "  " << argv0 << " has <file.d3l> <TAG> [TAG ...]\n"
            << "  " << argv0 << " extract <file.d3l> <TAG> [outfile]\n";
}

int cmdList(const std::vector<std::string>& files)
{
  int rc = 0;
  for (const std::string& file : files)
  {
    std::vector<d3l_chunk_t> chunks;
    uint32_t version = 0;
    std::string error;
    if (!D3LListChunks(file, chunks, &version, &error))
    {
      std::cerr << file << ": " << error << "\n";
      rc = 1;
      continue;
    }

    std::cout << file << ": version " << version << ", " << chunks.size()
              << " chunk(s)\n";
    for (size_t i = 0; i < chunks.size(); i++)
      std::cout << "  [" << i << "] " << chunks[i].tag << "  off=0x" << std::hex
                << chunks[i].offset << std::dec
                << "  body=" << chunks[i].body_size << "\n";
  }
  return rc;
}

int cmdHas(const std::vector<std::string>& args)
{
  const std::string& file = args[0];

  std::vector<d3l_chunk_t> chunks;
  std::string error;
  if (!D3LListChunks(file, chunks, nullptr, &error))
  {
    std::cerr << file << ": " << error << "\n";
    return 1;
  }

  int rc = 0;
  for (size_t i = 1; i < args.size(); i++)
  {
    bool found = false;
    for (const d3l_chunk_t& chunk : chunks)
    {
      if (chunk.tag == args[i])
      {
        found = true;
        break;
      }
    }
    std::cout << (found ? "yes " : "no  ") << args[i] << "\n";
    if (!found)
      rc = 1;
  }
  return rc;
}

int cmdExtract(const std::vector<std::string>& args)
{
  const std::string& file = args[0];
  const std::string& tag = args[1];

  std::vector<d3l_chunk_t> chunks;
  std::string error;
  if (!D3LListChunks(file, chunks, nullptr, &error))
  {
    std::cerr << file << ": " << error << "\n";
    return 1;
  }

  const d3l_chunk_t* match = nullptr;
  for (const d3l_chunk_t& chunk : chunks)
  {
    if (chunk.tag == tag)
    {
      match = &chunk;
      break;
    }
  }
  if (!match)
  {
    std::cerr << file << ": no " << tag << " chunk\n";
    return 1;
  }

  posix_istream in(file, std::ios_base::in | std::ios_base::binary);
  if (!in.is_open() || !in.seek(static_cast<off_t>(match->offset + 8)))
  {
    std::cerr << file << ": cannot read " << tag << " chunk\n";
    return 1;
  }

  std::string body(match->body_size, '\0');
  if (match->body_size)
    in.read(body.data(), match->body_size);

  if (args.size() >= 3)
  {
    std::ofstream out(args[2], std::ios::binary | std::ios::trunc);
    if (!out)
    {
      std::cerr << args[2] << ": cannot open for writing\n";
      return 1;
    }
    out.write(body.data(), static_cast<std::streamsize>(body.size()));
  }
  else
  {
    std::cout.write(body.data(), static_cast<std::streamsize>(body.size()));
  }
  return 0;
}

} // namespace

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    printUsage(argv[0]);
    return 2;
  }

  const std::string command = argv[1];
  const std::vector<std::string> args(argv + 2, argv + argc);

  if (command == "list" || command == "ls")
    return cmdList(args);

  if (command == "has")
  {
    if (args.size() < 2)
    {
      printUsage(argv[0]);
      return 2;
    }
    return cmdHas(args);
  }

  if (command == "extract" || command == "cat")
  {
    if (args.size() < 2)
    {
      printUsage(argv[0]);
      return 2;
    }
    return cmdExtract(args);
  }

  printUsage(argv[0]);
  return 2;
}
