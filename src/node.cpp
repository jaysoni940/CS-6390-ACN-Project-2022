/*  
 *  Simulates an actual node inside a network with capabilities to 
 *  send and receive data packets via files.
 *  Copyright (C) 2021 Sourabh J Choure
 * 
 *  I promise that the work presented below is of my own.
 * 
 *  To make it FOSS Compliant, this software is free to use.
 */

// STL
#include <iostream>
#include <fstream>
#include <string>
// SL
#include <cstdlib>
// Unix
#include <unistd.h>

using namespace std;

#define TOTAL_NODES 10

struct FileStructure
{
    // Store the name of the Files
    string input_file_name;
    string output_file_name;
    string receivedFileName;

    // File Desciptors
    fstream input;
    ofstream output;
    ofstream receivedData;
};

struct Queue
{
    // Constructor of the Queue
    Queue(int capacity) : capacity(capacity), pointer_to_array(new int[capacity]), queue_first(0), queue_last(0), no_of_elements(0){};

    // Store the total capacity of the Queue
    int capacity;

    // Pointer to array
    int *pointer_to_array;

    // Index to the front of the Queue
    int queue_first;

    // Index to the rear of the Queue
    int queue_last;

    // Total Number of elements in the Queue
    int no_of_elements;

    // Put the elements in the Queue
    void enqueue(int);

    // Remove the elements from the Queue
    int dequeue();

    // Check if the Queue is empty
    bool empty();
};

void Queue::enqueue(int val)
{
    if (no_of_elements == capacity)
    {
        // Queue is Full
    }

    // Put the val
    pointer_to_array[queue_last] = val;

    // Increment the rear index
    queue_last = (queue_last + 1) % capacity;

    // Increment the total number of elements
    no_of_elements++;
}

int Queue::dequeue()
{
    if (no_of_elements == 0)
    {
        // Queue is Empty
    }

    // Remove the element
    int temp = pointer_to_array[queue_first];

    // Increment the front index
    queue_first = (queue_first + 1) % capacity;

    // Decrement the number of elements
    no_of_elements--;

    // Return the element
    return temp;
}

bool Queue::empty()
{
    return (no_of_elements == 0) ? true : false;
}

struct nodeLevel
{
    int level = -1;
    int destination;
};

struct Routing
{
    Routing(int destination, string data_value) : destination(destination), data_value(data_value)
    {
        for (size_t i = 0; i < TOTAL_NODES; i++)
        {
            incoming_neighbors[i] = 0;
            incoming_neighbors_path[i] = "";

            for (size_t j = 0; j < TOTAL_NODES; j++)
            {
                in_tree[i][j] = 0;
                previous_in_tree[i][j] = 0;
                send_data_to_neighbors[i][j] = "";
            }
        }
    };

    // Destination Node
    int destination;

    // Path to Destination
    string destination_path = "";

    // Buffer for the data to be sent
    string data_value;

    // Buffer for passing message to Neighbor
    string send_data_to_neighbors[TOTAL_NODES][TOTAL_NODES];

    // Keep track of Incoming Neighbors
    int incoming_neighbors[TOTAL_NODES];

    // In-tree of a Node
    int in_tree[TOTAL_NODES][TOTAL_NODES];

    // Previous In-tree of a Node
    int previous_in_tree[TOTAL_NODES][TOTAL_NODES];

    // Check if the Intree changed
    bool send_in_tree_current = false;

    // Store the path to the neighbor
    string incoming_neighbors_path[TOTAL_NODES];

    // Check if incoming Neighbors is empty
    bool isINempty();

    // Find the path to the Incoming Neighbor
    void storeIncomingNeighborPath(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]);

    // buildSPT
    void buildSPT(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]);

    // Common Function
    void extendedBFSt(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]));

    void extendedBFSt(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES]));

    // Common Function
    void extendedBFSi(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]));

    void extendedBFSi(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES]));

    // Common Function Helper: Remove TmpTree
    void removeTmpTreePath(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]);

    // Common Function Helper: Remove InTree
    void removeInTreePath(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]);

    // Common Function Helper: pruneNode
    void pruneNode(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]);

    // Common Function Helper: add levels
    void addLevel(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES]);

    // Common Function Helper: remove levels
    void removeLevel(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES]);
};

bool Routing::isINempty()
{
    for (size_t i = 0; i < TOTAL_NODES; i++)
    {
        if (incoming_neighbors[i])
            return false;
    }

    return true;
}

void Routing::storeIncomingNeighborPath(size_t v, size_t rootedAt, int (&tempIntree)[TOTAL_NODES][TOTAL_NODES])
{
    // Add Node v to the path
    incoming_neighbors_path[rootedAt] = incoming_neighbors_path[rootedAt] + to_string(v) + " ";

    // Traverse the Graph
    for (size_t w = 0; w < TOTAL_NODES; w++)
    {
        if (tempIntree[v][w])
        {
            storeIncomingNeighborPath(w, rootedAt, tempIntree);
        }
    }
}

void Routing::removeTmpTreePath(size_t w, size_t v, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES])
{
    tmpIntree[w][v] = 0;
}

void Routing::removeInTreePath(size_t w, size_t v, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES])
{
    in_tree[w][v] = 0;
}

void Routing::pruneNode(size_t w, size_t v, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES])
{
    if (!tmpIntree[w][v])
    {
        in_tree[w][v] = 0;
    }
}

void Routing::addLevel(size_t w, size_t v, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&levels)[TOTAL_NODES])
{
    levels[w].level = levels[v].level + 1;
    levels[w].destination = v;
}

void Routing::removeLevel(size_t w, size_t v, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&levels)[TOTAL_NODES])
{
    tmpIntree[w][v] = 0;
    levels[w].level = -1;
    levels[w].destination = -1;
}

void Routing::extendedBFSt(size_t node_id, size_t rootedAt, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]))
{
    // Queue to traverse
    Queue qGraph(TOTAL_NODES);

    // Record for visited Nodes
    bool visNodes[TOTAL_NODES] = {false};

    // Enqueue the root
    qGraph.enqueue(node_id);

    // Mark the root as visited
    visNodes[node_id] = true;

    // Traverse till Queue is empty
    while (!qGraph.empty())
    {
        // Remove the element from the Queue
        int v = qGraph.dequeue();

        // Scan through all the nodes in the graph
        for (int w = 0; w < TOTAL_NODES; w++)
        {
            if (tmpIntree[w][v])
            {
                if (!visNodes[w])
                {
                    visNodes[w] = 1;
                    (this->*func)(w, v, tmpIntree);
                    qGraph.enqueue(w);
                }
            }
        }
    }
}

void Routing::extendedBFSt(size_t node_id, size_t rootedAt, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&levels)[TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES]))
{
    // Mark the levels as zero
    levels[node_id].level = 0;
    levels[node_id].destination = -1;

    // Queue to traverse
    Queue qGraph(TOTAL_NODES);

    // Record for visited Nodes
    bool visNodes[TOTAL_NODES] = {false};

    // Enqueue the root
    qGraph.enqueue(node_id);

    // Mark the root as visited
    visNodes[node_id] = true;

    // Traverse till Queue is empty
    while (!qGraph.empty())
    {
        // Remove the element from the Queue
        int v = qGraph.dequeue();

        // Scan through all the nodes in the graph
        for (int w = 0; w < TOTAL_NODES; w++)
        {
            if (tmpIntree[w][v])
            {
                if (!visNodes[w])
                {
                    visNodes[w] = 1;
                    (this->*func)(w, v, tmpIntree, levels);
                    qGraph.enqueue(w);
                }
            }
        }
    }
}

void Routing::extendedBFSi(size_t node_id, size_t rootedAt, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES]))
{
    // Queue to traverse
    Queue qGraph(TOTAL_NODES);

    // Record for visited Nodes
    bool visNodes[TOTAL_NODES] = {false};

    // Enqueue the root
    qGraph.enqueue(rootedAt);

    // Mark the root as visited
    visNodes[rootedAt] = true;

    // Traverse till Queue is empty
    while (!qGraph.empty())
    {
        // Remove the element from the Queue
        int v = qGraph.dequeue();

        // Scan through all the nodes in the graph
        for (int w = 0; w < TOTAL_NODES; w++)
        {
            if (in_tree[w][v])
            {
                if (!visNodes[w])
                {
                    visNodes[w] = 1;
                    (this->*func)(w, v, tmpIntree);
                    qGraph.enqueue(w);
                }
            }
        }
    }
}

void Routing::extendedBFSi(size_t node_id, size_t rootedAt, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&levels)[TOTAL_NODES], void (Routing::*func)(size_t, size_t, int (&)[TOTAL_NODES][TOTAL_NODES], nodeLevel (&)[TOTAL_NODES]))
{
    // Mark the levels as zero
    levels[rootedAt].level = 0;
    levels[rootedAt].destination = -1;

    // Queue to traverse
    Queue qGraph(TOTAL_NODES);

    // Record for visited Nodes
    bool visNodes[TOTAL_NODES] = {false};

    // Enqueue the root
    qGraph.enqueue(rootedAt);

    // Mark the root as visited
    visNodes[rootedAt] = true;

    // Traverse till Queue is empty
    while (!qGraph.empty())
    {
        // Remove the element from the Queue
        int v = qGraph.dequeue();

        // Scan through all the nodes in the graph
        for (int w = 0; w < TOTAL_NODES; w++)
        {
            if (in_tree[w][v])
            {
                if (!visNodes[w])
                {
                    visNodes[w] = 1;
                    (this->*func)(w, v, tmpIntree, levels);
                    qGraph.enqueue(w);
                }
            }
        }
    }
}

void Routing::buildSPT(size_t node_id, size_t rootedAt, int (&tmpIntree)[TOTAL_NODES][TOTAL_NODES])
{
    // Store the Previous Intree
    for(size_t i = 0; i < TOTAL_NODES; i++)
    {
        for(size_t j = 0; j < TOTAL_NODES; j++)
        {
            previous_in_tree[i][j] = in_tree[i][j];
        }
    }

    // Modify the intree of the incoming neighbor
    for (size_t i = 0; i < TOTAL_NODES; i++)
    {
        tmpIntree[node_id][i] = 0;
    }

    extendedBFSt(node_id, rootedAt, tmpIntree, &Routing::removeTmpTreePath);

    tmpIntree[rootedAt][node_id] = 1;

    // Prune the dead nodes from the in_tree by comparing it with the last tempintree
    extendedBFSi(node_id, rootedAt, tmpIntree, &Routing::pruneNode);

    // Mark all the nodes as unvisited at the start
    nodeLevel levelCur[TOTAL_NODES];
    nodeLevel levelTmp[TOTAL_NODES];

    extendedBFSi(rootedAt, node_id, tmpIntree, levelCur, &Routing::addLevel);

    extendedBFSt(node_id, rootedAt, tmpIntree, levelTmp, &Routing::addLevel);

    int mergeTree[TOTAL_NODES][TOTAL_NODES] = {0};

    // Merge the levels
    for (int hop = 1; hop < TOTAL_NODES; hop++)
    {
        int count = TOTAL_NODES;
        while (count--)
        {
            int cmpLvl = -1;
            int cmpTmp = -1;

            for (size_t curLvl = 0; curLvl < TOTAL_NODES; curLvl++)
            {
                if (levelCur[curLvl].level == hop)
                {
                    cmpLvl = curLvl;
                    break;
                }
            }

            for (size_t tmpLvl = 0; tmpLvl < TOTAL_NODES; tmpLvl++)
            {
                if (levelTmp[tmpLvl].level == hop)
                {
                    cmpTmp = tmpLvl;
                    break;
                }
            }

            if (cmpLvl == -1 && cmpTmp == -1)
            {
                break;
            }

            else if ((cmpLvl != -1 && cmpTmp == -1) || (cmpLvl != -1 && cmpTmp != -1 && cmpLvl < cmpTmp))
            {
                int destination = levelCur[cmpLvl].destination;

                if (levelTmp[cmpLvl].level == -1)
                {
                    mergeTree[cmpLvl][destination] = 1;
                }
                else
                {
                    mergeTree[cmpLvl][destination] = 1;

                    // Modify the intree of the incoming neighbor
                    tmpIntree[cmpLvl][levelTmp[cmpLvl].destination] = 0;

                    extendedBFSt(cmpLvl, rootedAt, tmpIntree, levelTmp, &Routing::removeLevel);
                }

                levelCur[cmpLvl].level = -1;
                levelCur[cmpLvl].destination = -1;

                levelTmp[cmpLvl].level = -1;
                levelTmp[cmpLvl].destination = -1;
            }

            else if ((cmpLvl == -1 && cmpTmp != -1) || (cmpLvl != -1 && cmpTmp != -1 && cmpLvl > cmpTmp))
            {
                int destination = levelTmp[cmpTmp].destination;

                if (levelCur[cmpTmp].level == -1)
                {
                    mergeTree[cmpTmp][destination] = 1;
                }
                else
                {
                    mergeTree[cmpTmp][destination] = 1;

                    // Modify the intree of the myself
                    in_tree[cmpTmp][levelCur[cmpTmp].destination] = 0;

                    extendedBFSi(node_id, cmpTmp, tmpIntree, levelCur, &Routing::removeLevel);
                }

                levelTmp[cmpTmp].level = -1;
                levelTmp[cmpTmp].destination = -1;

                levelCur[cmpTmp].level = -1;
                levelCur[cmpTmp].destination = -1;
            }

            else if (cmpLvl != -1 && cmpTmp != -1 && cmpLvl == cmpTmp)
            {
                int destination = levelCur[cmpLvl].destination;

                mergeTree[cmpLvl][destination] = 1;

                levelCur[cmpLvl].level = -1;
                levelCur[cmpLvl].destination = -1;

                levelTmp[cmpTmp].level = -1;
                levelTmp[cmpTmp].destination = -1;
            }
        }
    }

    // copy to intree
    for (size_t i = 0; i < TOTAL_NODES; i++)
        for (size_t j = 0; j < TOTAL_NODES; j++)
            in_tree[i][j] = mergeTree[i][j];

    // Check if the in_tree changed to push it immediately
    for(size_t i = 0; i < TOTAL_NODES; i++)
    {
        for(size_t j = 0; j < TOTAL_NODES; j++)
        {
            if(previous_in_tree[i][j] != in_tree[i][j])
            {
                send_in_tree_current = true;
            }
        }
    }
}

class Node
{
public:
    Node(size_t node_id, size_t time_interval, int destination, string data_value) : node_id(node_id), time_interval(time_interval), msg(destination, data_value)
    {
        setStreams();
    };
    ~Node();

    // ID of the node
    size_t node_id;

    // Duration
    size_t time_interval;

    // Hello Message Sender
    void helloProtocol();

    // Intree Protocol
    void intreeProtocol();

    // Data Protocol
    void dataProtocol();

    // Process Input File
    void processInputFile();

private:
    // Keep record of who sent the in_tree message
    bool gotIntree[TOTAL_NODES] = {0};

    // Channels of the Node
    FileStructure stream;

    // Routing Data Structure
    Routing msg;

    // init the streams
    void setStreams();

    // read the file contents line by line_number
    string readFile(fstream &);

    // Return the path to the destination
    void findPathToDest(int, string &);

    // Compute the Hello Messages
    void computeHello(string &);

    // Compute the intree Messages
    void computeIntree(string &);

    // Compute the Data Messages
    void computeData(string &);
};

Node::~Node()
{
    // Close the streams
    stream.input.close();
    stream.output.close();
    stream.receivedData.close();
}

void Node::setStreams()
{
    stream.input_file_name = string("input_") + char('0' + node_id);
    stream.output_file_name = string("output_") + char('0' + node_id);
    stream.receivedFileName = char('0' + node_id) + string("_received");

    stream.input.open(stream.input_file_name.c_str(), ios::out);
    stream.input.close();

    stream.input.open(stream.input_file_name.c_str(), ios::in);
    stream.output.open(stream.output_file_name.c_str(), ios::out | ios::app);
    stream.receivedData.open(stream.receivedFileName.c_str(), ios::out | ios::app);

    if (stream.input.fail())
    {
        cout << "Node " << node_id << ": No input file" << endl;
        exit(1);
    }
    if (stream.output.fail())
    {
        cout << "Node " << node_id << ": No output file" << endl;
        exit(1);
    }
    if (stream.receivedData.fail())
    {
        cout << "Node " << node_id << ": No receivedData file" << endl;
        exit(1);
    }
}

string Node::readFile(fstream &fd)
{
    string line_number = "";
    getline(fd, line_number);
    if (fd.eof())
    {
        fd.clear();
        line_number = "";
    }

    return line_number;
}

void Node::helloProtocol()
{
    // Send the Hello Message on the Output file for the controller to read
    stream.output << "Hello " << node_id << endl;
    stream.output.flush();
}

void Node::intreeProtocol()
{
    // Check the status of incoming Neighbors
    if (msg.isINempty())
    {
        stream.output << "Intree " << node_id << endl;
        stream.output.flush();
        return;
    }

    // Create a buffer for the message to send
    string buffer = "Intree " + to_string(node_id) + " ";

    // Traverse the Intree
    Queue qCurNode(TOTAL_NODES);

    // Visit Node
    bool visCur[TOTAL_NODES] = {false};

    // Enqueue the Node
    qCurNode.enqueue(node_id);

    // Mark it visited
    visCur[node_id] = true;

    // Check if a Current Node Queue is empty
    while (!qCurNode.empty())
    {
        // Remove the element from the Queue
        int v = qCurNode.dequeue();

        // Scan through all the nodes in the graph
        for (int w = 0; w < TOTAL_NODES; w++)
        {
            if (msg.in_tree[w][v])
            {
                if (!visCur[w])
                {
                    visCur[w] = 1;
                    qCurNode.enqueue(w);
                    buffer = buffer + "(" + to_string(w) + " " + to_string(v) + ")";
                }
            }
        }
    }

    // write to the file
    stream.output << buffer << endl;
    stream.output.flush();
}

void Node::findPathToDest(int v, string &path)
{
    // Store the path
    path = path + to_string(v) + " ";

    for (size_t w = 0; w < TOTAL_NODES; w++)
    {
        if (msg.in_tree[v][w])
        {
            findPathToDest(w, path);
        }
    }
}

void Node::dataProtocol()
{
    // Send the Data Message if the destination is not -1
    if (msg.destination != -1)
    {
        // Clear the old path
        msg.destination_path = "";

        // Find the new path
        findPathToDest(msg.destination, msg.destination_path);

        // Check if string was empty or not
        string tempCheck = to_string(msg.destination) + " ";
        if (msg.destination_path == tempCheck)
        {
            msg.destination_path = "";

            //exit
            return;
        }

        // Find the Incoming Neighbor
        size_t len = msg.destination_path.length();
        char in = msg.destination_path[len - 4];

        if (msg.incoming_neighbors_path[in - '0'] == "")
            return;

        string path = msg.incoming_neighbors_path[in - '0'];

        path.erase(0, 2);

        // Send the data to the Incoming Neighbor
        stream.output << "Data " << node_id << " " << msg.destination << " " << path << "begin " << msg.data_value << endl;
        stream.output.flush();
    }
}

void Node::computeHello(string &line_number)
{
    // Read the Input file to check for the message
    // and then update the incoming neighbors

    //Store the node number in char form
    char c = line_number[6];

    // Update the Incoming Neighbors
    msg.incoming_neighbors[c - '0'] = 1;
}

void Node::computeIntree(string &line_number)
{

    // Read the input file
    // Update the Intree Graph
    // Make the Intree Graph with the help of the Intree message and Incoming neighbors
    // Parse the Intree Message
    // Calculate the total length of the message
    size_t len = line_number.length();

    // Find who sent this message
    char c = line_number[7];
    // Let's convert it to int;
    int rootedAt = c - '0';
    // Store in the who sent Intree
    gotIntree[rootedAt] = true;

    // Calculate the length of body part of the message
    size_t body;
    if (len == 8)
    {
        // No body to calculate
        body = 0;
    }
    else
    {
        // Size of the header plus extra the space, starts from '(' and ends at ')'
        body = len - 9;
    }

    // Create a temporary Intree Graph of the received Intree message
    int tmpIntree[TOTAL_NODES][TOTAL_NODES] = {{0}};

    // Extract the node numbers from the message
    for (size_t i = 0; i < body; i += 5)
    {
        char queue_last = line_number[10 + i];
        char c = line_number[10 + i + 2];

        // Place a directed edge here
        tmpIntree[queue_last - '0'][c - '0'] = 1;
    }

    //Refresh the Contents in Path To Incoming Neighbor
    msg.incoming_neighbors_path[rootedAt] = "";
    // Find the path to the Incoming Neighbor
    msg.storeIncomingNeighborPath(node_id, rootedAt, tmpIntree);
    // Check if string was empty or not
    string tmpCheck = to_string(node_id) + " ";
    if (msg.incoming_neighbors_path[rootedAt] == tmpCheck)
        msg.incoming_neighbors_path[rootedAt] = "";

    // Merge the two trees
    msg.buildSPT(node_id, rootedAt, tmpIntree);
}

void Node::computeData(string &line_number)
{
    // Parse the input file
    // Extract the Intermediate node
    char dataInterDest = line_number[9];

    // Check if it is destined to me
    if (unsigned(dataInterDest - '0') != node_id)
        return;

    // Extract the Destination Node
    char dataDest = line_number[7];

    // Extract the Source Node
    char dataSrc = line_number[5];

    if (unsigned(dataDest - '0') == node_id && line_number[11] == 'b')
    {
        // Extract the data Message
        string message = line_number.erase(0, 17);

        // Add the data to the received file
        stream.receivedData << "Message from " << dataSrc << " to " << dataDest << " : " << message << endl;
    }
    else
    {
        if (line_number[11] == 'b')
        {

            // Extract the data Message
            string message = line_number.erase(0, 17);

            //Pass to Neighbor
            string intermediateNode = "";

            // Find the new path
            findPathToDest((dataDest - '0'), intermediateNode);

            // Check if string was empty or not
            string tempCheck = dataDest + " ";
            if (intermediateNode == tempCheck)
            {
                intermediateNode = "";
                return;
            }

            // Find the Incoming Neighbor
            size_t len = intermediateNode.length();
            char in = intermediateNode[len - 4];

            if (msg.incoming_neighbors_path[in - '0'] == "")
                return;

            string path = msg.incoming_neighbors_path[in - '0'];

            path.erase(0, 2);

            for (size_t j = 0; j < TOTAL_NODES; j++)
            {
                if (msg.send_data_to_neighbors[dataSrc - '0'][j] == "")
                {
                    msg.send_data_to_neighbors[dataSrc - '0'][j] = "Data " + to_string((dataSrc - '0')) + " " + to_string((dataDest - '0')) + " " + path + "begin " + message;
                    break;
                }
            }
        }
        else
        {
            // Remove myself from the intermediate nodes
            line_number.erase(line_number.begin() + 9);
            line_number.erase(line_number.begin() + 9);

            for (size_t j = 0; j < TOTAL_NODES; j++)
            {
                if (msg.send_data_to_neighbors[dataSrc - '0'][j] == "")
                {
                    msg.send_data_to_neighbors[dataSrc - '0'][j] = line_number;
                    break;
                }
            }
        }
    }
}

void Node::processInputFile()
{
    static size_t timer = 0;
    string line_number = "";
    while ((line_number = readFile(stream.input)) != "")
    {
        // Check for Hello Message
        if (line_number[0] == 'H')
            computeHello(line_number);

        // Check for Intree Message
        if (line_number[0] == 'I')
            computeIntree(line_number);

        // Check for Data Message
        if (line_number[0] == 'D')
            computeData(line_number);
    }

    if (timer > 0 && ((timer - 2) % 10) == 0)
    {
        // Keep with the neighbors who sent the Intree
        for (size_t i = 0; i < TOTAL_NODES; i++)
        {
            if (msg.incoming_neighbors[i] == 1 && gotIntree[i] == false)
            {
                cout << "Node " << node_id << ": oh no! Node " << i << " got killed! Time to adapt my peers!" << endl;

                // Modify the intree of the Node
                msg.in_tree[i][node_id] = 0;

                // Remove the subtree
                msg.extendedBFSi(node_id, i, msg.in_tree, &Routing::removeInTreePath);

                // Remove it from the Incoming Neighbor
                msg.incoming_neighbors[i] = 0;

                // Push the intree message Immediately
                msg.send_in_tree_current = true;
            }
            else if (msg.incoming_neighbors[i] == 0 && gotIntree[i] == true)
            {
                // Add it to the incoming Neighbors
                msg.incoming_neighbors[i] = 1;
            }

            gotIntree[i] = false;
        }
    }

    // Push the In-tree Immediately
    if(msg.send_in_tree_current)
    {
        intreeProtocol();
        msg.send_in_tree_current = false;
    }

    // Pass the Data Message to the Neighbor
    for (size_t i = 0; i < TOTAL_NODES; i++)
    {
        for (size_t j = 0; j < TOTAL_NODES; j++)
        {
            if (msg.send_data_to_neighbors[i][j] != "")
            {
                stream.output << msg.send_data_to_neighbors[i][j] << endl;
                stream.output.flush();
                msg.send_data_to_neighbors[i][j] = "";
            }
        }
    }

    timer++;
}

int main(int argc, char *argv[])
{
    //Check number of arguments
    if (argc < 4 || argc > 5)
    {
        cout << "too " << (argc < 4 ? "few " : "many ") << "arguments passed" << endl;
        cout << "Requires: node_id, Duration, Destination, Message(if Destination !=-1)" << endl;
        return -1;
    }

    //Convert Char Array to long int
    long int arg[3];
    for (int i = 0; i < 3; i++)
        arg[i] = strtol(argv[i + 1], NULL, 10);

    //Check if a node is going to send data or not
    string data;
    if (arg[2] == -1)
        data = "";
    else
        data = argv[4];

    //Create a node
    Node node(arg[0], arg[1], arg[2], data);

    for (size_t i = 0; i < node.time_interval; i++)
    {
        // Send Hello Message every 30 seconds
        if (i % 30 == 0)
            node.helloProtocol();

        // Send In tree message every 10 seconds
        if (i % 10 == 0)
            node.intreeProtocol();

        // Send Data message every 15 seconds
        if (i % 15 == 0)
            node.dataProtocol();

        // Read the Input file and update the received file if neccessary
        node.processInputFile();

        // Sleep for One second
        sleep(1);
    }

    cout << "Node " << node.node_id << " Done" << endl;

    return 0;
}