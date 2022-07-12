// HTTP credits: http://lcs.ios.ac.cn/~maxt/SPelton/reports/report-9aa0d3.html
// JSON credits: https://github.com/nlohmann/json
#include "./http_stuff.h"
using namespace std;
#include "./json.hpp"
#include "logic.cpp"
#include <iostream>
//using namespace nlohmann;

int main(void) {
    bool activateServer = true;
    if (activateServer) {
		httplib::Server svr;
		svr.Get("/", [](const auto &, auto &res) {
			string head = "default";  // TODO: Change head
			string tail = "default";  // TODO: Change tail
			string author = "";		  // TODO: Change your battlesnake username
			string color = "#FF0000"; // TODO: Change a hex color
			res.set_content(
				"{\"apiversion\":\"1\", \"head\":\"" + head +
					"\", \"tail\":\"" + tail + "\", \"color\":\"" + color +
					"\", " + "\"author\":\"" + author + "\"}",
				"application/json");
		});
		svr.Post("/end", [](const auto &, auto &res) {
			res.set_content("ok", "text/plain");
		});
		svr.Post("/start", [](const auto &, auto &res) {
			res.set_content("ok", "text/plain");
		});
		svr.Post("/move", [](auto &req, auto &res) {
			const json data = json::parse(req.body);
			//(cout << data;
			//cout << "\n\n";

			// vARIABLES NUEVAS
			// AQUI--------------------------------------------------
			int maxDepth = 50;
			MonteCarloSearchTree tree = MonteCarloSearchTree(data, maxDepth);
			//-----------------------------------------------------------
			// You can get the "you" property like this:
			// data["you"];
			// Almost alike python dictionary parsing, but with a semicolon at
			// the end of each line. You might need to make some structs to
			// store some data in a variable Example: you_struct you =
			// data["you"];
			string moves[4] = {"up", "down", "left", "right"};

			// ESCRIBIR CÓDIGO NUEVO AQUÍ -------------------------------
			// tree.debugJSON();
			string newMove = "";
			tree.nextMove();
			//---------------------
			int index = rand() % 4; // LO QUE HAY QUE SACAR
			//-----------------------------------------------------------

			res.set_content(
				"{\"move\": \"" + moves[index] + "\"}", "text/plain");
		});
		svr.listen("0.0.0.0", 8080);
		cout << "Server started";
	}

    return 0;
}
