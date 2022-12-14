/*  
 *  Simulates a fictional component inside a network
 *  to enable forwarding of data packets from one node to other node.
 *  Copyright (C) 2021 Sourabh J Choure
 * 
 *  I promise that the work presented below is of my own.
 * 
 *  To make it FOSS Compliant, this software is free to use.
 */

// STL
#include <iostream>
#include <fstream>
// SL
#include <cstdlib>
#include <cstring>
// Unix
#include <unistd.h>

using namespace std;

#define TOTAL_NODES 10

struct FileStructure
{
    // Store the name of the file
    string input_file_name;
    string output_file_name;

    // File Descriptors
    fstream input;
    ofstream output;
};

class NodeDetails
{
public:
    NodeDetails(){};
    ~NodeDetails(){};

    // Total number of nodes
    size_t total_no_of_nodes = 0;

    // Channels of Controller
    FileStructure *streams;

    // Topology Links
    int network_links[TOTAL_NODES][TOTAL_NODES] = {{0}};

    // Counter to check if the node is not responding
    int node_no_response[TOTAL_NODES] = {0};

    // Create the streams
    void createStreaams();
};

void NodeDetails::createStreaams()
{
    streams = new FileStructure[total_no_of_nodes];

    for (size_t i = 0; i < total_no_of_nodes; i++)
    {
        // Give a name to the files
        streams[i].input_file_name = string("output_") + char('0' + i);
        streams[i].output_file_name = string("input_") + char('0' + i);

        // Create the files
        streams[i].input.open(streams[i].input_file_name.c_str(), ios::in);
        streams[i].output.open(streams[i].output_file_name.c_str(), ios::out | ios::app);

        if (streams[i].input.fail())
        {
            cout << "Controller: Node " << i << " No input file" << endl;
            exit(1);
        }
        if (streams[i].output.fail())
        {
            cout << "Controller: Node " << i << " No output file" << endl;
            exit(1);
        }
    }
}

class Controller
{
public:
    Controller(size_t time_interval) : time_interval(time_interval)
    {
        setStream(); // topology
        createNodeStream(); // Node streams
    };
    ~Controller(){};

    // Duration
    size_t time_interval;

    // beta function
    void sendDataToNeighbors();

private:
    // Channels of Controller
    FileStructure stream;

    // Node Record Entries
    NodeDetails node_instance;

    // Init Channels
    void setStream();

    //Create New Channels
    void createNodeStream();

    //Parse the strings
    void parseString(string);

    // Read File
    string readFile(fstream &);
};

void Controller::parseString(string line_number) //Waring: Single Sequencial Digit Parser only!!
{
    // Store the characters
    char c1 = line_number[0];
    char c2 = line_number[2];

    if (unsigned(c1 - '0') + 1 > node_instance.total_no_of_nodes || unsigned(c2 - '0') + 1 > node_instance.total_no_of_nodes)
    {
        if ((c1 - '0') > (c2 - '0'))
            node_instance.total_no_of_nodes = c1 - '0' + 1;
        else
            node_instance.total_no_of_nodes = c2 - '0' + 1;
    }

    node_instance.network_links[unsigned(c1 - '0')][unsigned(c2 - '0')] = 1;
}

void Controller::createNodeStream()
{
    // Check and Parse the topology file
    string line_number;
    while (getline(stream.input, line_number) && !stream.input.eof())
    {
        parseString(line_number);
    }

    // Create the Channels
    node_instance.createStreaams();
}

void Controller::setStream()
{
    // Store the name of the file to open
    stream.input_file_name = string("topology");
    stream.output_file_name = "";

    // Open the file as input
    stream.input.open(stream.input_file_name.c_str(), ios::in);
    if (stream.input.fail())
    {
        cout << "No file";
        exit(1);
    }
}

string Controller::readFile(fstream &fd)
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

void Controller::sendDataToNeighbors()
{
    // Search through the topology links to find the neighbors
    for (size_t i = 0; i < node_instance.total_no_of_nodes; i++)
    {
        // Read the output file of the node for the hello message
        string line_number = "";
        while((line_number = readFile(node_instance.streams[i].input)) != "")
        {
            // Go through all the links of that particular node_instance
            for (size_t j = 0; j < node_instance.total_no_of_nodes; j++)
            {
                // If the link exist then put the message of that node_instance input file
                if (node_instance.network_links[i][j])
                {
                    node_instance.streams[j].output << line_number << endl;
                    node_instance.streams[j].output.flush(); //force
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    //Check number of arguments
    if (argc != 2)
    {
        cout << "too " << (argc < 3 ? "few " : "many ") << "arguments passed" << endl;
        cout << "Requires: Duration" << endl;
        return -1;
    }

    //Convert Char Array to long int
    long int arg = strtol(argv[1], NULL, 10);

    // Let the nodes get init
    sleep(1);

    cout << endl;

    //Create a node
    Controller controller(arg);

    // Start the algo
    for (size_t i = 0; i < controller.time_interval; i++)
    {
        controller.sendDataToNeighbors();
        sleep(1);
    }
    cout << "Controller Done" << endl;

    return 0;
}