#pragma once
#include <string>
#include <Common/httplib.h>

namespace Common
{
	void initializeLogger();
	void log_request_response(const httplib::Request &req, const httplib::Response &res);
}