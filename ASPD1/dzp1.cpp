#include <iostream>

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
		if(i >= count || j >= count)
			throw exception("Node does not exsist!");

		matrix[i][j] = matrix[j][i] = 1;
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

		matrix = new int * [capacity];
		for (int i = 0; i < capacity; i++)
			matrix[i] = new int[capacity]{ };
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

int main()
{
	bool exit = false;

	Graph<int>* graph = nullptr;
	while (!exit)
	{
		cout << "Enter your choice: " << endl
			<< "1. Create Graph" << endl
			<< "2. Add Node" << endl
			<< "3. Remove Node" << endl
			<< "4. Add Edge" << endl
			<< "5. Remove Edge" << endl
			<< "6. Output Graph" << endl
			<< "7. Delete Graph" << endl
			<< "8. Exit" << endl;

		int choice;
		cin >> choice;
		
		switch (choice)
		{
		case 1:
			if (graph == nullptr)
			{
				int n;
				cout << "Enter number of nodes: ";
				cin >> n;
				try
				{
					graph = new Graph<int>(n);
				}
				catch (exception & e)
				{
					cout << e.what() << endl;
				}
			}
			else
				cout << "Graph already exsists!" << endl;
			break;
		case 2:
			if (graph != nullptr)
			{
				int nodeValue;
				cout << "Enter node value: ";
				cin >> nodeValue;
				try
				{
					graph->InsertNode(nodeValue);
				}
				catch (exception & e)
				{
					cout << e.what() << endl;
				}
			}
			else
				cout << "Graph doesnt exsist!" << endl;
			break;
		case 3:
			if (graph != nullptr)
			{
				int node;
				cout << "Enter which node to remove: ";
				cin >> node;

				try
				{
					graph->DeleteNode(node);
				}
				catch (exception & e)
				{
					cout << e.what() << endl;
				}
			}
			else
				cout << "Graph doesnt exsist!" << endl;
			break;
		case 4:
			if (graph != nullptr)
			{
				int i, j;
				cout << "Enter which nodes to connect: ";
				cin >> i >> j;

				try
				{
					graph->AddEdge(i, j);
				}
				catch (exception & e)
				{
					cout << e.what() << endl;
				}
			}
			else
				cout << "Graph doesnt exsist!" << endl;
			break;
		case 5:
			if (graph != nullptr)
			{
				int i, j;
				cout << "Enter which nodes to disconnect: ";
				cin >> i >> j;

				try
				{
					graph->RemoveEdge(i, j);
				}
				catch (exception & e)
				{
					cout << e.what() << endl;
				}
			}
			else
				cout << "Graph doesnt exsist!" << endl;
			break;
		case 6:
			if (graph != nullptr)
				cout << *graph << endl;
			else
				cout << "Graph doesnt exsist!" << endl;
			break;
		case 7:
			if (graph != nullptr)
			{
				graph->Delete();
				graph = nullptr;
			}
			else
				cout << "Graph doesnt exsist!" << endl;
			break;
		case 8:
			cout << "Bye!" << endl;
			exit = true;
			break;
		default:
			cout << "Choice does not exsist!" << endl;
		}

		system("pause");
		system("cls");
	}

	return 0;
}