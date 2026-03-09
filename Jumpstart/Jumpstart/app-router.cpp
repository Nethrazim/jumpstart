#include "app-router.h"
#include "init-controller.h"

void AppRouter::setRoutes() {
	//define routes

	get("/", [this](const Request& req) {
		return initController.handleGet(req);
	});
}