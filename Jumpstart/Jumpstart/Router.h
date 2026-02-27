#pragma once
#include <string>
#include <vector>
#include <functional>
#include "http_request.h"
#include "http_response.h"
#include <unordered_map>

using std::string;
using std::vector;

class Router {
public:
	using Handler = std::function<Response*(const Request&)>;

	void get(const string& pattern, Handler handler)
	{
		routes_.push_back(Route{ "GET", pattern, std::move(handler) });
	}
	
	void post(const string& pattern, Handler handler)
	{
		routes_.push_back(Route{ "POST", pattern, std::move(handler) });
	}

	void put(const string& pattern, Handler handler)
	{
		routes_.push_back(Route{ "PUT", pattern, std::move(handler) });
	}

	void patch(const string& pattern, Handler handler)
	{
		routes_.push_back(Route{ "PATH", pattern, std::move(handler) });
	}

	void head(const string& pattern, Handler handler)
	{
		routes_.push_back(Route{ "HEAD", pattern, std::move(handler) });
	}

	void options(const string& pattern, Handler handler)
	{
		routes_.push_back(Route{ "OPTIONS", pattern, std::move(handler) });
	}

	void del(const string& pattern, Handler handler)
	{
		routes_.push_back(Route{ "DELETE", pattern, std::move(handler) });
	}

	void dispatch(const HttpRequest& req, HttpResponse& resp) const
	{
		for (const Route& route : routes_) {
			if (route.method == req.method && route.pattern == req.path) {
				Response* r = route.handler(Request{ req.method, req.path, req.body, req.headers });

				resp.headers = std::move(r->headers);
				resp.raw = std::move(r->raw);
				delete r;
				return;
			}
		}
		resp.raw = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
	}

private:
	struct Route {
		string method{};
		string pattern{};
		Handler handler;
	};

	vector<Route> routes_;
};