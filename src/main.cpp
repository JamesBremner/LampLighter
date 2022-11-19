#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

/// @brief An edge between sources containing lamps
class cEdge
{
public:
    int source1;   // id of fuel source at one end of edge
    int source2;   // id of fuel source at other end of edge
    int lampCount; // count of lamps on edge
    int fueled;    // count of lamps fuelled

    cEdge()
        : fueled(0)
    {
    }

    /// @brief get id of fuel source at other end of edge
    /// @param i id of fuel source whose 'partner is required
    /// @return id of fuel source at other end of edge
    int other(int i)
    {
        if (source1 == i)
            return source2;
        else
            return source1;
    }
};

/// @brief A fueling source
class cSource
{

    int id;                                 ///< source id
    std::vector<cEdge> vEdge;               ///< edges on this source
    int radius;                             ///< radius of reachable lamps
    static std::vector<cSource> vSource;    ///< vector of sources

public:

    /// @brief read input
    /// @param fname filename
    static void read(const std::string &fname);

    /// @brief find source from id
    /// @param id 
    /// @return reference to source
    static cSource &find(int id);

    /// @brief Fuel the lamps
    static void fuel();

    /// @brief Display source radii and total fuel
    static void totalFuel();

    /// @brief Fuel reachable lamps
    void fuelReachableLamps();

    /// @brief Single edge with unfuelled lamps
    /// @return pointer to unfuelled edge if just one, otherwise null
    cEdge *singleUnfueledEdge();

    static bool areAllLampsFueled();
};

std::vector<cSource> cSource::vSource;

cSource &cSource::find(int id)
{
    for (auto &s : vSource)
        if (s.id == id)
            return s;
    throw std::runtime_error(
        "Cannot find source " + std::to_string(id));
}

/// @brief read input file
/// @param fname input filename

void cSource::read(const std::string &fname)
{
    std::ifstream inf(fname);
    int n1, n2, lamp;
    cEdge e;

    while (!inf.eof())
    {
        // read ids of sources connected by an edge with a lamp count
        inf >> e.source1 >> e.source2 >> e.lampCount;

        // echo input
        std::cout << e.source1 << " " << e.source2 << " " << e.lampCount << "\n";

        // find first source in source vector
        try
        {
            cSource &s = cSource::find(e.source1);

            // add edge to previously found source
            s.vEdge.push_back(e);
        }
        catch (...)
        {
            // new source, add to source vector
            cSource s;
            s.id = e.source1;
            s.vEdge.push_back(e);
            s.radius = 0;
            vSource.push_back(s);
        }
        try
        {
            cSource &s = cSource::find(e.source2);

            // add edge to previously found source
            s.vEdge.push_back(e);
        }
        catch (...)
        {
            // new source, add to source vector
            cSource s;
            s.id = e.source2;
            s.vEdge.push_back(e);
            s.radius = 0;
            vSource.push_back(s);
        }
    }
}

/// @brief Fuel all reachable lamps from source
/// @param fuelSource fuel source with set radius

void cSource::fuelReachableLamps()
{
    // std::cout << "fuelling from " << fuelSource.id << "\n";

    // select other sources connected to fuelling source
    for (auto &e : vEdge)
    {
        e.fueled += radius;

        auto &otherSource = cSource::find(e.other(id));

        // loop over edges on other source
        for (auto &oe : otherSource.vEdge)
        {
            // if edge connects to fuelling source, fuel it
            if (oe.other(otherSource.id) == id)
                oe.fueled += radius;
        }
    }
}

cEdge *cSource::singleUnfueledEdge()
{
    cEdge *punfuelledEdge = 0;
    for (auto &e : vEdge)
    {
        if (e.lampCount > e.fueled)
        {
            if (punfuelledEdge)
            {
                punfuelledEdge = 0;
                break;
            }
            punfuelledEdge = &e;
        }
    }
    return punfuelledEdge;
}

/// @brief Fuel all the lamps
void cSource::fuel()
{
    // loop while lamps are being fuelled
    bool progress = true;
    while (progress)
    {
        progress = false;

        // loop over nodes
        for (auto &s : vSource)
        {
            // std::cout << s.id << "\n";

            // check that node has just one unfuelled edge
            cEdge *unfuelledEdge = s.singleUnfueledEdge();
            if (!unfuelledEdge)
                continue;

            // fuel the edge from the source at the other end of the unfuelled edge
            auto &fuelSource = cSource::find(unfuelledEdge->other(s.id));

            // increment radius sufficient to fuel
            // the unfuelled lamps on edge
            fuelSource.radius += unfuelledEdge->lampCount - unfuelledEdge->fueled;

            // fuel all edges reachable from source
            fuelSource.fuelReachableLamps();

            progress = true;
        }
    }
}

void cSource::totalFuel()
{
    std::cout << "\nsource radius\n";
    int fuel = 0;
    for (auto &s : vSource)
    {
        fuel += s.radius;
        std::cout << s.id << " r="
                  << s.radius << "\n";
    }
    std::cout << "\ntotal fuel " << fuel << "\n";
}

bool cSource::areAllLampsFueled()
{
    for( auto& s : vSource)
        for( auto& e : s.vEdge )
            if( e.fueled < e.lampCount )
                {
                    std::cout << "unfueled lamp!!! "
                        << e.source1 << " to " << e.source2 << "\n";
                    return false;
                }
    return true;
}

main( int argc, char * argv[] )
{
    if( argc != 2 )
    {
        std::cout << "usage >lamp <fname>\n";
        exit(1);
    }
    cSource::read(argv[1]);

    cSource::fuel();
    cSource::totalFuel();
    cSource::areAllLampsFueled();
    return 0;
}