#include "FileWatcher.h"

#include "Windows.h"

//------------------------------------------------------------------------------------
FileWatcher::FileWatcher( const std::filesystem::path& _dir, std::function<void( const std::filesystem::path& )> _cb )
	: m_Dir( _dir )
	, m_Cb( _cb )
{
	start();
}

//------------------------------------------------------------------------------------
FileWatcher::~FileWatcher()
{
	stop();
}

//------------------------------------------------------------------------------------
void FileWatcher::start()
{
	if ( !m_Running )
	{
		m_Running = true;
		m_Thread = std::thread( &FileWatcher::watch, this );
		m_Thread.detach();
	}
}

//------------------------------------------------------------------------------------
void FileWatcher::watch()
{
	HANDLE hDir{ CreateFileW(
		m_Dir.wstring().c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		nullptr
	) };

	BYTE buffer[1024];
	DWORD numBytesTransfer;
	FILE_NOTIFY_INFORMATION* pInfo;

	while ( m_Running )
	{
		if ( ReadDirectoryChangesW(
			hDir,
			buffer,
			sizeof( buffer ),
			false,
			FILE_NOTIFY_CHANGE_LAST_WRITE,
			&numBytesTransfer,
			nullptr,
			nullptr
		) )
		{
			pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>( buffer );

			while ( pInfo )
			{
				auto filename = std::wstring{ pInfo->FileName, ( pInfo->FileNameLength / sizeof( WCHAR ) ) };
				auto filepath = m_Dir / filename;

				if ( pInfo->Action == FILE_ACTION_MODIFIED )
					m_Cb( filepath );

				if ( pInfo->NextEntryOffset == 0 )
					break;

				pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>( reinterpret_cast<BYTE*>( pInfo ) + pInfo->NextEntryOffset );
			}
		}
	}
}

//------------------------------------------------------------------------------------
void FileWatcher::stop()
{
	m_Running = false;
}

