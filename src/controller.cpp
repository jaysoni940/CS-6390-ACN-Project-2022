#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

using namespace std;

#define N 10

struct InitialFileView
{
    string inside_file_view;
    string outside_file_view;
    fstream input;
    ofstream output;
};

class NodeDetail
{
public:
    NodeDetail(){};
    ~NodeDetail(){};
    size_t nn = 0;
    InitialFileView *streams;
    int network_links[N][N] = {{0}};
    int no_response_node[N] = {0};
    void setStreams();
};

void NodeDetail::setStreams()
{
    streams = new InitialFileView[nn];
    for (size_t i = 0; i < nn; i++)
    {
        streams[i].inside_file_view = string("output_") + char('0' + i);
        streams[i].outside_file_view = string("input_") + char('0' + i);
        streams[i].input.open(streams[i].inside_file_view.c_str(), ios::in);
        streams[i].output.open(streams[i].outside_file_view.c_str(), ios::out | ios::app);
        if (streams[i].input.fail())
        {
            cout << "CONTROLLER: NODE " << i << "THERE IS NO INPUT FILE" << endl;
            exit(1);
        }
        if (streams[i].output.fail())
        {
            cout << "CONTROLLER: NODE " << i << "THERE IS NO OUTPUT FILE" << endl;
            exit(1);
        }
    }
}

class Controller
{
public:
    Controller(size_t time_interval) : time_interval(time_interval)
    {
        createStream();
        setNodeStreams();
    };
    ~Controller(){};
    size_t time_interval;
    void sendToPeers();

private:
    InitialFileView stream;
    NodeDetail nodes;
    void createStream();
    void setNodeStreams();
    void parseString(string);
    string readFile(fstream &);
};

void Controller::parseString(string line_number)
{
    char char1 = line_number[0];
    char char2 = line_number[2];

    if (unsigned(char1 - '0') + 1 > nodes.nn || unsigned(char2 - '0') + 1 > nodes.nn)
    {
        if ((char1 - '0') > (char2 - '0'))
            nodes.nn = char1 - '0' + 1;
        else
            nodes.nn = char2 - '0' + 1;
    }
    nodes.network_links[unsigned(char1 - '0')][unsigned(char2 - '0')] = 1;
}

void Controller::setNodeStreams()
{
    string line_number;
    while (getline(stream.input, line_number) && !stream.input.eof())
    {
        parseString(line_number);
    }
    nodes.setStreams();
}

void Controller::createStream()
{
    stream.inside_file_view = string("topology");
    stream.outside_file_view = "";
    stream.input.open(stream.inside_file_view.c_str(), ios::in);
    if (stream.input.fail())
    {
        cout << "NO FILE FOUND";
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

void Controller::sendToPeers()
{
    for (size_t i = 0; i < nodes.nn; i++)
    {
        string line_number = "";
        while ((line_number = readFile(nodes.streams[i].input)) != "")
        {
            for (size_t j = 0; j < nodes.nn; j++)
            {
                if (nodes.network_links[i][j])
                {
                    nodes.streams[j].output << line_number << endl;
                    nodes.streams[j].output.flush();
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "ERROR: " << (argc < 3 ? "LESS " : "many ") << "PARAMETERS PASSED..." << endl;
        cout << "REQUIRED: Duration" << endl;
        return -1;
    }
    long int arg = strtol(argv[1], NULL, 10);
    sleep(1);
    cout << endl;
    Controller controller(arg);
    for (size_t i = 0; i < controller.time_interval; i++)
    {
        controller.sendToPeers();
        sleep(1);
    }
    cout << "CONTROLLER SUCCESS" << endl;
    return 0;
}