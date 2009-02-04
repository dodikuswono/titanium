/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "filesystem_binding.h"
#include "file.h"
#include "async_copy.h"

#include "api/file_utils.h"

#ifdef OS_OSX
#include <Cocoa/Cocoa.h>
#elif defined(OS_WIN32)
#include <windows.h>
#include <shlobj.h>
#include <process.h>
#endif

#include <Poco/TemporaryFile.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/LineEndingConverter.h>
#include <Poco/Exception.h>

namespace ti
{
	FilesystemBinding::FilesystemBinding(Host *host, SharedBoundObject global) : host(host), global(global)
	{
		this->SetMethod("createTempFile",&FilesystemBinding::CreateTempFile);
		this->SetMethod("createTempDirectory",&FilesystemBinding::CreateTempDirectory);
		this->SetMethod("getFile",&FilesystemBinding::GetFile);
		this->SetMethod("getApplicationDirectory",&FilesystemBinding::GetApplicationDirectory);
		this->SetMethod("getResourcesDirectory",&FilesystemBinding::GetResourcesDirectory);
		this->SetMethod("getDesktopDirectory",&FilesystemBinding::GetDesktopDirectory);
		this->SetMethod("getDocumentsDirectory",&FilesystemBinding::GetDocumentsDirectory);
		this->SetMethod("getUserDirectory",&FilesystemBinding::GetUserDirectory);
		this->SetMethod("getLineEnding",&FilesystemBinding::GetLineEnding);
		this->SetMethod("getSeparator",&FilesystemBinding::GetSeparator);
		this->SetMethod("getRootDirectories",&FilesystemBinding::GetRootDirectories);
		this->SetMethod("asyncCopy",&FilesystemBinding::AsyncCopy);
	}
	FilesystemBinding::~FilesystemBinding()
	{

	}
	void FilesystemBinding::CreateTempFile(const ValueList& args, SharedValue result)
	{
		try
		{
			Poco::TemporaryFile tempFile;
			tempFile.keepUntilExit();
			tempFile.createFile();

			ti::File* jsFile = new ti::File(tempFile.path());
			result->SetObject(jsFile);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::CreateTempDirectory(const ValueList& args, SharedValue result)
	{
		try
		{
			Poco::TemporaryFile tempDir;
			tempDir.keepUntilExit();
			tempDir.createDirectory();

			ti::File* jsFile = new ti::File(tempDir.path());
			result->SetObject(jsFile);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::GetFile(const ValueList& args, SharedValue result)
	{
		std::string filename;
		if (args.at(0)->IsList())
		{
			// you can pass in an array of parts to join
			SharedBoundList list = args.at(0)->ToList();
			for (int c=0;c<list->Size();c++)
			{
				std::string arg = list->At(c)->ToString();
				filename = kroll::FileUtils::Join(filename.c_str(),arg.c_str(),NULL);
			}
		}
		else
		{
			// you can pass in vararg of strings which acts like 
			// a join
			for (size_t c=0;c<args.size();c++)
			{
				std::string arg = args.at(c)->ToString();
				filename = kroll::FileUtils::Join(filename.c_str(),arg.c_str(),NULL);
			}
		}
		if (filename.empty())
		{
			throw ValueException::FromString("invalid file type");
		}
		ti::File* file = new ti::File(filename);
		result->SetObject(file);
	}
	void FilesystemBinding::GetApplicationDirectory(const ValueList& args, SharedValue result)
	{
		std::string dir = FileUtils::GetApplicationDirectory();
		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}
	void FilesystemBinding::GetResourcesDirectory(const ValueList& args, SharedValue result)
	{
		std::string dir = FileUtils::GetResourcesDirectory();
		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}
	void FilesystemBinding::GetDesktopDirectory(const ValueList& args, SharedValue result)
	{
		std::string dir;

#ifdef OS_WIN32
		char path[MAX_PATH];
		if(SHGetSpecialFolderPath(NULL,path,CSIDL_DESKTOP,FALSE))
		{
			dir.append(path);
		}
#elif OS_OSX
		NSString *fullPath = [@"~/Desktop" stringByExpandingTildeInPath];
		dir = [fullPath UTF8String];
#elif OS_LINUX
		// TODO
#endif
		if(dir.size() == 0)
		{
			result->SetNull();
		}
		else
		{
			ti::File* file = new ti::File(dir);
			result->SetObject(file);
		}
	}
	void FilesystemBinding::GetDocumentsDirectory(const ValueList& args, SharedValue result)
	{
		std::string dir;

#ifdef OS_WIN32
		char path[MAX_PATH];
		if(SHGetSpecialFolderPath(NULL,path,CSIDL_PERSONAL,FALSE))
		{
			dir.append(path);
		}
#elif OS_OSX
		NSString *fullPath = [@"~/Documents" stringByExpandingTildeInPath];
		dir = [fullPath UTF8String];
#elif OS_LINUX
		// TODO
#endif
		if(dir.size() == 0)
		{
			result->SetNull();
		}
		else
		{
			ti::File* file = new ti::File(dir);
			result->SetObject(file);
		}
	}
	void FilesystemBinding::GetUserDirectory(const ValueList& args, SharedValue result)
	{
		try
		{
			std::string dir = Poco::Path::home().c_str();
			ti::File* file = new ti::File(dir);
			result->SetObject(file);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::GetLineEnding(const ValueList& args, SharedValue result)
	{
		try
		{
			result->SetString(Poco::LineEnding::NEWLINE_LF.c_str());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::GetSeparator(const ValueList& args, SharedValue result)
	{
		try
		{
			std::string sep;
			sep += Poco::Path::separator();
			result->SetString(sep.c_str());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::GetRootDirectories(const ValueList& args, SharedValue result)
	{
		try
		{
			Poco::Path path;

			std::vector<std::string> roots;
			path.listRoots(roots);

			SharedPtr<StaticBoundList> rootList = new StaticBoundList();

			for(size_t i = 0; i < roots.size(); i++)
			{
				ti::File* file = new ti::File(roots.at(i));
				SharedValue value = Value::NewObject((SharedBoundObject) file);
				rootList->Append(value);
			}

			SharedPtr<BoundList> list = rootList;
			result->SetList(list);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::AsyncCopy(const ValueList& args, SharedValue result)
	{
		if (args.size()!=3)
		{
			throw ValueException::FromString("invalid arguments - this method takes 3 arguments");
		}
		std::vector<std::string> files;
		if (args.at(0)->IsString())
		{
			files.push_back(args.at(0)->ToString());
		}
		else if (args.at(0)->IsList())
		{
			SharedBoundList list = args.at(0)->ToList();
			for (int c=0;c<list->Size();c++)
			{
				SharedValue v = list->At(c);
				if (v->IsString())
				{
					files.push_back(v->ToString());
				}
				else if (v->IsObject())
				{
					SharedBoundObject bo = v->ToObject();
					SharedPtr<File> file = bo.cast<File>();
					if (file.isNull())
					{
						throw ValueException::FromString("invalid type passed as first argument");
					}
					files.push_back(file->GetFilename());
				}
			}
		}
		else if (args.at(0)->IsObject())
		{
			SharedBoundObject bo = args.at(0)->ToObject();
			SharedPtr<File> file = bo.cast<File>();
			if (file.isNull())
			{
				throw ValueException::FromString("invalid type passed as first argument");
			}
			files.push_back(file->GetFilename());
		}
		std::string destination;
		SharedValue v = args.at(1);
		if (v->IsString())
		{
			destination = v->ToString();
		}
		else if (v->IsObject())
		{
			SharedBoundObject bo = v->ToObject();
			SharedPtr<File> file = bo.cast<File>();
			if (file.isNull())
			{
				throw ValueException::FromString("invalid type passed as first argument");
			}
			destination = file->GetFilename();
		}
		SharedBoundMethod method = args.at(2)->ToMethod();
		SharedBoundObject copier = new ti::AsyncCopy(host,files,destination,method);
		result->SetObject(copier);
	}
}
