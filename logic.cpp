#pragma once
#include "./json.hpp"
#include <iostream>
#include <vector>
using namespace std;
using namespace nlohmann;

// Nodos que formarán el MCTS.
class Node {
private:
	int visitas;
	int puntuacion;
	Node *padre;
	vector<Node> hijos;
	json tablero;
	// DEBUGGING
	bool debug = true;
	bool debugCrearHijos = true;
	bool debugTotalSnakeMovementsRaw = true;
	bool debug_TotalSnakeMovementsRaw = false;
	bool debugMoveSnakeRaw = false;

	void crearHijo(json nuevoTablero) {
		// Variables locales
		Node nuevoHijo = Node(nuevoTablero, this);
		hijos.push_back(nuevoHijo);
		if (debug) {
			cout << "Nuevo Hijo" << endl;
		}
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
		vector<json> currentStates = {}; // Acá se agregan los tableros válidos
		json nuevoTablero = tablero;	 // Acá va cada tablero a agregar
		int totalSerpientes = tablero["board"]["snakes"].size();
		if (debug && debugCrearHijos) {
			cout << "Entrando a rawMovementStates. Serpientes: "
				 << totalSerpientes << endl;
		}
		// Movimientos posibles por serpiente = 4
		// Total de movimientos: 4^serpientes
		// ** El chequeo de validez y anti-suicida se hará después **
		vector<json> rawMovementStates = totalSnakeMovementsRaw(tablero);
		if (debug && debugCrearHijos) {
			cout << "Salio de funcion rawMovementStates!!!"
				 << "\n";
		}

		// ***TERMINAR********
	}

	// --------------Funciones de COMPROBACIÓN/CHEQUEO-----------------
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
	// ***ARREGLAR PARA QUE REVISE COLISION CON CUALQUIER CUERPO Y CUALQUIER
	// SERPIeNTE***
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
	//------------------FIN FUNCIONES CHEQUEO----------------------

	// Todos los movimientos brutos desde el tablero actual, recursivamente
	vector<json> _totalSnakeMovementsRaw(json currTablero, int snakeIndex) {
		// VARS
		vector<json> currMovs = {};
		int totalSerpientes = currTablero["board"]["snakes"].size();

		//------DEBUG--------------------
		if (debug && debug_TotalSnakeMovementsRaw) {
			cout << "_totalSnakeMovementsRaw. Serpientes: " << totalSerpientes
				 << "\n";
		}
		//--------------------------FINDEBUG-------------

		// Realizar movimiento
		json currSnake = currTablero["board"]["snakes"][snakeIndex];
		json movs1 = moveSnakeRaw(currTablero, currSnake, "ARRIBA");
		json movs2 = moveSnakeRaw(currTablero, currSnake, "ABAJO");
		json movs3 = moveSnakeRaw(currTablero, currSnake, "IZQUIERDA");
		json movs4 = moveSnakeRaw(currTablero, currSnake, "DERECHA");

		// Paso a la siguiente serpiente
		vector<json> nextMovs1 = {}, nextMovs2 = {}, nextMovs3 = {},
					 nextMovs4 = {};
		if (snakeIndex < totalSerpientes - 1) { /*NO es hoja*/
			nextMovs1 = _totalSnakeMovementsRaw(movs1, snakeIndex + 1);
			nextMovs2 = _totalSnakeMovementsRaw(movs2, snakeIndex + 1);
			nextMovs3 = _totalSnakeMovementsRaw(movs3, snakeIndex + 1);
			nextMovs4 = _totalSnakeMovementsRaw(movs4, snakeIndex + 1);

			// Unión de movimientos de esta rama
			currMovs.insert(currMovs.end(), nextMovs1.begin(), nextMovs1.end());
			currMovs.insert(currMovs.end(), nextMovs2.begin(), nextMovs2.end());
			currMovs.insert(currMovs.end(), nextMovs3.begin(), nextMovs3.end());
			currMovs.insert(currMovs.end(), nextMovs4.begin(), nextMovs4.end());

			return currMovs;
		} else { /*ES hoja*/
			vector<json> myMoves = {};
			myMoves.push_back(movs1);
			myMoves.push_back(movs2);
			myMoves.push_back(movs3);
			myMoves.push_back(movs4);
			currMovs.insert(currMovs.end(), myMoves.begin(), myMoves.end());
			return currMovs;
		}
	}
	// Funcion contenedora
	vector<json> totalSnakeMovementsRaw(json tablero) {
		//----------DEBUG PREV----------
		if (debug && debugTotalSnakeMovementsRaw) {
			cout << "CONTENEDORA. Entrando a _totalSnakeMovementsRaw. "
					"Serpientes:"
				 << tablero["board"]["snakes"].size() << "\n";
		}
		//------------CÓDIGO-----------------

		vector<json> movs = _totalSnakeMovementsRaw(tablero, 0);

		//----DEBUG POST------
		if (debug && debugTotalSnakeMovementsRaw) {
			cout << "CONTENEDORA. Tamano movimientos posibles raw: "
				 << movs.size() << "\n";
			for (int i = 0; i < movs.size(); i++) {
				cout << "CONTENEDORA: Movimientos:"
					 << "Movimiento " << i << ": " << endl
					 << movs[i]["board"]["snakes"][0]["body"] << endl
					 << movs[i]["board"]["snakes"][1]["body"] << endl;
			}
		}
		//----------

		return movs;
	}

	// Retorna movimiento sin chequeos
	json moveSnakeRaw(json tablero, json serpiente, string mov) {
		json postTablero = tablero; // Para cambios tablero
		json currSnake = serpiente; // Para cambios serpiente
		int totalSerpientes = tablero["board"]["snakes"].size();

		//---DEBUG---
		if (debug && debugMoveSnakeRaw) {
			cout << "moveSnakeRaw: Serp: " << totalSerpientes << "\n";
			cout << "moveSnakeRaw: Mov: " << mov << "\n";
			cout << "moveSnakeRaw: currSerp name: " << serpiente["name"]
				 << endl;
		}
		//---FIN DEBUG---

		// Index de serpiente
		int currSnakeIndex;
		for (int i = 0; i < totalSerpientes; i++) {
			if (tablero["board"]["snakes"][i]["id"] == currSnake["id"]) {
				//---DEBUG---
				if (debug && debugMoveSnakeRaw) {
					cout << "moveSnakeRaw:ID: "
						 << tablero["board"]["snakes"][i]["id"] << endl;
					cout << "moveSnakeRaw:currsnakeID: " << currSnake["id"]
						 << endl;
				}
				//---FIN DEBUG---
				currSnakeIndex = i;
				break;
			}
		}

		//---DEBUG---
		if (debug && debugMoveSnakeRaw) {
			cout << "moveSnakeRaw: currSerp index: " << currSnakeIndex << "\n";
		}
		//---FIN DEBUG---

		// Generar vector de movimiento correspondiente
		int moveVector[2];
		if (mov == "ARRIBA") {
			moveVector[0] = 0;
			moveVector[1] = 1;
		} else if (mov == "ABAJO") {
			moveVector[0] = 0;
			moveVector[1] = -1;
		} else if (mov == "DERECHA") {
			moveVector[0] = 1;
			moveVector[1] = 0;
		} else if (mov == "IZQUIERDA") {
			moveVector[0] = -1;
			moveVector[1] = 0;
		}

		//---DEBUG---
		if (debug && debugMoveSnakeRaw) {
			cout << "moveSnakeRaw: PREVcurrSnake head,x "
				 << postTablero["board"]["snakes"][currSnakeIndex]["head"]["x"]
				 << "\n";
			cout << "moveSnakeRaw: PREVcurrSnake head,y "
				 << postTablero["board"]["snakes"][currSnakeIndex]["head"]["y"]
				 << "\n";
			cout << "moveSnakeRaw: PREVcurrSnake BODY "
				 << postTablero["board"]["snakes"][currSnakeIndex]["body"]
				 << "\n";
		}
		//---FIN DEBUG---

		// Agregar nueva posicion cabeza en "head" y "body"
		currSnake["head"]["x"] = (int)currSnake["head"]["x"] + moveVector[0];
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

		//---DEBUG---
		if (debug && debugMoveSnakeRaw) {
			cout << "moveSnakeRaw: POSTcurrSnake head,x "
				 << postTablero["board"]["snakes"][currSnakeIndex]["head"]["x"]
				 << "\n";
			cout << "moveSnakeRaw: POSTcurrSnake head,y "
				 << postTablero["board"]["snakes"][currSnakeIndex]["head"]["y"]
				 << "\n";
			cout << "moveSnakeRaw: POSTcurrSnake BODY "
				 << postTablero["board"]["snakes"][currSnakeIndex]["body"]
				 << "\n";
		}
		//---FIN DEBUG---

		return postTablero;
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