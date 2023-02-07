#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

#define N 10

struct InitialFileView
{
    string inside_file_view;
    string outside_file_view;
    string got_file_view;
    fstream input;
    ofstream output;
    ofstream got_view;
};

struct Queue
{
    Queue(int capacity) : capacity(capacity), pointer_to_array(new int[capacity]), queue_first(0), queue_last(0), no_of_elements(0){};
    int capacity;
    int *pointer_to_array;
    int queue_first;
    int queue_last;
    int no_of_elements;
    void queue_insert(int);
    int queue_remove();
    bool queue_null();
};

void Queue::queue_insert(int val)
{
    pointer_to_array[queue_last] = val;
    queue_last = (queue_last + 1) % capacity;
    no_of_elements++;
}

int Queue::queue_remove()
{
    int var1 = pointer_to_array[queue_first];
    queue_first = (queue_first + 1) % capacity;
    no_of_elements--;
    return var1;
}

bool Queue::queue_null()
{
    if (no_of_elements == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

struct nodeHeight
{
    int height = -1;
    int final_loc;
};

struct PathFinder
{
    PathFinder(int final_loc, string data_value) : final_loc(final_loc), data_value(data_value)
    {
        for (size_t i = 0; i < N; i++)
        {
            peers_in[i] = 0;
            peers_in_route[i] = "";
            for (size_t j = 0; j < N; j++)
            {
                in_tree[i][j] = 0;
                previous_in_tree[i][j] = 0;
                send_data_to_neighbors[i][j] = "";
            }
        }
    };
    int final_loc;
    string destination_path = "";
    string data_value;
    string send_data_to_neighbors[N][N];
    int peers_in[N];
    int in_tree[N][N];
    int previous_in_tree[N][N];
    bool send_in_tree_current = false;
    string peers_in_route[N];
    bool isINnull();
    void storeInPeerRoute(size_t, size_t, int (&)[N][N]);
    void buildSPT(size_t, size_t, int (&)[N][N]);
    void breadthFirstSearcht(size_t, size_t, int (&)[N][N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N]));
    void breadthFirstSearcht(size_t, size_t, int (&)[N][N], nodeHeight (&)[N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N], nodeHeight (&)[N]));
    void breadthFirstSearchi(size_t, size_t, int (&)[N][N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N]));
    void breadthFirstSearchi(size_t, size_t, int (&)[N][N], nodeHeight (&)[N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N], nodeHeight (&)[N]));
    void deleteTempTreeRoute(size_t, size_t, int (&)[N][N]);
    void deleteInTreeRoute(size_t, size_t, int (&)[N][N]);
    void cutNode(size_t, size_t, int (&)[N][N]);
    void addHeight(size_t, size_t, int (&)[N][N], nodeHeight (&)[N]);
    void deleteHeight(size_t, size_t, int (&)[N][N], nodeHeight (&)[N]);
};

bool PathFinder::isINnull()
{
    for (size_t i = 0; i < N; i++)
    {
        if (peers_in[i])
            return false;
    }
    return true;
}

void PathFinder::storeInPeerRoute(size_t v, size_t root, int (&varInTree)[N][N])
{
    peers_in_route[root] = peers_in_route[root] + to_string(v) + " ";

    for (size_t w = 0; w < N; w++)
    {
        if (varInTree[v][w])
        {
            storeInPeerRoute(w, root, varInTree);
        }
    }
}

void PathFinder::buildSPT(size_t node_id, size_t root, int (&var1InTree)[N][N])
{
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            previous_in_tree[i][j] = in_tree[i][j];
        }
    }

    for (size_t i = 0; i < N; i++)
    {
        var1InTree[node_id][i] = 0;
    }
    breadthFirstSearcht(node_id, root, var1InTree, &PathFinder::deleteTempTreeRoute);
    var1InTree[root][node_id] = 1;
    breadthFirstSearchi(node_id, root, var1InTree, &PathFinder::cutNode);
    nodeHeight current_height[N];
    nodeHeight temp_height[N];
    breadthFirstSearchi(root, node_id, var1InTree, current_height, &PathFinder::addHeight);
    breadthFirstSearcht(node_id, root, var1InTree, temp_height, &PathFinder::addHeight);
    int join_tree[N][N] = {0};
    for (int jump = 1; jump < N; jump++)
    {
        int count = N;
        while (count--)
        {
            int compare_h1 = -1;
            int compare_temp = -1;
            for (size_t curLvl = 0; curLvl < N; curLvl++)
            {
                if (current_height[curLvl].height == jump)
                {
                    compare_h1 = curLvl;
                    break;
                }
            }
            for (size_t tmpLvl = 0; tmpLvl < N; tmpLvl++)
            {
                if (temp_height[tmpLvl].height == jump)
                {
                    compare_temp = tmpLvl;
                    break;
                }
            }
            if (compare_h1 == -1 && compare_temp == -1)
            {
                break;
            }
            else if ((compare_h1 != -1 && compare_temp == -1) || (compare_h1 != -1 && compare_temp != -1 && compare_h1 < compare_temp))
            {
                int final_loc = current_height[compare_h1].final_loc;
                if (temp_height[compare_h1].height == -1)
                {
                    join_tree[compare_h1][final_loc] = 1;
                }
                else
                {
                    join_tree[compare_h1][final_loc] = 1;
                    var1InTree[compare_h1][temp_height[compare_h1].final_loc] = 0;
                    breadthFirstSearcht(compare_h1, root, var1InTree, temp_height, &PathFinder::deleteHeight);
                }
                current_height[compare_h1].height = -1;
                current_height[compare_h1].final_loc = -1;
                temp_height[compare_h1].height = -1;
                temp_height[compare_h1].final_loc = -1;
            }
            else if ((compare_h1 == -1 && compare_temp != -1) || (compare_h1 != -1 && compare_temp != -1 && compare_h1 > compare_temp))
            {
                int final_loc = temp_height[compare_temp].final_loc;
                if (current_height[compare_temp].height == -1)
                {
                    join_tree[compare_temp][final_loc] = 1;
                }
                else
                {
                    join_tree[compare_temp][final_loc] = 1;
                    in_tree[compare_temp][current_height[compare_temp].final_loc] = 0;
                    breadthFirstSearchi(node_id, compare_temp, var1InTree, current_height, &PathFinder::deleteHeight);
                }
                temp_height[compare_temp].height = -1;
                temp_height[compare_temp].final_loc = -1;
                current_height[compare_temp].height = -1;
                current_height[compare_temp].final_loc = -1;
            }
            else if (compare_h1 != -1 && compare_temp != -1 && compare_h1 == compare_temp)
            {
                int final_loc = current_height[compare_h1].final_loc;
                join_tree[compare_h1][final_loc] = 1;
                current_height[compare_h1].height = -1;
                current_height[compare_h1].final_loc = -1;
                temp_height[compare_temp].height = -1;
                temp_height[compare_temp].final_loc = -1;
            }
        }
    }
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            in_tree[i][j] = join_tree[i][j];
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            if (previous_in_tree[i][j] != in_tree[i][j])
            {
                send_in_tree_current = true;
            }
        }
    }
}

void PathFinder::breadthFirstSearcht(size_t node_id, size_t root, int (&var1InTree)[N][N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N]))
{

    Queue graph_queue(N);
    bool visited[N] = {false};
    graph_queue.queue_insert(node_id);
    visited[node_id] = true;
    while (!graph_queue.queue_null())
    {

        int v = graph_queue.queue_remove();

        for (int w = 0; w < N; w++)
        {
            if (var1InTree[w][v])
            {
                if (!visited[w])
                {
                    visited[w] = 1;
                    (this->*func)(w, v, var1InTree);
                    graph_queue.queue_insert(w);
                }
            }
        }
    }
}

void PathFinder::breadthFirstSearcht(size_t node_id, size_t root, int (&var1InTree)[N][N], nodeHeight (&heights)[N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N], nodeHeight (&)[N]))
{

    heights[node_id].height = 0;
    heights[node_id].final_loc = -1;
    Queue graph_queue(N);
    bool visited[N] = {false};
    graph_queue.queue_insert(node_id);
    visited[node_id] = true;
    while (!graph_queue.queue_null())
    {
        int v = graph_queue.queue_remove();
        for (int w = 0; w < N; w++)
        {
            if (var1InTree[w][v])
            {
                if (!visited[w])
                {
                    visited[w] = 1;
                    (this->*func)(w, v, var1InTree, heights);
                    graph_queue.queue_insert(w);
                }
            }
        }
    }
}

void PathFinder::breadthFirstSearchi(size_t node_id, size_t root, int (&var1InTree)[N][N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N]))
{
    Queue graph_queue(N);
    bool visited[N] = {false};
    graph_queue.queue_insert(root);
    visited[root] = true;
    while (!graph_queue.queue_null())
    {
        int v = graph_queue.queue_remove();
        for (int w = 0; w < N; w++)
        {
            if (in_tree[w][v])
            {
                if (!visited[w])
                {
                    visited[w] = 1;
                    (this->*func)(w, v, var1InTree);
                    graph_queue.queue_insert(w);
                }
            }
        }
    }
}

void PathFinder::breadthFirstSearchi(size_t node_id, size_t root, int (&var1InTree)[N][N], nodeHeight (&heights)[N], void (PathFinder::*func)(size_t, size_t, int (&)[N][N], nodeHeight (&)[N]))
{
    heights[root].height = 0;
    heights[root].final_loc = -1;
    Queue graph_queue(N);
    bool visited[N] = {false};
    graph_queue.queue_insert(root);
    visited[root] = true;
    while (!graph_queue.queue_null())
    {
        int v = graph_queue.queue_remove();
        for (int w = 0; w < N; w++)
        {
            if (in_tree[w][v])
            {
                if (!visited[w])
                {
                    visited[w] = 1;
                    (this->*func)(w, v, var1InTree, heights);
                    graph_queue.queue_insert(w);
                }
            }
        }
    }
}

void PathFinder::deleteTempTreeRoute(size_t w, size_t v, int (&var1InTree)[N][N])
{
    var1InTree[w][v] = 0;
}

void PathFinder::deleteInTreeRoute(size_t w, size_t v, int (&var1InTree)[N][N])
{
    in_tree[w][v] = 0;
}

void PathFinder::cutNode(size_t w, size_t v, int (&var1InTree)[N][N])
{
    if (!var1InTree[w][v])
    {
        in_tree[w][v] = 0;
    }
}

void PathFinder::addHeight(size_t w, size_t v, int (&var1InTree)[N][N], nodeHeight (&heights)[N])
{
    heights[w].height = heights[v].height + 1;
    heights[w].final_loc = v;
}

void PathFinder::deleteHeight(size_t w, size_t v, int (&var1InTree)[N][N], nodeHeight (&heights)[N])
{
    var1InTree[w][v] = 0;
    heights[w].height = -1;
    heights[w].final_loc = -1;
}


class Node
{
public:
    Node(size_t node_id, size_t time_interval, int final_loc, string data_value) : node_id(node_id), time_interval(time_interval), msg(final_loc, data_value)
    {
        setStreams();
    };
    ~Node();
    size_t node_id;
    size_t time_interval;
    void helloFunc();
    void intreeFunc();
    void dataFunc();
    void processInputFile();

private:
    bool gotIntree[N] = {0};
    InitialFileView stream;
    PathFinder msg;
    void setStreams();
    string readFile(fstream &);
    void findPathToDest(int, string &);
    void computeHello(string &);
    void computeIntree(string &);
    void computeData(string &);
};

Node::~Node()
{
    stream.input.close();
    stream.output.close();
    stream.got_view.close();
}

void Node::setStreams()
{
    stream.inside_file_view = string("input_") + char('0' + node_id);
    stream.outside_file_view = string("output_") + char('0' + node_id);
    stream.got_file_view = char('0' + node_id) + string("_received");
    stream.input.open(stream.inside_file_view.c_str(), ios::out);
    stream.input.close();
    stream.input.open(stream.inside_file_view.c_str(), ios::in);
    stream.output.open(stream.outside_file_view.c_str(), ios::out | ios::app);
    stream.got_view.open(stream.got_file_view.c_str(), ios::out | ios::app);
    if (stream.input.fail())
    {
        cout << "NODE " << node_id << ": THERE IS NO INPUT FILE" << endl;
        exit(1);
    }
    if (stream.output.fail())
    {
        cout << "NODE " << node_id << ": THERE IS NO OUTPUT FILE" << endl;
        exit(1);
    }
    if (stream.got_view.fail())
    {
        cout << "NODE " << node_id << ": THERE IS NO RECEIVED FILE" << endl;
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

void Node::helloFunc()
{
    stream.output << "HELLO " << node_id << endl;
    stream.output.flush();
}

void Node::intreeFunc()
{
    if (msg.isINnull())
    {
        stream.output << "INTREE " << node_id << endl;
        stream.output.flush();
        return;
    }
    string buffer = "INTREE " + to_string(node_id) + " ";
    Queue qCurNode(N);
    bool visCur[N] = {false};
    qCurNode.queue_insert(node_id);
    visCur[node_id] = true;
    while (!qCurNode.queue_null())
    {
        int v = qCurNode.queue_remove();
        for (int w = 0; w < N; w++)
        {
            if (msg.in_tree[w][v])
            {
                if (!visCur[w])
                {
                    visCur[w] = 1;
                    qCurNode.queue_insert(w);
                    buffer = buffer + "(" + to_string(w) + " " + to_string(v) + ")";
                }
            }
        }
    }
    stream.output << buffer << endl;
    stream.output.flush();
}

void Node::findPathToDest(int v, string &path)
{
    path = path + to_string(v) + " ";
    for (size_t w = 0; w < N; w++)
    {
        if (msg.in_tree[v][w])
        {
            findPathToDest(w, path);
        }
    }
}

void Node::dataFunc()
{
    if (msg.final_loc != -1)
    {
        msg.destination_path = "";
        findPathToDest(msg.final_loc, msg.destination_path);
        string tempCheck = to_string(msg.final_loc) + " ";
        if (msg.destination_path == tempCheck)
        {
            msg.destination_path = "";
            return;
        }
        size_t len = msg.destination_path.length();
        char in = msg.destination_path[len - 4];
        if (msg.peers_in_route[in - '0'] == "")
            return;
        string path = msg.peers_in_route[in - '0'];
        path.erase(0, 2);
        stream.output << "DATA " << node_id << " " << msg.final_loc << " " << path << "START " << msg.data_value << endl;
        stream.output.flush();
    }
}

void Node::computeHello(string &line_number)
{
    char c = line_number[6];
    msg.peers_in[c - '0'] = 1;
}

void Node::computeIntree(string &line_number)
{
    size_t len = line_number.length();
    char c = line_number[7];
    int root = c - '0';
    gotIntree[root] = true;
    size_t body;
    if (len == 8)
    {
        body = 0;
    }
    else
    {
        body = len - 9;
    }
    int var1InTree[N][N] = {{0}};
    for (size_t i = 0; i < body; i += 5)
    {
        char queue_last = line_number[10 + i];
        char c = line_number[10 + i + 2];
        var1InTree[queue_last - '0'][c - '0'] = 1;
    }
    msg.peers_in_route[root] = "";
    msg.storeInPeerRoute(node_id, root, var1InTree);
    string tmpCheck = to_string(node_id) + " ";
    if (msg.peers_in_route[root] == tmpCheck)
        msg.peers_in_route[root] = "";
    msg.buildSPT(node_id, root, var1InTree);
}

void Node::computeData(string &line_number)
{
    char dataInterDest = line_number[9];
    if (unsigned(dataInterDest - '0') != node_id)
        return;
    char dataDest = line_number[7];
    char dataSrc = line_number[5];
    if (unsigned(dataDest - '0') == node_id && line_number[11] == 'S')
    {
        string message = line_number.erase(0, 17);
        stream.got_view << "MESSAGE FROM  " << dataSrc << " TO " << dataDest << " : " << message << endl;
    }
    else
    {
        if (line_number[11] == 'S')
        {
            string message = line_number.erase(0, 17);
            string intermediateNode = "";
            findPathToDest((dataDest - '0'), intermediateNode);
            string tempCheck = dataDest + " ";
            if (intermediateNode == tempCheck)
            {
                intermediateNode = "";
                return;
            }
            size_t len = intermediateNode.length();
            char in = intermediateNode[len - 4];
            if (msg.peers_in_route[in - '0'] == "")
                return;
            string path = msg.peers_in_route[in - '0'];
            path.erase(0, 2);
            for (size_t j = 0; j < N; j++)
            {
                if (msg.send_data_to_neighbors[dataSrc - '0'][j] == "")
                {
                    msg.send_data_to_neighbors[dataSrc - '0'][j] = "DATA " + to_string((dataSrc - '0')) + " " + to_string((dataDest - '0')) + " " + path + "START " + message;
                    break;
                }
            }
        }
        else
        {
            line_number.erase(line_number.begin() + 9);
            line_number.erase(line_number.begin() + 9);
            for (size_t j = 0; j < N; j++)
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
        if (line_number[0] == 'H')
            computeHello(line_number);
        if (line_number[0] == 'I')
            computeIntree(line_number);
        if (line_number[0] == 'D')
            computeData(line_number);
    }
    if (timer > 0 && ((timer - 2) % 10) == 0)
    {
        for (size_t i = 0; i < N; i++)
        {
            if (msg.peers_in[i] == 1 && gotIntree[i] == false)
            {
                cout << "NODE " << node_id << ": PHSS! NODE " << i << " IS DESTROYED... NOW FOLLOWING PEERS!" << endl;
                msg.in_tree[i][node_id] = 0;
                msg.breadthFirstSearchi(node_id, i, msg.in_tree, &PathFinder::deleteInTreeRoute);
                msg.peers_in[i] = 0;
                msg.send_in_tree_current = true;
            }
            else if (msg.peers_in[i] == 0 && gotIntree[i] == true)
            {
                msg.peers_in[i] = 1;
            }
            gotIntree[i] = false;
        }
    }
    if (msg.send_in_tree_current)
    {
        intreeFunc();
        msg.send_in_tree_current = false;
    }
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
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
    if (argc < 4 || argc > 5)
    {
        cout << "ERROR: " << (argc < 4 ? "LESS " : "MANY ") << "PARAMETERS PASSED" << endl;
        cout << "REQUIRED: node_id, Duration, Destination, Message" << endl;
        return -1;
    }
    long int arg[3];
    for (int i = 0; i < 3; i++)
        arg[i] = strtol(argv[i + 1], NULL, 10);
    string data;
    if (arg[2] == -1)
        data = "";
    else
        data = argv[4];
    Node node(arg[0], arg[1], arg[2], data);
    for (size_t i = 0; i < node.time_interval; i++)
    {
        if (i % 30 == 0)
            node.helloFunc();
        if (i % 10 == 0)
            node.intreeFunc();
        if (i % 15 == 0)
            node.dataFunc();
        node.processInputFile();
        sleep(1);
    }
    cout << "NODE " << node.node_id << " SUCCESS" << endl;
    return 0;
}
