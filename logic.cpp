#pragma once
#include "./json.hpp"
#include <cmath>
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
	bool debugCrearHijo = false;
	bool debugCrearHijos = false;
	bool debugTotalSnakeMovementsRaw = false;
	bool debug_TotalSnakeMovementsRaw = true;
	bool debugMoveSnakeRaw = false;
	bool debugChequearMuro = false;
	bool debugChequearColisionCuello = false;

	void crearHijo(json nuevoTablero) {
		// Variables locales
		Node nuevoHijo = Node(nuevoTablero, this);
		hijos.push_back(nuevoHijo);
		if (debug && debugCrearHijo) {
			cout << "CrearHijo:Nuevo Hijo. \"Head\": "
				 << nuevoTablero["you"]["head"] << endl;
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
		// vector<json> currentStates = {}; // Acá se agregan los tableros
		// válidos json nuevoTablero = tablero;	 // Acá va cada tablero a
		// agregar
		int totalSerpientes = tablero["board"]["snakes"].size();
		if (debug && debugCrearHijos) {
			cout << "CrearHijos:Entrando a totalSnakeMovementsRaw. Serpientes: "
				 << totalSerpientes << endl;
		}
		// Movimientos posibles por serpiente = 4
		// Total de movimientos: 4^serpientes
		// ** El chequeo de validez y anti-suicida se hará después **
		vector<json> rawMovementStates = totalSnakeMovementsRaw(tablero);

		// DEBUG---
		if (debug && debugCrearHijos) {
			cout << "CrearHijos:Salio de funcion totalSnakeMovementsRaw!!!"
				 << "\n"
				 << "CrearHijos:Largo de rawMovementStates:"
				 << rawMovementStates.size() << "\n";
		}
		// FIN DEBUG---

		// Chequeo anti-choque con muros y con cuello
		vector<json> validMovementStates = {};
		for (int i = 0; i < rawMovementStates.size(); i++) {
			json currState = rawMovementStates[i];

			// DEBUG---
			if (debug && debugCrearHijos) {
				cout << "CrearHijos. i=" << i << " "
					 << "Head: " << currState["you"]["head"] << endl;
			}
			// FIN DEBUG---

			if (chequearMuro(currState) && chequearColisionCuello(currState)) {
				validMovementStates.push_back(currState);
			}
		}

		// Creacion hijos
		for (int i = 0; i < validMovementStates.size(); i++) {
			crearHijo(validMovementStates[i]);
		}
	}

	// --------------Funciones de COMPROBACIÓN/CHEQUEO-----------------
	// Chequeo de muros
	bool chequearMuro(json tablero) {
		int totalSerpientes = tablero["board"]["snakes"].size();
		int stageWidth = (int)tablero["board"]["width"];
		int stageHeight = (int)tablero["board"]["height"];

		// DEBUG----
		if (debug && debugChequearMuro) {
			cout << "CHEQUEAR MURO con HEAD en: " << tablero["you"]["head"]
				 << endl;
		}
		//----FIN DEBUG-----

		for (int i = 0; i < totalSerpientes; i++) {
			json currSnake = tablero["board"]["snakes"][i];
			if ((int)currSnake["head"]["x"] > stageWidth - 1) {
				return false;
			} else if ((int)currSnake["head"]["y"] > stageHeight - 1) {
				return false;
			} else if ((int)currSnake["head"]["x"] < 0) {
				return false;
			} else if ((int)currSnake["head"]["y"] < 0) {
				return false;
			}
		}
		// DEBUG----
		if (debug && debugChequearMuro) {
			cout << "Retornando TRUE con HEAD en: " << tablero["you"]["head"]
				 << endl;
		}
		//----FIN DEBUG-----

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

			if ((int)currCabeza["x"] == (int)currCuello["x"] &&
				(int)currCabeza["y"] == (int)currCuello["y"]) {
				return false;
			}
		}
		return true;
	}
	//------------------FIN FUNCIONES CHEQUEO----------------------

	// Todos los movimientos brutos en un turno desde el tablero actual,
	// recursivamente
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

			//---DEBUG
			if (debug && debug_TotalSnakeMovementsRaw) {
				cout << "currMovs.size(): " << currMovs.size() << endl;
				cout << "currMovs Movs1head1: " << currMovs[0]["you"]["head"]
					 << endl;
				cout << "currMovs Movs2head1: " << currMovs[1]["you"]["head"]
					 << endl;
			}
			//--FINDEBUG

			return currMovs;
		} else { /*ES hoja*/
			vector<json> myMoves = {};
			myMoves.push_back(movs1);
			myMoves.push_back(movs2);
			myMoves.push_back(movs3);
			myMoves.push_back(movs4);
			currMovs.insert(currMovs.end(), myMoves.begin(), myMoves.end());

			//---DEBUG
			if (debug && debug_TotalSnakeMovementsRaw) {
				cout << "mymoves.size(): " << myMoves.size() << endl;
				cout << "mymoves head1: " << myMoves[0]["you"]["head"] << endl;
				cout << "mymoves head2: " << myMoves[1]["you"]["head"] << endl;
				cout << "mymoves head3: " << myMoves[2]["you"]["head"] << endl;
				cout << "mymoves head4: " << myMoves[3]["you"]["head"] << endl;
			}
            //---FINDEBUG

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

	// GETTER: Hijos
	vector<Node> getHijos() {
		return hijos;
	}

	// GETTER: tablero
	json getTablero() {
		return tablero;
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
	// debug
	bool debugNextMove = true;

public:
	MonteCarloSearchTree(json _tablero, int _maxDepth)
		: tableroInicial(_tablero),
		  maxDepth(_maxDepth),
		  rootNode(Node(tableroInicial)) {
	}

	// ******FUNCION FUERTEMENTE SIMPLIFICADA USANDO HEURISTICA SIMPLE******
	string nextMove() {
		float mejorValor = -99999;
		Node currBestNode = Node(tableroInicial);

		// Creacion de hijos de rootNode (1er nivel de movimientos)
		rootNode.crearHijos();

		// Iteracion sobre nodos (*** TEMPORAL. DEBE REEMPLAZARSE POR
		// EXPLORACION DE ARBOL***)

		// DEBUG--------
		if (debugNextMove) {
			cout << "nextMove:Entrando a returnSimpleHeuristicValue..." << endl;
		}
		// FIN DEBUG-----

		vector<Node> nodosARevisar = rootNode.getHijos();
		for (int i = 0; i < nodosARevisar.size(); i++) {
			Node currNodo = nodosARevisar[i];
			float currValor = returnSimpleHeuristicValue(currNodo);
			if (currValor > mejorValor) {
				currBestNode = currNodo;
			}
		}

		// DEBUG--------
		if (debugNextMove) {
			cout << "nextMove:SALIENDO de returnSimpleHeuristicValue..."
				 << endl;
			cout << "nextMove: " << currBestNode.getTablero()["you"]["body"]
				 << endl;
		}
		// FIN DEBUG-----

		// Movimiento correspondiente a nodo elegido?
		string stringMovimiento = "";
		int diffX = (int)currBestNode.getTablero()["you"]["head"]["x"] -
			(int)tableroInicial["you"]["head"]["x"];
		int diffY = (int)currBestNode.getTablero()["you"]["head"]["y"] -
			(int)tableroInicial["you"]["head"]["y"];

		if (diffX == 0 && diffY == 1) {
			stringMovimiento = "up";
		} else if (diffX == 0 && diffY == -1) {
			stringMovimiento = "down";
		} else if (diffX == 1 && diffY == 0) {
			stringMovimiento = "right";
		} else if (diffX == -1 && diffY == 0) {
			stringMovimiento = "left";
		} else {
			cout << "SE PRODUJO UN ERROR. DIFF INVALIDA ENTRE NODO INICIAL Y "
					"MEJOR NODO."
				 << endl;
		}

		return stringMovimiento;
	}

	// Retorna valor del nodo segun heuristica simple basada en vida y
	// "distancia euclidiana a comida mas cercana VS oponente mas cercano"
	float returnSimpleHeuristicValue(Node node) {
		int totalSerpientes = node.getTablero()["board"]["snakes"].size();
		int mySnakeHeadX = node.getTablero()["you"]["head"]["x"];
		int mySnakeHeadY = node.getTablero()["you"]["head"]["y"];

		// Vida actual
		int currHealth = node.getTablero()["you"]["health"];

		// Oponente + cercano
		float shortestDistanceSquared = 9999999;
		string closestSnake = {};

		for (int i = 0; i < totalSerpientes; i++) {
			json currSnake = node.getTablero()["board"]["snakes"][i];
			int currSnakeHeadX = currSnake["head"]["x"];
			int currSnakeHeadY = currSnake["head"]["y"];

			if (currSnake["id"] !=
				node.getTablero()["you"]["id"]) // No debo ser yo
			{
				float distanceSquared =
					(float)(pow(mySnakeHeadX - currSnakeHeadX, 2) + pow(mySnakeHeadY - currSnakeHeadY, 2));
				if (distanceSquared < shortestDistanceSquared) {
					shortestDistanceSquared = distanceSquared;
					closestSnake = currSnake;
				}
			}
		}

		// Comida + cercana
		int totalComidas = node.getTablero()["board"]["food"].size();
		float shortestDistanceSquaredFood = 9999999;
		json closestFood = {};

		for (int i = 0; i < totalComidas; i++) {
			json currFood = node.getTablero()["board"]["food"][i];
			int currFoodX = currFood["x"];
			int currFoodY = currFood["y"];

			float distanceSquared =
				(float)(pow(mySnakeHeadX - currFoodX, 2) + pow(mySnakeHeadY - currFoodY, 2));
			if (distanceSquared < shortestDistanceSquaredFood) {
				shortestDistanceSquaredFood = distanceSquared;
				closestFood = currFood;
			}
		}

		// Está el enemigo mas cerca de la comida que yo?
		float competitiveDistanceFactor = 0;

		// Cuadrado distancia enemigo-comida
		json enemySnake = closestSnake;
		float enemyFoodDistanceSquared =
			(float)pow(
				(int)enemySnake["head"]["x"] - (int)closestFood["x"], 2) +
			(float)pow((int)enemySnake["head"]["y"] - (int)closestFood["y"], 2);
		// Estoy mas cerca de la comida que mi enemigo = menos relevante, y
		// vice-versa
		if (shortestDistanceSquaredFood < enemyFoodDistanceSquared) {
			competitiveDistanceFactor = 0;
		} else {
			competitiveDistanceFactor = 1;
		}

		// Normalizacion entre 0 y 1
		const int MAX_HEALTH = 100;
		float normalizedHealth = (float)currHealth / (float)MAX_HEALTH;
		const int MAX_DISTANCE_SQUARED =
			pow((int)node.getTablero()["board"]["width"] - 1, 2) +
			pow((int)node.getTablero()["board"]["height"] - 1,
				2); // "-1" para dist. max. real posible en Battlesnake
		float normalizedDistanceEnemy =
			shortestDistanceSquared / MAX_DISTANCE_SQUARED;
		float normalizedDistanceFood =
			shortestDistanceSquaredFood / MAX_DISTANCE_SQUARED;
		// Weights
		float A = 1, B = 1, C = 1, D = 1;
		// Valor resultante normalizado
		float effectiveNormalizedNodeValue =
			(A * normalizedHealth + B * normalizedDistanceEnemy +
			 C * normalizedDistanceFood + D * competitiveDistanceFactor) /
			4;

		return effectiveNormalizedNodeValue;
	}

	// DEBUGGING
	void debug() {
		rootNode.debugNodoInfo();
	}
};