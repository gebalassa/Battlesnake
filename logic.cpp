#pragma once
#include "./json.hpp"
#include <iostream>
#include <list>
using namespace std;
using namespace nlohmann;

// Nodos que formarán el MCST.
class Node {
private:
	int visitas;
	int puntuacion;
	Node *padre;
	list<Node> hijos;
	json tablero;
	// DEBUGGING
	bool debug = true;

	void crearHijo(json nuevoTablero) {
		// Variables locales
		Node nuevoHijo = Node(nuevoTablero, this);
		hijos.push_back(nuevoHijo);
		if (debug) {
			cout << "Nuevo Hijo" << endl;
		}
	}

	bool checkCollision(json tablero) {
		// COMPLETAR
		return true;
	}

public:
	Node(json _tablero, Node *_padre = nullptr)
		: tablero(_tablero), padre(_padre) {
		visitas = 0;
		puntuacion = 0;
		hijos = {};
	}

	void crearHijos() {
		list<json> currentStates = {};

		// Movimientos posibles del enemigo
		int totalEnemigos = tablero["board"]["snakes"].size();
		// cout << "BANANA: "<<totalEnemigos<<endl;
		// cout << tablero["board"]["snakes"]<<endl;

		// 3 Movimientos posibles por cada enemigo
		for (int i = 0; i < totalEnemigos; i++) {
			json currSnake = tablero["board"]["snakes"][i];
			// cout << currSnake["name"] << endl;

			// 3 Movimientos posibles
			// ARRIBA
			json nuevoTablero = tablero;
			//cout << currSnake["head"]<<endl;
			currSnake["head"]["y"] = (int)currSnake["head"]["y"] + 1;
			nuevoTablero["board"]["snakes"][i]["head"]["y"] =
				currSnake["head"]["y"];
			// SEGUIR ACA******
		}
	}

	// DEBUGGEO/testeo del tablero JSON
	void debugNodoInfo() {
		cout << "Nodo Vida: " << tablero["you"]["health"] << endl;
	}
};

class MonteCarloSearchTree {
	static int currentDepth; // Se usará para determinar profundidad actual
	int maxDepth; // Se usará para evitar demora excesiva de simulación
	json tableroInicial;
	Node rootNode;

public:
	MonteCarloSearchTree(json _tablero, int _maxDepth)
		: tableroInicial(_tablero),
		  maxDepth(_maxDepth),
		  rootNode(Node(tableroInicial)) {
	}

	string nextMove() {
		// string moves[4] = {"up", "down", "left", "right"};
		rootNode.crearHijos();

		return "PLACEHOLDER";
	}

	// DEBUGGING
	void debug() {
		rootNode.debugNodoInfo();
	}
};