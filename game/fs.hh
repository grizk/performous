#pragma once

#include <boost/filesystem/path.hpp>
#include <list>

namespace fs = boost::filesystem;

typedef std::vector<std::uint8_t> BinaryBuffer;

/// Read an entire file into a buffer
BinaryBuffer readFile(fs::path const& path);

/// Determine where the important system paths and most importantly the config schema are. Must be run before any of the functions below.
void pathBootstrap();

/// Do full data dir (search path) init or re-init after config options have been modified.
/// - Full search path and themes will only be available after this.
/// - Logging and config must be running before pathInit (pathInit is first called from configuration.cc).
void pathInit();

std::list<std::string> getThemes();  ///< Find all theme folders and return theme names.

fs::path getLogFilename();  ///< Get the log filename.
fs::path getSchemaFilename();  ///< Get the config schema filename.
fs::path getHomeDir();  ///< Get user's home folder.
fs::path getConfigDir();  ///< Get user-writable Performous config folder.
fs::path getSysConfigDir();  ///< Get root-writable Performous config folder.
fs::path getDataDir();  ///< Get user-writable Performous data folder.
fs::path getCacheDir();  ///< Get user-writable temporary folder.
fs::path getShareDir();  ///< Get Performous system-level data folder.
fs::path getLocaleDir();  ///< Get the system local folder.

fs::path findFile(fs::path const& filename);  ///< Look for the specified file in theme and data folders.

typedef std::list<fs::path> Paths;

Paths const& getPaths();  ///< Get the data file search path
Paths getPathsConfig(std::string const& confOption);  ///< Return expanded list of paths specified by a path config option

