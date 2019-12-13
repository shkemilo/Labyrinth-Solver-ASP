#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

template<class T>
class Graph
{
public:
	Graph(int capacity)
	{
		if (capacity <= 0)
			throw exception("Can not create graph with non-positive capacity!");

		this->capacity = capacity;
		count = capacity;

		allocate();
	}

	Graph(const Graph& graph)
	{
		copy(graph);
	}

	Graph(Graph&& graph)
	{
		move(graph);
	}

	Graph& operator=(const Graph& graph)
	{
		if (this == &graph)
			return *this;

		destroy();
		copy(graph);

		return *this;
	}

	Graph& operator=(Graph&& graph)
	{
		if (this == &graph)
			return *this;

		destroy();
		move(graph);

		return *this;
	}

	int GetCount() const
	{
		return count;
	}

	void InsertNode(const T& node)
	{
		if (count == capacity)
			throw exception("Overflow!");

		nodes[count++] = node;
	}

	void DeleteNode(int n)
	{
		if (n >= count)
			throw exception("Node does not exsist");

		for (int i = 0; i < count; i++)
			RemoveEdge(n, i);

		for (int i = n; i < count - 1; i++)
		{
			nodes[i] = nodes[i + 1];

			for (int j = 0; j < count; j++)
			{
				matrix[j][i] = matrix[j][i + 1];
				matrix[i][j] = matrix[i + 1][j];
			}
		}

		count--;
	}

	void AddEdge(int i, int j)
	{
		if (i >= count || j >= count || i < 0 || j < 0)
			throw exception("Node does not exsist!");

		matrix[i][j] = matrix[j][i] = 1;
	}

	void AddEdge(const T& node1, const T& node2)
	{
		int i1 = GetNodeIndex(node1);
		int i2 = GetNodeIndex(node2);

		AddEdge(i1, i2);
	}

	void RemoveEdge(int i, int j)
	{
		if (i >= count || j >= count)
			throw exception("Node does not exsist!");

		matrix[i][j] = matrix[j][i] = 0;
	}

	void Delete()
	{
		destroy();
	}

	T& operator[](int index)
	{
		if (index < 0 && index >= count)
			throw exception("Index out of bounds!");

		return nodes[index];
	}

	bool AreConnected(int i, int j)
	{
		return matrix[i][j];
	}

	int GetNodeIndex(const T& node) const
	{
		for (int i = 0; i < count; i++)
			if (nodes[i] == node)
				return i;

		return -1;
	}

	friend ostream& operator<<(ostream& os, const Graph& graph)
	{
		if (graph.nodes != nullptr)
		{
			os << "Node values are: " << endl;
			for (int i = 0; i < graph.count; i++)
				os << "Node " << i << ": " << graph.nodes[i] << " ";
		}

		if (graph.matrix != nullptr)
		{
			os << endl << "Adjacency matrix: " << endl;
			for (int i = 0; i < graph.count; i++)
			{
				for (int j = 0; j < graph.count; j++)
					os << graph.matrix[i][j] << " ";
				os << endl;
			}
		}

		return os;
	}

	~Graph()
	{
		destroy();
	}

private:
	void allocate()
	{
		nodes = new T[capacity]{ };

		matrix = new int* [capacity];
		for (int i = 0; i < capacity; i++)
			matrix[i] = new int[capacity] {};
	}

	void copy(const Graph& graph)
	{
		count = graph.count;
		capacity = graph.capacity;

		allocate();
		for (int i = 0; i < capacity; i++)
		{
			nodes[i] = graph.nodes[i];

			for (int j = 0; j < capacity; j++)
				matrix[i][j] = graph.matrix[i][j];
		}
	}

	void move(Graph& graph)
	{
		count = graph.count;
		capacity = graph.capacity;

		matrix = graph.matrix;
		graph.matrix = nullptr;

		nodes = graph.nodes;
		graph.nodes = nullptr;
	}

	void destroy()
	{
		for (int i = 0; i < count; i++)
			delete[] matrix[i];

		delete[] matrix;
		matrix = nullptr;

		delete[] nodes;
		nodes = nullptr;
	}

	T* nodes = nullptr;

	int** matrix = nullptr;

	int count;
	int capacity;
};

struct Point
{
	int i;
	int j;

	Point(int i = -1, int j = -1) : i(i), j(j) { }

	friend istream& operator>>(istream& is, Point& point)
	{
		return is >> point.i >> point.j;
	}

	friend ostream& operator<<(ostream& os, const Point& point)
	{
		return os << '(' << point.i << ", " << point.j << ')';
	}

	friend bool operator==(const Point& point1, const Point& point2)
	{
		return (point1.i == point2.i) && (point1.j == point2.j);
	}
};

class Labyrinth
{
public:
	Labyrinth(int n, int m, const Point& enterance, const Point& exit) : n(n), m(m), enterance(enterance), exit(exit)
	{
		allocate();
		setField(enterance, 'o');
		setField(exit, 'x');
	}

	void AddPath(const Point& path)
	{
		if (matrix[path.i][path.j] == '*')
		{
			setField(path, ' ');
			count++;
		}
	}

	void GenerateGraph()
	{
		delete graph;

		graph = new Graph<Point>(count);
		int index = 0;
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
				if (matrix[i][j] != '*')
					(*graph)[index++] = Point(i, j);

		for (int i = 0; i < count; i++)
		{
			Point temp = (*graph)[i];

			connectPoint(temp, temp.i, temp.j);
			connectPoint(temp, temp.i + 1, temp.j);
			connectPoint(temp, temp.i - 1, temp.j);
			connectPoint(temp, temp.i, temp.j + 1);
			connectPoint(temp, temp.i, temp.j - 1);
		}
	}

	Graph<Point>& GetGraph()
	{
		return *graph;
	}

	~Labyrinth()
	{
		destroy();
	}

	friend ostream& operator<<(ostream& os, const Labyrinth& labyrinth)
	{
		for (int i = 0; i < labyrinth.n; i++)
		{
			for (int j = 0; j < labyrinth.m; j++)
				os << labyrinth.matrix[i][j] << " ";
			os << endl;
		}

		return os;
	}

private:
	void allocate()
	{
		matrix = new char* [n];
		for (int i = 0; i < n; i++)
		{
			matrix[i] = new char[m];
			for (int j = 0; j < m; j++)
				matrix[i][j] = '*';
		}
	}

	void setField(const Point& point, char marker)
	{
		matrix[point.i][point.j] = marker;
	}

	void connectPoint(const Point& point, int i, int j)
	{
		if (i < 0 || j < 0)
			return;
		if (i >= n || j >= m)
			return;
		if (matrix[i][j] == '*')
			return;

		graph->AddEdge(point, Point(i, j));
	}

	void destroy()
	{
		for (int i = 0; i < n; i++)
			delete[] matrix[i];

		delete[] matrix;
		matrix = nullptr;

		delete graph;
		graph = nullptr;
	}

	char** matrix;

	int n;
	int m;

	const Point& enterance;
	const Point& exit;

	Graph<Point>* graph = nullptr;
	int count = 2;
};

const int INF = 100000;

template<class T>
class FloydWarshall
{
public:
	FloydWarshall(Graph<T>& graph) : graph(graph)
	{
		n = graph.GetCount();
		allocate();
		copyFW();
	}

	void GenerateShortestPaths()
	{
		for (int k = 0; k < n; k++)
		{
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					if ((sol[i][k] + sol[k][j] < sol[i][j]))
					{
						sol[i][j] = sol[i][k] + sol[k][j];
						paths[i][j] = paths[k][j];
					}
				}
			}
		}
	}

	void PrintPath(int i, int j, ostream& os = cout, int count = 0) //return by array
	{
		if (i == j)
		{
			os << graph[i];
			if (count != 0)
				os << "->";
			else
				os << endl;

			return;
		}
		else
			if (paths[i][j] == -1)
			{
				os << "Path from node " << i << " to node " << j << "is impossible!";
				return;
			}
			else
			{
				PrintPath(i, paths[i][j], os, count + 1);
				os << graph[j];
				if (count != 0)
					os << "->";
				else
					os << endl;
			}
	}

	int** GetShortestPaths()
	{
		return sol;
	}

	bool IsNodeAccessible(int i, int j)
	{
		return sol[i][j] != INF;
	}

	int GetPathLength(int i, int j)
	{
		return sol[i][j];
	}

	friend ostream& operator<<(ostream& os, const FloydWarshall& fw)
	{
		os << "Adjacency matrix with shortest distances is: " << endl;
		for (int i = 0; i < fw.n; i++)
		{
			for (int j = 0; j < fw.n; j++)
				if (fw.sol[i][j] == INF)
					os << "INF" << " ";
				else
					os << setw(3) << fw.sol[i][j] << " ";
			os << endl;
		}

		return os;
	}

	~FloydWarshall()
	{
		for (int i = 0; i < n; i++)
		{
			delete[] sol[i];
			delete[] paths[i];
		}

		delete[] paths;
		paths = nullptr;
		delete[] sol;
		sol = nullptr;
	}

private:
	void allocate()
	{
		sol = new int* [n]; paths = new int* [n];
		for (int i = 0; i < n; i++)
		{
			sol[i] = new int[n];
			paths[i] = new int[n];
		}
	}

	void copyFW()
	{
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				if (i == j)
					sol[i][j] = 0;
				else if (graph.AreConnected(i, j) == false)
					sol[i][j] = INF;
				else
					sol[i][j] = graph.AreConnected(i, j);

		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				if (i == j || sol[i][j] == INF)
					paths[i][j] = -1;
				else
					paths[i][j] = i;
	}
	
	Graph<T>& graph;

	int** sol;
	int** paths;
	int n;
};

const int maxN = 80;
const int maxM = 50;

int main()
{
	Labyrinth* labyrinth = nullptr;
	FloydWarshall<Point>* floydWarshall = nullptr;
	Point enterance, exit;

	bool finish = false;
	bool fileOutput = false;

	ofstream of;
	of.open("Output.txt");
	while (!finish)
	{
		system("cls");
		cout << "Labyrinth Solver 2k19" << endl
			<< "1. Enter Labyrinth" << endl
			<< "2. Print Labyrinth" << endl
			<< "3. Print Graph" << endl
			<< "4. Generate Shortest Paths" << endl
			<< "5. Print Adjencecy Matrix of Shortest Paths" << endl
			<< "6. Find Way Out of Labyrinth" << endl
			<< "7. Delete Labyrinth" << endl
			<< "8. Exit program" << endl;

		int choice;
		cout << "Enter your choice: ";
		cin >> choice;
		system("cls");

		if (choice > 8 || choice < 1)
		{
			cout << "Invalid choice!" << endl;
			system("pause");
			continue;

		}

		if (labyrinth != nullptr && choice == 1)
		{
			cout << "Please delete the current labyrinth before entering a new one!" << endl;
			system("pause");
			continue;
		}

		if (labyrinth == nullptr && choice != 1 && choice != 8 && choice != 9)
		{
			cout << "Labyrinth is not created yet!" << endl;
			system("pause");
			continue;
		}

		switch (choice)
		{
		case 1:
			cout << "Output to standard output(0) or file(1): ";
			int outputChoice;
			cin >> outputChoice;
			if (!(outputChoice == 0 || outputChoice == 1))
			{
				cout << "Invalid choice!" << endl;
				system("pause");
				continue;
			}
			if (outputChoice == 0)
				fileOutput = false;
			else
				fileOutput = true;

			cout << "Enter dimensions of matrix: ";
			int n, m;
			cin >> n >> m;
			if ((n > maxN || m > maxM) && !fileOutput)
			{
				cout << "Labyrinth too large for Standard Ouput (max size: 80x50), try outputing to a file!" << endl;
				system("pause");
				continue;
			}


			cout << "Enter the position of the enterance and exit: ";
			cin >> enterance >> exit;
			if (!(((enterance.i == n - 1) || (enterance.j == m - 1) || (enterance.i == 0) || (enterance.j == 0))
				&& ((exit.i == n - 1) || (exit.j == m - 1) || (exit.i == 0) || (exit.j == 0))))
			{
				cout << "Enterance and/or exit are not on the edges!" << endl;
				system("pause");
				continue;
			}

			labyrinth = new Labyrinth(n, m, enterance, exit);

			while (true)
			{
				system("cls");
				if (!fileOutput)
				{
					cout << *labyrinth << endl
						<< "Labyrinth dimensions are: " << n << "x" << m << endl;
				}
				cout << "Entering invalid positions will stop the input!" << endl
					<< "Enter the positions of paths: ";

				Point temp;
				cin >> temp;
				if (temp.i < 0 || temp.i >= n || temp.j < 0 || temp.j >= m)
					break;

				labyrinth->AddPath(temp);
			}

			labyrinth->GenerateGraph();
			cout << "Labyrinth created succssesfully!" << endl;
			break;
		case 2:
			if (fileOutput)
				of << *labyrinth << endl;
			else
				cout << *labyrinth << endl;

			cout << "Output successfull!" << endl;
			break;
		case 3:
			if (fileOutput)
				of << labyrinth->GetGraph() << endl;
			else
				cout << labyrinth->GetGraph() << endl;

			cout << "Output successfull!" << endl;
			break;
		case 4:
			floydWarshall = new FloydWarshall<Point>(labyrinth->GetGraph());
			floydWarshall->GenerateShortestPaths();
			cout << "Shortest Paths generated successfully!" << endl;
			break;
		case 5:
			if (floydWarshall == nullptr)
			{
				cout << "Shortest Paths not generated yet!" << endl;
				system("pause");
				continue;
			}

			if (fileOutput)
				of << *floydWarshall << endl;
			else
				cout << *floydWarshall << endl;

			cout << "Output successfull!" << endl;
			break;
		case 6:
			if (floydWarshall == nullptr)
			{
				cout << "Shortest Paths not generated yet!" << endl;
				system("pause");
				continue;
			}
			{
				int enteranceIndex = labyrinth->GetGraph().GetNodeIndex(enterance);
				int exitIndex = labyrinth->GetGraph().GetNodeIndex(exit);
				if (floydWarshall->IsNodeAccessible(enteranceIndex, exitIndex))
				{
					if (fileOutput)
					{
						of << "Exit is accessible from the Enterance!" << endl;
						of << "The Shortest Path from the Enterance to the Exit is: " << floydWarshall->GetPathLength(enteranceIndex, exitIndex) << endl;
						of << "Path from the Enterance to the Exit is: " << endl;
						floydWarshall->PrintPath(enteranceIndex, exitIndex, of);
					}
					else
					{
						cout << "Exit is accessible from the Enterance!" << endl;
						cout << "The Shortest Path from the Enterance to the Exit is: " << floydWarshall->GetPathLength(enteranceIndex, exitIndex) << endl;
						cout << "Path from the Enterance to the Exit is: " << endl;
						floydWarshall->PrintPath(enteranceIndex, exitIndex);
					}
				}
				else
					if (fileOutput)
						of << "The Exit is not accessible from the Enterance!" << endl;
					else
						cout << "The Exit is not accessible from the Enterance!" << endl;

				cout << "Output successfull!" << endl;
			}
			break;
		case 7:
			delete floydWarshall;
			floydWarshall = nullptr;

			delete labyrinth;
			labyrinth = nullptr;
			cout << "Labyrinth deleted succssesfully!" << endl;
			break;
		case 8:
			cout << "Bye!" << endl;
			finish = true;
			break;
		}
		system("pause");
	}

	delete floydWarshall;
	delete labyrinth;
	of.close();
	return 0;
}