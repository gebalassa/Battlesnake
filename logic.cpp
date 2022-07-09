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

	// Creación de hijos (movimientos posibles)
	void crearHijos() {
		// VARS
		list<json> currentStates = {}; // Acá se agregan los tableros válidos
		json nuevoTablero = tablero;   // Acá va cada tablero a agregar
		int totalSerpientes = tablero["board"]["snakes"].size();
        string listaMovimientos[4] = {"ARRIBA", "ABAJO", "DERECHA", "IZQUIERDA"};
        
		// Movimientos posibles por serpiente = 4
        // Total de movimientos: 4^serpientes
		// ** El chequeo de validez y anti-suicida se hará después **
		for (int i = 0; i < totalSerpientes; i++) {
            
		}
	}

	// Funciones de COMPROBACIÓN/CHEQUEO
	// Chequeo de muros
	bool chequearMuro(json tablero) {
		int totalSerpientes = tablero["board"]["snakes"].size();
		int stageWidth = tablero["board"]["width"];
		int stageHeight = tablero["board"]["height"];

		for (int i = 0; i < totalSerpientes; i++) {
			json currSnake = tablero["board"]["snakes"][i];
			if (currSnake["head"]["x"] > stageWidth - 1) {
				return false;
			} else if (currSnake["head"]["y"] > stageHeight - 1) {
				return false;
			} else if (currSnake["head"]["x"] < 0) {
				return false;
			} else if (currSnake["head"]["y"] < 0) {
				return false;
			}
		}
		return true;
	}

	// Chequeo colisión con cuello en cada serpiente
	bool chequearColisionCuello(json tablero) {
		int totalSerpientes = tablero["board"]["snakes"].size();

		for (int i = 0; i < totalSerpientes; i++) {
			json currSnake = tablero["board"]["snakes"][i];
			json currCabeza = currSnake["head"];
			json currCuello = currSnake["body"][1];

			if (currCabeza["x"] == currCuello["x"] ||
				currCabeza["y"] == currCuello["y"]) {
				return false;
			}
		}
		return true;
	}

	// Retorna movimiento sin chequeos
	json moveSnakeRaw(json tablero, json serpiente, string movimiento) {
		// VARS
		json prevTablero = tablero;	// Para guardar cambios definitivos
		json postTablero = tablero;	// Para guardar cambios brutos
		json initialSnake = serpiente; // Para devolverse
		json currSnake = initialSnake; // Para cambios brutos serpiente
		int totalSerpientes = tablero["board"]["snakes"].size();

		// Index de serpiente
		int currSnakeIndex;
		for (int i = 0; i < totalSerpientes; i++) {
			if (tablero["board"]["snakes"][i]["id"] == currSnake["id"]) {
				currSnakeIndex = i;
				break;
			} else {
				throw(-1);
			}
		}

		// Generar vector de movimiento correspondiente
		int moveVector[2];
		if (movimiento == "ARRIBA") {
			moveVector[0] = 0;
			moveVector[1] = 1;
		} else if (movimiento == "ABAJO") {
			moveVector[0] = 0;
			moveVector[1] = -1;
		} else if (movimiento == "DERECHA") {
			moveVector[0] = 1;
			moveVector[1] = 0;
		} else if (movimiento == "IZQUIERDA") {
			moveVector[0] = -1;
			moveVector[1] = 0;
		}

		// Agregar nueva posicion cabeza
		currSnake["head"]["y"] = (int)currSnake["head"]["x"] + moveVector[0];
		currSnake["head"]["y"] = (int)currSnake["head"]["y"] + moveVector[1];

		postTablero["board"]["snakes"][currSnakeIndex]["head"]["x"] =
			currSnake["head"]["x"];
		postTablero["board"]["snakes"][currSnakeIndex]["head"]["y"] =
			currSnake["head"]["y"]; // Cambio en "head"
		postTablero["board"]["snakes"][currSnakeIndex]["body"].insert(
			postTablero["board"]["snakes"][currSnakeIndex]["body"].begin(),
			currSnake["head"]); // Cambio en "body"

		// Remover cola
		postTablero["board"]["snakes"][currSnakeIndex]["body"].erase(
			postTablero["board"]["snakes"][currSnakeIndex]["body"].size() - 1);
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