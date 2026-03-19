#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "http_request.h"
#include "http_response.h"

using std::string;
using std::vector;

class Router {

public:
	using Handler = std::function<Response*(const Request&)>;

	void get(const string& pattern, Handler handler);

	void post(const string& pattern, Handler handler);

	void put(const string& pattern, Handler handler);

	void patch(const string& pattern, Handler handler);

	void head(const string& pattern, Handler handler);

	void options(const string& pattern, Handler handler);

	void del(const string& pattern, Handler handler);

	void dispatch(const HttpRequest& req, HttpResponse& resp) const;

	virtual void setRoutes();

private:
	struct Route {
		string method{};
		string pattern{};
		Handler handler;
	};

	//todo route parameter query parameters body parameters
	vector<Route> routes_;
};