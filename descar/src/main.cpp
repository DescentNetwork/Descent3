// System
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <string>
#include <stdexcept>
#include <functional>
#include <list>

// Project
#include <version.h>
#include "hog2_format.h"
#include "string_hash.h"
#include <posix_stream.h>

// see also: https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t#answers
template <typename TP>
std::time_t to_time_t(TP tp)
{
  using namespace std::chrono;
  auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                                                      + system_clock::now());
  return system_clock::to_time_t(sctp);
}

hog2::archive_t::const_iterator find_file_entry(const hog2::archive_t& archive, const std::string& name)
{
  auto fn = [&name](const auto& hfe) { return lowercase(name) == lowercase(hfe.name.string()); };
  if (auto it = std::find_if(std::begin(archive), std::end(archive), fn); it != std::end(archive))
    return it;
  return std::end(archive);
}

std::list<std::filesystem::path> validate_files(const std::filesystem::path& base_path,
                                                std::list<std::filesystem::path> filepaths)
{
  bool printed_base_dir = false;
  std::list<std::filesystem::path> valid_files;
  for (auto& filepath : filepaths)
  {
    if (!std::filesystem::exists(base_path / filepath))
    {
      if (!printed_base_dir)
      {
        printed_base_dir = true;
        std::cout << "File paths relative to " << std::filesystem::absolute(base_path) << std::endl;
      }
      std::cout << "File " << filepath << " does not exist! Skipping... " << std::endl;
    }
    else
    {
      if (!std::filesystem::is_regular_file(base_path / filepath))
        throw std::runtime_error(filepath.string() + " is not a regular file.");

      if (filepath.string().size() > sizeof(hog2::entry_t::name))
        throw std::runtime_error("Length of filepath \"" + filepath.string() + "\" exceeds 36 bytes.");

      valid_files.emplace_back(filepath);
    }
  }
  return valid_files;
}

namespace cli
{
  std::filesystem::path archive_path; // path of hog archive
  std::filesystem::path base_path;
  std::list<std::filesystem::path> file_path_list;

  void add_files_to_header(hog2::archive_t& archive, const std::list<std::filesystem::path>& path_list)
  {
    for (const auto& i : path_list)
    {
      hog2::entry_t entry;
      entry.name = i.string();
      entry.flags = 0;
      entry.len = static_cast<uint32_t>(std::filesystem::file_size(base_path / i));

      // NOTE: time_t is not of a defined size. recently it has been switched to a 64-bit value
      entry.timestamp = uint32_t(to_time_t(std::filesystem::last_write_time(base_path / i)));

      archive.addEntry(entry);
    }
  }

  void amend_hog(hog2::archive_t& old_header, hog2::archive_t& new_header)
  {
    posix_istream old_hog;
    posix_ostream new_hog;
    auto tmp_archive = archive_path;
    tmp_archive += ".tmp";
    bool modify_mode = std::distance(std::begin(old_header), std::end(old_header));

    if (modify_mode)
    {
      old_hog = posix_istream(archive_path, std::ios_base::in | std::ios_base::binary);
      new_hog = posix_ostream(tmp_archive, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    }
    else
      new_hog = posix_ostream(archive_path, std::ios_base::out | std::ios_base::binary);

    new_hog << new_header;

    for (const hog2::entry_t& entry : new_header)
    {
      if (auto it = find_file_entry(old_header, entry.name.string()); it != std::end(old_header))
      {
        std::cout << "Copying " << entry.name.string() << "... ";
        auto old_offset = old_header.fileOffset(it);
        old_hog.seek(old_offset);
        auto length = it->len;
        while(length--)
          new_hog.put(old_hog.get());
        std::cout << "[ok]" << std::endl;
      }
      else
      {
        std::cout << "Adding " << entry.name << "... ";
        posix_istream input_file(base_path / entry.name.string(), std::ios_base::in | std::ios_base::binary);
        while(!input_file.eof())
          new_hog.put(input_file.get());
        std::cout << "[ok]" << std::endl;
      }
    }

    if(old_hog.is_open())
      old_hog.close();
    new_hog.close();

    if (modify_mode)
      std::filesystem::rename(tmp_archive, archive_path);
  }

  void create(void)
  {
    file_path_list = validate_files(base_path, file_path_list);

    hog2::archive_t hog_header, dummy_header;
    add_files_to_header(hog_header, file_path_list);

    std::cout << "Creating " << archive_path << "..." << std::endl;
    std::filesystem::create_directories(std::filesystem::absolute(archive_path).parent_path());
    amend_hog(dummy_header, hog_header);
  }

  void add(void)
  {
    file_path_list = validate_files(base_path, file_path_list);

    posix_istream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
    hog2::archive_t old_header, new_header;
    ifs >> old_header;
    ifs.close();
    new_header = old_header;

    add_files_to_header(new_header, file_path_list);
    amend_hog(old_header, new_header);
  }

  void remove(void)
  {
    posix_istream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
    hog2::archive_t old_header, new_header;
    ifs >> old_header;
    ifs.close();
    new_header = old_header;

    for (auto& file_path : file_path_list)
      if (auto it = find_file_entry(new_header, file_path.string()); it != std::end(new_header))
        new_header.removeEntry(it);

    amend_hog(old_header, new_header);
  }

  void list(void)
  {
    hog2::archive_t hog_table;

    posix_istream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
    ifs >> hog_table;
    for (const hog2::entry_t& entry : hog_table)
      std::cout << entry.name.string() << std::endl;
  }

  void extract(void)
  {
    hog2::archive_t archive;
    posix_istream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
    ifs >> archive;

    auto extractor = [&archive, &ifs](auto it)
    {
      std::cout << "Extracting " << it->name.string() << std::endl;
      std::filesystem::create_directories(std::filesystem::absolute(base_path / it->name.string()).parent_path());
      posix_ostream ofs(base_path / it->name.string(), std::ios_base::out | std::ios_base::binary);

      ifs.seek(archive.fileOffset(it));
      auto length = it->len;
      while(length-- && !ifs.eof())
        ofs.put(ifs.get());
    };

    if (file_path_list.empty())
    {
      for (auto it = std::begin(archive); it != std::end(archive); it++)
        extractor(it);
    }
    else
    {
      for (const auto& i : file_path_list)
      {
        if (auto it = find_file_entry(archive, i.string()); it != std::end(archive))
          extractor(it);
        else
          std::cout << "File " << i << " is not in the hog file. Skipping..." << std::endl;
      }
    }
  }


  void version(void)
  {
    std::cout << "Descent Archiver, version "
              << DESCAR_MAJORVER << "."
              << DESCAR_MINORVER << "."
              << DESCAR_BUILD << " "
              << DESCAR_GIT_HASH << std::endl
              << R"(Copyright (C) 2024 Descent Network

This is public domain software; you are free to change and redistribute it.)" << std::endl;
  }


  void help(void)
  {
    std::cout << R"(Usage:  descar <command> archive [--directory dirname] filename...
        descar <command> archive [--directory dirname] --files-from filename

  commands:
    --create      Create a new hog file that will contain the specified files.
                  * The directory option specifies the base directory of the specified files.
                  * Will overwrite an existing hog archive.

    --add         Add one or more files to an existing hog file.
                  * The directory option specifies the base directory of the specified files.

    --delete      Delete one or more files from a hog file.
                  * Filenames are those listed in the hog file.

    --list        Print a list of filenames of files contained in the hog file.

    --extract     Extract the specified list of files from the hog file.
                  * If no filenames are specified then all files will be extracted.
                  * The directory option specifies extraction location.

  options:
    --directory   Specifies the directory for the command to use.
                  * If not specified then it is implied to be the current directory.

    --files-from  Load the list of files from a text file instead the command line.
                  * Format must be one filepath per line.
                  * If the directory option is not specified then the filepaths will be relative
                    to the path of this file.
)";
  }
}

static inline int get_hog_path(int parsed, const int argc, char* const argv[])
{
  parsed++;
  if (parsed < argc)
    cli::archive_path = argv[parsed];
  else
    throw std::invalid_argument("No hog file specified.");
  return parsed;
}

static inline int get_base_path(int parsed, const int argc, char* const argv[])
{
  parsed++;
  if (parsed < argc)
    cli::base_path = argv[parsed];
  else
    throw std::invalid_argument("Missing directory argument");
  return parsed;
}

static inline int get_file_path_list(int parsed, const int argc, char* const argv[])
{
  parsed++;
  if (parsed < argc)
  {
    std::filesystem::path txt_path = argv[parsed];
    if (cli::base_path.empty())
      cli::base_path = txt_path.parent_path();

    std::ifstream input(txt_path, std::ios_base::in);
    for (std::string line; std::getline(input, line);)
      cli::file_path_list.push_back(line);
  }
  else
    throw std::invalid_argument("Missing filename argument");
  return parsed;
}

static inline uint64_t get_command(const char* arg, std::size_t length)
{
  if(length > 1 && arg[0] == '-')
    return hash(arg, length);
  return 0;
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    cli::help();
    std::exit(EXIT_FAILURE);
  }

  std::function<void(void)> cli_operation = nullptr;
  try
  {
    bool parsing_files = false;
    for (int parsed = 1; parsed < argc; parsed++)
    {
      switch (get_command(argv[parsed], std::strlen(argv[parsed])))
      {
      case "--create"_hash:
        cli_operation = cli::create;
        parsed = get_hog_path(parsed, argc, argv);
        break;
      case "--add"_hash:
        cli_operation = cli::add;
        parsed = get_hog_path(parsed, argc, argv);
        break;
      case "--delete"_hash:
        cli_operation = cli::remove;
        parsed = get_hog_path(parsed, argc, argv);
        break;
      case "--list"_hash:
        cli_operation = cli::list;
        parsed = get_hog_path(parsed, argc, argv);
        break;
      case "--extract"_hash:
        cli_operation = cli::extract;
        parsed = get_hog_path(parsed, argc, argv);
        break;
      case "--directory"_hash:
        parsed = get_base_path(parsed, argc, argv);
        parsing_files = false; // files may have already been specified
        break;
      case "--files-from"_hash:
        get_file_path_list(parsed, argc, argv);
        parsing_files = false; // files may have already been specified
        break;
      case "--version"_hash:
        cli::version();
        std::exit(EXIT_SUCCESS);
        break;
      case "--help"_hash:
        cli::help();
        std::exit(EXIT_SUCCESS);
        break;
      default:
        if (cli_operation == nullptr ||                      // if no hog_operation was specified
            (!parsing_files && !cli::file_path_list.empty())) // if files were already specified
          throw std::invalid_argument(argv[parsed]);

        parsing_files = true; // now specifying files
        cli::file_path_list.push_back(argv[parsed]);
        break;
      }
    }
  }
  catch (std::invalid_argument& e)
  {
    std::cerr << "Invalid program argument: " << e.what() << std::endl;
    cli::help();
    std::exit(EXIT_FAILURE);
  }

  try
  {
    if (cli::base_path.empty())
      cli::base_path = std::filesystem::current_path();
    cli_operation();
    std::cout << "Done!" << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Fatal: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
