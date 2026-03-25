#include "app-router.h"

void AppRouter::setRoutes() {
	//define routes

	get("/", [this](const Request& req) {
		return initController.handleGet(req);
	});

	post("/", [this](const Request& req) {
		return initController.handleGet(req);
	});
}