/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include "pipe.h"
#include <vector>

namespace ti
{
	Pipe::Pipe(Poco::PipeIOS* pipe) : pipe(pipe), closed(false)
	{
		this->Set("closed",Value::NewBool(false));
		this->SetMethod("close",&Pipe::Close);
		this->SetMethod("write",&Pipe::Write);
		this->SetMethod("read",&Pipe::Read);
	}
	Pipe::~Pipe()
	{
		Close();
		delete pipe;
		pipe = NULL;
	}
	void Pipe::Write(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw Value::NewString("pipe already closed");
		}
		if (!args.at(0)->IsString())
		{
			throw Value::NewString("can only write string data");
		}
		Poco::PipeOutputStream *os = dynamic_cast<Poco::PipeOutputStream*>(pipe);
		if (os==NULL)
		{
			throw Value::NewString("stream is not writeable");
		}
		const char *data = args.at(0)->ToString();
		int len = (int)strlen(data);
		try
		{
			os->write(data,len);
			result->SetInt(len);
		}
		catch (Poco::WriteFileException &e)
		{
			throw Value::NewString(e.what());
		}
	}
	void Pipe::Read(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw Value::NewString("pipe already closed");
		}
		Poco::PipeInputStream *is = dynamic_cast<Poco::PipeInputStream*>(pipe);
		if (is==NULL)
		{
			throw Value::NewString("stream is not readable");
		}
		char *buf = NULL;
		try
		{
			int size = 1024;
			// allow the size of the returned buffer to be 
			// set by the caller - defaults to 1024 if not specified
			if (args.size()>0 && args.at(0)->IsInt())
			{
				size = args.at(0)->ToInt();
			}
			buf = new char[size];
			is->read(buf,size);
			int count = is->gcount();
			if (count <=0)
			{
				result->SetNull();
			}
			else
			{
				buf[count] = '\0';
				result->SetString(buf);
			}
			delete [] buf;
		}
		catch (Poco::ReadFileException &e)
		{
			if (buf) delete[] buf;
			throw Value::NewString(e.what());
		}
	}
	void Pipe::Close(const ValueList& args, SharedValue result)
	{
		Close();
	}
	void Pipe::Close()
	{
		if (!closed)
		{
			pipe->close();
			closed=true;
			this->Set("closed",Value::NewBool(true));
		}
	}
}