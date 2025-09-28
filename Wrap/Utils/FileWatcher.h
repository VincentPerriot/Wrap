#pragma once

#include <filesystem>
#include <thread>
#include <functional>

class FileWatcher {

public:
	FileWatcher( const std::filesystem::path& _dir, std::function<void( const std::filesystem::path& )> _cb );
	~FileWatcher();

	FileWatcher( const FileWatcher& ) = delete;
	FileWatcher& operator=( const FileWatcher& ) = delete;
	FileWatcher( FileWatcher&& ) = delete;
	FileWatcher& operator=( FileWatcher&& ) = delete;

private:
	void start();
	void watch();
	void stop();

	std::filesystem::path m_Dir;
	std::thread           m_Thread;
	bool                  m_Running{ false };

	std::function<void( const std::filesystem::path& )> m_Cb;
};