#pragma once
#include "http_request.h"


class Response;

class InitController
{

public:
	Response* handleGet(const Request& req);
};

