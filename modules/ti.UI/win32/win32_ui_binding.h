/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WIN32_UI_BINDING_H_
#define _WIN32_UI_BINDING_H_

#include "../ui_module.h"

namespace ti
{
	class Win32UIBinding : public UIBinding
	{

	public:
		Win32UIBinding();
		~Win32UIBinding();

		SharedPtr<MenuItem> CreateMenu();
		void SetMenu(SharedPtr<MenuItem>);
		void SetContextMenu(SharedPtr<MenuItem>);
		void SetIcon(SharedString icon_path);
		SharedPtr<TrayItem> AddTray(SharedString icon_path,
		                            SharedBoundMethod cb);

		void OpenFiles(
			SharedBoundMethod callback,
			bool multiple,
			bool files,
			bool directories,
			std::string& path,
			std::string& file,
			std::vector<std::string>& types);

		long GetSystemIdleTime();

	private:
		static SharedBoundList SelectDirectory(
			bool multiple,
			std::string& path,
			std::string& file);

		static SharedBoundList SelectFile(
			SharedBoundMethod callback,
			bool multiple,
			std::string& path,
			std::string& file,
			std::vector<std::string>& types)

			static void ParseStringNullSeparated(const char *s, std::vector<std::string> &tokens);
	};
}

#endif
