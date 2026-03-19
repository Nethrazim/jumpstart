#include "router.h"


void Router::get(const string& pattern, Handler handler) {
	routes_.push_back(Route{ "GET", pattern, std::move(handler) });
}

void Router::post(const string& pattern, Handler handler) {
	routes_.push_back(Route{ "POST", pattern, std::move(handler) });
}

void Router::put(const string& pattern, Handler handler)
{
	routes_.push_back(Route{ "PUT", pattern, std::move(handler) });
}

void Router::patch(const string& pattern, Handler handler)
{
	routes_.push_back(Route{ "PATCH", pattern, std::move(handler) });
}

void Router::head(const string& pattern, Handler handler)
{
	routes_.push_back(Route{ "HEAD", pattern, std::move(handler) });
}

void Router::options(const string& pattern, Handler handler)
{
	routes_.push_back(Route{ "OPTIONS", pattern, std::move(handler) });
}

void Router::del(const string& pattern, Handler handler)
{
	routes_.push_back(Route{ "DELETE", pattern, std::move(handler) });
}

void Router::dispatch(const HttpRequest& req, HttpResponse& resp) const {
	for (const Route& route : routes_) {
		if (route.method == req.method && route.pattern == req.path) {
			Response* controllerResponse = route.handler(Request{ req.method, req.path, req.body, req.headers });

			resp.headers = std::move(controllerResponse->headers);
			resp.raw = std::move(controllerResponse->raw);
			delete controllerResponse;
			return;
		}
	}
	resp.raw = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
}

void Router::setRoutes() {

}